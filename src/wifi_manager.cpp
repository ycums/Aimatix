#include "wifi_manager.h"
#include <M5Stack.h>

// シングルトンインスタンス
WiFiManager* WiFiManager::instance = nullptr;
WiFiManager* wifiManager = nullptr;

// 定数定義
static const char* PREF_NAMESPACE = "wifi";
static const char* PREF_SSID_KEY = "ssid";
static const char* PREF_PASSWORD_KEY = "password";
static const char* PREF_CONFIGURED_KEY = "configured";

WiFiManager::WiFiManager() 
  : isConnected(false), lastConnectionAttempt(0) {
  config.isConfigured = false;
}

WiFiManager* WiFiManager::getInstance() {
  if (instance == nullptr) {
    instance = new WiFiManager();
  }
  return instance;
}

bool WiFiManager::initialize() {
  // Preferencesの初期化
  if (!preferences.begin(PREF_NAMESPACE, false)) {
    Serial.println("Failed to initialize WiFi preferences");
    return false;
  }
  
  // 保存された設定を読み込み
  if (!loadWiFiConfig()) {
    Serial.println("No saved WiFi configuration found");
    return false;
  }
  
  Serial.println("WiFiManager initialized successfully");
  return true;
}

void WiFiManager::begin() {
  // WiFi初期化
  WiFi.mode(WIFI_STA);
  
  // 保存された設定がある場合は自動接続を試行
  if (config.isConfigured && !config.ssid.empty()) {
    Serial.println("Attempting to connect with saved configuration");
    connect();
  }
}

bool WiFiManager::loadWiFiConfig() {
  config.ssid = preferences.getString(PREF_SSID_KEY, "").c_str();
  config.password = preferences.getString(PREF_PASSWORD_KEY, "").c_str();
  config.isConfigured = preferences.getBool(PREF_CONFIGURED_KEY, false);
  
  Serial.printf("Loaded WiFi config - SSID: %s, Configured: %s\n", 
                config.ssid.c_str(), config.isConfigured ? "true" : "false");
  
  return config.isConfigured && !config.ssid.empty();
}

void WiFiManager::saveWiFiConfig() {
  preferences.putString(PREF_SSID_KEY, config.ssid.c_str());
  preferences.putString(PREF_PASSWORD_KEY, config.password.c_str());
  preferences.putBool(PREF_CONFIGURED_KEY, config.isConfigured);
  
  Serial.printf("Saved WiFi config - SSID: %s, Configured: %s\n", 
                config.ssid.c_str(), config.isConfigured ? "true" : "false");
}

bool WiFiManager::connect() {
  if (config.ssid.empty() || !config.isConfigured) {
    Serial.println("No WiFi configuration available");
    return false;
  }
  
  Serial.printf("Connecting to WiFi: %s\n", config.ssid.c_str());
  
  WiFi.begin(config.ssid.c_str(), config.password.c_str());
  lastConnectionAttempt = millis();
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startTime < CONNECTION_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isConnected = true;
    Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
  } else {
    isConnected = false;
    Serial.println("\nWiFi connection failed");
    return false;
  }
}

bool WiFiManager::connectWithSmartConfig() {
  Serial.println("Starting SmartConfig...");
  
  // SmartConfigの開始
  WiFi.mode(WIFI_AP_STA);
  
  // プロビジョニングの開始
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_NONE,
                          WIFI_PROV_SECURITY_1, "Aimatix-Timer", "Aimatix WiFi Setup");
  
  // プロビジョニング完了を待機
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    M5.update();
    
    // ボタンCでSmartConfigをキャンセル
    if (M5.BtnC.wasPressed()) {
      Serial.println("SmartConfig cancelled by user");
      WiFiProv.stopProvisioning();
      WiFi.mode(WIFI_STA);
      return false;
    }
  }
  
  // 接続成功時の処理
  isConnected = true;
  config.ssid = WiFi.SSID().c_str();
  config.password = WiFi.psk().c_str();
  config.isConfigured = true;
  
  // 設定を保存
  saveWiFiConfig();
  
  // WiFiモードを通常モードに戻す
  WiFi.mode(WIFI_STA);
  
  Serial.printf("SmartConfig successful! SSID: %s, IP: %s\n", 
                config.ssid.c_str(), WiFi.localIP().toString().c_str());
  
  return true;
}

void WiFiManager::disconnect() {
  WiFi.disconnect();
  isConnected = false;
  Serial.println("WiFi disconnected");
}

bool WiFiManager::isWiFiConnected() const {
  return isConnected && WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::setWiFiConfig(const std::string& ssid, const std::string& password) {
  config.ssid = ssid;
  config.password = password;
  config.isConfigured = true;
  
  saveWiFiConfig();
  
  // 新しい設定で接続を試行
  return connect();
}

bool WiFiManager::clearWiFiConfig() {
  config.ssid.clear();
  config.password.clear();
  config.isConfigured = false;
  
  preferences.clear();
  
  // 現在の接続を切断
  disconnect();
  
  Serial.println("WiFi configuration cleared");
  return true;
}

WiFiConfig WiFiManager::getWiFiConfig() const {
  return config;
}

void WiFiManager::update() {
  // 接続状態の監視
  bool currentStatus = (WiFi.status() == WL_CONNECTED);
  
  if (currentStatus != isConnected) {
    isConnected = currentStatus;
    if (isConnected) {
      Serial.printf("WiFi reconnected! IP: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println("WiFi connection lost");
      
      // 自動再接続の試行
      if (millis() - lastConnectionAttempt > RECONNECT_INTERVAL) {
        Serial.println("Attempting to reconnect...");
        connect();
      }
    }
  }
}

void WiFiManager::resetConnectionAttempt() {
  lastConnectionAttempt = millis();
}

std::string WiFiManager::getLocalIP() const {
  if (isWiFiConnected()) {
    return WiFi.localIP().toString().c_str();
  }
  return "";
}

int WiFiManager::getRSSI() const {
  if (isWiFiConnected()) {
    return WiFi.RSSI();
  }
  return -100; // 接続していない場合は-100dBm
}

std::string WiFiManager::getSSID() const {
  if (isWiFiConnected()) {
    return WiFi.SSID().c_str();
  }
  return config.ssid;
} 