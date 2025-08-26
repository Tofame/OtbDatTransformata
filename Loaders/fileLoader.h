#pragma once

#include <limits>
#include <vector>
#include <array>
#include <exception>
#include <string_view>
#include <fstream>
#include <memory>

class PropStream;

namespace OTB {
    using Identifier = std::array<char, 4>;

    struct Node
    {
        using ChildrenVector = std::vector<Node>;

        ChildrenVector children;
        const char*    propsBegin;
        const char*    propsEnd;
        uint8_t        type;

        enum NodeChar: uint8_t
        {
            ESCAPE = 0xFD,
            START  = 0xFE,
            END    = 0xFF,
        };
    };

    struct LoadError : std::exception {
        const char* what() const noexcept override = 0;
    };

    struct InvalidOTBFormat final : LoadError {
        const char* what() const noexcept override {
            return "Invalid OTBM file format";
        }
    };

    class Loader {
        std::vector<char> fileData;
        Node root;
        std::vector<char> propBuffer;

    public:
        Loader(const std::string& fileName, const std::array<char, 4>& acceptedIdentifier);
        bool getProps(const Node& node, PropStream& props);
        const Node& parseTree();
    };

} //namespace OTB

class PropStream
{
public:
    void init(const char* a, size_t size) {
        p = a;
        end = a + size;
    }

    size_t size() const {
        return end - p;
    }

    template <typename T>
    bool read(T& ret) {
        if (size() < sizeof(T)) {
            return false;
        }

        memcpy(&ret, p, sizeof(T));
        p += sizeof(T);
        return true;
    }

    std::pair<std::string_view, bool> readString() {
        uint16_t strLen;
        if (!read<uint16_t>(strLen)) {
            return { "", false };
        }

        if (size() < strLen) {
            return { "", false };
        }

        std::string_view ret{ p, strLen };
        p += strLen;
        return { ret, true };
    }

    bool skip(size_t n) {
        if (size() < n) {
            return false;
        }

        p += n;
        return true;
    }

    bool readBytes(void* dest, size_t n) {
        if (size() < n) {
            return false;
        }
        memcpy(dest, p, n);
        p += n;
        return true;
    }

private:
    const char* p = nullptr;
    const char* end = nullptr;
};

class PropWriteStream
{
public:
    PropWriteStream() = default;

    // non-copyable
    PropWriteStream(const PropWriteStream&) = delete;
    PropWriteStream& operator=(const PropWriteStream&) = delete;

    std::string_view getStream() const {
        return { buffer.data(), buffer.size() };
    }

    void clear() {
        buffer.clear();
    }

    template <typename T>
    void write(T add) {
        char* addr = reinterpret_cast<char*>(&add);
        std::copy(addr, addr + sizeof(T), std::back_inserter(buffer));
    }

    void writeString(const std::string& str) {
        size_t strLength = str.size();
        if (strLength > std::numeric_limits<uint16_t>::max()) {
            write<uint16_t>(0);
            return;
        }

        write(static_cast<uint16_t>(strLength));
        std::copy(str.begin(), str.end(), std::back_inserter(buffer));
    }

private:
    std::vector<char> buffer;
};