// 入力モードのロジック実装
#include "input.h"
#include "alarm.h"
#include <algorithm>

// 入力値をバリデーションし、アラームリストに追加
bool confirmInputAndAddAlarm() {
  // 時刻バリデーション（0-23, 0-59）
  if (inputState.hours < 0 || inputState.hours > 23) return false;
  if (inputState.minutes < 0 || inputState.minutes > 59) return false;
  // 未来時刻のみ許可
  time_t now = time(NULL);
  struct tm tminfo;
  localtime_r(&now, &tminfo);
  tminfo.tm_hour = inputState.hours;
  tminfo.tm_min = inputState.minutes;
  tminfo.tm_sec = 0;
  time_t alarmT = mktime(&tminfo);
  if (alarmT <= now) return false;
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
