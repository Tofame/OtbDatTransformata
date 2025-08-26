#pragma once

#include "MetadataWriter.h"
#include "MetadataFlags6.h"
#include "ThingCategory.h"
#include "../ItemType.h"
#include "../Loaders/fileloader.h"

class MetadataWriter6 : public MetadataWriter
{
public:
    MetadataWriter6() = default;

    bool writeProperties(const ItemType& type, PropWriteStream& stream) override
    {
        // Right now we only save items anyway.
        if(true) {
            return false;
        }
//        if (type.category == ThingCategory::ITEM)
//            return false;

        if (type.hasLight) {
            stream.write<uint8_t>(MetadataFlags6::HAS_LIGHT);
            stream.write<uint16_t>(type.lightLevel);
            stream.write<uint16_t>(type.lightColor);
        }

        if (type.hasOffset) {
            stream.write<uint8_t>(MetadataFlags6::HAS_OFFSET);
            stream.write<uint16_t>(type.offsetX);
            stream.write<uint16_t>(type.offsetY);
        }

        if (type.animateAlways)
            stream.write<uint8_t>(MetadataFlags6::ANIMATE_ALWAYS);

// For effects, but right now we only need items compiled
//        if (type.bottomEffect && type.category == ThingCategory::EFFECT)
//            stream.write<uint8_t>(MetadataFlags6::BOTTOM_EFFECT);

        stream.write<uint8_t>(MetadataFlags6::LAST_FLAG);
        return true;
    }

    bool writeItemProperties(const ItemType& type, PropWriteStream& stream) override
    {
        // Right now we only save items anyway.
//        if (type.category != ThingCategory::ITEM)
//            return false;

        if (type.isGround) {
            stream.write<uint8_t>(MetadataFlags6::GROUND);
            stream.write<uint16_t>(type.groundSpeed);
        } else if (type.isGroundBorder)
            stream.write<uint8_t>(MetadataFlags6::GROUND_BORDER);
        else if (type.isOnBottom)
            stream.write<uint8_t>(MetadataFlags6::ON_BOTTOM);
        else if (type.isOnTop)
            stream.write<uint8_t>(MetadataFlags6::ON_TOP);

        if (type.isContainer)
            stream.write<uint8_t>(MetadataFlags6::CONTAINER);

        if (type.stackable)
            stream.write<uint8_t>(MetadataFlags6::STACKABLE);

        if (type.forceUse)
            stream.write<uint8_t>(MetadataFlags6::FORCE_USE);

        if (type.multiUse)
            stream.write<uint8_t>(MetadataFlags6::MULTI_USE);

        if (type.writable) {
            stream.write<uint8_t>(MetadataFlags6::WRITABLE);
            stream.write<uint16_t>(type.maxTextLength);
        }

        if (type.writableOnce) {
            stream.write<uint8_t>(MetadataFlags6::WRITABLE_ONCE);
            stream.write<uint16_t>(type.maxTextLength);
        }

        if (type.isFluidContainer)
            stream.write<uint8_t>(MetadataFlags6::FLUID_CONTAINER);

        if (type.isFluid)
            stream.write<uint8_t>(MetadataFlags6::FLUID);

        if (type.isUnpassable)
            stream.write<uint8_t>(MetadataFlags6::UNPASSABLE);

        if (type.isUnmoveable)
            stream.write<uint8_t>(MetadataFlags6::UNMOVEABLE);

        if (type.blockMissile)
            stream.write<uint8_t>(MetadataFlags6::BLOCK_MISSILE);

        if (type.blockPathfind)
            stream.write<uint8_t>(MetadataFlags6::BLOCK_PATHFIND);

        if (type.noMoveAnimation)
            stream.write<uint8_t>(MetadataFlags6::NO_MOVE_ANIMATION);

        if (type.pickupable)
            stream.write<uint8_t>(MetadataFlags6::PICKUPABLE);

        if (type.hangable)
            stream.write<uint8_t>(MetadataFlags6::HANGABLE);

        if (type.isVertical)
            stream.write<uint8_t>(MetadataFlags6::VERTICAL);

        if (type.isHorizontal)
            stream.write<uint8_t>(MetadataFlags6::HORIZONTAL);

        if (type.rotatable)
            stream.write<uint8_t>(MetadataFlags6::ROTATABLE);

        if (type.hasLight) {
            stream.write<uint8_t>(MetadataFlags6::HAS_LIGHT);
            stream.write<uint16_t>(type.lightLevel);
            stream.write<uint16_t>(type.lightColor);
        }

        if (type.dontHide)
            stream.write<uint8_t>(MetadataFlags6::DONT_HIDE);

        if (type.isTranslucent)
            stream.write<uint8_t>(MetadataFlags6::TRANSLUCENT);

        if (type.hasOffset) {
            stream.write<uint8_t>(MetadataFlags6::HAS_OFFSET);
            stream.write<uint16_t>(type.offsetX);
            stream.write<uint16_t>(type.offsetY);
        }

        if (type.hasElevation) {
            stream.write<uint8_t>(MetadataFlags6::HAS_ELEVATION);
            stream.write<uint16_t>(type.elevation);
        }

        if (type.isLyingObject)
            stream.write<uint8_t>(MetadataFlags6::LYING_OBJECT);

        if (type.animateAlways)
            stream.write<uint8_t>(MetadataFlags6::ANIMATE_ALWAYS);

        if (type.miniMap) {
            stream.write<uint8_t>(MetadataFlags6::MINI_MAP);
            stream.write<uint16_t>(type.miniMapColor);
        }

        if (type.isLensHelp) {
            stream.write<uint8_t>(MetadataFlags6::LENS_HELP);
            stream.write<uint16_t>(type.lensHelp);
        }

        if (type.isFullGround)
            stream.write<uint8_t>(MetadataFlags6::FULL_GROUND);

        if (type.ignoreLook)
            stream.write<uint8_t>(MetadataFlags6::IGNORE_LOOK);

        if (type.cloth) {
            stream.write<uint8_t>(MetadataFlags6::CLOTH);
            stream.write<uint16_t>(type.clothSlot);
        }

        if (type.isMarketItem) {
            stream.write<uint8_t>(MetadataFlags6::MARKET_ITEM);
            stream.write<uint16_t>(type.marketCategory);
            stream.write<uint16_t>(type.marketTradeAs);
            stream.write<uint16_t>(type.marketShowAs);
            stream.write<uint16_t>(static_cast<uint16_t>(type.marketName.size()));
            stream.writeString(type.marketName);
            stream.write<uint16_t>(type.marketRestrictProfession);
            stream.write<uint16_t>(type.marketRestrictLevel);
        }

        if (type.hasDefaultAction) {
            stream.write<uint8_t>(MetadataFlags6::DEFAULT_ACTION);
            stream.write<uint16_t>(type.defaultAction);
        }

        if (type.wrappable)
            stream.write<uint8_t>(MetadataFlags6::WRAPPABLE);

        if (type.unwrappable)
            stream.write<uint8_t>(MetadataFlags6::UNWRAPPABLE);

        if (type.usable)
            stream.write<uint8_t>(MetadataFlags6::USABLE);

        stream.write<uint8_t>(MetadataFlags6::LAST_FLAG);
        return true;
    }
};