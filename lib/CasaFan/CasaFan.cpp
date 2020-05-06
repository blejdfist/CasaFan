#ifdef UNIT_TEST
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

#include "CasaFan.h"
#include "CasaFanLineEncoding.h"

namespace
{
    template<const size_t N, const size_t M>
    void writeBits(etl::bitset<N>& output, const size_t position, const etl::bitset<M>& source)
    {
        for (size_t bit = 0; bit < M; ++bit)
        {
            output[position+bit] = source[M-bit-1];
        }
    }
}

CasaFan::CasaFan(unsigned int addr4bit, unsigned int pin)
: pin_(pin)
, addr_(addr4bit)
, light_("111111")
, fan_speed_("111")
{
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
}

void CasaFan::setBrightness(float brightness)
{
    if (brightness == 0.0f) {
        // All bits on = off
        light_ = 63;
    } else {
        light_ = round((kMaxLightValue - kMinLightValue) * brightness + kMinLightValue);
    }
    needs_transmit_ = true;
}

void CasaFan::setSpeed(unsigned int speed)
{
    if (speed > 7) {
        speed = 7;
    }

    fan_speed_ = 7-speed;
    needs_transmit_ = true;
}

void CasaFan::setDirection(CasaFan::FanDirection direction)
{
    fan_direction_ = direction;
    needs_transmit_ = true;
}

void CasaFan::transmit()
{
    const auto payload = buildPayload();
    auto line_coded = CasaFanLineEncoding::encode(payload);

    for (auto repeat = 0; repeat < 8; ++repeat) {
        for (size_t bit = 0; bit < line_coded.size(); ++bit) {
            digitalWrite(pin_, line_coded.test(bit) ? HIGH : LOW);
            delayMicroseconds(380);
        }
        digitalWrite(pin_, LOW);
        delay(10);
    }

    needs_transmit_ = false;
}

bool CasaFan::needsToTransmit() const
{
    return needs_transmit_;
}

etl::bitset<31> CasaFan::buildPayload() const
{
    etl::bitset<31> payload{0ull};

    // 21 bits
    // AAAA LLLLLL 1 SSS D PP CCCC
    // A = Address (4 bits)
    // L = Light level (6 bits)
    // S = Fan speed (3 bits)
    // D = Fan direction (1 bit)
    // P = Fan pattern (2 bits)
    // C = Checksum

    /*
    writeBits(payload, 0, addr_);
    writeBits(payload, 4, light_);
    payload[10] = true;
    writeBits(payload, 11, fan_speed_);
    payload[14] = fan_direction_ == FanDirection::Forward;
    writeBits(payload, 15, etl::bitset<2>(3));  // Unused
    writeBits(payload, 17, calculateChecksum(payload));
     */

    writeBits(payload, 0, addr_);
    payload[16] = fan_direction_ == FanDirection::Forward;
    writeBits(payload, 17, light_);
    payload[23] = true;
    writeBits(payload, 24, fan_speed_);
    writeBits(payload, 27, calculateChecksum(payload));


    return payload;
}

etl::bitset<4> CasaFan::calculateChecksum(const etl::bitset<31>& payload) {
    /*
    unsigned int crc = 7;

    for (size_t bit = 0; bit < 17; ++bit)
    {
        crc += (payload[bit] ? 1 : 0) * (1ul << (3 - (bit + 1) % 4));
    }
     */
    return etl::bitset<4>(payload[16] == 0 ? 0x6 : 0);
    //return etl::bitset<4>(crc & static_cast<unsigned int>(0xf));
}
