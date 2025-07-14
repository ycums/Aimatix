#include <unity.h>
#include <Preferences.h>
#include <WiFi.h>
#include "wifi_manager.h"

// モック設定
extern "C" {
  // WiFi関連のモック関数
  wl_status_t mock_wifi_status = WL_DISCONNECTED;
  bool mock_wifi_connected = false;
  std::string mock_wifi_ssid = "";
  std::string mock_wifi_password = "";
  IPAddress mock_wifi_ip(192, 168, 1, 100);
  int mock_wifi_rssi = -50;
  
  // Preferences関連のモック
  bool mock_preferences_begin = true;
  bool mock_preferences_put_string = true;
  bool mock_preferences_put_bool = true;
  bool mock_preferences_get_string = true;
  bool mock_preferences_get_bool = true;
  bool mock_preferences_clear = true;
  
  std::string mock_stored_ssid = "";
  std::string mock_stored_password = "";
  bool mock_stored_configured = false;
}

// テスト用のWiFiManagerインスタンス
WiFiManager* testWiFiManager = nullptr;

void setUp(void) {
  // テスト前の初期化
  mock_wifi_status = WL_DISCONNECTED;
  mock_wifi_connected = false;
  mock_wifi_ssid = "";
  mock_wifi_password = "";
  mock_preferences_begin = true;
  mock_preferences_put_string = true;
  mock_preferences_put_bool = true;
  mock_preferences_get_string = true;
  mock_preferences_get_bool = true;
  mock_preferences_clear = true;
  mock_stored_ssid = "";
  mock_stored_password = "";
  mock_stored_configured = false;
  
  // WiFiManagerインスタンスのリセット
  if (testWiFiManager) {
    delete testWiFiManager;
    testWiFiManager = nullptr;
  }
}

void tearDown(void) {
  // テスト後のクリーンアップ
  if (testWiFiManager) {
    delete testWiFiManager;
    testWiFiManager = nullptr;
  }
}

// テストケース1: WiFiManagerの初期化テスト
void test_wifi_manager_initialization() {
  testWiFiManager = WiFiManager::getInstance();
  
  TEST_ASSERT_NOT_NULL(testWiFiManager);
  
  // 初期化テスト
  bool initResult = testWiFiManager->initialize();
  TEST_ASSERT_TRUE(initResult);
}

// テストケース2: 設定保存・読み込みテスト
void test_wifi_config_save_load() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // テスト用の設定
  std::string testSSID = "TestWiFi";
  std::string testPassword = "TestPassword123";
  
  // 設定保存テスト
  bool saveResult = testWiFiManager->setWiFiConfig(testSSID, testPassword);
  TEST_ASSERT_TRUE(saveResult);
  
  // 設定読み込みテスト
  WiFiConfig loadedConfig = testWiFiManager->getWiFiConfig();
  TEST_ASSERT_EQUAL_STRING(testSSID.c_str(), loadedConfig.ssid.c_str());
  TEST_ASSERT_EQUAL_STRING(testPassword.c_str(), loadedConfig.password.c_str());
  TEST_ASSERT_TRUE(loadedConfig.isConfigured);
}

// テストケース3: WiFi接続テスト（成功ケース）
void test_wifi_connection_success() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 接続成功のモック設定
  mock_wifi_status = WL_CONNECTED;
  mock_wifi_connected = true;
  
  // 設定をセット
  testWiFiManager->setWiFiConfig("TestWiFi", "TestPassword");
  
  // 接続テスト
  bool connectResult = testWiFiManager->connect();
  TEST_ASSERT_TRUE(connectResult);
  TEST_ASSERT_TRUE(testWiFiManager->isWiFiConnected());
}

// テストケース4: WiFi接続テスト（失敗ケース）
void test_wifi_connection_failure() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 接続失敗のモック設定
  mock_wifi_status = WL_CONNECT_FAILED;
  mock_wifi_connected = false;
  
  // 設定をセット
  testWiFiManager->setWiFiConfig("InvalidWiFi", "InvalidPassword");
  
  // 接続テスト
  bool connectResult = testWiFiManager->connect();
  TEST_ASSERT_FALSE(connectResult);
  TEST_ASSERT_FALSE(testWiFiManager->isWiFiConnected());
}

