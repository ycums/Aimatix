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
    return UNITY_END();
}


