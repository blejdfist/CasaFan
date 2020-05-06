#include <ArduinoFake.h>
#include <CasaFan.h>
#include <CasaFanLineEncoding.h>
#include <CasaFanPayload.h>

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
    CasaFanState state;
    TEST_ASSERT_EQUAL_STRING("101011111111111111101", to_string(CasaFanPayload::buildHouseCodePayload(0xA, state)).c_str());
}

void test_payload_fan_direction()
{
    CasaFanState state;
    state.fan_direction = CasaFanState::FanDirection::Forward;
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(CasaFanPayload::buildHouseCodePayload(0, state)).c_str());

    state.fan_direction = CasaFanState::FanDirection::Reverse;
    TEST_ASSERT_EQUAL_STRING("000011111111110110111", to_string(CasaFanPayload::buildHouseCodePayload(0, state)).c_str());
}

void test_payload_fan_speed()
{
    CasaFanState state;
    state.fan_speed = 0;
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(CasaFanPayload::buildHouseCodePayload(0, state)).c_str());

    state.fan_speed = 7;
    TEST_ASSERT_EQUAL_STRING("000011111110001111010", to_string(CasaFanPayload::buildHouseCodePayload(0, state)).c_str());
}

void test_fan_speed()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    CasaFan fan(HouseCode(0x0));

    // Fan speed is reversed

    fan.setSpeed(0);
    TEST_ASSERT_EQUAL(7, CasaFanPayload::buildFanSpeed(fan.getRawState().fan_speed).value<unsigned int>());

    fan.setSpeed(1);
    TEST_ASSERT_EQUAL(6, CasaFanPayload::buildFanSpeed(fan.getRawState().fan_speed).value<unsigned int>());

    fan.setSpeed(7);
    TEST_ASSERT_EQUAL(0, CasaFanPayload::buildFanSpeed(fan.getRawState().fan_speed).value<unsigned int>());
}

void test_brightness()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    CasaFan fan(HouseCode(0x0));

    // Light off is a special case
    fan.setBrightness(0.0f);
    TEST_ASSERT_EQUAL(63, CasaFanPayload::buildBrightness(fan.getRawState().brightness).value<unsigned int>());

    fan.setBrightness(0.1f);
    TEST_ASSERT_EQUAL(24, CasaFanPayload::buildBrightness(fan.getRawState().brightness).value<unsigned int>());

    fan.setBrightness(0.5f);
    TEST_ASSERT_EQUAL(41, CasaFanPayload::buildBrightness(fan.getRawState().brightness).value<unsigned int>());

    fan.setBrightness(1.0f);
    TEST_ASSERT_EQUAL(62, CasaFanPayload::buildBrightness(fan.getRawState().brightness).value<unsigned int>());
}

int main() {
    ArduinoFakeReset();

    UNITY_BEGIN();

    RUN_TEST(test_fan_speed);
    RUN_TEST(test_brightness);
    RUN_TEST(test_line_encoder);
    RUN_TEST(test_payload_off);
    RUN_TEST(test_payload_fan_direction);
    RUN_TEST(test_payload_fan_speed);

    UNITY_END();

    return 0;
}
