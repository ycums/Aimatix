# Aimatix テスト戦略書

## 1. 概要

Aimatixプロジェクトのテスト戦略について説明します。プロジェクトの進化に伴い、段階的に改善・拡張されています。

## 2. テスト戦略の基本方針

### 2.1 テストの目的
- **品質保証**: 機能の正確性、信頼性の確保
- **回帰防止**: 既存機能の破壊を防止
- **開発効率**: 早期バグ発見による開発効率向上
- **保守性**: コード変更時の安全性確保

### 2.2 テストの原則
- **自動化**: 可能な限りテストを自動化
- **早期実行**: 開発サイクルの早期にテスト実行
- **継続的実行**: 継続的インテグレーションでの自動実行
- **カバレッジ**: 適切なテストカバレッジの維持

## 3. テスト環境構成

### 3.1 テスト環境

#### 3.1.1 Native環境（Unit Test）
- **目的**: 純粋ロジックの単体テスト
- **環境**: Windows native環境
- **フレームワーク**: Unity
- **対象**: `lib/libaimatix/src/`配下の純粋ロジック

#### 3.1.2 M5Stack環境（統合テスト）
- **目的**: ハードウェア連携の統合テスト
- **環境**: M5Stack Fire実機
- **フレームワーク**: Unity
- **対象**: 全機能の統合動作

#### 3.1.3 ESP32環境（Unit Test）
- **目的**: ESP32環境での単体テスト
- **環境**: ESP32開発ボード
- **フレームワーク**: Unity
- **対象**: ESP32固有の機能

### 3.2 ディレクトリ構成
```
test/
├── pure/                          # 純粋ロジックテスト
│   ├── test_button_manager_pure/
│   │   ├── test_main.cpp
│   │   ├── mock_button_manager.h/.cpp
│   │   └── mock_time.h
│   ├── test_alarm_logic_pure/
│   │   └── test_main.cpp
│   ├── test_debounce_manager_pure/
│   │   └── test_main.cpp
│   ├── test_input_logic_pure/
│   │   └── test_main.cpp
│   ├── test_settings_logic_pure/
│   │   └── test_main.cpp
│   └── test_time_logic_simple/
│       └── test_main.cpp
├── mock/                          # 共通モック
│   ├── mock_button_manager.h/.cpp
│   ├── mock_m5stack.h
│   └── mock_time.h
├── integration/                   # 統合テスト（未使用）
│   └── test_integration.cpp
└── unity_config.h                 # Unity設定
```

## 4. テスト戦略の進化

### 4.1 Phase 0.2: 基本戦略の確立

#### 4.1.1 基本方針
- **Unit Test**: 純粋ロジックの単体テスト
- **統合テスト**: モジュール間の連携テスト
- **実機テスト**: ハードウェア動作確認

#### 4.1.2 カバレッジ目標
- **Unit Test**: 80%以上
- **統合テスト**: 95%以上
- **実機テスト**: 基本動作確認のみ

#### 4.1.3 テスト環境
- **Native環境**: Windows + Unity
- **M5Stack環境**: M5Stack Fire + Unity
- **ESP32環境**: ESP32 + Unity

### 4.2 Phase 0.7: 実装改善

#### 4.2.1 Unityライブラリ問題の解決
- **問題**: Unityライブラリの不安定性
- **解決**: カスタムテストフレームワークの実装
- **効果**: テスト結果の信頼性向上

#### 4.2.2 統合テスト環境の改善
- **問題**: 統合テストの実行困難
- **解決**: モック活用による統合テスト
- **効果**: 実機テストの最小化

#### 4.2.3 カバレッジ測定の信頼性向上
- **問題**: カバレッジ測定の不正確性
- **解決**: 改善版カバレッジ測定スクリプト
- **効果**: 正確なカバレッジ測定

### 4.3 Phase 0.9: 純粋ロジック配置

#### 4.3.1 純粋ロジックの分離
- **配置**: `lib/libaimatix/src/`配下
- **対象**: TimeLogic, AlarmLogic, InputLogic, SettingsLogic等
- **効果**: テスト容易性の向上

#### 4.3.2 LDF活用
- **機能**: Library Dependency Finder
- **効果**: 自動ビルド＆インクルード
- **利点**: 明示的なインクルードパス不要

#### 4.3.3 DIパターン対応
- **設計**: 依存性注入パターン
- **効果**: モック注入によるテスト
- **利点**: ハードウェア非依存のテスト

## 5. テスト実行方法

### 5.1 基本的なテスト実行

#### 5.1.1 全テスト実行
```bash
# Native環境で全テスト実行
pio test -e native

# 特定テスト実行
pio test -e native -f test_button_manager_pure
```

#### 5.1.2 カバレッジ測定
```bash
# カバレッジ測定
pio test -e native --coverage
```

#### 5.1.3 統合テスト実行
```bash
# M5Stack環境で統合テスト
pio test -e test-m5stack-fire
```

### 5.2 テスト環境別実行

#### 5.2.1 Native環境
```bash
# ビルド
pio run -e native

# テスト実行
pio test -e native
```

