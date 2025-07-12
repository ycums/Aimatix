#include "alarm.h"
#include <M5Stack.h>
#include "settings.h" // settings構造体を参照するため

// Global variables
std::vector<time_t> alarmTimes;

// デバッグ用：初期値を5件追加
void addDebugAlarms() {
  time_t now = time(NULL);
  struct tm *timeinfo = localtime(&now);
  
  // 現在時刻から1時間後、2時間後、3時間後、4時間後、5時間後の時刻を追加
  for (int i = 1; i <= 5; i++) {
    time_t debugTime = now + (i * 3600); // i時間後
    alarmTimes.push_back(debugTime);
  }
  
  // 時刻順にソート
  std::sort(alarmTimes.begin(), alarmTimes.end());
}

void sortAlarms() {
  if (alarmTimes.empty()) return;
  std::sort(alarmTimes.begin(), alarmTimes.end());
}

time_t getNextAlarmTime() {
  time_t now = time(NULL);
  time_t next = 0;
  
  for (time_t t : alarmTimes) {
    if (t > now) {
      next = t;
      break;
    }
  }
  return next;
}

void playAlarm() {
  // settings構造体を参照
  if (settings.sound_enabled) {
    M5.Speaker.tone(880, 500);  // 880Hz, 500ms
  }
  // バイブレーション機能は非搭載のため削除
}

void stopAlarm() {
  // settings構造体を参照
  if (settings.sound_enabled) {
    M5.Speaker.mute();
  }
  // バイブレーション停止は不要
}

void removePastAlarms() {
  time_t now = time(NULL);
  alarmTimes.erase(std::remove_if(alarmTimes.begin(), alarmTimes.end(), [now](time_t t){ return t <= now; }), alarmTimes.end());
}