// テストケース5: SmartConfigテスト
void test_smartconfig_functionality() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // SmartConfig成功のモック設定
  mock_wifi_status = WL_CONNECTED;
  mock_wifi_connected = true;
  mock_wifi_ssid = "SmartConfigWiFi";
  mock_wifi_password = "SmartConfigPassword";
  
  // SmartConfigテスト
  bool smartConfigResult = testWiFiManager->connectWithSmartConfig();
  TEST_ASSERT_TRUE(smartConfigResult);
  
  // 設定が保存されているかチェック
  WiFiConfig config = testWiFiManager->getWiFiConfig();
  TEST_ASSERT_EQUAL_STRING("SmartConfigWiFi", config.ssid.c_str());
  TEST_ASSERT_EQUAL_STRING("SmartConfigPassword", config.password.c_str());
  TEST_ASSERT_TRUE(config.isConfigured);
}

// テストケース6: 設定クリアテスト
void test_wifi_config_clear() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 初期設定
  testWiFiManager->setWiFiConfig("TestWiFi", "TestPassword");
  
  // 設定クリアテスト
  bool clearResult = testWiFiManager->clearWiFiConfig();
  TEST_ASSERT_TRUE(clearResult);
  
  // 設定がクリアされているかチェック
  WiFiConfig config = testWiFiManager->getWiFiConfig();
  TEST_ASSERT_TRUE(config.ssid.empty());
  TEST_ASSERT_TRUE(config.password.empty());
  TEST_ASSERT_FALSE(config.isConfigured);
}

// テストケース7: ユーティリティ関数テスト
void test_wifi_utility_functions() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 接続状態のモック設定
  mock_wifi_status = WL_CONNECTED;
  mock_wifi_connected = true;
  mock_wifi_ssid = "TestWiFi";
  
  // 設定をセットして接続
  testWiFiManager->setWiFiConfig("TestWiFi", "TestPassword");
  testWiFiManager->connect();
  
  // ユーティリティ関数テスト
  TEST_ASSERT_FALSE(testWiFiManager->getLocalIP().empty());
  TEST_ASSERT_GREATER_THAN(-100, testWiFiManager->getRSSI());
  TEST_ASSERT_EQUAL_STRING("TestWiFi", testWiFiManager->getSSID().c_str());
}

// テストケース8: 接続状態監視テスト
void test_wifi_connection_monitoring() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 初期接続状態
  mock_wifi_status = WL_CONNECTED;
  mock_wifi_connected = true;
  testWiFiManager->setWiFiConfig("TestWiFi", "TestPassword");
  testWiFiManager->connect();
  
  // 接続状態監視テスト
  testWiFiManager->update();
  TEST_ASSERT_TRUE(testWiFiManager->isWiFiConnected());
  
  // 接続切断のシミュレーション
  mock_wifi_status = WL_DISCONNECTED;
  mock_wifi_connected = false;
  testWiFiManager->update();
  TEST_ASSERT_FALSE(testWiFiManager->isWiFiConnected());
}

// テストケース9: エラーハンドリングテスト
void test_wifi_error_handling() {
  testWiFiManager = WiFiManager::getInstance();
  
  // Preferences初期化失敗のモック設定
  mock_preferences_begin = false;
  
  // 初期化失敗テスト
  bool initResult = testWiFiManager->initialize();
  TEST_ASSERT_FALSE(initResult);
  
  // Preferences初期化成功に戻す
  mock_preferences_begin = true;
  initResult = testWiFiManager->initialize();
  TEST_ASSERT_TRUE(initResult);
}

// テストケース10: 境界値テスト
void test_wifi_boundary_values() {
  testWiFiManager = WiFiManager::getInstance();
  testWiFiManager->initialize();
  
  // 空のSSIDでの接続テスト
  bool connectResult = testWiFiManager->connect();
  TEST_ASSERT_FALSE(connectResult);
  
  // 空のパスワードでの接続テスト
  testWiFiManager->setWiFiConfig("TestWiFi", "");
  connectResult = testWiFiManager->connect();
  TEST_ASSERT_FALSE(connectResult);
  
  // 正常な設定での接続テスト
  mock_wifi_status = WL_CONNECTED;
  mock_wifi_connected = true;
  testWiFiManager->setWiFiConfig("TestWiFi", "TestPassword");
  connectResult = testWiFiManager->connect();
  TEST_ASSERT_TRUE(connectResult);
}

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  
  RUN_TEST(test_wifi_manager_initialization);
  RUN_TEST(test_wifi_config_save_load);
  RUN_TEST(test_wifi_connection_success);
  RUN_TEST(test_wifi_connection_failure);
  RUN_TEST(test_smartconfig_functionality);
  RUN_TEST(test_wifi_config_clear);
  RUN_TEST(test_wifi_utility_functions);
  RUN_TEST(test_wifi_connection_monitoring);
  RUN_TEST(test_wifi_error_handling);
  RUN_TEST(test_wifi_boundary_values);
  
  UNITY_END();
}

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
} 