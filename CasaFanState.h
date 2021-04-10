#pragma once
#include <etl/bitset.h>

struct CasaFanState
{
    enum class FanDirection {
        Forward,
        Reverse
    };

    float brightness{0};
    unsigned int fan_speed{0};
    FanDirection fan_direction{FanDirection::Forward};
};

