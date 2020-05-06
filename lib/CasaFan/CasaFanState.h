#pragma once
#include <etl/bitset.h>

struct CasaFanState
{
    enum class FanDirection {
        Forward,
        Reverse
    };

    float brightness{0};
    etl::bitset<3> fan_speed{"111"};
    FanDirection fan_direction{FanDirection::Forward};
};
