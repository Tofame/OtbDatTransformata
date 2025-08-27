#pragma once

#include "../ItemType.h"
#include "../Loaders/fileLoader.h"

class MetadataWriter {
public:
    virtual bool writeProperties(const ItemType& type, PropWriteStream& stream) {
        std::cout << "[ERROR::writeProperties] Use descendants of MetadataWriter\n";
        return false;
    };
    virtual bool writeItemProperties(const ItemType& type, PropWriteStream& stream) {
        std::cout << "[ERROR::writeItemProperties] Use descendants of MetadataWriter\n";
        return false;
    };

    void writeTexturePatterns(ItemType& it, PropWriteStream& writer, bool extended = g_extended, bool frameDurations = g_frameDurations, bool frameGroups = g_frameGroups)
    {
        uint8_t groupCount = 1;
//        if(frameGroups && type.category == ThingCategory.OUTFIT) {
//            groupCount = type.frameGroups.length;
//            writeByte(groupCount);
//        }

        uint8_t i;
        uint8_t groupType = 0;
        for(groupType = 0; groupType < groupCount; groupType++)
        {
//            if(frameGroups && type.category == ThingCategory.OUTFIT)
//            {
//                var group:uint = groupType;
//                if(groupCount < 2)
//                    group = 1;
//
//                writeByte(group);
//            }

            auto& frameGroup = it.getFrameGroup(groupType);
            writer.write<uint8_t>(frameGroup.width);  // Write width
            writer.write<uint8_t>(frameGroup.height); // Write height

            if (frameGroup.width > 1 || frameGroup.height > 1)
                writer.write<uint8_t>(frameGroup.exactSize); // Write exact size

            writer.write<uint8_t>(frameGroup.layers);   // Write layers
            writer.write<uint8_t>(frameGroup.patternX); // Write pattern X
            writer.write<uint8_t>(frameGroup.patternY); // Write pattern Y
            writer.write<uint8_t>(frameGroup.patternZ); // Write pattern Z
            writer.write<uint8_t>(frameGroup.frames);   // Write frames

            if (frameDurations && frameGroup.isAnimation) {
                writer.write<uint8_t>(static_cast<uint8_t>(frameGroup.animationMode));   // 1 byte
                writer.write<int32_t>(frameGroup.loopCount);       // 4 bytes
                writer.write<int8_t>(frameGroup.startFrame);       // 1 byte

                for (uint8_t i = 0; i < frameGroup.frames; i++) {
                    writer.write<uint32_t>(frameGroup.frameDurations[i].minimum); // 4 bytes
                    writer.write<uint32_t>(frameGroup.frameDurations[i].maximum); // 4 bytes
                }
            }

            const std::vector<uint32_t>& spriteIndex = frameGroup.spriteIndex;
            for(size_t i = 0; i < spriteIndex.size(); ++i) {
                if(extended)
                    writer.write<uint32_t>(spriteIndex[i]);
                else
                    writer.write<uint16_t>(static_cast<uint16_t>(spriteIndex[i]));
            }
        }
    }
};