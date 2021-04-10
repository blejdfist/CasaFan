#include <ArduinoFake.h>
#include <CasaFan/CasaFan.h>
#include <CasaFan/CasaFanLineEncoding.h>
#include <CasaFan/PayloadEncoder.h>

#include <unity.h>

#include <etl/string.h>
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
    std::array test_sets_housecode {
        etl::make_pair(from_string("000011111011111110110"),
                       from_string("1001001001001011011011011011001011011011011011011011001011011001")),
        etl::make_pair(from_string("000111111111111110000"),
                       from_string("1001001001011011011011011011011011011011011011011011001001001001")),
        etl::make_pair(from_string("001011111111111111001"),
                       from_string("1001001011001011011011011011011011011011011011011011011001001011"))
    };
    for (const auto& [input, expected] : test_sets_housecode) {
        auto result = CasaFanLineEncoding::encode(input);
        TEST_ASSERT_EQUAL_STRING(to_string(expected).c_str(), to_string(result).c_str());
    }

    std::array test_sets_selflearning {
        etl::make_pair(from_string("1100100100010110011111110100110"),
                       from_string("1011011001001011001001011001001001011001011011001001011011011011011011011001011001001011011001"))
    };
    for (const auto& [input, expected] : test_sets_selflearning) {
        auto result = CasaFanLineEncoding::encode(input);
        TEST_ASSERT_EQUAL_STRING(to_string(expected).c_str(), to_string(result).c_str());
    }
}

void test_payload_off()
{
    CasaFanState state;
    TEST_ASSERT_EQUAL_STRING("101011111111111111101", to_string(HouseCodePayloadEncoder::build(0xA, state)).c_str());
    TEST_ASSERT_EQUAL_STRING("1100100100010110111111111110110", to_string(SelfLearningPayloadEncoder::build(0xc916, state)).c_str());
}

void test_payload_fan_direction()
{
    CasaFanState state;
    state.fan_direction = CasaFanState::FanDirection::Forward;
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(HouseCodePayloadEncoder::build(0, state)).c_str());
    TEST_ASSERT_EQUAL_STRING("1100100100010110111111111110110", to_string(SelfLearningPayloadEncoder::build(0xc916, state)).c_str());

    state.fan_direction = CasaFanState::FanDirection::Reverse;
    TEST_ASSERT_EQUAL_STRING("000011111111110110111", to_string(HouseCodePayloadEncoder::build(0, state)).c_str());
    TEST_ASSERT_EQUAL_STRING("1100100100010110011111111110110", to_string(SelfLearningPayloadEncoder::build(0xc916, state)).c_str());
}

void test_payload_fan_speed()
{
    CasaFanState state;
    state.fan_speed = 0;
    TEST_ASSERT_EQUAL_STRING("000011111111111111000", to_string(HouseCodePayloadEncoder::build(0, state)).c_str());

    state.fan_speed = 7;
    TEST_ASSERT_EQUAL_STRING("000011111110001111010", to_string(HouseCodePayloadEncoder::build(0, state)).c_str());
}

void test_fan_speed()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    // Fan speed is reversed
    TEST_ASSERT_EQUAL(7, PayloadEncoderBase::buildFanSpeed(0).value<unsigned int>());
    TEST_ASSERT_EQUAL(6, PayloadEncoderBase::buildFanSpeed(1).value<unsigned int>());
    TEST_ASSERT_EQUAL(0, PayloadEncoderBase::buildFanSpeed(7).value<unsigned int>());
}

void test_brightness()
{
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), digitalWrite)).Return();

    // Light off is a special case
    TEST_ASSERT_EQUAL(63, PayloadEncoderBase::buildBrightness(0.0f).value<unsigned int>());

    TEST_ASSERT_EQUAL(24, PayloadEncoderBase::buildBrightness(0.1f).value<unsigned int>());
    TEST_ASSERT_EQUAL(41, PayloadEncoderBase::buildBrightness(0.5f).value<unsigned int>());
    TEST_ASSERT_EQUAL(62, PayloadEncoderBase::buildBrightness(1.0f).value<unsigned int>());
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
