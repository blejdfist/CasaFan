#pragma once

namespace CasaFanPayload
{
    static auto buildBrightness(float brightness)
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

    static auto buildFanSpeed(unsigned int speed)
    {
        if (speed > 7) {
            speed = 7;
        }

        return etl::bitset<3>(7-speed);
    }
}
