#ifndef TYPES_H
#define TYPES_H

// Mode definition
enum Mode {
  MAIN_DISPLAY,
  NTP_SYNC,
  ABS_TIME_INPUT,
  REL_PLUS_TIME_INPUT,
  REL_MINUS_TIME_INPUT,
  SCHEDULE_SELECT,
  ALARM_MANAGEMENT,  // アラーム管理画面を追加
  ALARM_ACTIVE,
  SETTINGS_MENU,
  INFO_DISPLAY,
  WARNING_COLOR_TEST
};

// 設定メニュー用の構造体
struct SettingsMenu {
  const char* items[5] = {
    "SOUND: ",
    "LCD BRIGHTNESS",
    "WARNING COLOR TEST",
    "ALL CLEAR",
    "INFO"
  };
  int selectedItem = 0;
  int itemCount = 5;
};

// Global variables (defined in main.cpp)
extern SettingsMenu settingsMenu;

#endif // TYPES_H
