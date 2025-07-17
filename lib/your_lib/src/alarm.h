#ifndef ALARM_H
#define ALARM_H

#include <vector>
#include <time.h>
#include <algorithm>
#include "../include/ISpeaker.h"

// Global variables (declared in alarm.cpp)
extern std::vector<time_t> alarmTimes;

// Function prototypes
void addDebugAlarms(); // デバッグ用：初期値を追加
void sortAlarms();
time_t getNextAlarmTime();
void playAlarm(ISpeaker* speaker, bool soundEnabled);
void stopAlarm(ISpeaker* speaker, bool soundEnabled);
void removePastAlarms();

#endif // ALARM_H
