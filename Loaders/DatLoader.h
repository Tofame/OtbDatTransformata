#pragma once

#include <string>
#include "../Items.h"

class DatLoader
{
    public:
        DatLoader() = default;
        ~DatLoader() = default;

        bool loadFromDat(const std::string& file, Items& items);
        void unserialize(ItemType& itemType, uint16_t clientId, std::ifstream& fin);

    uint32_t m_datSignature = 0;
    uint16_t m_loadedItemsCount = 0;
    uint16_t m_loadedOutfitsCount = 0;
    uint16_t m_loadedEffectsCount = 0;
    uint16_t m_loadedMissilesCount = 0;

    // Returns everything leftover after items from .dat
    std::vector<char> getBytesOfTheRestOfDat() {
        return m_rawRest;
    }
private:
    bool m_datLoaded = false;
    std::vector<char> m_rawRest;
};