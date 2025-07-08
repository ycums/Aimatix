#include "alarm.h"
#include <M5Stack.h>
#include "settings.h" // settings構造体を参照するため

// Global variables
std::vector<time_t> alarmTimes;

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
  if (settings.vibration_enabled) {
    // バイブレーション機能（ハードウェア依存）
    // M5Stack Fireのバイブレーション制御ピンを確認し、実装する
  }
}

void stopAlarm() {
  // settings構造体を参照
  if (settings.sound_enabled) {
    M5.Speaker.mute();
  }
  // バイブレーション停止（ハードウェア依存）
  // M5Stack Fireのバイブレーション制御ピンを確認し、実装する
}
