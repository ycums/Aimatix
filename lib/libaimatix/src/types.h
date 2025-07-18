#ifndef TYPES_H
#define TYPES_H

// Mode definition - 不要なモードを削除し、長押し処理に対応
enum Mode {
  MAIN_DISPLAY,           // メイン表示画面
  NTP_SYNC,              // NTP同期画面
  ABS_TIME_INPUT,        // 絶対時刻入力画面
  REL_PLUS_TIME_INPUT,   // 相対時刻加算入力画面
  ALARM_MANAGEMENT,      // アラーム管理画面
  ALARM_ACTIVE,          // アラーム鳴動中
  SETTINGS_MENU,         // 設定メニュー
  INFO_DISPLAY,          // 情報表示画面
  WARNING_COLOR_TEST     // 警告色テスト画面
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
