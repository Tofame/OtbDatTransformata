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
private:
    bool m_datLoaded = false;
    uint32_t m_datSignature = 0;
    uint16_t m_contentRevision = 0;
};