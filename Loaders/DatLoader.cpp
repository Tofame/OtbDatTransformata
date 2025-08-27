#include <fstream>
#include <iostream>
#include "DatLoader.h"
#include "DatAttributes.h"
#include "../globals.h"

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
        fin.read(reinterpret_cast<char *>(&m_loadedItemsCount), sizeof(m_loadedItemsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedOutfitsCount), sizeof(m_loadedOutfitsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedEffectsCount), sizeof(m_loadedEffectsCount));
        fin.read(reinterpret_cast<char *>(&m_loadedMissilesCount), sizeof(m_loadedMissilesCount));

        // Load items
        auto &itemTypesDat = items.getItemTypesDat();
        itemTypesDat.resize(m_loadedItemsCount + 1);
        uint16_t firstId = g_MIN_ITEM_ID; // .dat items start at 100

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

        if(g_protocolVersion >= 1000) {
            /* In 10.10+ all attributes from 16 and up were
             * incremented by 1 to make space for 16 as
             * "No Movement Animation" flag.
             */
            if(attr == 16)
                attr = ThingAttrNoMoveAnimation;
            else if(attr > 16)
                attr -= 1;
        } else if(g_protocolVersion >= 860) {
            /* Default attribute values follow
             * the format of 8.6-9.86.
             * Therefore no changes here.
             */
        } else {
            throw std::runtime_error("DatLoader::unserialize - no protocols below 8.6 are supported, change globals.h, or add support.");
        }

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
            frameGroup.exactSize = g_SpritesExactSize;
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
        const size_t bytesToRead = totalSprites * (g_extended ? sizeof(uint32_t) : sizeof(uint16_t));
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
            if (g_extended) {
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