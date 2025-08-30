#include "PluginThree.h"

bool PluginThree::unserializeDatAttribute(ItemType &itemType, int& flagByte, std::ifstream& fin)
{
    ItemFlag flag = (ItemFlag)flagByte;

    switch (flag)
    {
        case ItemFlag::Ground: {
            itemType.isGround = true;
            uint16_t groundSpeed;
            fin.read(reinterpret_cast<char*>(&groundSpeed), 2);
            itemType.groundSpeed = groundSpeed;
            break;
        }

        case ItemFlag::GroundBorder: {
            itemType.isGroundBorder = true;
            break;
        }

        case ItemFlag::OnBottom: {
            itemType.isOnBottom = true;
            break;
        }

        case ItemFlag::OnTop: {
            itemType.isOnTop = true;
            break;
        }

        case ItemFlag::Container:
            itemType.isContainer = true;
            break;

        case ItemFlag::Stackable:
            itemType.stackable = true;
            break;

        case ItemFlag::ForceUse:
            itemType.forceUse = true;
            break;

        case ItemFlag::MultiUse:
            itemType.multiUse = true;
            break;

        case ItemFlag::Writable: {
            itemType.writable = true;
            uint16_t maxLen;
            fin.read(reinterpret_cast<char*>(&maxLen), 2);
            itemType.maxTextLength = maxLen;
            break;
        }

        case ItemFlag::WritableOnce: {
            itemType.writableOnce = true;
            uint16_t maxLen;
            fin.read(reinterpret_cast<char *>(&maxLen), 2);
            itemType.maxTextLength = maxLen;
            break;
        }

        case ItemFlag::FluidContainer:
            itemType.isFluidContainer = true;
            break;

        case ItemFlag::Fluid: // fluid (splash)
            itemType.isFluid = true;
            break;

        case ItemFlag::IsUnpassable:
            itemType.isUnpassable = true;
            itemType.blockSolid   = true;
            break;

        case ItemFlag::IsUnmoveable:
            itemType.isUnmoveable = true;
            itemType.moveable     = false;
            break;

        case ItemFlag::BlockMissiles:
            itemType.blockMissile    = true; // blockProjectile
            break;

        case ItemFlag::BlockPathfinder:
            itemType.blockPathfind = true;
            break;

        case ItemFlag::NoMoveAnimation:
            // item.noMoveAnimation = true;
            break;

        case ItemFlag::Pickupable:
            itemType.pickupable = true;
            break;

        case ItemFlag::Hangable:
            itemType.hangable = true;
            break;

        case ItemFlag::IsHorizontal:
            itemType.isHorizontal = true;
            break;

        case ItemFlag::IsVertical:
            itemType.isVertical = true;
            break;

        case ItemFlag::Rotatable:
            itemType.rotatable = true;
            break;

        case ItemFlag::HasLight: {
            uint16_t intensity, color;
            fin.read(reinterpret_cast<char *>(&intensity), 2);
            fin.read(reinterpret_cast<char *>(&color), 2);
            itemType.hasLight = true;
            itemType.lightLevel = static_cast<uint8_t>(intensity);
            itemType.lightColor = static_cast<uint8_t>(color);
            break;
        }

        case ItemFlag::DontHide:
            break;

        case ItemFlag::Translucent:
            break;

        case ItemFlag::HasOffset: {
            uint16_t dx, dy;
            fin.read(reinterpret_cast<char *>(&dx), 2);
            fin.read(reinterpret_cast<char *>(&dy), 2);
            itemType.hasOffset = true;
            itemType.offsetX = dx;
            itemType.offsetY = dy;
            break;
        }

        case ItemFlag::HasElevation: {
            uint16_t elevation;
            fin.read(reinterpret_cast<char *>(&elevation), 2);
            itemType.hasElevation = true;
            itemType.elevation = elevation;
            break;
        }

        case ItemFlag::Lying:
            break;

        case ItemFlag::AnimateAlways:
            break;

        case ItemFlag::Minimap: {
            uint16_t color;
            fin.read(reinterpret_cast<char *>(&color), 2);
            itemType.miniMap = true;
            itemType.miniMapColor = static_cast<uint8_t>(color);
            break;
        }

        case ItemFlag::LensHelp: {
            uint16_t id;
            fin.read(reinterpret_cast<char *>(&id), 2);
            itemType.isLensHelp = true;
            itemType.lensHelp = id;
            break;
        }

        case ItemFlag::FullGround:
            itemType.isFullGround = true;
            break;

        case ItemFlag::IgnoreLook:
            itemType.ignoreLook = true;
            break;

        case ItemFlag::Cloth: {
            uint16_t clothSlot;
            fin.read(reinterpret_cast<char*>(&clothSlot), 2);
            itemType.cloth = true;
            itemType.clothSlot = clothSlot;
            break;
        }

        case ItemFlag::Market: {
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

        case ItemFlag::DefaultAction: {
            uint16_t defaultaction;
            fin.read(reinterpret_cast<char *>(&defaultaction), 2);
            itemType.hasDefaultAction = true;
            itemType.defaultAction = defaultaction;
            break;
        }

        case ItemFlag::Wrappable:
            itemType.wrappable = true;
            break;

        case ItemFlag::Unwrappable:
            itemType.unwrappable = true;
            break;

        case ItemFlag::TopEffect:
            break;

        case ItemFlag::Usable: {
            itemType.usable = true;
            break;
        }

        case ItemFlag::LastFlag:
            break;

        default:
            return false;
    }

    return true;
}