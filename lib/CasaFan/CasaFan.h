#ifndef FAN_TRANSMITTER_CASAFAN_H
#define FAN_TRANSMITTER_CASAFAN_H

#include <etl/bitset.h>

class CasaFan {
public:
    enum class FanDirection {
        Forward,
        Reverse
    };

    /**
     * Constructor
     * @param addr4bit Address of fan 0-15
     * @param pin Pin to use for 433MHz transmitter
     */
    explicit CasaFan(unsigned int addr4bit = 0x0, unsigned int pin = 11);

    /**
     * Set the brightness of the light
     * @param brightness
     */
    void setBrightness(float brightness);
    void setSpeed(unsigned int speed);
    void setDirection(FanDirection direction);
    void transmit();

    bool needsToTransmit() const;

    etl::bitset<21> buildPayload() const;

private:
    static etl::bitset<4> calculateChecksum(const etl::bitset<21>& payload);

    bool needs_transmit_{true};
    unsigned int pin_;
    etl::bitset<4> addr_{0ull};
    etl::bitset<6> light_{0ull};
    etl::bitset<3> fan_speed_{0ull};
    FanDirection fan_direction_{FanDirection::Forward};

    static constexpr unsigned int kPeriodUsec = 400;
    static constexpr unsigned int kMinLightValue = 20;
    static constexpr unsigned int kMaxLightValue = 62;
};


#endif //FAN_TRANSMITTER_CASAFAN_H
