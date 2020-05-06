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
}
