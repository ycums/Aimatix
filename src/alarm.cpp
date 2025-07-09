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
