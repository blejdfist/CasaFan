#ifndef FAN_TRANSMITTER_CASAFAN_H
#define FAN_TRANSMITTER_CASAFAN_H

#include "CasaFanState.h"

struct HouseCode {
    explicit HouseCode(unsigned int addr) : value(addr) {}
    unsigned int value;
};

struct SelfLearning {
    explicit SelfLearning(unsigned int addr) : value(addr) {}
    unsigned int value;
};

class CasaFan {
public:
    enum class ProtocolType {
        HouseCode,
        SelfLearning
    };

    /**
     * Constructor
     * @param address Address of fan 0-15 or 0x0 - 0xffff for self learning fan
     * @param pin Pin to use for 433MHz transmitter
     */
    explicit CasaFan(HouseCode address, unsigned int pin = 11);
    explicit CasaFan(SelfLearning address, unsigned int pin = 11);



    /**
     * Set the brightness of the light
     * @param brightness
     */
    void setBrightness(float brightness);
    void setSpeed(unsigned int speed);
    void setDirection(CasaFanState::FanDirection direction);
    void transmit();

    bool needsToTransmit() const;

    const CasaFanState& getRawState() const { return state_; }

private:
    void performTransmission(const etl::ibitset& bits) const;

    bool needs_transmit_{true};
    unsigned int pin_;
    unsigned int address_;
    ProtocolType protocol_;
    CasaFanState state_;

    static constexpr unsigned int kPeriodUsec = 400;
};


#endif //FAN_TRANSMITTER_CASAFAN_H
