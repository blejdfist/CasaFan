#pragma once
#include <CasaFan/CasaFanState.h>
#include <bitset>

class PayloadEncoderBase {
public:
    static std::bitset<6> buildBrightness(float brightness);
    static std::bitset<3> buildFanSpeed(unsigned int speed);
    static bool buildFanDirection(CasaFanState::FanDirection direction);
};

struct HouseCodePayloadEncoder : public PayloadEncoderBase
{
    static std::bitset<21> build(unsigned int address, const CasaFanState &state);
};

struct SelfLearningPayloadEncoder : public PayloadEncoderBase
{
    static std::bitset<31> build(unsigned int address, const CasaFanState &state);
};
