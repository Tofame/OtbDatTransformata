#pragma once

extern inline int g_protocolVersion = 1098; // not widely supported, rn majorly for compiling xd
extern inline bool g_otbPrint_SpritesHash = false;
extern inline std::string g_compileDatPath = "transformata.dat";
extern inline constexpr uint32_t g_SpritesExactSize = 32;

extern inline bool g_extended = true || g_protocolVersion >= 960;
extern inline bool g_frameDurations = false || g_protocolVersion >= 1050;
extern inline bool g_frameGroups = false || g_protocolVersion >= 1057;

extern inline uint32_t g_MIN_ITEM_ID = 100;