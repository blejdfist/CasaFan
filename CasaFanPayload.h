#pragma once
#include <etl/bitset.h>
#include "CasaFanState.h"

class CasaFanPayload
{
public:
    static etl::bitset<6> buildBrightness(float brightness);
    static etl::bitset<3> buildFanSpeed(unsigned int speed);
    static bool buildFanDirection(CasaFanState::FanDirection direction);

    static etl::bitset<21> buildHouseCodePayload(unsigned int address, const CasaFanState& state);
    static etl::bitset<31> buildSelfLearningPayload(unsigned int address, const CasaFanState& state);
};
