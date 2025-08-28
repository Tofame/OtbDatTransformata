#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>

#include <toml++/toml.h>

class Settings {
private:
    inline static Settings* instance = nullptr;

    Settings() = default;
    ~Settings() = default;

    // prevent copies
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

public:
    static Settings& getInstance() {
        if (!instance) {
            instance = new Settings();
            instance->load();
        }
        return *instance;
    }

    uint32_t protocolVersion = 0;
    std::string compileDatPath;
    uint32_t spritesExactSize = 32;
    uint32_t MIN_ITEM_ID = 100;

    bool isExtended = false;
    bool isFrameDurations = false;
    bool isFrameGroups = false;

    void load(const std::string& filename = "settings.toml") {
        try {
            auto config = toml::parse_file(filename);

            protocolVersion      = config["protocolVersion"].value_or(1098);
            compileDatPath       = config["compileDatPath"].value_or("items.dat");

            isExtended       = config["isExtended"].value_or(true);
            isFrameDurations = config["isFrameDurations"].value_or(true);
            isFrameGroups    = config["isFrameGroups"].value_or(true);
            if(protocolVersion >= 960) {
                isExtended = true;
            }
            if(protocolVersion >= 1050) {
                isFrameDurations = true;
            }
            if(protocolVersion >= 1057) {
                isFrameGroups = true;
            }

            spritesExactSize     = config["SpritesExactSize"].value_or(32u);
            MIN_ITEM_ID          = config["MIN_ITEM_ID"].value_or(100u);
        }
        catch (const toml::parse_error& err) {
            std::cerr << "Error parsing settings.toml: " << err.description()
                      << "\n   (" << err.source().begin << ")\n";
        }
    }
};