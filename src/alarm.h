#ifndef ALARM_H
#define ALARM_H

#include <vector>
#include <time.h>
#include <algorithm>

// Global variables (declared in alarm.cpp)
extern std::vector<time_t> alarmTimes;

// Function prototypes
void sortAlarms();
time_t getNextAlarmTime();
void playAlarm();
void stopAlarm();

#endif // ALARM_H
