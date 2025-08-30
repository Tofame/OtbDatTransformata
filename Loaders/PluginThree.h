#pragma once

#include "../ItemType.h"
#include "PluginMain.h"

#include <cstdint>

class PluginThree : public PluginMain {
private:

    enum class ItemFlag : std::uint8_t {
        Ground            = 0x00, // 0
        GroundBorder      = 0x01, // 1
        OnBottom          = 0x02, // 2
        OnTop             = 0x03, // 3
        Container         = 0x04, // 4
        Stackable         = 0x05, // 5
        ForceUse          = 0x06, // 6
        MultiUse          = 0x07, // 7
        Writable          = 0x08, // 8
        WritableOnce      = 0x09, // 9
        FluidContainer    = 0x0A, // 10
        Fluid             = 0x0B, // 11
        IsUnpassable      = 0x0C, // 12
        IsUnmoveable      = 0x0D, // 13
        BlockMissiles     = 0x0E, // 14
        BlockPathfinder   = 0x0F, // 15
        NoMoveAnimation   = 0x10, // 16
        Pickupable        = 0x11, // 17
        Hangable          = 0x12, // 18
        IsHorizontal      = 0x13, // 19
        IsVertical        = 0x14, // 20
        Rotatable         = 0x15, // 21
        HasLight          = 0x16, // 22
        DontHide          = 0x17, // 23
        Translucent       = 0x18, // 24
        HasOffset         = 0x19, // 25
        HasElevation      = 0x1A, // 26
        Lying             = 0x1B, // 27
        AnimateAlways     = 0x1C, // 28
        Minimap           = 0x1D, // 29
        LensHelp          = 0x1E, // 30
        FullGround        = 0x1F, // 31
        IgnoreLook        = 0x20, // 32
        Cloth             = 0x21, // 33
        Market            = 0x22, // 34
        DefaultAction     = 0x23, // 35
        Wrappable         = 0x24, // 36
        Unwrappable       = 0x25, // 37
        TopEffect         = 0x26, // 38
        Usable            = 0xFE, // 254

        LastFlag          = 0xFF  // 255
    };

public:
    bool unserializeDatAttribute(ItemType &itemType, int& flagByte, std::ifstream& fin) override;
};