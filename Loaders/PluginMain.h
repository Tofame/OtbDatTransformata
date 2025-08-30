#pragma once

#include "../ItemType.h"

class PluginMain {
public:
    inline static int LastFlag = 0xFF;

    virtual bool unserializeDatAttribute(ItemType &itemType, int& flagByte, std::ifstream& fin) {
        std::cout << "[ERROR::unserializeDatItem] Use descendants of PluginMain\n";
        return false;
    };
};