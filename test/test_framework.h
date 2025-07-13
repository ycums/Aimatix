#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <unity.h>
#include <cstdio>
#include <cstring>

// Unityライブラリの問題を解決するためのカスタムテストマクロ
// TEST_ASSERT_GREATER_THANの代替実装

// カスタムテストマクロの実装
#define CUSTOM_TEST_ASSERT_GREATER_THAN(actual, expected) \
    do { \
        if ((actual) <= (expected)) { \
            char message[256]; \
            snprintf(message, sizeof(message), "Expected %s to be greater than %s (actual: %ld, expected: %ld)", \
                    #actual, #expected, (long)(actual), (long)(expected)); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

#define CUSTOM_TEST_ASSERT_LESS_THAN(actual, expected) \
    do { \
        if ((actual) >= (expected)) { \
            char message[256]; \
            snprintf(message, sizeof(message), "Expected %s to be less than %s (actual: %ld, expected: %ld)", \
                    #actual, #expected, (long)(actual), (long)(expected)); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

#define CUSTOM_TEST_ASSERT_GREATER_THAN_OR_EQUAL(actual, expected) \
    do { \
        if ((actual) < (expected)) { \
            char message[256]; \
            snprintf(message, sizeof(message), "Expected %s to be greater than or equal to %s (actual: %ld, expected: %ld)", \
                    #actual, #expected, (long)(actual), (long)(expected)); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

#define CUSTOM_TEST_ASSERT_LESS_THAN_OR_EQUAL(actual, expected) \
    do { \
        if ((actual) > (expected)) { \
            char message[256]; \
            snprintf(message, sizeof(message), "Expected %s to be less than or equal to %s (actual: %ld, expected: %ld)", \
                    #actual, #expected, (long)(actual), (long)(expected)); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

// 時刻比較用のカスタムマクロ
#define CUSTOM_TEST_ASSERT_TIME_GREATER_THAN(actual, expected) \
    do { \
        time_t actual_time = (actual); \
        time_t expected_time = (expected); \
        if (actual_time <= expected_time) { \
            char message[256]; \
            struct tm* actual_tm = localtime(&actual_time); \
            struct tm* expected_tm = localtime(&expected_time); \
            snprintf(message, sizeof(message), "Expected time %s to be greater than %s (actual: %02d:%02d, expected: %02d:%02d)", \
                    #actual, #expected, \
                    actual_tm->tm_hour, actual_tm->tm_min, \
                    expected_tm->tm_hour, expected_tm->tm_min); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

// 文字列比較用のカスタムマクロ
#define CUSTOM_TEST_ASSERT_STRING_CONTAINS(haystack, needle) \
    do { \
        if (strstr((haystack), (needle)) == NULL) { \
            char message[256]; \
            snprintf(message, sizeof(message), "Expected string '%s' to contain '%s'", \
                    (haystack), (needle)); \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

// 配列比較用のカスタムマクロ
#define CUSTOM_TEST_ASSERT_ARRAY_EQUAL(expected, actual, size) \
    do { \
        for (size_t i = 0; i < (size); i++) { \
            if ((expected)[i] != (actual)[i]) { \
                char message[256]; \
                snprintf(message, sizeof(message), "Array element at index %zu differs: expected %ld, actual %ld", \
                        i, (long)(expected)[i], (long)(actual)[i]); \
                TEST_FAIL_MESSAGE(message); \
            } \
        } \
    } while(0)

// テスト結果の詳細表示用マクロ
#define CUSTOM_TEST_ASSERT_WITH_MESSAGE(condition, message) \
    do { \
        if (!(condition)) { \
            TEST_FAIL_MESSAGE(message); \
        } \
    } while(0)

// テスト実行前のセットアップ用マクロ
#define CUSTOM_TEST_SETUP() \
    do { \
        printf("=== テスト開始: %s ===\n", __FUNCTION__); \
    } while(0)

// テスト実行後のクリーンアップ用マクロ
#define CUSTOM_TEST_TEARDOWN() \
    do { \
        printf("=== テスト完了: %s ===\n", __FUNCTION__); \
    } while(0)

// テスト結果の詳細表示
#define CUSTOM_TEST_RESULT(test_name, result) \
    do { \
        if (result) { \
            printf("✓ %s: 成功\n", test_name); \
        } else { \
            printf("✗ %s: 失敗\n", test_name); \
        } \
    } while(0)

#endif // TEST_FRAMEWORK_H 