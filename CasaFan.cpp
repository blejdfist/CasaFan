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
    switch (protocol_) {
        case ProtocolType::HouseCode:
        {
            const auto payload = CasaFanPayload::buildHouseCodePayload(address_, state_);
            auto line_coded = CasaFanLineEncoding::encode(payload);
            performTransmission(line_coded);
            break;
        }

        case ProtocolType::SelfLearning:
        {
            const auto payload = CasaFanPayload::buildSelfLearningPayload(address_, state_);
            auto line_coded = CasaFanLineEncoding::encode(payload);
            performTransmission(line_coded);
            break;
        }
    }

    needs_transmit_ = false;
}

void CasaFan::performTransmission(const etl::ibitset& bits) const
{
    for (auto repeat = 0; repeat < 8; ++repeat)
    {
        for (size_t bit = 0; bit < bits.size(); ++bit)
        {
            digitalWrite(pin_, bits.test(bit) ? HIGH : LOW);
            delayMicroseconds(380);
        }
        digitalWrite(pin_, LOW);
        delay(10);
    }
}

bool CasaFan::needsToTransmit() const
{
    return needs_transmit_;
}

