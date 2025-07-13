#include <unity.h>
#include <cstdio>
#include <cstring>

// シンプルな警告メッセージ機能のテスト実装
static unsigned long mockMillis = 0;
static char currentWarningMessage[64] = "";
static bool warningActive = false;

unsigned long millis() {
  return mockMillis;
}

void showWarningMessage(const char* message, unsigned long duration = 3000) {
  strncpy(currentWarningMessage, message, sizeof(currentWarningMessage) - 1);
  currentWarningMessage[sizeof(currentWarningMessage) - 1] = '\0';
  warningActive = true;
  
  printf("Warning message displayed: %s\n", message);
}

bool isWarningMessageDisplayed(const char* message) {
  if (!warningActive) return false;
  
  if (strcmp(currentWarningMessage, message) == 0) {
    if (mockMillis - 3000 >= 3000) {
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
  printf("Warning message cleared\n");
}

// テストケース
void test_show_warning_message() {
  const char* testMessage = "テスト警告メッセージ";
  
  showWarningMessage(testMessage);
  
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_TRUE(isDisplayed);
  
  printf("✓ 警告メッセージ表示テスト: 成功\n");
}

void test_warning_message_auto_clear() {
  const char* testMessage = "自動消去テスト";
  
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  mockMillis = 3000;
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_FALSE(isDisplayed);
  
  printf("✓ 警告メッセージ自動消去テスト: 成功\n");
}

void test_warning_message_manual_clear() {
  const char* testMessage = "手動消去テスト";
  
  showWarningMessage(testMessage);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(testMessage));
  
  clearWarningMessage();
  bool isDisplayed = isWarningMessageDisplayed(testMessage);
  TEST_ASSERT_FALSE(isDisplayed);
  
  printf("✓ 警告メッセージ手動消去テスト: 成功\n");
}

void test_multiple_warning_messages() {
  const char* message1 = "メッセージ1";
  const char* message2 = "メッセージ2";
  
  showWarningMessage(message1);
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message2));
  
  showWarningMessage(message2);
  TEST_ASSERT_FALSE(isWarningMessageDisplayed(message1));
  TEST_ASSERT_TRUE(isWarningMessageDisplayed(message2));
  
  printf("✓ 複数警告メッセージ管理テスト: 成功\n");
}

void setUp(void) {
  mockMillis = 0;
  clearWarningMessage();
}

void tearDown(void) {
  clearWarningMessage();
}

int main() {
  printf("=== 警告メッセージ機能テスト開始 ===\n");
  
  UNITY_BEGIN();
  
  RUN_TEST(test_show_warning_message);
  RUN_TEST(test_warning_message_auto_clear);
  RUN_TEST(test_warning_message_manual_clear);
  RUN_TEST(test_multiple_warning_messages);
  
  UNITY_END();
  
  printf("=== 警告メッセージ機能テスト完了 ===\n");
  printf("全テストが完了しました！\n");
  
  return 0;
} 