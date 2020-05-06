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
    const auto payload = CasaFanPayload::buildHouseCodePayload(address_, state_);
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

