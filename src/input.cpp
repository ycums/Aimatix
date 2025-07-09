// 入力モードのロジック実装
#include "input.h"
#include "alarm.h"
#include <algorithm>
#include "types.h"
extern Mode currentMode;

// 入力値をバリデーションし、アラームリストに追加
bool confirmInputAndAddAlarm() {
  // 時刻バリデーション（0-23, 0-59）
  if (inputState.hours < 0 || inputState.hours > 23) return false;
  if (inputState.minutes < 0 || inputState.minutes > 59) return false;
  time_t now = time(NULL);
  time_t alarmT = 0;
  if (currentMode == REL_PLUS_TIME_INPUT || currentMode == REL_MINUS_TIME_INPUT) {
    // 相対時刻入力: 現在時刻±入力値
    struct tm tminfo;
    localtime_r(&now, &tminfo);
    int total = inputState.hours * 60 + inputState.minutes;
    if (currentMode == REL_MINUS_TIME_INPUT) total = -total;
    // 現在時刻+total分
    time_t base = now;
    base += total * 60;
    struct tm t;
    localtime_r(&base, &t);
    t.tm_sec = 0;
    alarmT = mktime(&t);
    if (alarmT <= now) return false;
  } else {
    // 絶対時刻入力
    struct tm tminfo;
    localtime_r(&now, &tminfo);
    tminfo.tm_hour = inputState.hours;
    tminfo.tm_min = inputState.minutes;
    tminfo.tm_sec = 0;
    alarmT = mktime(&tminfo);
    if (alarmT <= now) return false;
  }
  // 既存重複チェック
  if (std::find(alarmTimes.begin(), alarmTimes.end(), alarmT) != alarmTimes.end()) return false;
  // 最大件数制限
  if (alarmTimes.size() >= 5) return false;
  alarmTimes.push_back(alarmT);
  sortAlarms();
  return true;
}
#include "input.h"

// 入力モード用の変数
InputState inputState;

void resetInput() {
  inputState = InputState();
}
