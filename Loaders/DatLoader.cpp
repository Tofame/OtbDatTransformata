#include <fstream>
#include <iostream>
#include "DatLoader.h"
#include "DatAttributes.h"

bool DatLoader::loadFromDat(const std::string& file, Items& items) {
    m_datLoaded = false; // Start by assuming the load will fail
    m_datSignature = 0;
    m_contentRevision = 0;

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
        auto &itemTypesDat = items.getItemTypesDat();
        itemTypesDat.resize(itemCount + 1);

        // .dat items start at 100
        uint16_t firstId = 100;

        for (uint16_t id = firstId; id < itemTypesDat.size(); ++id) {
            auto type = ItemType();
            unserialize(type, id, fin);
            itemTypesDat[id] = type;
        }

        m_datLoaded = true;
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error loading DAT file: " << e.what() << std::endl;
        return false;
    }
}

void DatLoader::unserialize(ItemType& itemType, uint16_t cid, std::ifstream& fin) {
    itemType.clientId = cid;

    int count = 0, attr = -1;
    bool done = false;
    for(int i = 0; i < ThingLastAttr; ++i) {
        count++;
        attr = fin.get();
        if(attr == ThingLastAttr) {
            done = true;
            break;
        }

        // if(g_game.getClientVersion() >= 1000) {
        if(attr == 16)
            attr = ThingAttrNoMoveAnimation;
        else if(attr > 16)
            attr -= 1;
        //} else if (g_game.getClientVersion() >= 780) {
        // other stuff for other protocols...

        switch(attr) {
            case ThingAttrDisplacement: {
                uint16_t m_displacementX;
                uint16_t m_displacementY;
                fin.read(reinterpret_cast<char*>(&m_displacementX), 2);
                fin.read(reinterpret_cast<char*>(&m_displacementY), 2);
                // m_attribs.set(attr, true); // or store x/y if needed
                break;
            }
            case ThingAttrLight: {
                //Light light;
                uint16_t intensity, color;
                fin.read(reinterpret_cast<char*>(&intensity), 2);
                fin.read(reinterpret_cast<char*>(&color), 2);
//                light.intensity = intensity;
//                light.color = color;
//                m_attribs.set(attr, light);
                break;
            }
            case ThingAttrMarket: {
                // Move it at later, it's 1:30AM...
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

                //m_attribs.set(attr, market);
                break;
            }
            case ThingAttrElevation: {
                uint16_t elevation;
                fin.read(reinterpret_cast<char*>(&elevation), 2);
                //m_elevation = elevation;
                //m_attribs.set(attr, elevation);
                break;
            }
            case ThingAttrUsable:
            case ThingAttrGround:
            case ThingAttrWritable:
            case ThingAttrWritableOnce:
            case ThingAttrMinimapColor:
            case ThingAttrCloth:
            case ThingAttrLensHelp: {
                uint16_t val;
                fin.read(reinterpret_cast<char*>(&val), 2);
                //m_attribs.set(attr, val);
                break;
            }
            case ThingAttrBones: {
                //std::vector<Point> m_bones;
                //m_bones.resize(4);
                for(int dir = 0; dir < 4; ++dir) {
                    uint16_t x, y;
                    fin.read(reinterpret_cast<char*>(&x), 2);
                    fin.read(reinterpret_cast<char*>(&y), 2);
                    //m_bones[dir] = Point(x, y); // assuming Point(x, y) is a thing
                }
                //m_attribs.set(attr, true);
                break;
            }

            // Custom, quickly, just to prove, TO-DO make m_attribs.set
            case ThingAttrPickupable:
                itemType.pickupable = true;
                break;
                //m_attribs.set(attr, true);
            default:
                if(not
                    ((attr >= ThingAttrGround and attr <= ThingAttrDontCenterOutfit) or
                    (attr >= ThingAttrOpacity and attr <= ThingAttrNotPreWalkable) or
                    (attr >= ThingAttrDefaultAction and attr <=ThingLastAttr)))
                {
                    std::cout << "[DatLoader::unserialize] Unknown attribute spotted: " << static_cast<int>(attr) << std::endl;
                }
                break;
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