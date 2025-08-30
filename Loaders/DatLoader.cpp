#include <fstream>
#include <iostream>
#include <filesystem>
#include "DatLoader.h"
#include "DatAttributes.h"
#include "../settings.h"
#include "PluginMain.h"
#include "PluginThree.h"

bool DatLoader::loadFromDat(const std::string& file, Items& items) {
    m_datLoaded = false; // Start by assuming the load will fail
    m_datSignature = 0;
    m_rawRest.clear();

    try {
        // Otcv8 was 'guessing' the path here, we just tell it in the param
        std::string datFile = file;

        std::ifstream fin(datFile, std::ios::binary);
        if (!fin.is_open()) {
            throw std::runtime_error(
                    "Failed to open DAT file: " + datFile +
                    " (Looked in: " + std::filesystem::absolute(datFile).string() + ")"
            );
        }

        // Read signature
        fin.read(reinterpret_cast<char *>(&m_datSignature), sizeof(m_datSignature));

        // Read the item count, outfit count, effect count, missiles count (distance effects)
        fin.read(reinterpret_cast<char *>(&m_loadedItemsCount), sizeof(m_loadedItemsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedOutfitsCount), sizeof(m_loadedOutfitsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedEffectsCount), sizeof(m_loadedEffectsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedMissilesCount), sizeof(m_loadedMissilesCount));

        // Load items
        auto &itemTypesDat = items.getItemTypesDat();
        itemTypesDat.resize(m_loadedItemsCount + 1);
        uint16_t firstId = Settings::getInstance().MIN_ITEM_ID; // .dat items start at 100

        for (uint16_t id = firstId; id < itemTypesDat.size(); ++id) {
            auto type = ItemType();
            unserialize(type, id, fin);
            itemTypesDat[id] = type;
        }

        // Preserve remaining bytes (outfits, effects, missiles)
        // 1. Determine how many bytes are left in the file
        std::streampos restStart = fin.tellg();
        fin.seekg(0, std::ios::end);
        std::streampos restEnd = fin.tellg();
        size_t restSize = static_cast<size_t>(restEnd - restStart);

        // 2. Resize the vector to exactly hold the remaining bytes
        m_rawRest.resize(restSize);

        // 3. Seek back to the start of the leftover section
        fin.seekg(restStart, std::ios::beg);

        // 4. Read directly into the vector
//        if (restSize > 0) {
//            fin.seekg(restStart, std::ios::beg);
//            fin.read(m_rawRest.data(), restSize);
//            if (!fin) {
//                throw std::runtime_error("Failed to read leftover bytes from DAT file");
//            }
//        }
        fin.read(m_rawRest.data(), restSize);

        m_datLoaded = true;
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error loading DAT file: " << e.what() << std::endl;
        return false;
    }
}

