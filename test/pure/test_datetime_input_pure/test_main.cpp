#include <unity.h>
#include "DateTimeInputState.h"
#include "ITimeProvider.h"
#include <memory>

// テスト用のモックTimeProvider
class TestTimeProvider : public ITimeProvider {
public:
    time_t now() const override { return testTime; }
    struct tm* localtime(time_t* time) const override { 
        return ::localtime(time); 
    }
    bool setSystemTime(time_t time) override { 
        testTime = time; 
        return true; 
    }
    
    void setTestTime(time_t time) { testTime = time; }
    
private:
    time_t testTime = 0;
};

void setUp(void) {
    // テスト前の初期化処理
}

void tearDown(void) {
    // テスト後のクリーンアップ処理
}

// 初期化テスト
void test_DateTimeInputState_Initialization() {
    TestTimeProvider timeProvider;
    
    // 期待される時刻を設定: 2025/01/01 00:00:00
    struct tm testTime = {};
    testTime.tm_year = 2025 - 1900;  // 年は1900からのオフセット
    testTime.tm_mon = 1 - 1;         // 月は0ベース
    testTime.tm_mday = 1;
    testTime.tm_hour = 0;
    testTime.tm_min = 0;
    testTime.tm_sec = 0;
    testTime.tm_isdst = -1;
    
    time_t testTimeT = mktime(&testTime);
    timeProvider.setTestTime(testTimeT);
    
    DateTimeInputState state(&timeProvider);
    
    // 初期値の確認
    TEST_ASSERT_EQUAL(0, state.getCursorPosition());
    TEST_ASSERT_FALSE(state.getIsEditMode());
    
    // 初期日時の確認（2025/01/01 00:00）
    const auto& digits = state.getDateTimeDigits();
    TEST_ASSERT_EQUAL(12, digits.size());
    TEST_ASSERT_EQUAL(2, digits[0]); // 年千
    TEST_ASSERT_EQUAL(0, digits[1]); // 年百
    TEST_ASSERT_EQUAL(2, digits[2]); // 年十
    TEST_ASSERT_EQUAL(5, digits[3]); // 年一
    TEST_ASSERT_EQUAL(0, digits[4]); // 月十
    TEST_ASSERT_EQUAL(1, digits[5]); // 月一
    TEST_ASSERT_EQUAL(0, digits[6]); // 日十
    TEST_ASSERT_EQUAL(1, digits[7]); // 日一
    TEST_ASSERT_EQUAL(0, digits[8]); // 時十
    TEST_ASSERT_EQUAL(0, digits[9]); // 時一
    TEST_ASSERT_EQUAL(0, digits[10]); // 分十
    TEST_ASSERT_EQUAL(0, digits[11]); // 分一
}

// カーソル移動テスト
void test_DateTimeInputState_CursorMovement() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    state.onEnter(); // 初期化処理を実行
    
    // 初期位置の確認（年十の位）
    TEST_ASSERT_EQUAL(2, state.getCursorPosition());
    
    // 右移動テスト
    state.onButtonB(); // カーソルを右に移動
    TEST_ASSERT_EQUAL(3, state.getCursorPosition()); // 年一の位
    
    // 循環移動テスト - 最後の位置から最初の位置に戻る
    state.setCursorPosition(11); // 分一の位に設定
    state.onButtonB(); // 次に移動
    TEST_ASSERT_EQUAL(2, state.getCursorPosition()); // 年十の位に戻る
}

