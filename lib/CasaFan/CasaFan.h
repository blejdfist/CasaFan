#ifndef FAN_TRANSMITTER_CASAFAN_H
#define FAN_TRANSMITTER_CASAFAN_H

#include "CasaFanState.h"

class CasaFan {
public:

    /**
     * Constructor
     * @param address Address of fan 0-15
     * @param pin Pin to use for 433MHz transmitter
     */
    explicit CasaFan(unsigned int address = 0x0, unsigned int pin = 11);

    /**
     * Set the brightness of the light
     * @param brightness
     */
    void setBrightness(float brightness);
    void setSpeed(unsigned int speed);
    void setDirection(CasaFanState::FanDirection direction);
    void transmit();

    bool needsToTransmit() const;

    etl::bitset<21> buildHouseCodePayload() const;
    const CasaFanState& getRawState() const { return state_; }

private:
    static etl::bitset<4> calculateChecksum(const etl::bitset<21>& payload);

    bool needs_transmit_{true};
    unsigned int pin_;
    unsigned int address_;

    CasaFanState state_;

    static constexpr unsigned int kPeriodUsec = 400;
};


#endif //FAN_TRANSMITTER_CASAFAN_H
