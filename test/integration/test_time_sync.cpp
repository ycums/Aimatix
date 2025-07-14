#include <unity.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include "time_sync.h"
#include "wifi_manager.h"

// モック設定
extern "C" {
  // NTP関連のモック関数
  bool mock_ntp_update = true;
  time_t mock_ntp_epoch_time = 1640995200; // 2022-01-01 00:00:00 UTC
  bool mock_settimeofday_success = true;
  
  // WiFi関連のモック関数
  bool mock_wifi_connected = false;
  bool mock_wifi_disconnect_called = false;
  
  // 時間関連のモック
  unsigned long mock_millis = 0;
  time_t mock_time = 0;
}

// テスト用のTimeSyncインスタンス
TimeSync* testTimeSync = nullptr;
WiFiManager* testWiFiManager = nullptr;

void setUp(void) {
  // テスト前の初期化
  mock_ntp_update = true;
  mock_ntp_epoch_time = 1640995200;
  mock_settimeofday_success = true;
  mock_wifi_connected = false;
  mock_wifi_disconnect_called = false;
  mock_millis = 0;
  mock_time = 0;
  
  // インスタンスのリセット
  if (testTimeSync) {
    delete testTimeSync;
    testTimeSync = nullptr;
  }
  if (testWiFiManager) {
    delete testWiFiManager;
    testWiFiManager = nullptr;
  }
}

void tearDown(void) {
  // テスト後のクリーンアップ
  if (testTimeSync) {
    delete testTimeSync;
    testTimeSync = nullptr;
  }
  if (testWiFiManager) {
    delete testWiFiManager;
    testWiFiManager = nullptr;
  }
}

// テストケース1: TimeSyncの初期化テスト
void test_time_sync_initialization() {
  testTimeSync = TimeSync::getInstance();
  
  TEST_ASSERT_NOT_NULL(testTimeSync);
  
  // 初期化テスト
  bool initResult = testTimeSync->initialize();
  TEST_ASSERT_TRUE(initResult);
}

// テストケース2: NTP同期テスト（成功ケース）
void test_ntp_sync_success() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // WiFi接続状態のモック設定
  mock_wifi_connected = true;
  
  // NTP同期成功のモック設定
  mock_ntp_update = true;
  mock_ntp_epoch_time = 1640995200;
  mock_settimeofday_success = true;
  
  // 同期テスト
  bool syncResult = testTimeSync->syncTime();
  TEST_ASSERT_TRUE(syncResult);
  TEST_ASSERT_TRUE(testTimeSync->isTimeSynced());
}

// テストケース3: NTP同期テスト（失敗ケース）
void test_ntp_sync_failure() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // WiFi接続状態のモック設定
  mock_wifi_connected = true;
  
  // NTP同期失敗のモック設定
  mock_ntp_update = false;
  
  // 同期テスト
  bool syncResult = testTimeSync->syncTime();
  TEST_ASSERT_FALSE(syncResult);
  TEST_ASSERT_FALSE(testTimeSync->isTimeSynced());
}

// テストケース4: WiFi自動OFF機能テスト
void test_wifi_auto_off_after_sync() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // WiFi接続状態のモック設定
  mock_wifi_connected = true;
  
  // NTP同期成功のモック設定
  mock_ntp_update = true;
  mock_ntp_epoch_time = 1640995200;
  mock_settimeofday_success = true;
  
  // WiFi自動OFF機能付き同期テスト
  bool syncResult = testTimeSync->syncTimeWithWiFiOff();
  TEST_ASSERT_TRUE(syncResult);
  TEST_ASSERT_TRUE(testTimeSync->isTimeSynced());
  
  // WiFiが自動OFFされているかチェック
  TEST_ASSERT_TRUE(mock_wifi_disconnect_called);
}

// テストケース5: WiFi未接続時の同期テスト
void test_sync_without_wifi() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // WiFi未接続状態のモック設定
  mock_wifi_connected = false;
  
  // 同期テスト
  bool syncResult = testTimeSync->syncTime();
  TEST_ASSERT_FALSE(syncResult);
  
  // WiFi自動OFF機能付き同期テスト
  bool syncWithOffResult = testTimeSync->syncTimeWithWiFiOff();
  TEST_ASSERT_FALSE(syncWithOffResult);
}