void DatLoader::unserialize(ItemType& itemType, uint16_t cid, std::ifstream& fin) {
    itemType.clientId = cid;

    std::unique_ptr<PluginMain> plugin;
    if(Settings::getInstance().protocolVersion >= 1011) {
        /* In 10.10+ all attributes from 16 and up were
         * incremented by 1 to make space for 16 as
         * "No Movement Animation" flag.
         */
        plugin = std::make_unique<PluginThree>();
    } else {
        throw std::runtime_error("DatLoader::unserialize - no protocols below 10.11 are supported, change globals.h, or add support.");
    }

    int prevAttr = -1;
    int attr = -1;
    bool done = false;

    for(int i = 0; i < PluginMain::LastFlag; ++i) {
        prevAttr = attr;
        attr = fin.get();
        if(attr == PluginMain::LastFlag) {
            done = true;
            break;
        }

        if(!plugin->unserializeDatAttribute(itemType, attr, fin)) {
            std::cout << "[DatLoader::unserialize] Unknown attribute: "
                      << static_cast<int>(attr) << " previous attr: " << prevAttr << std::dec << "\n";
            break;
        };
    }

    if(!done)
        throw std::runtime_error("Couldn't finish unserializing attributes in: " + itemType.clientId);

    // Read sprite data
    uint8_t groupCount = 1;
//    if(frameGroups && type.category == ThingCategory.OUTFIT) {
//        groupCount = readUnsignedByte();
//    }
    uint32_t i;
    uint8_t groupType;
    FrameGroup frameGroup = FrameGroup();

    for(groupType = 0; groupType < groupCount; groupType++) {
        frameGroup.width = fin.get();
        frameGroup.height = fin.get();
        if (frameGroup.width > 1 || frameGroup.height > 1) {
            frameGroup.exactSize = fin.get(); // realSize, exactSize
        } else {
            // not really needed for our app, OB would use it probably (?), however Writer won't use it anyway
            // only uses it when: frameGroup.width > 1 || frameGroup.height > 1
            frameGroup.exactSize = Settings::getInstance().spritesExactSize;
        }

        frameGroup.layers = fin.get();
        frameGroup.patternX = fin.get();
        frameGroup.patternY = fin.get();
        frameGroup.patternZ = fin.get();
        frameGroup.frames = fin.get(); // animationPhases

        if (frameGroup.frames > 1) { // && g_game.getFeature(Otc::GameEnhancedAnimations), which is >= 1050
            frameGroup.isAnimation = true;
            frameGroup.frameDurations = std::vector<FrameDuration>(frameGroup.frames);
            //uint8_t m_async = fin.get(); // m_async
            //uint32_t m_loopCount = 0; // m_loopCount
            //fin.read(reinterpret_cast<char *>(&m_loopCount), sizeof(m_loopCount));
            //uint8_t m_startPhase = fin.get();

            // Maybe irrevelant check here? or a pointer vector should be implemented to check for nullptr vector.
            //if (!frameGroup.frameDurations.empty()) {
                frameGroup.animationMode = (AnimationMode)fin.get();
                fin.read(reinterpret_cast<char*>(&frameGroup.loopCount), sizeof(frameGroup.loopCount));
                frameGroup.startFrame = fin.get();

                for (int i = 0; i < frameGroup.frames; ++i) {
                    uint32_t minimum = 0;
                    fin.read(reinterpret_cast<char *>(&minimum), sizeof(minimum));
                    uint32_t maximum = 0;
                    fin.read(reinterpret_cast<char *>(&maximum), sizeof(maximum));
                    if(minimum > maximum) {
                        std::cout << "[WARNING]: Cid " << cid << " item has minimum greater than maximum - " << minimum << ">" << maximum << "\n";
                    }
                    frameGroup.frameDurations[i] = FrameDuration(minimum, maximum);
                }
            //}
        }

        // Sprite IDs
        // itemType.width * itemType.height * layers * patternX * patternY * patternZ * animationPhases
        //uint32_t totalSprites = frameGroup.getTotalSprites();
        uint32_t totalSprites = frameGroup.getTotalSprites();
        constexpr uint32_t MAX_SPRITES = 1000000; // pick a safe upper bound
        if (totalSprites > MAX_SPRITES) {
            throw std::runtime_error("Item has too many sprites: " + std::to_string(totalSprites));
        }

        // Calculate the number of bytes to read for sprites
        const size_t bytesToRead = totalSprites * (Settings::getInstance().isExtended ? sizeof(uint32_t) : sizeof(uint16_t));
        std::streampos currentPos = fin.tellg();
        fin.seekg(0, std::ios::end);
        std::streampos endPos = fin.tellg();
        fin.seekg(currentPos, std::ios::beg);

        if (static_cast<size_t>(endPos - currentPos) < bytesToRead) {
            throw std::runtime_error("Not enough data in DAT file to read all sprites for item " + std::to_string(itemType.clientId) +
                                     ". Expected " + std::to_string(bytesToRead) + " bytes, but only " + std::to_string(endPos - currentPos) + " remain.");
        }

        frameGroup.spriteIndex.resize(totalSprites);
        //std::cout << "Total sprites " << totalSprites << "\n";

        //fin.ignore(totalSprites * sizeof(uint32_t));
        for (i = 0; i < totalSprites; i++) {
            if (Settings::getInstance().isExtended) {
                uint32_t spId = 0;
                if (!fin.read(reinterpret_cast<char*>(&spId), sizeof(spId))) {
                    throw std::runtime_error("Failed to read extended sprite index " + std::to_string(i) +
                                             " for item " + std::to_string(itemType.clientId) + " spId " + std::to_string(spId));
                }
                frameGroup.spriteIndex[i] = spId;
            } else {
                uint16_t spId = 0;
                fin.read(reinterpret_cast<char *>(&spId), sizeof(spId));
                frameGroup.spriteIndex[i] = spId;
            }
        }

        itemType.setFrameGroup(groupType, frameGroup);

        // Skip sprite IDs
//        uint32_t spriteCount = frameGroup.width * frameGroup.height * frameGroup.layers * frameGroup.patternX * frameGroup.patternY * frameGroup.patternZ * frameGroup.frames;
//        if(/* clientVersion >= */ true) { // U32 sprites, extended type shii
//            fin.ignore(spriteCount * 4);
//        } else {
//            fin.ignore(spriteCount * 2);
//        }
    }
}