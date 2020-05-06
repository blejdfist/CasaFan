#ifdef UNIT_TEST
#include <ArduinoFake.h>
#else
#include <Arduino.h>
#endif

#include "CasaFan.h"
#include "CasaFanLineEncoding.h"
#include "CasaFanPayload.h"

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

CasaFan::CasaFan(HouseCode address, unsigned int pin)
: pin_(pin)
, address_(address.value)
, protocol_(ProtocolType::HouseCode)
{
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
}

CasaFan::CasaFan(SelfLearning address, unsigned int pin)
: pin_(pin)
, address_(address.value)
, protocol_(ProtocolType::SelfLearning)
{
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, LOW);
}

void CasaFan::setBrightness(float brightness)
{
    state_.brightness = brightness;
    needs_transmit_ = true;
}

void CasaFan::setSpeed(unsigned int speed)
{
    state_.fan_speed = speed;
    needs_transmit_ = true;
}

void CasaFan::setDirection(CasaFanState::FanDirection direction)
{
    state_.fan_direction = direction;
    needs_transmit_ = true;
}

void CasaFan::transmit()
{
    const auto payload = buildHouseCodePayload();
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

etl::bitset<21> CasaFan::buildHouseCodePayload() const
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

    writeBits(payload, 0, etl::bitset<4>(address_));
    writeBits(payload, 4, CasaFanPayload::buildBrightness(state_.brightness));
    payload[10] = true;
    writeBits(payload, 11, CasaFanPayload::buildFanSpeed(state_.fan_speed));
    payload[14] = CasaFanPayload::buildFanDirection(state_.fan_direction);
    writeBits(payload, 15, etl::bitset<2>(3));  // Unused
    writeBits(payload, 17, calculateChecksum(payload));

    return payload;
}

etl::bitset<4> CasaFan::calculateChecksum(const etl::bitset<21>& payload) {
    unsigned int crc = 7;

    for (size_t bit = 0; bit < 17; ++bit)
    {
        crc += (payload[bit] ? 1 : 0) * (1ul << (3 - (bit + 1) % 4));
    }

    return etl::bitset<4>(crc & static_cast<unsigned int>(0xf));
}
