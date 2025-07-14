#include <unity.h>
#include <cstdio>
#include <cstring>
#include <ctime>

// WarningMessagesの純粋ロジックテスト
// M5Stack依存を排除し、標準C++のみでテスト

// 警告メッセージ状態管理のモック実装
static unsigned long mockMillis = 0;
static unsigned long warningStartTime = 0;
static char currentWarningMessage[64] = "";
static bool warningActive = false;

// モック関数
unsigned long millis() {
  return mockMillis;
}

void setMockTime(unsigned long time) {
  mockMillis = time;
}

// 警告メッセージ表示機能の純粋ロジック
void showWarningMessage(const char* message, unsigned long duration = 3000) {
  strncpy(currentWarningMessage, message, sizeof(currentWarningMessage) - 1);
  currentWarningMessage[sizeof(currentWarningMessage) - 1] = '\0';
  warningStartTime = mockMillis;
  warningActive = true;
}

bool isWarningMessageDisplayed(const char* message) {
  if (!warningActive) return false;
  
  // 指定されたメッセージと一致するかチェック
  if (strcmp(currentWarningMessage, message) == 0) {
    // 表示時間が経過したかチェック
    if (mockMillis - warningStartTime >= 3000) {
      warningActive = false;
      return false;
    }
    return true;
  }
  return false;
}

void clearWarningMessage() {
  warningActive = false;
  currentWarningMessage[0] = '\0';
}

// 警告メッセージ表示テスト
void test_show_warning_message() {
  const char* testMessage = "テスト警告メッセージ";
  
  // 初期状態の確認
  TEST_ASSERT_FALSE(warningActive);
  TEST_ASSERT_EQUAL(0, strlen(currentWarningMessage));
  
  // 警告メッセージを表示
  showWarningMessage(testMessage);
  
  // 表示後の状態確認
  TEST_ASSERT_TRUE(warningActive);
  TEST_ASSERT_EQUAL_STRING(testMessage, currentWarningMessage);
  
  printf("✓ 警告メッセージ表示テスト: 成功\n");
}

// 警告メッセージ自動クリアテスト
void test_warning_message_auto_clear() {
  const char* testMessage = "自動クリアテスト";
  
  // 警告メッセージを表示
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 時間経過前（まだ表示中）
  setMockTime(1000);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 時間経過後（自動クリア）
  setMockTime(4000);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  TEST_ASSERT_FALSE(warningActive);
  
  printf("✓ 警告メッセージ自動クリアテスト: 成功\n");
}

// 警告メッセージ手動クリアテスト
void test_warning_message_manual_clear() {
  const char* testMessage = "手動クリアテスト";
  
  // 警告メッセージを表示
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 手動でクリア
  clearWarningMessage();
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  TEST_ASSERT_FALSE(warningActive);
  TEST_ASSERT_EQUAL(0, strlen(currentWarningMessage));
  
  printf("✓ 警告メッセージ手動クリアテスト: 成功\n");
}

// 複数警告メッセージテスト
void test_multiple_warning_messages() {
  const char* message1 = "メッセージ1";
  const char* message2 = "メッセージ2";
  
  // 最初のメッセージを表示
  showWarningMessage(message1);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message2));
  
  // 2番目のメッセージを表示（上書き）
  showWarningMessage(message2);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message2));
  
  // クリア
  clearWarningMessage();
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message2));
  
  printf("✓ 複数警告メッセージテスト: 成功\n");
}

// 空の警告メッセージテスト
void test_empty_warning_message() {
  const char* emptyMessage = "";
  
  // 空のメッセージを表示
  showWarningMessage(emptyMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(emptyMessage));
  TEST_ASSERT_EQUAL(0, strlen(currentWarningMessage));
  
  printf("✓ 空の警告メッセージテスト: 成功\n");
}

// 長い警告メッセージテスト
void test_long_warning_message() {
  const char* longMessage = "これは非常に長い警告メッセージで、バッファの境界をテストするためのものです。";
  
  // 長いメッセージを表示
  showWarningMessage(longMessage);
  
  // バッファサイズ制限の確認（63文字まで）
  TEST_ASSERT_LESS_OR_EQUAL(63, strlen(currentWarningMessage));
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(longMessage));
  
  printf("✓ 長い警告メッセージテスト: 成功\n");
}

// 特殊文字を含む警告メッセージテスト
void test_special_characters_warning_message() {
  const char* specialMessage = "特殊文字: !@#$%^&*()_+-=[]{}|;':\",./<>?";
  
  // 特殊文字を含むメッセージを表示
  showWarningMessage(specialMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(specialMessage));
  
  printf("✓ 特殊文字を含む警告メッセージテスト: 成功\n");
}

// 連続警告メッセージテスト
void test_consecutive_warning_messages() {
  const char* messages[] = {
    "メッセージ1",
    "メッセージ2", 
    "メッセージ3",
    "メッセージ4",
    "メッセージ5"
  };
  
  // 連続してメッセージを表示
  for (int i = 0; i < 5; i++) {
    showWarningMessage(messages[i]);
    TEST_ASSERT_TRUE(isWarningMessageDisplayed(messages[i]));
    
    // 他のメッセージは表示されていないことを確認
    for (int j = 0; j < 5; j++) {
      if (i != j) {
        TEST_ASSERT_FALSE(isWarningMessageDisplayed(messages[j]));
      }
    }
  }
  
  printf("✓ 連続警告メッセージテスト: 成功\n");
}

// 警告メッセージ状態管理テスト
void test_warning_message_state_management() {
  // 初期状態
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(""));
  
  // メッセージ表示
  const char* testMessage = "状態管理テスト";
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  // 手動クリア
  clearWarningMessage();
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  // 再度表示
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  printf("✓ 警告メッセージ状態管理テスト: 成功\n");
}

// 警告メッセージ時間進行テスト
void test_warning_message_time_progression() {
  const char* testMessage = "時間進行テスト";
  
  // メッセージ表示
  setMockTime(0);
  showWarningMessage(testMessage);
  
  // 時間進行とともに状態を確認
  setMockTime(1000);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  setMockTime(2000);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  setMockTime(3000);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  setMockTime(4000);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  setMockTime(5000);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(testMessage));
  
  printf("✓ 警告メッセージ時間進行テスト: 成功\n");
}

void setUp(void) {
  // テスト前の初期化
  clearWarningMessage();
  setMockTime(0);
}

void tearDown(void) {
  // テスト後のクリーンアップ
  clearWarningMessage();
}

// メイン関数
int main() {
  UNITY_BEGIN();
  
  printf("=== WarningMessages 純粋ロジックテスト ===\n");
  
  RUN_TEST(test_show_warning_message);
  RUN_TEST(test_warning_message_auto_clear);
  RUN_TEST(test_warning_message_manual_clear);
  RUN_TEST(test_multiple_warning_messages);
  RUN_TEST(test_empty_warning_message);
  RUN_TEST(test_long_warning_message);
  RUN_TEST(test_special_characters_warning_message);
  RUN_TEST(test_consecutive_warning_messages);
  RUN_TEST(test_warning_message_state_management);
  RUN_TEST(test_warning_message_time_progression);
  
  printf("=== 全テスト完了 ===\n");
  
  return UNITY_END();
} 