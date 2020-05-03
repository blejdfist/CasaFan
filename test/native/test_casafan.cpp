#include <ArduinoFake.h>
#include <CasaFan.h>
#include <CasaFanLineEncoding.h>

#include <unity.h>

#include <etl/cstring.h>
#include <array>


using fakeit::When;

template<const size_t N>
etl::string<N> to_string(etl::bitset<N> value)
{
    etl::string<N> result;
    for (int bit = 0; bit < N; ++bit) {
        result.append(value[bit] ? "1" : "0");
    }
    return result;
}

template<const size_t N>
etl::bitset<N-1> from_string(const char(&bits)[N]) {
    etl::bitset<N-1> result;
    for (size_t i = 0; i < N-1; ++i) {
        result.set(i, bits[i] == '1');
    }
    return result;
}

void test_line_encoder()
{
    std::array test_sets{
        etl::make_pair(from_string("000011111011111110110"),
                       from_string("1001001001001011011011011011001011011011011011011011001011011001")),
        etl::make_pair(from_string("000111111111111110000"),
                       from_string("1001001001011011011011011011011011011011011011011011001001001001")),
        etl::make_pair(from_string("001011111111111111001"),
                       from_string("1001001011001011011011011011011011011011011011011011011001001011"))
    };

    for (const auto& [input, expected] : test_sets) {
        auto result = CasaFanLineEncoding::encode(input);
        TEST_ASSERT_EQUAL_STRING(to_string(expected).c_str(), to_string(result).c_str());
    }
}

void test_payload_off()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    CasaFan fan(0xA);
    TEST_ASSERT_EQUAL_STRING("101011111111111111101", to_string(fan.buildPayload()).c_str());
}

void test_payload_fan_direction()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    CasaFan fan(0x0);

    fan.setDirection(CasaFan::FanDirection::Forward);
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(fan.buildPayload()).c_str());

    fan.setDirection(CasaFan::FanDirection::Reverse);
    TEST_ASSERT_EQUAL_STRING("000011111111110110111", to_string(fan.buildPayload()).c_str());
}

void test_payload_fan_speed()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    CasaFan fan(0x0);

    fan.setSpeed(0);  // Off -> Speed 111
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(fan.buildPayload()).c_str());

    fan.setSpeed(7);  // Full speed -> Speed 000
    TEST_ASSERT_EQUAL_STRING("000011111110001111010", to_string(fan.buildPayload()).c_str());
}

int main() {
    ArduinoFakeReset();

    UNITY_BEGIN();

    RUN_TEST(test_line_encoder);
    RUN_TEST(test_payload_off);
    RUN_TEST(test_payload_fan_direction);
    RUN_TEST(test_payload_fan_speed);

    UNITY_END();

    return 0;
}
