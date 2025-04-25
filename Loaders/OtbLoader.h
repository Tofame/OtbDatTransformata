#pragma once

#include <vector>
#include "../Items.h"

class OtbLoader
{
    uint32_t majorVersion = 0;
    uint32_t minorVersion = 0;
    uint32_t buildNumber = 0;

    public:
        bool loadFromOtb(const std::string& file, Items& items);
};