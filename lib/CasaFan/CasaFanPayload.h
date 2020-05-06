#pragma once

namespace CasaFanPayload
{
    static etl::bitset<6> buildBrightness(float brightness)
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

    static etl::bitset<3> buildFanSpeed(unsigned int speed)
    {
        if (speed > 7) {
            speed = 7;
        }

        return etl::bitset<3>(7-speed);
    }

    static bool buildFanDirection(CasaFanState::FanDirection direction)
    {
        return direction == CasaFanState::FanDirection::Forward;
    }
}
