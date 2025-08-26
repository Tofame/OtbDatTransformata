#pragma once

#include "../ItemType.h"
#include "../Loaders/fileLoader.h"

class MetadataWriter {
protected:
    virtual bool writeProperties(const ItemType& type, PropWriteStream& stream) {
        std::cout << "[ERROR::writeProperties] Use descendants of MetadataWriter\n";
        return false;
    };
    virtual bool writeItemProperties(const ItemType& type, PropWriteStream& stream) {
        std::cout << "[ERROR::writeItemProperties] Use descendants of MetadataWriter\n";
        return false;
    };

    void writeTexturePatterns(PropWriteStream& writer, ItemType& it, bool extended = false, bool frameDurations = false, bool frameGroups = false)
    {
        uint8_t groupCount = 1;
//        if(frameGroups && type.category == ThingCategory.OUTFIT) {
//            groupCount = type.frameGroups.length;
//            writeByte(groupCount);
//        }

        uint8_t i;
        uint8_t groupType;
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

            auto frameGroup = it.getFrameGroup(groupType);
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
                writer.write<uint8_t>((uint8_t)frameGroup.animationMode);   // Write animation type
                writer.write<uint8_t>(frameGroup.loopCount);        // Write loop count
                writer.write<uint8_t>(frameGroup.startFrame);      // Write start frame

                for (i = 0; i < frameGroup.frames; i++) {
                    writer.write<uint32_t>(frameGroup.frameDurations[i].minimum); // Write minimum duration
                    writer.write<uint32_t>(frameGroup.frameDurations[i].maximum); // Write maximum duration
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