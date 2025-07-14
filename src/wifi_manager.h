#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiProv.h>
#include <Preferences.h>
#include <string>

// Wi-Fi設定用の構造体
struct WiFiConfig {
  std::string ssid;
  std::string password;
  bool isConfigured;
};

// Wi-Fi管理クラス
class WiFiManager {
private:
  static WiFiManager* instance;
  Preferences preferences;
  WiFiConfig config;
  bool isConnected;
  unsigned long lastConnectionAttempt;
  static const unsigned long CONNECTION_TIMEOUT = 20000; // 20秒
  static const unsigned long RECONNECT_INTERVAL = 30000; // 30秒

  WiFiManager();
  bool loadWiFiConfig();
  void saveWiFiConfig();
  void startSmartConfig();
  void stopSmartConfig();

public:
  static WiFiManager* getInstance();
  
  // 初期化
  bool initialize();
  void begin();
  
  // 接続管理
  bool connect();
  bool connectWithSmartConfig();
  void disconnect();
  bool isWiFiConnected() const;
  
  // 設定管理
  bool setWiFiConfig(const std::string& ssid, const std::string& password);
  bool clearWiFiConfig();
  WiFiConfig getWiFiConfig() const;
  
  // 状態管理
  void update();
  void resetConnectionAttempt();
  
  // ユーティリティ
  std::string getLocalIP() const;
  int getRSSI() const;
  std::string getSSID() const;
};

// グローバル関数
extern WiFiManager* wifiManager;

#endif // WIFI_MANAGER_H 