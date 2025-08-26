#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <optional>
#include <cstdint>

class ThingCategory final {
public:
    ThingCategory() = delete;

    // Enum for categories
    enum Value : uint8_t {
        NONE = 0,
        ITEM = 1,
        OUTFIT = 2,
        EFFECT = 3,
        MISSILE = 4
    };

    // Check if category string is valid
    static bool isValid(const std::string& category) {
        return category == "item" || category == "outfit" ||
               category == "effect" || category == "missile";
    }

    // Convert string to category enum
    static Value getCategory(const std::string& value) {
        std::string key = toKeyString(value);
        if (key == "item") return ITEM;
        if (key == "outfit") return OUTFIT;
        if (key == "effect") return EFFECT;
        if (key == "missile") return MISSILE;
        return NONE;
    }

    // Convert numeric value to category enum
    static Value getCategoryByValue(uint32_t value) {
        switch (value) {
            case 1: return ITEM;
            case 2: return OUTFIT;
            case 3: return EFFECT;
            case 4: return MISSILE;
            default: return NONE;
        }
    }

    // Convert category enum to numeric value
    static uint32_t getValue(Value category) {
        return static_cast<uint32_t>(category);
    }

    // Convert category enum to string
    static std::string toString(Value category) {
        switch (category) {
            case ITEM: return "item";
            case OUTFIT: return "outfit";
            case EFFECT: return "effect";
            case MISSILE: return "missile";
            default: return "";
        }
    }

private:
    // Utility: convert string to lowercase and remove spaces (similar to StringUtil.toKeyString)
    static std::string toKeyString(std::string str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
};