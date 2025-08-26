#include <cstdint>
#include <iostream>
#include "DatCompiler.h"
#include "../globals.h"
#include <memory>

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
    writer.write<uint16_t>(itemTypes.size());
    writer.write<uint16_t>(datLoader.m_loadedOutfitsCount);
    writer.write<uint16_t>(datLoader.m_loadedEffectsCount);
    writer.write<uint16_t>(datLoader.m_loadedMissilesCount);

    for (uint32_t id = g_MIN_ITEM_ID; id < itemTypes.size(); id++) {
        auto& otb_it = itemTypes.at(id);
        auto& dat_it = datItemTypes.at(otb_it.clientId);
        metaWriter->writeItemProperties(dat_it, writer);
        metaWriter->writeTexturePatterns(dat_it, writer);
    }

    // Dump the preserved outfits/effects/missiles bytes
    const auto& restBytes = datLoader.getBytesOfTheRestOfDat();
    writer.writeBytes(restBytes.data(), restBytes.size());

    if (!writer.saveToFile(g_compileDatPath)) {
        std::cerr << "Failed to save file\n";
    }
}
