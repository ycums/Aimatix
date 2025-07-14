#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include "wifi_manager.h"

// NTP同期管理クラス
class TimeSync {
private:
  static TimeSync* instance;
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  bool isInitialized;
  bool syncCompleted;
  unsigned long lastSyncTime;
  static const unsigned long SYNC_INTERVAL = 3600000; // 1時間
  static const unsigned long SYNC_TIMEOUT = 10000; // 10秒
  static const int SYNC_RETRY_COUNT = 3;

  TimeSync();
  bool performNTPSync();
  void setSystemTime(time_t epochTime);

public:
  static TimeSync* getInstance();
  
  // 初期化
  bool initialize();
  void begin();
  
  // 同期管理
  bool syncTime();
  bool syncTimeWithWiFiOff();
  bool isTimeSynced() const;
  
  // 状態管理
  void update();
  void resetSync();
  
  // ユーティリティ
  time_t getLastSyncTime() const;
  std::string getFormattedTime() const;
  bool isSyncRequired() const;
  
  // エラーハンドリング
  int getLastError() const;
  std::string getLastErrorMessage() const;
};

// グローバル関数
extern TimeSync* timeSync;

#endif // TIME_SYNC_H 