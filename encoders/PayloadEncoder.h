#pragma once
#include "CasaFanState.h"
#include <etl/bitset.h>

class PayloadEncoderBase {
public:
    static etl::bitset<6> buildBrightness(float brightness);
    static etl::bitset<3> buildFanSpeed(unsigned int speed);
    static bool buildFanDirection(CasaFanState::FanDirection direction);
};

struct HouseCodePayloadEncoder : public PayloadEncoderBase
{
    static etl::bitset<21> build(unsigned int address, const CasaFanState &state);
};

struct SelfLearningPayloadEncoder : public PayloadEncoderBase
{
    static etl::bitset<31> build(unsigned int address, const CasaFanState &state);
};
