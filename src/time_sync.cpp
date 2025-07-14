#include "time_sync.h"
#include <M5Stack.h>

// シングルトンインスタンス
TimeSync* TimeSync::instance = nullptr;
TimeSync* timeSync = nullptr;

// エラーコード
enum TimeSyncError {
  TIME_SYNC_SUCCESS = 0,
  TIME_SYNC_WIFI_NOT_CONNECTED = -1,
  TIME_SYNC_NTP_FAILED = -2,
  TIME_SYNC_TIMEOUT = -3,
  TIME_SYNC_NOT_INITIALIZED = -4
};

TimeSync::TimeSync() 
  : timeClient(ntpUDP, "ntp.nict.jp", 32400, 3600), // JST (+9:00)
    isInitialized(false), 
    syncCompleted(false), 
    lastSyncTime(0) {
}

TimeSync* TimeSync::getInstance() {
  if (instance == nullptr) {
    instance = new TimeSync();
  }
  return instance;
}

bool TimeSync::initialize() {
  if (isInitialized) {
    return true;
  }
  
  // NTPクライアントの初期化
  timeClient.begin();
  timeClient.setUpdateInterval(SYNC_INTERVAL);
  
  isInitialized = true;
  Serial.println("TimeSync initialized successfully");
  return true;
}

void TimeSync::begin() {
  if (!isInitialized) {
    Serial.println("TimeSync not initialized");
    return;
  }
  
  // 初回同期の実行
  if (syncTimeWithWiFiOff()) {
    Serial.println("Initial time sync completed");
  } else {
    Serial.println("Initial time sync failed");
  }
}

bool TimeSync::syncTime() {
  if (!isInitialized) {
    Serial.println("TimeSync not initialized");
    return false;
  }
  
  // WiFi接続確認
  if (!wifiManager->isWiFiConnected()) {
    Serial.println("WiFi not connected for time sync");
    return false;
  }
  
  return performNTPSync();
}

bool TimeSync::syncTimeWithWiFiOff() {
  if (!isInitialized) {
    Serial.println("TimeSync not initialized");
    return false;
  }
  
  // WiFi接続確認
  if (!wifiManager->isWiFiConnected()) {
    Serial.println("WiFi not connected for time sync");
    return false;
  }
  
  Serial.println("Starting time sync with WiFi auto-off...");
  
  // NTP同期実行
  bool syncResult = performNTPSync();
  
  if (syncResult) {
    Serial.println("Time sync completed successfully");
    
    // 同期完了後、WiFiを自動OFF
    Serial.println("Turning off WiFi after time sync...");
    wifiManager->disconnect();
    
    return true;
  } else {
    Serial.println("Time sync failed");
    return false;
  }
}

bool TimeSync::performNTPSync() {
  Serial.println("Performing NTP sync...");
  
  // 複数回のリトライ
  for (int retry = 0; retry < SYNC_RETRY_COUNT; retry++) {
    Serial.printf("NTP sync attempt %d/%d\n", retry + 1, SYNC_RETRY_COUNT);
    
    // NTP更新の実行
    if (timeClient.update()) {
      time_t epochTime = timeClient.getEpochTime();
      
      // システム時刻の設定
      setSystemTime(epochTime);
      
      // 同期完了フラグの設定
      syncCompleted = true;
      lastSyncTime = millis();
      
      Serial.printf("NTP sync successful! Epoch time: %ld\n", epochTime);
      return true;
    } else {
      Serial.printf("NTP sync attempt %d failed\n", retry + 1);
      delay(1000); // 1秒待機してリトライ
    }
  }
  
  Serial.println("All NTP sync attempts failed");
  return false;
}

void TimeSync::setSystemTime(time_t epochTime) {
  struct timeval tv;
  tv.tv_sec = epochTime;
  tv.tv_usec = 0;
  
  if (settimeofday(&tv, nullptr) == 0) {
    Serial.printf("System time set to: %ld\n", epochTime);
  } else {
    Serial.println("Failed to set system time");
  }
}

bool TimeSync::isTimeSynced() const {
  return syncCompleted && isInitialized;
}

void TimeSync::update() {
  if (!isInitialized) {
    return;
  }
  
  // 定期的な同期チェック（WiFi接続時のみ）
  if (wifiManager->isWiFiConnected() && isSyncRequired()) {
    Serial.println("Periodic time sync check...");
    syncTimeWithWiFiOff();
  }
}

void TimeSync::resetSync() {
  syncCompleted = false;
  lastSyncTime = 0;
  Serial.println("Time sync reset");
}

time_t TimeSync::getLastSyncTime() const {
  return lastSyncTime;
}

std::string TimeSync::getFormattedTime() const {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  
  return std::string(buffer);
}

bool TimeSync::isSyncRequired() const {
  if (!syncCompleted) {
    return true; // 初回同期が必要
  }
  
  // 最後の同期から1時間経過しているかチェック
  return (millis() - lastSyncTime) > SYNC_INTERVAL;
}

int TimeSync::getLastError() const {
  // 簡易的なエラーコード（実際の実装ではより詳細なエラー管理が必要）
  if (!isInitialized) {
    return TIME_SYNC_NOT_INITIALIZED;
  }
  
  if (!wifiManager->isWiFiConnected()) {
    return TIME_SYNC_WIFI_NOT_CONNECTED;
  }
  
  if (!syncCompleted) {
    return TIME_SYNC_NTP_FAILED;
  }
  
  return TIME_SYNC_SUCCESS;
}

std::string TimeSync::getLastErrorMessage() const {
  int error = getLastError();
  
  switch (error) {
    case TIME_SYNC_SUCCESS:
      return "Success";
    case TIME_SYNC_WIFI_NOT_CONNECTED:
      return "WiFi not connected";
    case TIME_SYNC_NTP_FAILED:
      return "NTP sync failed";
    case TIME_SYNC_TIMEOUT:
      return "Sync timeout";
    case TIME_SYNC_NOT_INITIALIZED:
      return "Not initialized";
    default:
      return "Unknown error";
  }
} 