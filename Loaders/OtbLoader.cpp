#include <string>
#include <iostream>
#include "OtbLoader.h"
#include "fileLoader.h"
#include "itemLoader_enums.h"
#include "../tools.h"

constexpr auto OTBI = OTB::Identifier{{'O','T', 'B', 'I'}};

bool OtbLoader::loadFromOtb(const std::string& file, Items& items)
{
    OTB::Loader loader{file, OTBI};

    auto& root = loader.parseTree();

    PropStream props;
    if (loader.getProps(root, props)) {
        //4 byte flags
        //attributes
        //0x01 = version data
        uint32_t flags;
        if (!props.read<uint32_t>(flags)) {
            return false;
        }

        uint8_t attr;
        if (!props.read<uint8_t>(attr)) {
            return false;
        }

        if (attr == ROOT_ATTR_VERSION) {
            uint16_t datalen;
            if (!props.read<uint16_t>(datalen)) {
                return false;
            }

            if (datalen != sizeof(VERSIONINFO)) {
                return false;
            }

            VERSIONINFO vi;
            if (!props.read(vi)) {
                return false;
            }

            majorVersion = vi.dwMajorVersion; //items otb format file version
            minorVersion = vi.dwMinorVersion; //client version
            buildNumber = vi.dwBuildNumber; //revision
        }
    }

    if (majorVersion == 0xFFFFFFFF) {
        std::cout << "[Warning - Items::loadFromOtb] items.otb using generic client version." << std::endl;
    } else if (majorVersion != 3) {
        std::cout << "Old version detected, a newer version of items.otb is required." << std::endl;
        return false;
    } else if (minorVersion < CLIENT_VERSION_1098) {
        std::cout << "A newer version of items.otb is required." << std::endl;
        return false;
    }

    for (auto& itemNode : root.children) {
        PropStream stream;
        if (!loader.getProps(itemNode, stream)) {
            return false;
        }

        uint32_t flags;
        if (!stream.read<uint32_t>(flags)) {
            return false;
        }

        uint16_t serverId = 0;
        uint16_t clientId = 0;
        uint16_t speed = 0;
        uint16_t wareId = 0;
        uint8_t lightLevel = 0;
        uint8_t lightColor = 0;
        uint8_t alwaysOnTopOrder = 0;

        uint8_t attrib;
        while (stream.read<uint8_t>(attrib)) {
            uint16_t datalen;
            if (!stream.read<uint16_t>(datalen)) {
                return false;
            }

            switch (attrib) {
                case ITEM_ATTR_SERVERID: {
                    if (datalen != sizeof(uint16_t)) {
                        return false;
                    }

                    if (!stream.read<uint16_t>(serverId)) {
                        return false;
                    }
                    break;
                }

                case ITEM_ATTR_CLIENTID: {
                    if (datalen != sizeof(uint16_t)) {
                        return false;
                    }

                    if (!stream.read<uint16_t>(clientId)) {
                        return false;
                    }
                    break;
                }

                case ITEM_ATTR_SPEED: {
                    if (datalen != sizeof(uint16_t)) {
                        return false;
                    }

                    if (!stream.read<uint16_t>(speed)) {
                        return false;
                    }
                    break;
                }

                case ITEM_ATTR_LIGHT2: {
                    if (datalen != sizeof(lightBlock2)) {
                        return false;
                    }

                    lightBlock2 lb2;
                    if (!stream.read(lb2)) {
                        return false;
                    }

                    lightLevel = static_cast<uint8_t>(lb2.lightLevel);
                    lightColor = static_cast<uint8_t>(lb2.lightColor);
                    break;
                }

                case ITEM_ATTR_TOPORDER: {
                    if (datalen != sizeof(uint8_t)) {
                        return false;
                    }

                    if (!stream.read<uint8_t>(alwaysOnTopOrder)) {
                        return false;
                    }
                    break;
                }

                case ITEM_ATTR_WAREID: {
                    if (datalen != sizeof(uint16_t)) {
                        return false;
                    }

                    if (!stream.read<uint16_t>(wareId)) {
                        return false;
                    }
                    break;
                }

                case ITEM_ATTR_CLASSIFICATION: {
                    if (!stream.skip(1)) {
                        return false;
                    }
                    break;
                }

                default: {
                    //skip unknown attributes
                    if (!stream.skip(datalen)) {
                        return false;
                    }
                    break;
                }
            }
        }

        items.getCidToSidMap().emplace(clientId, serverId);

        // store the found item
        if (serverId >= items.getItemTypes().size()) {
            items.getItemTypes().resize(serverId + 1);
        }
        ItemType& iType = items.getItemTypes()[serverId];

        iType.group = static_cast<itemgroup_t>(itemNode.type);
        switch (itemNode.type) {
            case ITEM_GROUP_CONTAINER:
                iType.type = ITEM_TYPE_CONTAINER;
                break;
            case ITEM_GROUP_DOOR:
                //not used
                iType.type = ITEM_TYPE_DOOR;
                break;
            case ITEM_GROUP_MAGICFIELD:
                //not used
                iType.type = ITEM_TYPE_MAGICFIELD;
                break;
            case ITEM_GROUP_TELEPORT:
                //not used
                iType.type = ITEM_TYPE_TELEPORT;
                break;
            case ITEM_GROUP_NONE:
            case ITEM_GROUP_GROUND:
            case ITEM_GROUP_SPLASH:
            case ITEM_GROUP_FLUID:
            case ITEM_GROUP_CHARGES:
            case ITEM_GROUP_DEPRECATED:
            case ITEM_GROUP_PODIUM:
                break;
            default:
                return false;
        }

        iType.blockSolid = hasBitSet(FLAG_BLOCK_SOLID, flags);
        iType.blockProjectile = hasBitSet(FLAG_BLOCK_PROJECTILE, flags);
        iType.blockPathFind = hasBitSet(FLAG_BLOCK_PATHFIND, flags);
        iType.hasHeight = hasBitSet(FLAG_HAS_HEIGHT, flags);
        iType.useable = hasBitSet(FLAG_USEABLE, flags);
        iType.pickupable = hasBitSet(FLAG_PICKUPABLE, flags);
        iType.moveable = hasBitSet(FLAG_MOVEABLE, flags);
        iType.stackable = hasBitSet(FLAG_STACKABLE, flags);

        iType.alwaysOnTop = hasBitSet(FLAG_ALWAYSONTOP, flags);
        iType.isVertical = hasBitSet(FLAG_VERTICAL, flags);
        iType.isHorizontal = hasBitSet(FLAG_HORIZONTAL, flags);
        iType.isHangable = hasBitSet(FLAG_HANGABLE, flags);
        iType.allowDistRead = hasBitSet(FLAG_ALLOWDISTREAD, flags);
        iType.rotatable = hasBitSet(FLAG_ROTATABLE, flags);
        iType.canReadText = hasBitSet(FLAG_READABLE, flags);
        iType.lookThrough = hasBitSet(FLAG_LOOKTHROUGH, flags);
        iType.isAnimation = hasBitSet(FLAG_ANIMATION, flags);
        // iType.walkStack = !hasBitSet(FLAG_FULLTILE, flags);
        iType.forceUse = hasBitSet(FLAG_FORCEUSE, flags);

        iType.id = serverId;
        iType.clientId = clientId;
        iType.speed = speed;
        iType.lightLevel = lightLevel;
        iType.lightColor = lightColor;
        iType.wareId = wareId;
        iType.alwaysOnTopOrder = alwaysOnTopOrder;
    }

    items.getItemTypes().shrink_to_fit();
    return true;
}