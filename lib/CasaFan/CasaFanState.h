#pragma once
#include <etl/bitset.h>

struct CasaFanState
{
    enum class FanDirection {
        Forward,
        Reverse
    };

    etl::bitset<6> brightness{"111111"};
    etl::bitset<3> fan_speed{"111"};
    FanDirection fan_direction{FanDirection::Forward};
};
