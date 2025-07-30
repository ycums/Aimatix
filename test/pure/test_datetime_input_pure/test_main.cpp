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

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_DateTimeInputState_Initialization);
    RUN_TEST(test_DateTimeInputState_CursorMovement);
    RUN_TEST(test_DateTimeInputState_Increment);
    RUN_TEST(test_DateTimeInputState_Validation);
    RUN_TEST(test_DateTimeInputState_FormatString);
    
    return UNITY_END();
} 