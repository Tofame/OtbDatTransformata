#include <fstream>
#include <iostream>
#include "DatLoader.h"
#include "DatAttributes.h"

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
            case ThingAttrGround: {
                itemType.isGround = true;
                uint16_t groundSpeed;
                fin.read(reinterpret_cast<char*>(&groundSpeed), 2);
                itemType.groundSpeed = groundSpeed;
                break;
            }

            case ThingAttrGroundBorder:
                itemType.isGroundBorder = true;
                break;

            case ThingAttrOnBottom:
                itemType.isOnBottom = true;
                break;

            case ThingAttrOnTop:
                itemType.isOnTop = true;
                break;

            case ThingAttrContainer:
                itemType.isContainer = true;
                break;

            case ThingAttrStackable:
                itemType.stackable = true;
                break;

            case ThingAttrForceUse:
                itemType.forceUse = true;
                break;

            case ThingAttrMultiUse:
                itemType.multiUse = true;
                break;

            case ThingAttrFluidContainer:
                itemType.isFluidContainer = true;
                break;
            case ThingAttrSplash: // fluid (splash)
                itemType.isFluid = true;
                break;

            // ---- blocking / pathing ----
            case ThingAttrNotWalkable:
                itemType.isUnpassable = true;
                itemType.blockSolid   = true;
                break;
            case ThingAttrNotMoveable:
                itemType.isUnmoveable = true;
                itemType.moveable     = false;
                break;
            case ThingAttrBlockProjectile:
                itemType.blockMissile    = true; // blockProjectile
                break;
            case ThingAttrNotPathable:
                itemType.blockPathfind = true;
                break;

            case ThingAttrWritable: {
                itemType.writable = true;
                uint16_t maxLen;
                fin.read(reinterpret_cast<char*>(&maxLen), 2);
                itemType.maxTextLength = maxLen;
                break;
            }

            case ThingAttrWritableOnce: {
                itemType.writableOnce = true;
                uint16_t maxLen;
                fin.read(reinterpret_cast<char*>(&maxLen), 2);
                itemType.maxTextLength = maxLen;
                break;
            }

            case ThingAttrMinimapColor: {
                uint16_t color;
                fin.read(reinterpret_cast<char*>(&color), 2);
                itemType.miniMap = true;
                itemType.miniMapColor = static_cast<uint8_t>(color);
                break;
            }

            case ThingAttrCloth: {
                uint16_t clothSlot;
                fin.read(reinterpret_cast<char*>(&clothSlot), 2);
                itemType.cloth = true;
                itemType.clothSlot = clothSlot;
                break;
            }

            case ThingAttrLensHelp: {
                uint16_t id;
                fin.read(reinterpret_cast<char*>(&id), 2);
                itemType.isLensHelp = true;
                itemType.lensHelp = id;
                break;
            }

            case ThingAttrUsable: {
                uint16_t val;
                fin.read(reinterpret_cast<char*>(&val), 2);
                itemType.usable = true;
                break;
            }

            case ThingAttrPickupable:
                itemType.pickupable = true;
                break;

            case ThingAttrLight: {
                uint16_t intensity, color;
                fin.read(reinterpret_cast<char*>(&intensity), 2);
                fin.read(reinterpret_cast<char*>(&color), 2);
                itemType.hasLight = true;
                itemType.lightLevel = static_cast<uint8_t>(intensity);
                itemType.lightColor = static_cast<uint8_t>(color);
                break;
            }

            case ThingAttrDisplacement: {
                uint16_t dx, dy;
                fin.read(reinterpret_cast<char*>(&dx), 2);
                fin.read(reinterpret_cast<char*>(&dy), 2);
                itemType.hasOffset = true;
                itemType.offsetX = dx;
                itemType.offsetY = dy;
                break;
            }

            case ThingAttrElevation: {
                uint16_t elevation;
                fin.read(reinterpret_cast<char*>(&elevation), 2);
                itemType.hasElevation = true;
                itemType.elevation = elevation;
                break;
            }

            case ThingAttrMarket: {
                itemType.isMarketItem = true;
                uint16_t len;
                fin.read(reinterpret_cast<char*>(&itemType.marketCategory), 2);
                fin.read(reinterpret_cast<char*>(&itemType.marketTradeAs), 2);
                fin.read(reinterpret_cast<char*>(&itemType.marketShowAs), 2);
                fin.read(reinterpret_cast<char*>(&len), 2);

                itemType.marketName.resize(len);
                fin.read(&itemType.marketName[0], len);

                fin.read(reinterpret_cast<char*>(&itemType.marketRestrictProfession), 2);
                fin.read(reinterpret_cast<char*>(&itemType.marketRestrictLevel), 2);
                break;
            }

            case ThingAttrNoMoveAnimation:
                itemType.noMoveAnimation = true;
                break;

            // ---- hangable / hooks / rotation ----
            case ThingAttrHangable:
                itemType.isHangable = true; // you also have `hangable`; set both if you want
                itemType.hangable   = true;
                break;
            case ThingAttrHookSouth:
                itemType.isVertical = true;
                break;
            case ThingAttrHookEast:
                itemType.isHorizontal = true;
                break;
            case ThingAttrRotateable:
                itemType.rotatable = true;
                break;

            // ---- visibility / translucency ----
            case ThingAttrDontHide:
                itemType.dontHide = true;
                break;
            case ThingAttrTranslucent:
                itemType.isTranslucent = true;
                break;

            // ---- corpse/anim ----
            case ThingAttrLyingCorpse:
                itemType.isLyingObject = true;
                break;
            case ThingAttrAnimateAlways:
                itemType.animateAlways = true;
                break;

            // ---- full ground / look ----
            case ThingAttrFullGround:
                itemType.isFullGround = true;
                break;
            case ThingAttrLook:
                itemType.ignoreLook = true;
                break;

            case ThingAttrWrapable:
                itemType.wrappable = true;
                break;

            case ThingAttrUnwrapable:
                itemType.unwrappable = true;
                break;

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
                std::cout << "Notices: bones detected, but rn not supported!\n";
                break;
            }

            case ThingAttrDefaultAction:
                uint8_t defaultaction;
                fin.read(reinterpret_cast<char*>(&defaultaction), 1);
                itemType.hasDefaultAction = true;
                itemType.defaultAction = defaultaction;
                break;

            default:
                std::cout << "[DatLoader::unserialize] Unknown attribute: "
                          << static_cast<int>(attr) << std::dec << "\n";
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