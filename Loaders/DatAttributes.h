#pragma once

#include <cstdint>

enum ThingCategory : uint8_t {
    ThingCategoryItem = 0,
    ThingCategoryCreature,
    ThingCategoryEffect,
    ThingCategoryMissile,
    ThingInvalidCategory,
    ThingLastCategory = ThingInvalidCategory
};

enum ThingAttr : uint8_t {
    ThingAttrGround           = 0,
    ThingAttrGroundBorder     = 1,
    ThingAttrOnBottom         = 2,
    ThingAttrOnTop            = 3,
    ThingAttrContainer        = 4,
    ThingAttrStackable        = 5,
    ThingAttrForceUse         = 6,
    ThingAttrMultiUse         = 7,
    ThingAttrWritable         = 8,
    ThingAttrWritableOnce     = 9,
    ThingAttrFluidContainer   = 10,
    ThingAttrSplash           = 11,
    ThingAttrNotWalkable      = 12,
    ThingAttrNotMoveable      = 13,
    ThingAttrBlockProjectile  = 14,
    ThingAttrNotPathable      = 15,
    ThingAttrPickupable       = 16,
    ThingAttrHangable         = 17,
    ThingAttrHookSouth        = 18,
    ThingAttrHookEast         = 19,
    ThingAttrRotateable       = 20,
    ThingAttrLight            = 21,
    ThingAttrDontHide         = 22,
    ThingAttrTranslucent      = 23,
    ThingAttrDisplacement     = 24,
    ThingAttrElevation        = 25,
    ThingAttrLyingCorpse      = 26,
    ThingAttrAnimateAlways    = 27,
    ThingAttrMinimapColor     = 28,
    ThingAttrLensHelp         = 29,
    ThingAttrFullGround       = 30,
    ThingAttrLook             = 31,
    ThingAttrCloth            = 32,
    ThingAttrMarket           = 33,
    ThingAttrUsable           = 34,
    ThingAttrWrapable         = 35,
    ThingAttrUnwrapable       = 36,
    ThingAttrBottomEffect     = 37,
    ThingAttrBones            = 38,
    ThingAttrDontCenterOutfit = 39,

    // additional
    ThingAttrOpacity          = 100,
    ThingAttrNotPreWalkable   = 101,

    ThingAttrDefaultAction = 251,

    ThingAttrFloorChange      = 252,
    ThingAttrNoMoveAnimation  = 253, // 10.10: real value is 16, but we need to do this for backwards compatibility
    ThingAttrChargeable       = 254, // deprecated
    ThingLastAttr             = 255
};