#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "globals.h"

struct FrameDuration {
    uint32_t minimum;
    uint32_t maximum;

    FrameDuration(uint32_t min = 0, uint32_t max = 0)
            : minimum(min), maximum(max) {}

    FrameDuration clone() const {
        return FrameDuration(minimum, maximum);
    }
};

struct Size {
    uint8_t width = 0;
    uint8_t height = 0;
};

struct SpriteExtent {
    static constexpr uint32_t DEFAULT_SIZE = g_SpritesExactSize;
};

enum class AnimationMode : uint8_t {
    ASYNCHRONOUS = 0
};

class FrameGroup {
public:
    // Properties
    uint8_t type = 0;
    uint8_t width = 1;
    uint8_t height = 1;
    uint8_t exactSize = SpriteExtent::DEFAULT_SIZE;
    uint8_t layers = 1;
    uint8_t patternX = 1;
    uint8_t patternY = 1;
    uint8_t patternZ = 1;
    uint8_t frames = 1;

    std::vector<uint32_t> spriteIndex;

    bool isAnimation = false;
    AnimationMode animationMode = AnimationMode::ASYNCHRONOUS;
    int32_t loopCount = 0;
    uint8_t startFrame = 0;

    std::vector<FrameDuration> frameDurations;

    // Constructor
    FrameGroup() = default;

    // Getters
    FrameDuration* getFrameDuration(size_t index) {
        if(index < frameDurations.size())
            return &frameDurations[index];
        return nullptr;
    }

    uint32_t getTotalX() const {
        return patternZ * patternX * layers;
    }

    uint32_t getTotalY() const {
        return frames * patternY;
    }

    uint32_t getTotalSprites() const {
        return width * height * patternX * patternY * patternZ * frames * layers;
    }

    uint32_t getTotalTextures() const {
        return patternX * patternY * patternZ * frames * layers;
    }

    uint32_t getSpriteIndex(uint32_t w, uint32_t h, uint32_t layer,
                            uint32_t px, uint32_t py, uint32_t pz, uint32_t frame) const {
        return ((((((frame % frames) * patternZ + pz) * patternY + py) * patternX + px) * layers + layer) * height + h) * width + w;
    }

    int32_t getTextureIndex(uint32_t layer, uint32_t px, uint32_t py, uint32_t pz, uint32_t frame) const {
        return (((frame % frames * patternZ + pz) * patternY + py) * patternX + px) * layers + layer;
    }

    Size getSpriteSheetSize() const {
        Size size;
        size.width = patternZ * patternX * layers * width * SpriteExtent::DEFAULT_SIZE;
        size.height = frames * patternY * height * SpriteExtent::DEFAULT_SIZE;
        return size;
    }

    void makeOutfitGroup(uint32_t duration) {
        patternX = 4; // Directions
        frames = 1;   // Animations
        isAnimation = false;

        frameDurations.resize(frames);
        for(uint32_t i = 0; i < frames; ++i)
            frameDurations[i] = FrameDuration(duration, duration);

        spriteIndex.resize(getTotalSprites());
    }

    FrameGroup clone() const {
        FrameGroup group;
        group.type = type;
        group.width = width;
        group.height = height;
        group.layers = layers;
        group.frames = frames;
        group.patternX = patternX;
        group.patternY = patternY;
        group.patternZ = patternZ;
        group.exactSize = exactSize;

        group.spriteIndex = spriteIndex;

        group.animationMode = animationMode;
        group.loopCount = loopCount;
        group.startFrame = startFrame;

        if(frames > 1) {
            group.isAnimation = true;
            group.frameDurations.resize(frames);
            for(uint32_t i = 0; i < frames; ++i)
                group.frameDurations[i] = frameDurations[i].clone();
        }

        return group;
    }
};