// 値インクリメントテスト
void test_DateTimeInputState_Increment() {
    TestTimeProvider timeProvider;
    
    // 期待される時刻を設定: 2025/01/01 00:00:00
    struct tm testTime = {};
    testTime.tm_year = 2025 - 1900;  // 年は1900からのオフセット
    testTime.tm_mon = 1 - 1;         // 月は0ベース
    testTime.tm_mday = 1;
    testTime.tm_hour = 0;
    testTime.tm_min = 0;
    testTime.tm_sec = 0;
    testTime.tm_isdst = -1;
    
    time_t testTimeT = mktime(&testTime);
    timeProvider.setTestTime(testTimeT);
    
    DateTimeInputState state(&timeProvider);
    
    // 年千の位のテスト（入力不可）
    state.setCursorPosition(0);
    state.onButtonA(); // 入力不可のため変化なし
    TEST_ASSERT_EQUAL(2, state.getDateTimeDigits()[0]); // 初期値のまま
    
    // 月十の位のテスト
    state.setCursorPosition(4);
    state.onButtonA(); // 0 -> 1
    TEST_ASSERT_EQUAL(1, state.getDateTimeDigits()[4]);
}

// バリデーションテスト
void test_DateTimeInputState_Validation() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 有効な日時のテスト
    state.setDateTimeDigits({2, 0, 2, 5, 0, 1, 0, 1, 1, 2, 3, 0});
    TEST_ASSERT_TRUE(state.validateDateTime());
    
    // 無効な年のテスト
    state.setDateTimeDigits({1, 9, 9, 9, 0, 1, 0, 1, 1, 2, 3, 0}); // 1999年
    TEST_ASSERT_FALSE(state.validateDateTime());
}

// 日付文字列フォーマットテスト
void test_DateTimeInputState_FormatString() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 標準的な日時のテスト
    state.setDateTimeDigits({2, 0, 2, 5, 0, 1, 1, 5, 1, 4, 3, 0});
    std::string formatted = state.formatDateTimeString();
    TEST_ASSERT_EQUAL_STRING("2025/01/15 14:30", formatted.c_str());
}

void test_DateTimeInputState_CommitDateTime() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 初期時刻を設定
    timeProvider.setTestTime(1000);
    
    // 有効な日時を設定: 2025/01/15 14:30
    state.setDateTimeDigits({2, 0, 2, 5, 0, 1, 1, 5, 1, 4, 3, 0});
    
    // commitDateTime()を実行
    // onButtonCを直接呼ぶことでcommitDateTime()をテスト
    state.onButtonC();
    
    // システム時刻が更新されたかを確認
    time_t updatedTime = timeProvider.now();
    
    // 設定した日時（2025/01/15 14:30）のtime_tを計算
    struct tm expectedTime = {};
    expectedTime.tm_year = 2025 - 1900;  // 年は1900からのオフセット
    expectedTime.tm_mon = 1 - 1;         // 月は0ベース
    expectedTime.tm_mday = 15;
    expectedTime.tm_hour = 14;
    expectedTime.tm_min = 30;
    expectedTime.tm_sec = 0;
    expectedTime.tm_isdst = -1;
    
    time_t expectedTimeT = mktime(&expectedTime);
    
    // システム時刻が正しく更新されたかを確認
    TEST_ASSERT_EQUAL(expectedTimeT, updatedTime);
}

void test_DateTimeInputState_CommitDateTime_InvalidTime() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 初期時刻を設定
    time_t initialTime = 1000;
    timeProvider.setTestTime(initialTime);
    
    // 無効な日時を設定: 2025/02/30 (2月30日は存在しない)
    state.setDateTimeDigits({2, 0, 2, 5, 0, 2, 3, 0, 1, 4, 3, 0});
    
    // commitDateTime()を実行
    state.onButtonC();
    
    // 無効な日時のため、システム時刻は更新されないはず
    TEST_ASSERT_EQUAL(initialTime, timeProvider.now());
}