// テストケース6: 時刻フォーマットテスト
void test_time_formatting() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // テスト用の時刻設定
  mock_time = 1640995200; // 2022-01-01 00:00:00 UTC
  
  // 時刻フォーマットテスト
  std::string formattedTime = testTimeSync->getFormattedTime();
  TEST_ASSERT_FALSE(formattedTime.empty());
  TEST_ASSERT_TRUE(formattedTime.find("2022") != std::string::npos);
}

// テストケース7: 同期間隔チェックテスト
void test_sync_interval_check() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // 初回同期の確認
  TEST_ASSERT_TRUE(testTimeSync->isSyncRequired());
  
  // 同期実行
  mock_wifi_connected = true;
  mock_ntp_update = true;
  testTimeSync->syncTime();
  
  // 同期後は不要
  TEST_ASSERT_FALSE(testTimeSync->isSyncRequired());
  
  // 時間を進めて同期間隔を超える
  mock_millis = 3600001; // 1時間 + 1ms
  
  // 同期間隔を超えた場合は必要
  TEST_ASSERT_TRUE(testTimeSync->isSyncRequired());
}

// テストケース8: エラーハンドリングテスト
void test_error_handling() {
  testTimeSync = TimeSync::getInstance();
  
  // 未初期化状態でのテスト
  int error = testTimeSync->getLastError();
  TEST_ASSERT_EQUAL(-4, error); // TIME_SYNC_NOT_INITIALIZED
  
  std::string errorMessage = testTimeSync->getLastErrorMessage();
  TEST_ASSERT_EQUAL_STRING("Not initialized", errorMessage.c_str());
  
  // 初期化
  testTimeSync->initialize();
  
  // WiFi未接続状態でのテスト
  mock_wifi_connected = false;
  error = testTimeSync->getLastError();
  TEST_ASSERT_EQUAL(-1, error); // TIME_SYNC_WIFI_NOT_CONNECTED
  
  errorMessage = testTimeSync->getLastErrorMessage();
  TEST_ASSERT_EQUAL_STRING("WiFi not connected", errorMessage.c_str());
  
  // 正常状態でのテスト
  mock_wifi_connected = true;
  mock_ntp_update = true;
  testTimeSync->syncTime();
  
  error = testTimeSync->getLastError();
  TEST_ASSERT_EQUAL(0, error); // TIME_SYNC_SUCCESS
  
  errorMessage = testTimeSync->getLastErrorMessage();
  TEST_ASSERT_EQUAL_STRING("Success", errorMessage.c_str());
}

// テストケース9: 同期リセットテスト
void test_sync_reset() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // 初期同期
  mock_wifi_connected = true;
  mock_ntp_update = true;
  testTimeSync->syncTime();
  
  TEST_ASSERT_TRUE(testTimeSync->isTimeSynced());
  TEST_ASSERT_FALSE(testTimeSync->isSyncRequired());
  
  // 同期リセット
  testTimeSync->resetSync();
  
  TEST_ASSERT_FALSE(testTimeSync->isTimeSynced());
  TEST_ASSERT_TRUE(testTimeSync->isSyncRequired());
}

// テストケース10: 境界値テスト
void test_boundary_values() {
  testTimeSync = TimeSync::getInstance();
  testTimeSync->initialize();
  
  // システム時刻設定失敗のテスト
  mock_settimeofday_success = false;
  mock_wifi_connected = true;
  mock_ntp_update = true;
  
  bool syncResult = testTimeSync->syncTime();
  // settimeofdayが失敗してもNTP同期自体は成功とみなす
  TEST_ASSERT_TRUE(syncResult);
  
  // 正常状態に戻す
  mock_settimeofday_success = true;
  syncResult = testTimeSync->syncTime();
  TEST_ASSERT_TRUE(syncResult);
}

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_time_sync_initialization);
  RUN_TEST(test_ntp_sync_success);
  RUN_TEST(test_ntp_sync_failure);
  RUN_TEST(test_wifi_auto_off_after_sync);
  RUN_TEST(test_sync_without_wifi);
  RUN_TEST(test_time_formatting);
  RUN_TEST(test_sync_interval_check);
  RUN_TEST(test_error_handling);
  RUN_TEST(test_sync_reset);
  RUN_TEST(test_boundary_values);
  
  UNITY_END();
}

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
} 