#pragma once

#include <cstdint>

constexpr bool hasBitSet(uint32_t flag, uint32_t flags) {
    return (flags & flag) != 0;
}