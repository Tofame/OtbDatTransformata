#include "fileLoader.h"
#include <stack>
#include <fstream>
#include <iterator>
#include <algorithm>

namespace OTB {

    constexpr Identifier wildcard = {{'\0', '\0', '\0', '\0'}};

    Loader::Loader(const std::string& fileName, const std::array<char, 4>& acceptedIdentifier) {
        // Read entire file into memory
        std::ifstream file(fileName, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw InvalidOTBFormat{};
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        constexpr auto minimalSize = sizeof(std::array<char, 4>) + sizeof(Node::START) + sizeof(Node::type) + sizeof(Node::END);
        if (size <= minimalSize) {
            throw InvalidOTBFormat{};
        }

        fileData.resize(size);
        if (!file.read(fileData.data(), size)) {
            throw InvalidOTBFormat{};
        }

        // Check identifier
        std::array<char, 4> fileIdentifier;
        std::copy_n(fileData.begin(), fileIdentifier.size(), fileIdentifier.begin());
        if (fileIdentifier != acceptedIdentifier && fileIdentifier != wildcard) {
            throw InvalidOTBFormat{};
        }

        // Initialize root node propsBegin to point to the data
        root.propsBegin = fileData.data();
    }

    using NodeStack = std::stack<Node*, std::vector<Node*>>;
    static Node& getCurrentNode(const NodeStack& nodeStack) {
        if (nodeStack.empty()) {
            throw InvalidOTBFormat{};
        }
        return *nodeStack.top();
    }

    const Node& Loader::parseTree() {
        auto it = fileData.begin() + sizeof(std::array<char, 4>);
        if (static_cast<uint8_t>(*it) != Node::START) {
            throw InvalidOTBFormat{};
        }

        root.type = *(++it);
        root.propsBegin = &(*it) + 1; // Points to first byte after type

        NodeStack parseStack;
        parseStack.push(&root);

        for (; it != fileData.end(); ++it) {
            switch(static_cast<uint8_t>(*it)) {
                case Node::START: {
                    auto& currentNode = getCurrentNode(parseStack);
                    if (currentNode.children.empty()) {
                        currentNode.propsEnd = &(*it);
                    }
                    currentNode.children.emplace_back();
                    auto& child = currentNode.children.back();
                    if (++it == fileData.end()) {
                        throw InvalidOTBFormat{};
                    }
                    child.type = *it;
                    child.propsBegin = &(*it) + 1;
                    parseStack.push(&child);
                    break;
                }
                case Node::END: {
                    auto& currentNode = getCurrentNode(parseStack);
                    if (currentNode.children.empty()) {
                        currentNode.propsEnd = &(*it);
                    }
                    parseStack.pop();
                    break;
                }
                case Node::ESCAPE: {
                    if (++it == fileData.end()) {
                        throw InvalidOTBFormat{};
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        if (!parseStack.empty()) {
            throw InvalidOTBFormat{};
        }

        return root;
    }

    bool Loader::getProps(const Node& node, PropStream& props) {
        if (node.propsBegin == nullptr || node.propsEnd == nullptr || node.propsEnd <= node.propsBegin) {
            return false;
        }

        size_t size = node.propsEnd - node.propsBegin;
        propBuffer.resize(size);

        bool lastEscaped = false;
        auto escapedPropEnd = std::copy_if(node.propsBegin, node.propsEnd, propBuffer.begin(),
                                           [&lastEscaped](const char byte) {
                                               lastEscaped = (byte == static_cast<char>(Node::ESCAPE)) && !lastEscaped;
                                               return !lastEscaped;
                                           });

        props.init(propBuffer.data(), std::distance(propBuffer.begin(), escapedPropEnd));
        return true;
    }
}