#include <unity.h>
#include "ui.h"
#include "mock_m5stack.h"

// テスト用のモック関数
extern unsigned long mockMillis;

// テスト用のグローバル変数
extern TFT_eSprite sprite;

// テスト前のセットアップ
void setUp(void) {
  mockMillis = 0;
  // 警告メッセージ状態をリセット
  clearWarningMessage();
}

// テスト後のクリーンアップ
void tearDown(void) {
  clearWarningMessage();
}

// テスト1: 警告メッセージの表示
void test_show_warning_message() {
  const char* testMessage = "テスト警告メッセージ";
  
  showWarningMessage(testMessage);
  
  // 警告メッセージが表示されているかチェック
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_TRUE(isDisplayed);
  
  Serial.println("✓ 警告メッセージ表示テスト: 成功");
}

// テスト2: 警告メッセージの自動消去（時間経過）
void test_warning_message_auto_clear() {
  const char* testMessage = "自動消去テスト";
  
  showWarningMessage(testMessage);
  
  // 初期状態では表示されている
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 3秒経過をシミュレート
  mockMillis = 3000;
  
  // 時間経過後は自動的に消去される
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_FALSE(isDisplayed);
  
  Serial.println("✓ 警告メッセージ自動消去テスト: 成功");
}

// テスト3: 警告メッセージの手動消去
void test_warning_message_manual_clear() {
  const char* testMessage = "手動消去テスト";
  
  showWarningMessage(testMessage);
  
  // 初期状態では表示されている
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 手動で消去
  clearWarningMessage();
  
  // 消去後は表示されていない
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_FALSE(isDisplayed);
  
  Serial.println("✓ 警告メッセージ手動消去テスト: 成功");
}

// テスト4: 複数の警告メッセージの管理
void test_multiple_warning_messages() {
  const char* message1 = "メッセージ1";
  const char* message2 = "メッセージ2";
  
  // 最初のメッセージを表示
  showWarningMessage(message1);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message2));
  
  // 2番目のメッセージを表示（最初のメッセージは上書きされる）
  showWarningMessage(message2);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message2));
  
  Serial.println("✓ 複数警告メッセージ管理テスト: 成功");
}

// テスト5: 空のメッセージの処理
void test_empty_warning_message() {
  const char* emptyMessage = "";
  
  showWarningMessage(emptyMessage);
  
  // 空のメッセージでも正常に処理される
  bool isDisplayed = isWarningMessageDisplayed(emptyMessage);
  TEST_ASSERT_TRUE(isDisplayed);
  
  Serial.println("✓ 空メッセージ処理テスト: 成功");
}

// テスト6: 長いメッセージの処理
void test_long_warning_message() {
  const char* longMessage = "これは非常に長い警告メッセージで、バッファサイズの限界をテストするためのものです。";
  
  showWarningMessage(longMessage);
  
  // 長いメッセージでも正常に処理される（切り詰められる可能性がある）
  bool isDisplayed = isWarningMessageDisplayed(longMessage);
  TEST_ASSERT_TRUE(isDisplayed);
  
  Serial.println("✓ 長いメッセージ処理テスト: 成功");
}

// テスト7: 特殊文字を含むメッセージ
void test_special_characters_message() {
  const char* specialMessage = "特殊文字テスト: !@#$%^&*()_+-=[]{}|;':\",./<>?";
  
  showWarningMessage(specialMessage);
  
  // 特殊文字を含むメッセージでも正常に処理される
  bool isDisplayed = isWarningMessageDisplayed(specialMessage);
  TEST_ASSERT_TRUE(isDisplayed);
  
  Serial.println("✓ 特殊文字メッセージテスト: 成功");
}

// テスト8: 連続的な警告メッセージの表示
void test_consecutive_warning_messages() {
  const char* messages[] = {
    "メッセージ1",
    "メッセージ2", 
    "メッセージ3",
    "メッセージ4",
    "メッセージ5"
  };
  
  for (int i = 0; i < 5; i++) {
    showWarningMessage(messages[i]);
    TEST_ASSERT_TRUE(isWarningMessageDisplayed(messages[i]));
    
    // 前のメッセージは上書きされている
    for (int j = 0; j < i; j++) {
      TEST_ASSERT_FALSE(isWarningMessageDisplayed(messages[j]));
    }
  }
  
  Serial.println("✓ 連続警告メッセージテスト: 成功");
}

// テスト9: 警告メッセージの状態管理
void test_warning_message_state_management() {
  // 初期状態では警告メッセージは表示されていない
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(""));
  
  const char* testMessage = "状態管理テスト";
  showWarningMessage(testMessage);
  
  // 表示後は状態が正しく管理されている
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  clearWarningMessage();
  
  // クリア後は状態がリセットされている
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  Serial.println("✓ 警告メッセージ状態管理テスト: 成功");
}

// テスト10: 時間経過による状態変化
void test_warning_message_time_progression() {
  const char* testMessage = "時間経過テスト";
  
  showWarningMessage(testMessage);
  
  // 0ms: 表示開始
  mockMillis = 0;
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 1000ms: 1秒経過
  mockMillis = 1000;
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 2000ms: 2秒経過
  mockMillis = 2000;
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 3000ms: 3秒経過（自動消去）
  mockMillis = 3000;
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  // 4000ms: 4秒経過（消去状態維持）
  mockMillis = 4000;
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  Serial.println("✓ 時間経過による状態変化テスト: 成功");
}

// Arduinoフレームワーク用のsetup関数
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== 警告メッセージ機能テスト開始 ===");
  
  UNITY_BEGIN();
  
  // テストケースの実行
  RUN_TEST(test_show_warning_message);
  RUN_TEST(test_warning_message_auto_clear);
  RUN_TEST(test_warning_message_manual_clear);
  RUN_TEST(test_multiple_warning_messages);
  RUN_TEST(test_empty_warning_message);
  RUN_TEST(test_long_warning_message);
  RUN_TEST(test_special_characters_message);
  RUN_TEST(test_consecutive_warning_messages);
  RUN_TEST(test_warning_message_state_management);
  RUN_TEST(test_warning_message_time_progression);
  
  UNITY_END();
  
  Serial.println("=== 警告メッセージ機能テスト完了 ===");
  Serial.println("全テストが完了しました！");
}

// Arduinoフレームワーク用のloop関数
void loop() {
  // テスト完了後は何もしない
  delay(1000);
} 