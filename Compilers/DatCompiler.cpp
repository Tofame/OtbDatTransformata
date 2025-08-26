#include <cstdint>
#include <iostream>
#include "DatCompiler.h"
#include "../globals.h"
#include <memory>

// 10.98 .dat compiler
void DatCompiler::compile(DatLoader& datLoader) {
    // For custom compiled we will have to tweak by hand.
    bool extended = g_protocolVersion >= 960;
    bool frameDurations = g_protocolVersion >= 1050;
    bool frameGroups = g_protocolVersion >= 1057;

    std::unique_ptr<MetadataWriter> metaWriter;
    if(g_protocolVersion > 986) {
        std::unique_ptr<MetadataWriter> metaWriter(new MetadataWriter6());
    } else {
        std::cout << "DatCompiler::compile currently only supports above 9.86 protocols.\n";
    }

    PropWriteStream writer;
    writer.write<uint32_t>(datLoader.m_datSignature);

    writer.write<uint16_t>(datLoader.m_loadedItemsCount);
    writer.write<uint16_t>(datLoader.m_loadedOutfitsCount);
    writer.write<uint16_t>(datLoader.m_loadedEffectsCount);
    writer.write<uint16_t>(datLoader.m_loadedMissilesCount);



    if (!writer.saveToFile(g_compileDatPath)) {
        std::cerr << "Failed to save file\n";
    }
}