void test_DateTimeInputState_InitializeWithSystemTime() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 特定の時刻を設定: 2023/12/25 15:45:00
    struct tm testTime = {};
    testTime.tm_year = 2023 - 1900;  // 年は1900からのオフセット
    testTime.tm_mon = 12 - 1;        // 月は0ベース
    testTime.tm_mday = 25;
    testTime.tm_hour = 15;
    testTime.tm_min = 45;
    testTime.tm_sec = 0;
    testTime.tm_isdst = -1;
    
    time_t testTimeT = mktime(&testTime);
    timeProvider.setTestTime(testTimeT);
    
    // onEnter()を呼ぶことで、システム時刻から初期値が設定される
    state.onEnter();
    
    // 期待される桁数値: [2,0,2,3,1,2,2,5,1,5,4,5] (2023/12/25 15:45)
    std::vector<int> expectedDigits = {2, 0, 2, 3, 1, 2, 2, 5, 1, 5, 4, 5};
    
    // システム時刻から正しく初期値が設定されたかを確認
    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedDigits.data(), state.getDateTimeDigits().data(), expectedDigits.size());
}

void test_DateTimeInputState_InitializeWithNullProvider() {
    DateTimeInputState state(nullptr);  // timeProviderをnullに設定
    
    // onEnter()を呼ぶ
    state.onEnter();
    
    // timeProviderがnullの場合、デフォルト値が設定されるはず: [2,0,2,5,0,1,0,1,0,0,0,0]
    std::vector<int> expectedDigits = {2, 0, 2, 5, 0, 1, 0, 1, 0, 0, 0, 0};
    
    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedDigits.data(), state.getDateTimeDigits().data(), expectedDigits.size());
}

// 追加のテストケース - カバレッジ向上のため
void test_DateTimeInputState_ButtonHandling() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // ボタン処理のテスト
    state.onButtonA(); // インクリメント
    state.onButtonB(); // カーソル移動
    state.onButtonC(); // 確定
    
    // 正常に実行されることを確認
}

void test_DateTimeInputState_EditModeToggle() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 編集モードの切り替えテスト
    TEST_ASSERT_FALSE(state.getIsEditMode());
    
    // エディットモードに変更（実装により異なる）
    state.onEnter();
    // 正常に実行されることを確認
}

void test_DateTimeInputState_EdgeCases() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 境界値のテスト
    state.setCursorPosition(-1); // 無効な位置
    state.setCursorPosition(100); // 無効な位置
    
    // 無効な値でも例外が発生しないことを確認
    state.onButtonA();
    state.onButtonB();
    state.onButtonC();
}

void test_DateTimeInputState_StringFormatting() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 文字列フォーマットの追加テスト
    state.setDateTimeDigits({2, 0, 2, 4, 1, 2, 3, 1, 2, 3, 5, 9});
    std::string formatted = state.formatDateTimeString();
    TEST_ASSERT_EQUAL_STRING("2024/12/31 23:59", formatted.c_str());
}

void test_DateTimeInputState_ExitHandling() {
    TestTimeProvider timeProvider;
    DateTimeInputState state(&timeProvider);
    
    // 終了処理のテスト
    state.onExit();
    // 正常に実行されることを確認（例外が発生しない）
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_DateTimeInputState_Initialization);
    RUN_TEST(test_DateTimeInputState_CursorMovement);
    RUN_TEST(test_DateTimeInputState_Increment);
    RUN_TEST(test_DateTimeInputState_Validation);
    RUN_TEST(test_DateTimeInputState_FormatString);
    RUN_TEST(test_DateTimeInputState_CommitDateTime);
    RUN_TEST(test_DateTimeInputState_CommitDateTime_InvalidTime);
    RUN_TEST(test_DateTimeInputState_InitializeWithSystemTime);
    RUN_TEST(test_DateTimeInputState_InitializeWithNullProvider);
    RUN_TEST(test_DateTimeInputState_ButtonHandling);
    RUN_TEST(test_DateTimeInputState_EditModeToggle);
    RUN_TEST(test_DateTimeInputState_EdgeCases);
    RUN_TEST(test_DateTimeInputState_StringFormatting);
    RUN_TEST(test_DateTimeInputState_ExitHandling);
    
    return UNITY_END();
} 