#### 5.2.2 M5Stack環境
```bash
# ビルド
pio run -e m5stack-fire

# テスト実行
pio test -e test-m5stack-fire
```

#### 5.2.3 ESP32環境
```bash
# ビルド
pio run -e unit-test-esp32

# テスト実行
pio test -e unit-test-esp32
```

## 6. モック戦略

### 6.1 モックの配置

#### 6.1.1 共通モック（test/mock/）
- **用途**: 複数テストで使用されるモック
- **対象**: M5Stack, ButtonManager, Time等
- **特徴**: 再利用性の高いモック

#### 6.1.2 固有モック（test/pure/各ディレクトリ）
- **用途**: 特定テスト専用のモック
- **対象**: テスト固有の要件
- **特徴**: テスト専用のカスタマイズ

### 6.2 モック実装例

#### 6.2.1 MockButtonManager
```cpp
class MockButtonManager : public IButtonManager {
public:
    void setButtonState(int buttonId, bool pressed) {
        buttonStates[buttonId] = pressed;
    }
    
    bool isPressed(int buttonId) override {
        return buttonStates[buttonId];
    }
    
    bool isLongPressed(int buttonId) override {
        return longPressStates[buttonId];
    }
    
    void update() override {
        // モック実装
    }
    
private:
    bool buttonStates[3] = {false, false, false};
    bool longPressStates[3] = {false, false, false};
};
```

#### 6.2.2 MockTime
```cpp
class MockTime {
public:
    static void setCurrentTime(time_t time) {
        currentTime = time;
    }
    
    static time_t now() {
        return currentTime;
    }
    
private:
    static time_t currentTime;
};
```

## 7. テストケース設計

### 7.1 テストケースの種類

#### 7.1.1 正常系テスト
- **目的**: 正常な動作の確認
- **対象**: 基本的な機能動作
- **例**: 時刻入力、アラーム設定

#### 7.1.2 異常系テスト
- **目的**: エラー処理の確認
- **対象**: 境界値、無効入力
- **例**: 無効な時刻入力、最大値超過

#### 7.1.3 境界値テスト
- **目的**: 境界値での動作確認
- **対象**: 最小値、最大値、ゼロ値
- **例**: 00:00, 23:59, 最大アラーム数

### 7.2 テストケース例

#### 7.2.1 時刻入力テスト
```cpp
TEST_CASE("時刻入力の正常系テスト") {
    // セットアップ
    InputLogic inputLogic;
    
    // テスト実行
    bool result = inputLogic.validateTimeInput(12, 30);
    
    // 検証
    TEST_ASSERT_TRUE(result);
}

TEST_CASE("時刻入力の異常系テスト") {
    // セットアップ
    InputLogic inputLogic;
    
    // テスト実行
    bool result = inputLogic.validateTimeInput(25, 70);
    
    // 検証
    TEST_ASSERT_FALSE(result);
}
```

#### 7.2.2 アラーム管理テスト
```cpp
TEST_CASE("アラーム追加の正常系テスト") {
    // セットアップ
    AlarmLogic alarmLogic;
    
    // テスト実行
    bool result = alarmLogic.addAlarm(12, 30);
    
    // 検証
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, alarmLogic.getAlarmCount());
}
```

## 8. 品質指標

### 8.1 カバレッジ目標
- **Unit Test**: 80%以上
- **統合テスト**: 95%以上
- **実機テスト**: 基本動作確認のみ

### 8.2 実行成功率
- **テスト実行成功率**: 100%
- **ビルド成功率**: 100%
- **テスト環境安定性**: 100%

### 8.3 パフォーマンス指標
- **テスト実行時間**: 5分以内
- **メモリ使用量**: 適切な範囲内
- **CPU使用率**: 適切な範囲内

## 9. 継続的インテグレーション

### 9.1 CI/CDパイプライン
1. **コードコミット**: 開発者のコードコミット
2. **自動ビルド**: PlatformIOによる自動ビルド
3. **自動テスト**: 全テストの自動実行
4. **カバレッジ測定**: テストカバレッジの測定
5. **結果通知**: テスト結果の通知

### 9.2 品質ゲート
- **ビルド成功**: 全環境でのビルド成功
- **テスト成功**: 全テストの成功
- **カバレッジ達成**: 目標カバレッジの達成
- **コードレビュー**: コードレビューの完了

## 10. 今後の改善計画

### 10.1 短期改善（Phase 1）
- **テストケース拡充**: 不足しているテストケースの追加
- **カバレッジ向上**: 目標カバレッジの達成
- **実行時間短縮**: テスト実行時間の最適化

### 10.2 中期改善（Phase 2）
- **統合テスト強化**: 統合テストの充実
- **パフォーマンステスト**: パフォーマンステストの追加
- **セキュリティテスト**: セキュリティテストの追加

### 10.3 長期改善（Phase 3）
- **自動化強化**: テスト自動化の強化
- **AI活用**: AIを活用したテストケース生成
- **継続的改善**: 継続的なテスト戦略の改善

---

**作成日**: 2025年1月  
**バージョン**: 1.0.0  
**更新日**: 2025年1月 