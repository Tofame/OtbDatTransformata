#include <fstream>
#include <iostream>
#include "DatLoader.h"
#include "DatAttributes.h"
#include "../Compilers/MetadataFlags6.h"

bool DatLoader::loadFromDat(const std::string& file, Items& items) {
    m_datLoaded = false; // Start by assuming the load will fail
    m_datSignature = 0;
    m_contentRevision = 0;
    m_rawRest.clear();

    try {
        // Otcv8 was 'guessing' the path here, we just tell it in the param
        std::string datFile = file;

        std::ifstream fin(datFile, std::ios::binary);
        if (!fin.is_open()) {
            throw std::runtime_error("Failed to open DAT file: " + datFile);
        }

        // Read signature and revision
        fin.read(reinterpret_cast<char *>(&m_datSignature), sizeof(m_datSignature));
        m_contentRevision = static_cast<uint16_t>(m_datSignature);

        // Read the item count, outfit count, effect count, missiles count (distance effects)
        uint16_t itemCount = 0, outfitCount = 0, effectCount = 0, distanceCount = 0;
        fin.read(reinterpret_cast<char *>(&itemCount), sizeof(itemCount));
        fin.read(reinterpret_cast<char *>(&outfitCount), sizeof(outfitCount));
        fin.read(reinterpret_cast<char *>(&effectCount), sizeof(effectCount));
        fin.read(reinterpret_cast<char *>(&distanceCount), sizeof(distanceCount));

        // Load items
        auto &itemTypesDat = items.getItemTypesDat();
        itemTypesDat.resize(itemCount + 1);
        uint16_t firstId = 100; // .dat items start at 100

        for (uint16_t id = firstId; id < itemTypesDat.size(); ++id) {
            auto type = ItemType();
            unserialize(type, id, fin);
            itemTypesDat[id] = type;
        }

        // Preserve remaining bytes (outfits, effects, missiles)
        std::streampos restStart = fin.tellg();
        fin.seekg(0, std::ios::end);
        std::streampos restEnd = fin.tellg();
        size_t restSize = static_cast<size_t>(restEnd - restStart);

        m_rawRest.resize(restSize);
        fin.seekg(restStart, std::ios::beg);
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

    int count = 0;
    uint8_t attr = 0;
    bool done = false;
    for(int i = 0; i < MetadataFlags6::LAST_FLAG; ++i) {
        count++;
        attr = fin.get();
        if(attr == MetadataFlags6::LAST_FLAG) {
            done = true;
            break;
        }

        if(attr > 16)
            attr -= 1;

        switch (attr) {
            case MetadataFlags6::HAS_OFFSET: { // ThingAttrDisplacement
                uint16_t m_displacementX, m_displacementY;
                fin.read(reinterpret_cast<char*>(&m_displacementX), 2);
                fin.read(reinterpret_cast<char*>(&m_displacementY), 2);

                itemType.hasOffset = true;
                itemType.offsetX = m_displacementX;
                itemType.offsetY = m_displacementY;
                break;
            }

            case MetadataFlags6::HAS_LIGHT: { // ThingAttrLight
                uint16_t intensity, color;
                fin.read(reinterpret_cast<char*>(&intensity), 2);
                fin.read(reinterpret_cast<char*>(&color), 2);

                itemType.hasLight = true;
                itemType.lightLevel = intensity;
                itemType.lightColor = color;
                break;
            }

            case MetadataFlags6::MARKET_ITEM: { // ThingAttrMarket
                struct MarketData {
                    std::string name;
                    int category;
                    uint16_t requiredLevel;
                    uint16_t restrictVocation;
                    uint16_t showAs;
                    uint16_t tradeAs;
                };
                MarketData market;
                uint16_t len;

                fin.read(reinterpret_cast<char*>(&market.category), 2);
                fin.read(reinterpret_cast<char*>(&market.tradeAs), 2);
                fin.read(reinterpret_cast<char*>(&market.showAs), 2);
                fin.read(reinterpret_cast<char*>(&len), 2);

                market.name.resize(len);
                fin.read(&market.name[0], len);

                fin.read(reinterpret_cast<char*>(&market.restrictVocation), 2);
                fin.read(reinterpret_cast<char*>(&market.requiredLevel), 2);

                itemType.isMarketItem = true;
                itemType.marketCategory = market.category;
                itemType.marketTradeAs = market.tradeAs;
                itemType.marketShowAs = market.showAs;
                itemType.marketName = market.name;
                itemType.marketRestrictProfession = market.restrictVocation;
                itemType.marketRestrictLevel = market.requiredLevel;
                break;
            }

            case MetadataFlags6::HAS_ELEVATION: { // ThingAttrElevation
                uint16_t elevation;
                fin.read(reinterpret_cast<char*>(&elevation), 2);

                itemType.hasElevation = true;
                itemType.elevation = elevation;
                break;
            }

            case MetadataFlags6::USABLE:
            case MetadataFlags6::WRITABLE:
            case MetadataFlags6::WRITABLE_ONCE:
            case MetadataFlags6::MINI_MAP:
            case MetadataFlags6::CLOTH:
            case MetadataFlags6::LENS_HELP: {
                uint16_t val;
                fin.read(reinterpret_cast<char*>(&val), 2);

                if (attr == MetadataFlags6::WRITABLE || attr == MetadataFlags6::WRITABLE_ONCE) {
                    itemType.maxTextLength = val;
                    itemType.writable = (attr == MetadataFlags6::WRITABLE);
                    itemType.writableOnce = (attr == MetadataFlags6::WRITABLE_ONCE);
                } else if (attr == MetadataFlags6::MINI_MAP) {
                    itemType.miniMap = true;
                    itemType.miniMapColor = val;
                } else if (attr == MetadataFlags6::CLOTH) {
                    itemType.cloth = true;
                    itemType.clothSlot = val;
                } else if (attr == MetadataFlags6::LENS_HELP) {
                    itemType.isLensHelp = true;
                    itemType.lensHelp = val;
                } else if (attr == MetadataFlags6::USABLE) {
                    itemType.usable = true;
                }
                break;
            }

            case MetadataFlags6::GROUND: {
                uint16_t groundSpeed;
                fin.read(reinterpret_cast<char*>(&groundSpeed), 2);

                itemType.isGround = true;
                itemType.groundSpeed = groundSpeed;
                break;
            }

            case MetadataFlags6::GROUND_BORDER: {
                itemType.isGroundBorder = true;
                break;
            }

            case MetadataFlags6::ON_BOTTOM: {
                itemType.isOnBottom = true;
                break;
            }

            case MetadataFlags6::ON_TOP: {
                itemType.isOnTop = true;
                break;
            }

            case MetadataFlags6::LYING_OBJECT: {
                itemType.isLyingObject = true;
                break;
            }

            case MetadataFlags6::ANIMATE_ALWAYS: {
                itemType.animateAlways = true;
                break;
            }

            case MetadataFlags6::PICKUPABLE: {
                itemType.pickupable = true;
                break;
            }

            case MetadataFlags6::BONES: {
                //std::vector<Point> m_bones;
                //m_bones.resize(4);
                for(int dir = 0; dir < 4; ++dir) {
                    uint16_t x, y;
                    fin.read(reinterpret_cast<char*>(&x), 2);
                    fin.read(reinterpret_cast<char*>(&y), 2);
                    //m_bones[dir] = Point(x, y); // assuming Point(x, y) is a thing
                }
            }

            default: {
                std::cout << "[DatLoader::unserialize] Unknown flag: "
                          << static_cast<int>(attr) << std::endl;
                break;
            }
        }
    }

    if(!done)
        throw std::runtime_error("Couldn't finish unserializing attributes in: " + itemType.clientId);

    // Read sprite data
    itemType.width = fin.get();
    itemType.height = fin.get();
    if(itemType.width > 1 || itemType.height > 1) {
        fin.get(); // realSize
    }
    uint8_t layers = fin.get();
    uint8_t patternX = fin.get();
    uint8_t patternY = fin.get();
    uint8_t patternZ = fin.get();
    uint8_t animationPhases = fin.get();

    if(animationPhases > 1) { // && g_game.getFeature(Otc::GameEnhancedAnimations), which is >= 1050
        uint8_t m_async = fin.get();
        uint32_t m_loopCount = 0;
        fin.read(reinterpret_cast<char*>(&m_loopCount), sizeof(m_loopCount));
        uint8_t m_startPhase = fin.get();

        for(int i = 0; i < animationPhases; ++i) {
            uint32_t minimum = 0;
            fin.read(reinterpret_cast<char*>(&minimum), sizeof(minimum));
            uint32_t maximum = 0;
            fin.read(reinterpret_cast<char*>(&maximum), sizeof(maximum));
        }
    }

    // Skip sprite IDs
    uint32_t spriteCount = itemType.width * itemType.height * layers * patternX * patternY * patternZ * animationPhases;
    if(/* clientVersion >= */ true) { // U32 sprites, extended type shii
        fin.ignore(spriteCount * 4);
    } else {
        fin.ignore(spriteCount * 2);
    }
}