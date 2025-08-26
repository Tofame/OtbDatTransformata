#pragma once

#include <cstdint>
#include <string>
#include "Loaders/itemLoader_enums.h"
#include <fstream>

enum SlotPositionBits : uint32_t {
    SLOTP_WHEREEVER = 0xFFFFFFFF,
    SLOTP_HEAD = 1 << 0,
    SLOTP_NECKLACE = 1 << 1,
    SLOTP_BACKPACK = 1 << 2,
    SLOTP_ARMOR = 1 << 3,
    SLOTP_RIGHT = 1 << 4,
    SLOTP_LEFT = 1 << 5,
    SLOTP_LEGS = 1 << 6,
    SLOTP_FEET = 1 << 7,
    SLOTP_RING = 1 << 8,
    SLOTP_AMMO = 1 << 9,
    SLOTP_DEPOT = 1 << 10,
    SLOTP_TWO_HAND = 1 << 11,
    SLOTP_HAND = (SLOTP_LEFT | SLOTP_RIGHT)
};

enum ItemTypes_t {
    ITEM_TYPE_NONE,
    ITEM_TYPE_DEPOT,
    ITEM_TYPE_MAILBOX,
    ITEM_TYPE_TRASHHOLDER,
    ITEM_TYPE_CONTAINER,
    ITEM_TYPE_DOOR,
    ITEM_TYPE_MAGICFIELD,
    ITEM_TYPE_TELEPORT,
    ITEM_TYPE_BED,
    ITEM_TYPE_KEY,
    ITEM_TYPE_RUNE,
    ITEM_TYPE_REWARDCHEST,
    ITEM_TYPE_REWARDCONTAINER,
    ITEM_TYPE_LAST
};

class ItemType
{
public:
    ItemType() = default;

    //non-copyable
    ItemType(const ItemType& other) = delete;
        // delete -> default, as I want to assign them while loading .dat
    ItemType& operator=(const ItemType& other) = default;

    ItemType(ItemType&& other) = default;
    ItemType& operator=(ItemType&& other) = default;

    itemgroup_t group = ITEM_GROUP_NONE;
    ItemTypes_t type = ITEM_TYPE_NONE;
    uint16_t id = 0;
    uint16_t clientId = 0;
    bool stackable = false;
    bool isAnimation = false;

    std::string name;
    std::string article;
    std::string pluralName;
    std::string description;
    std::string classification;
    std::string tier;
    std::string runeSpellName;
    std::string vocationString;

//    std::unique_ptr<Abilities> abilities;
//    std::unique_ptr<ConditionDamage> conditionDamage;
//    std::unordered_set<std::string> augments;

    uint32_t attackSpeed = 0;
    uint32_t weight = 0;
    uint32_t levelDoor = 0;
    uint32_t decayTime = 0;
    uint32_t wieldInfo = 0;
    uint32_t minReqLevel = 0;
    uint32_t minReqMagicLevel = 0;
    uint32_t charges = 0;
    int32_t maxHitChance = -1;
    int32_t decayTo = -1;
    int32_t attack = 0;
    int32_t defense = 0;
    int32_t extraDefense = 0;
    int32_t armor = 0;
    uint16_t rotateTo = 0;
    int32_t runeMagLevel = 0;
    int32_t runeLevel = 0;
    uint64_t worth = 0;

    uint16_t transformToOnUse[2] = {0, 0};
    uint16_t transformToFree = 0;
    uint16_t destroyTo = 0;
    uint16_t maxTextLength = 0;
    uint16_t writeOnceItemId = 0;
    uint16_t transformEquipTo = 0;
    uint16_t transformDeEquipTo = 0;
    uint16_t maxItems = 8;
    uint16_t slotPosition = SLOTP_HAND;
    uint16_t equipSlot = SLOTP_HAND;
    uint16_t speed = 0;
    uint16_t wareId = 0;
    uint16_t imbuementslots = 0;

//    MagicEffectClasses magicEffect = CONST_ME_NONE;
//    Direction bedPartnerDir = DIRECTION_NONE;
//    WeaponType_t weaponType = WEAPON_NONE;
//    Ammo_t ammoType = AMMO_NONE;
//    ShootType_t shootType = CONST_ANI_NONE;
//    RaceType_t corpseType = RACE_NONE;
//    FluidTypes_t fluidSource = FLUID_NONE;

    uint8_t floorChange = 0;
    uint8_t alwaysOnTopOrder = 0;
    uint8_t lightLevel = 0;
    uint8_t lightColor = 0;
    uint8_t shootRange = 1;
    int8_t hitChance = 0;

    bool storeItem = false;
    bool forceUse = false;
    bool forceSerialize = false;
    bool hasHeight = false;
    bool walkStack = true;
    bool blockSolid = false;
    bool blockPickupable = false;
    bool blockProjectile = false;
    bool blockPathFind = false;
    bool allowPickupable = false;
    bool showDuration = false;
    bool showCharges = false;
    bool showAttributes = false;
    bool replaceable = true;
    bool pickupable = false;
    bool rotatable = false;
    bool useable = false;
    bool moveable = false;
    bool alwaysOnTop = false;
    bool canReadText = false;
    bool canWriteText = false;
    bool isVertical = false;
    bool isHorizontal = false;
    bool isHangable = false;
    bool allowDistRead = false;
    bool lookThrough = false;
    bool stopTime = false;
    bool showCount = true;

    // Added to make it possible to compile .dat based on items.
    bool hasLight = false;
    bool hasOffset = false;
    uint16_t offsetX = 0;
    uint16_t offsetY = 0;
    bool animateAlways = false;
    bool isGround = false;
    bool isGroundBorder = false;
    uint16_t groundSpeed = 0;
    bool isOnBottom = false;
    bool isOnTop = false;
    bool isContainer = false;
    bool isFluidContainer = false;
    bool isFluid = false;
    bool isUnpassable = false;
    bool isUnmoveable = false;
    bool blockMissile = false;
    bool blockPathfind = false;
    bool noMoveAnimation = false;
    bool isTranslucent = false;
    bool hasElevation = false;
    uint16_t elevation = 0;
    bool isLyingObject = false;
    bool isFullGround = false;
    bool ignoreLook = false;
    bool cloth = false;
    uint16_t clothSlot = 0;
    bool isMarketItem = false;
    uint16_t marketCategory = 0;
    uint16_t marketTradeAs = 0;
    uint16_t marketShowAs = 0;
    std::string marketName;
    uint16_t marketRestrictProfession = 0;
    uint16_t marketRestrictLevel = 0;
    bool hasDefaultAction = false;
    uint16_t defaultAction = 0;
    bool wrappable = false;
    bool unwrappable = false;
    bool bottomEffect = false;
    bool multiUse = false;
    bool writable = false;
    bool writableOnce = false;
    bool hangable = false;
    bool dontHide = false;
    bool miniMap = false;
    uint8_t miniMapColor = 0;
    bool isLensHelp = false;
    uint16_t lensHelp = 0;
    bool usable = false;

    // Dat properties
    uint8_t height;
    uint8_t width;
};
