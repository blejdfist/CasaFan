#include <CasaFan/PayloadEncoder.h>

namespace {
    template<const size_t N, const size_t M>
    void writeBits(etl::bitset<N>& output, const size_t position, const etl::bitset<M>& source)
    {
        for (size_t bit = 0; bit < M; ++bit)
        {
            output[position + bit] = source[M - bit - 1];
        }
    }

    /**
     * Calculate checksum for house code fans
     * @param payload Payload to calculate checksum for
     * @return Checksum (4 bits)
     */
    etl::bitset<4> calculateChecksum(const etl::bitset<21>& payload)
    {
        unsigned int crc = 7;

        for (size_t bit = 0; bit < 17; ++bit)
        {
            crc += (payload[bit] ? 1 : 0) * (1ul << (3 - (bit + 1) % 4));
        }

        return etl::bitset<4>(crc & static_cast<unsigned int>(0xf));
    }
} // namespace

etl::bitset<6> PayloadEncoderBase::buildBrightness(float brightness)
{
    etl::bitset<6> payload;
    constexpr unsigned int kMinLightValue = 20;
    constexpr unsigned int kMaxLightValue = 62;

    if (brightness == 0.0f) {
        // All bits on = off
        payload = 63;
    } else {
        payload = lround((kMaxLightValue - kMinLightValue) * brightness + kMinLightValue);
    }

    return payload;
}

etl::bitset<3> PayloadEncoderBase::buildFanSpeed(unsigned int speed)
{
    if (speed > 7) {
        speed = 7;
    }

    return etl::bitset<3>(7-speed);
}

bool PayloadEncoderBase::buildFanDirection(CasaFanState::FanDirection direction)
{
    return direction == CasaFanState::FanDirection::Forward;
}

etl::bitset<21> HouseCodePayloadEncoder::build(unsigned int address, const CasaFanState &state)
{
    etl::bitset<21> payload{0ull};

    // 21 bits
    // AAAA LLLLLL 1 SSS D PP CCCC
    // A = Address (4 bits)
    // L = Light level (6 bits)
    // S = Fan speed (3 bits)
    // D = Fan direction (1 bit)
    // P = Fan pattern (2 bits)
    // C = Checksum

    writeBits(payload, 0, etl::bitset<4>(address));
    writeBits(payload, 4, buildBrightness(state.brightness));
    payload[10] = true;
    writeBits(payload, 11, buildFanSpeed(state.fan_speed));
    payload[14] = buildFanDirection(state.fan_direction);
    writeBits(payload, 15, etl::bitset<2>(3));  // Unused
    writeBits(payload, 17, calculateChecksum(payload));

    return payload;
}

etl::bitset<31> SelfLearningPayloadEncoder::build(unsigned int address, const CasaFanState &state)
{
    etl::bitset<31> payload{0ull};

    // 31 bits
    // AAAAAAAAAAAAAAAA D LLLLLL 1 SSS CCCC
    // D = Fan direction (1 bit)
    // A = Address (16 bits)
    // L = Light level (6 bits)
    // S = Fan speed (3 bits)
    // C = Checksum (Always 0110 ?)

    writeBits(payload, 0, etl::bitset<16>(address));
    payload[16] = buildFanDirection(state.fan_direction);
    writeBits(payload, 17, buildBrightness(state.brightness));
    payload[23] = true;
    writeBits(payload, 24, buildFanSpeed(state.fan_speed));
    writeBits(payload, 27, etl::bitset<4>(6u));

    return payload;
}
