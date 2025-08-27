#include <cstdint>
#include <iostream>
#include "DatCompiler.h"
#include "../globals.h"
#include <memory>
#include <unordered_set>

static const std::unordered_set<uint32_t> issueProneIds = {
        1205, 1294, 1426, 1505, 1508, 1510, 1678, 1679, 1680, 1681, 1682, 1683, 1684
};

// 10.98 .dat compiler
void DatCompiler::compile(DatLoader& datLoader, Items& items) {
    std::unique_ptr<MetadataWriter> metaWriter;
    if(g_protocolVersion > 986) {
        metaWriter = std::make_unique<MetadataWriter6>();
    } else {
        std::cout << "DatCompiler::compile currently only supports above 9.86 protocols.\n";
    }

    PropWriteStream writer;
    writer.write<uint32_t>(datLoader.m_datSignature);

    auto& itemTypes = items.getItemTypes();
    auto& datItemTypes = items.getItemTypesDat();
    writer.write<uint16_t>(itemTypes.size() - 1);
    writer.write<uint16_t>(datLoader.m_loadedOutfitsCount);
    writer.write<uint16_t>(datLoader.m_loadedEffectsCount);
    writer.write<uint16_t>(datLoader.m_loadedMissilesCount);

    for (uint32_t id = g_MIN_ITEM_ID; id < itemTypes.size(); id++) {
        auto& otb_it = itemTypes.at(id);
        if(otb_it.group == ITEM_GROUP_DEPRECATED) { //|| issueProneIds.count(id)) {
            metaWriter->writeItemProperties(items.substituteItemType, writer);
            metaWriter->writeTexturePatterns(items.substituteItemType, writer);
        } else {
            auto& dat_it = datItemTypes.at(otb_it.clientId);
            metaWriter->writeItemProperties(dat_it, writer);
            metaWriter->writeTexturePatterns(dat_it, writer);
        }
    }

    // Dump the preserved outfits/effects/missiles bytes
    const auto& restBytes = datLoader.getBytesOfTheRestOfDat();
    writer.writeBytes(restBytes.data(), restBytes.size());

    if (!writer.saveToFile(g_compileDatPath)) {
        std::cerr << "Failed to save file\n";
    }
}
