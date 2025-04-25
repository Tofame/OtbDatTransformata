#include <fstream>
#include <iostream>
#include "DatLoader.h"

bool DatLoader::loadFromDat(const std::string& file, Items& items)
{
    m_datLoaded = false; // Start by assuming the load will fail
    m_datSignature = 0;
    m_contentRevision = 0;

    try {
        // Otcv8 was 'guessing' the path here, we just tell it in the param
        std::string datFile = file;

        std::ifstream fin(datFile, std::ios::binary);
        if (!fin.is_open()) {
            throw std::runtime_error("Failed to open DAT file: " + datFile);
        }

        // Read signature and revision
        fin.read(reinterpret_cast<char*>(&m_datSignature), sizeof(m_datSignature));
        m_contentRevision = static_cast<uint16_t>(m_datSignature);

        // Read the item count, outfit count, effect count, missiles count (distance effects)
        uint16_t itemCount = 0, outfitCount = 0, effectCount = 0, distanceCount = 0;
        fin.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
        fin.read(reinterpret_cast<char*>(&outfitCount), sizeof(outfitCount));
        fin.read(reinterpret_cast<char*>(&effectCount), sizeof(effectCount));
        fin.read(reinterpret_cast<char*>(&distanceCount), sizeof(distanceCount));
        auto& itemTypesDat = items.getItemTypesDat();
        itemTypesDat.resize(itemCount + 1);

        // .dat items start at 100
        uint16_t firstId = 100;

        for (uint16_t id = firstId; id < itemTypesDat.size(); ++id) {
            auto type = ItemType();
            type.unserialize(id, fin);
            itemTypesDat[id] = type;
        }

        m_datLoaded = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading DAT file: " << e.what() << std::endl;
        return false;
    }
}