#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>

class MetadataFlags6 final {
public:
    MetadataFlags6() = delete; // Abstract class equivalent

    // Flags
    static constexpr uint8_t GROUND = 0x00;
    static constexpr uint8_t GROUND_BORDER = 0x01;
    static constexpr uint8_t ON_BOTTOM = 0x02;
    static constexpr uint8_t ON_TOP = 0x03;
    static constexpr uint8_t CONTAINER = 0x04;
    static constexpr uint8_t STACKABLE = 0x05;
    static constexpr uint8_t FORCE_USE = 0x06;
    static constexpr uint8_t MULTI_USE = 0x07;
    static constexpr uint8_t WRITABLE = 0x08;
    static constexpr uint8_t WRITABLE_ONCE = 0x09;
    static constexpr uint8_t FLUID_CONTAINER = 0x0A;
    static constexpr uint8_t FLUID = 0x0B;
    static constexpr uint8_t UNPASSABLE = 0x0C;
    static constexpr uint8_t UNMOVEABLE = 0x0D;
    static constexpr uint8_t BLOCK_MISSILE = 0x0E;
    static constexpr uint8_t BLOCK_PATHFIND = 0x0F;
    static constexpr uint8_t NO_MOVE_ANIMATION = 0x10;
    static constexpr uint8_t PICKUPABLE = 0x11;
    static constexpr uint8_t HANGABLE = 0x12;
    static constexpr uint8_t VERTICAL = 0x13;
    static constexpr uint8_t HORIZONTAL = 0x14;
    static constexpr uint8_t ROTATABLE = 0x15;
    static constexpr uint8_t HAS_LIGHT = 0x16;
    static constexpr uint8_t DONT_HIDE = 0x17;
    static constexpr uint8_t TRANSLUCENT = 0x18;
    static constexpr uint8_t HAS_OFFSET = 0x19;
    static constexpr uint8_t HAS_ELEVATION = 0x1A;
    static constexpr uint8_t LYING_OBJECT = 0x1B;
    static constexpr uint8_t ANIMATE_ALWAYS = 0x1C;
    static constexpr uint8_t MINI_MAP = 0x1D;
    static constexpr uint8_t LENS_HELP = 0x1E;
    static constexpr uint8_t FULL_GROUND = 0x1F;
    static constexpr uint8_t IGNORE_LOOK = 0x20;
    static constexpr uint8_t CLOTH = 0x21;
    static constexpr uint8_t MARKET_ITEM = 0x22;
    static constexpr uint8_t DEFAULT_ACTION = 0x23;
    static constexpr uint8_t WRAPPABLE = 0x24;
    static constexpr uint8_t UNWRAPPABLE = 0x25;
    static constexpr uint8_t BOTTOM_EFFECT = 0x26;
    static constexpr uint8_t USABLE = 0xFE;
    static constexpr uint8_t BONES = 0X27; // custom
    static constexpr uint8_t LAST_FLAG = 0xFF;

    static constexpr const char* STRING_CHARSET = "iso-8859-1";
};