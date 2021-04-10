#pragma once
#include <Arduino.h>

#include "CasaFanState.h"
#include "encoders/CasaFanLineEncoding.h"
#include "encoders/PayloadEncoder.h"

struct HouseCodeFan
{
    using payload_builder = HouseCodePayloadEncoder;
};

struct SelfLearningFan
{
    using payload_builder = SelfLearningPayloadEncoder;
};

class CasaFanDriverBase {
public:
    /**
     * Constructor
     * @param address Address of fan 0-15 or 0x0 - 0xffff for self learning fan
     * @param pin Pin to use for 433MHz transmitter
     */
    explicit CasaFanDriverBase(unsigned int address, unsigned int pin)
        : pin_(pin)
        , address_(address)
    {
        pinMode(pin_, OUTPUT);
        digitalWrite(pin_, LOW);
    }

    /**
     * Set the brightness of the light
     * @param brightness
     */
    void setBrightness(float brightness)
    {
        state_.brightness = brightness;
        needs_transmit_ = true;
    }

    void setSpeed(unsigned int speed)
    {
        state_.fan_speed = speed;
        needs_transmit_ = true;
    }

    void setDirection(CasaFanState::FanDirection direction)
    {
        state_.fan_direction = direction;
        needs_transmit_ = true;
    }

    bool needsToTransmit() const { return needs_transmit_; }

    virtual void transmit() = 0;

protected:
    ~CasaFanDriverBase() = default;

    void performTransmission(const etl::ibitset &bits)
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

        needs_transmit_ = false;
    }

    bool needs_transmit_{false};
    unsigned int pin_;
    unsigned int address_;
    CasaFanState state_;
};

template<typename FanTraits>
class CasaFanDriver : public CasaFanDriverBase {
public:
    explicit CasaFanDriver(unsigned int address, unsigned int pin)
        : CasaFanDriverBase(address, pin)
    {
    }

    void transmit() override
    {
        const auto payload = FanTraits::payload_builder::build(address_, state_);
        auto line_coded = CasaFanLineEncoding::encode(payload);
        performTransmission(line_coded);
    }
};

using CasaFanHouseCode = CasaFanDriver<HouseCodeFan>;
using CasaFanSelfLearning = CasaFanDriver<SelfLearningFan>;
