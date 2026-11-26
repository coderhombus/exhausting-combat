#pragma once
#include <cstdint>
#include <string_view>
#include "REX/REX/TOML.h"

namespace SETTINGS
{
enum class eSliderType
{
    INT,
    FLOAT,
    BOOL
};
struct IntSetting
{
    eSliderType sliderType;
    std::string_view key;
    REX::TOML::F32 tomlEntry;
    std::string_view label;
    std::string_view description;

    int defaultValue;
    int minValue;
    int maxValue;

    int value;
};

struct FloatSetting
{
    eSliderType sliderType;
    std::string_view key;
    REX::TOML::F32 tomlEntry;
    std::string_view label;
    std::string_view description;

    float defaultValue;
    float minValue;
    float maxValue;

    float value;
};

struct BoolSetting
{
    eSliderType sliderType;
    std::string_view key;
    REX::TOML::Bool tomlEntry;
    std::string_view label;
    std::string_view description;

    bool defaultValue;

    bool value;
};
}
