#include <unity.h>
#include "TimeSyncCore.h"
#include <string>

void setUp() {}
void tearDown() {}

void test_build_wifi_qr_payload_basic() {
    auto s = TimeSyncCore::buildWifiQrPayload("SSID", "PSK");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SSID;P:PSK;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_escape_backslash() {
    auto s = TimeSyncCore::buildWifiQrPayload("SS\\ID", "P\\SK");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SS\\\\ID;P:P\\\\SK;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_escape_semicolon() {
    auto s = TimeSyncCore::buildWifiQrPayload("SS;ID", "P;SK");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SS\\;ID;P:P\\;SK;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_escape_comma() {
    auto s = TimeSyncCore::buildWifiQrPayload("SS,ID", ",PSK");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SS\\,ID;P:\\,PSK;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_escape_colon() {
    auto s = TimeSyncCore::buildWifiQrPayload("SS:ID", "PS:K");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SS\\:ID;P:PS\\:K;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_empty_ssid() {
    auto s = TimeSyncCore::buildWifiQrPayload("", "PSK");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:;P:PSK;H:false;;", s.c_str());
}

void test_build_wifi_qr_payload_empty_psk() {
    auto s = TimeSyncCore::buildWifiQrPayload("SSID", "");
    TEST_ASSERT_EQUAL_STRING("WIFI:T:WPA;S:SSID;P:;H:false;;", s.c_str());
}

void test_build_url() {
    auto s = TimeSyncCore::buildUrl("192.168.4.1", "ABCDEF1234");
    TEST_ASSERT_EQUAL_STRING("http://192.168.4.1/sync?t=ABCDEF1234", s.c_str());
}

void test_format_offset_pos() {
    auto s = TimeSyncCore::formatOffsetHHMM(540); // +09:00
    TEST_ASSERT_EQUAL_STRING("+0900", s.c_str());
}

void test_verify_token_equal() {
    TEST_ASSERT_TRUE(TimeSyncCore::verifyToken("aa", "aa"));
}

void test_is_within_window_true() {
    TEST_ASSERT_TRUE(TimeSyncCore::isWithinWindow(1000u, 1500u, 600u));
}

void test_json_extracts(void) {
    std::string body = "{\"epochMs\":1754892706965,\"tzOffsetMin\":540,\"token\":\"ABC\"}";
    int64_t epoch = 0; int tz = 0; std::string tok;
    TEST_ASSERT_TRUE(TimeSyncCore::jsonExtractInt64(body, "epochMs", epoch));
    TEST_ASSERT_EQUAL_INT64(1754892706965LL, epoch);
    TEST_ASSERT_TRUE(TimeSyncCore::jsonExtractInt(body, "tzOffsetMin", tz));
    TEST_ASSERT_EQUAL_INT(540, tz);
    TEST_ASSERT_TRUE(TimeSyncCore::jsonExtractString(body, "token", tok));
    TEST_ASSERT_EQUAL_STRING("ABC", tok.c_str());
}

void test_build_posix_tz_whole_hour(void) {
    TEST_ASSERT_EQUAL_STRING("LT-9", TimeSyncCore::buildPosixTZ(540).c_str());
    TEST_ASSERT_EQUAL_STRING("LT+2", TimeSyncCore::buildPosixTZ(-120).c_str());
}

void test_build_posix_tz_half_hour(void) {
    TEST_ASSERT_EQUAL_STRING("LT-5:30", TimeSyncCore::buildPosixTZ(330).c_str());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_build_wifi_qr_payload_basic);
    RUN_TEST(test_build_wifi_qr_payload_escape_backslash);
    RUN_TEST(test_build_wifi_qr_payload_escape_semicolon);
    RUN_TEST(test_build_wifi_qr_payload_escape_comma);
    RUN_TEST(test_build_wifi_qr_payload_escape_colon);
    RUN_TEST(test_build_wifi_qr_payload_empty_ssid);
    RUN_TEST(test_build_wifi_qr_payload_empty_psk);
    // New tests for URL and token/window utilities (one assert per test)
    RUN_TEST(test_build_url);
    RUN_TEST(test_format_offset_pos);
    RUN_TEST(test_verify_token_equal);
    RUN_TEST(test_is_within_window_true);
    RUN_TEST(test_json_extracts);
    RUN_TEST(test_build_posix_tz_whole_hour);
    RUN_TEST(test_build_posix_tz_half_hour);
    return UNITY_END();
}


