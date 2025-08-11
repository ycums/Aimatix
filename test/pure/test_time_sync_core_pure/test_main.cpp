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

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_build_wifi_qr_payload_basic);
    RUN_TEST(test_build_wifi_qr_payload_escape_backslash);
    RUN_TEST(test_build_wifi_qr_payload_escape_semicolon);
    RUN_TEST(test_build_wifi_qr_payload_escape_comma);
    RUN_TEST(test_build_wifi_qr_payload_escape_colon);
    RUN_TEST(test_build_wifi_qr_payload_empty_ssid);
    RUN_TEST(test_build_wifi_qr_payload_empty_psk);
    return UNITY_END();
}


