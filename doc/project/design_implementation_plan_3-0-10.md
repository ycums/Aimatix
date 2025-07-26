# 3-0-10: 相対値入力機能 設計・実装計画書

## 概要
本計画書は、相対値入力機能（REL_PLUS_TIME_INPUT）の設計・実装方針を明文化するものです。
設計原則は `doc/design/` に準拠し、責務分離・テスト容易性・拡張性を重視します。

---

## 1. アーキテクチャ設計

### 1.1 全体構成
```
┌─────────────────────────────────────┐
│              UI Layer               │
│  StateManager + InputDisplayState   │
├─────────────────────────────────────┤
│           Business Logic            │
│    InputLogic (既存活用)            │
├─────────────────────────────────────┤
│         Hardware Adapters           │
│  DisplayAdapter (既存活用)          │
├─────────────────────────────────────┤
│           M5Stack Hardware          │
│     (M5Stack Library)              │
└─────────────────────────────────────┘
```

### 1.2 責務分離（@/design/architecture.md準拠）

#### UI層（StateManager + InputDisplayState）
- **責務**: 相対値入力画面の状態管理・画面遷移・UI描画
- **実装**: 既存のInputDisplayStateを拡張（モード切り替え）
- **依存**: InputLogic（純粋ロジック）、DisplayAdapter（ハード抽象化）

#### ロジック層（InputLogic）
- **責務**: 入力値の検証・変換・管理（既存活用）
- **拡張**: なし（既存のgetValue()等をそのまま使用）
- **設計**: 純粋ロジック、UI依存なし

#### アダプタ層（DisplayAdapter）
- **責務**: 画面描画の抽象化（既存活用）
- **実装**: 既存のDisplayAdapterをそのまま使用

### 1.3 状態管理設計（@/design/ui_state_management.md準拠）

#### StateManager拡張
```cpp
// 既存のStateManagerにREL_PLUS_TIME_INPUT状態を追加
enum class DisplayMode {
    MAIN_DISPLAY,
    ABS_TIME_INPUT,
    REL_PLUS_TIME_INPUT,  // 新規追加
    // ...
};
```

#### InputDisplayState拡張
```cpp
class InputDisplayState : public IState {
private:
    bool isRelativeMode;  // 相対値入力モードフラグ
    
public:
    // 既存メソッドを活用
    void onEnter() override;
    void onDraw() override;
    void onButtonC() override;
    
    // 相対値計算専用メソッド
    void calculateRelativeTime();
    void updatePreview();
};
```

---

## 2. インターフェース設計（@/design/interface_design.md準拠）

### 2.1 既存インターフェースの活用
- **IInputDisplayView**: 既存のInputDisplayViewImplをそのまま使用
- **IDisplay**: 既存のDisplayAdapterをそのまま使用
- **InputLogic**: 既存のAPIをそのまま使用

### 2.2 相対値計算の実装場所
```cpp
// InputDisplayState内で実装（UI層）
void InputDisplayState::calculateRelativeTime() {
    if (!inputLogic) return;
    
    // 現在時刻取得
    time_t now = time(nullptr);
    struct tm* tm_now = localtime(&now);
    
    // 入力値取得（既存のInputLogic API使用）
    int inputValue = inputLogic->getValue();
    if (inputValue == InputLogic::EMPTY_VALUE) return;
    
    // 相対値計算
    int inputHour = inputValue / 100;
    int inputMinute = inputValue % 100;
    
    // 現在時刻 + 入力値
    int resultHour = tm_now->tm_hour + inputHour;
    int resultMinute = tm_now->tm_min + inputMinute;
    
    // 繰り上げ処理
    if (resultMinute >= 60) {
        resultHour += resultMinute / 60;
        resultMinute %= 60;
    }
    
    // 日付跨ぎ処理
    bool nextDay = false;
    if (resultHour >= 24) {
        resultHour -= 24;
        nextDay = true;
    }
    
    // プレビュー表示更新
    updatePreview(resultHour, resultMinute, nextDay);
}
```

---

## 3. イベント通知設計（@/design/event_notification_design.md準拠）

### 3.1 コマンド/イベント駆動方式
```cpp
// 相対値入力確定時のコマンド発行
void InputDisplayState::onButtonC() {
    if (!inputLogic) return;
    
    // 相対値計算
    time_t relativeTime = calculateRelativeTime();
    
    // アラーム追加（既存のAlarmLogic API使用）
    extern std::vector<time_t> alarm_times;
    time_t now = time(nullptr);
    AlarmLogic::AddAlarmResult result;
    std::string msg;
    
    bool ok = AlarmLogic::addAlarmFromPartialInput(
        alarm_times, now, 
        inputLogic->getDigits(), inputLogic->getEntered(), 
        result, msg
    );
    
    if (ok) {
        // 成功時：メイン画面に遷移
        if (manager && mainDisplayState) {
            manager->setState(mainDisplayState);
        }
    } else {
        // 失敗時：エラー表示
        if (view) {
            view->showPreview(msg.c_str());
        }
    }
}
```

### 3.2 プレビュー更新の即時反映
```cpp
void InputDisplayState::onDraw() {
    // 既存の描画処理
    if (inputLogic && view) {
        // 入力値の描画（既存処理）
        const int* digits = inputLogic->getDigits();
        const bool* entered = inputLogic->getEntered();
        for (int i = 0; i < 4; ++i) {
            if (digits[i] != lastDigits[i] || entered[i] != lastEntered[i]) {
                view->showDigit(i, digits[i], entered[i]);
                lastDigits[i] = digits[i];
                lastEntered[i] = entered[i];
            }
        }
        
        // 相対値プレビュー更新（新規追加）
        if (isRelativeMode) {
            calculateRelativeTime();
        }
    }
}
```

---

## 4. 実装順序・マイルストーン

### Phase 1: 状態管理拡張
1. **StateManager拡張**
   - REL_PLUS_TIME_INPUT状態の追加
   - メイン画面B短押しでの遷移実装

2. **InputDisplayState拡張**
   - 相対値入力モードフラグ追加
   - タイトル表示の切り替え（"INPUT" → "REL+"）

### Phase 2: 相対値計算実装
3. **相対値計算ロジック**
   - calculateRelativeTime()メソッド実装
   - 現在時刻 + 入力値の計算
   - 日付跨ぎ処理

4. **プレビュー表示実装**
   - updatePreview()メソッド実装
   - 入力値変更時の即時更新

### Phase 3: アラーム設定機能
5. **アラーム追加機能**
   - 相対値計算結果のアラーム追加
   - エラー処理（重複・上限・未入力）

6. **エラー表示**
   - 英語エラーメッセージ表示
   - 効果音/バイブ（ハード依存部）

### Phase 4: テスト・品質保証
7. **Unit Test実装**
   - 相対値計算の境界値テスト
   - 日付跨ぎ処理のテスト

8. **Integration Test実装**
   - 画面遷移のテスト
   - アラーム追加の結合テスト

9. **静的解析・カバレッジ**
   - Clang-Tidy実行
   - カバレッジ品質ゲート確認

---

## 5. テスト設計（@/design/test_case_policy.md準拠）

### 5.1 Unit Test観点

#### 相対値計算テスト
```cpp
// 正常系テスト
TEST_CASE("相対値計算 - 正常系") {
    // 現在時刻14:30、入力__:30 → 15:00
    // 現在時刻14:30、入力_2:45 → 17:15
    // 現在時刻23:45、入力_0:30 → +1d 00:15
}

// 境界値テスト
TEST_CASE("相対値計算 - 境界値") {
    // 23:59 + 1分 → +1d 00:00
    // 23:59 + 1時間 → +1d 00:59
    // 00:00 + 23:59 → 23:59
}

// 部分入力テスト
TEST_CASE("相対値計算 - 部分入力") {
    // __:01 → 現在時刻 + 1分
    // _0:1_ → 現在時刻 + 10分
    // 1_:__ → 現在時刻 + 1時間
}
```

#### プレビュー表示テスト
```cpp
TEST_CASE("プレビュー表示 - 更新タイミング") {
    // 入力値変更時の即時更新
    // 日付跨ぎ時の"+1d"表示
    // 未入力時の表示制御
}
```

### 5.2 Integration Test観点

#### 画面遷移テスト
```cpp
TEST_CASE("画面遷移 - 相対値入力") {
    // メイン画面B短押し → 相対値入力画面
    // 相対値入力画面C短押し → メイン画面（成功時）
    // 相対値入力画面C長押し → メイン画面（キャンセル）
}
```

#### アラーム追加テスト
```cpp
TEST_CASE("アラーム追加 - 相対値") {
    // 正常な相対値計算結果のアラーム追加
    // 重複時のエラー処理
    // 上限超過時のエラー処理
    // 未入力時のエラー処理
}
```

---

## 6. 設計原則・制約事項

### 6.1 必須遵守事項
- **責務分離**: UI層とロジック層の明確な分離
- **純粋ロジック**: InputLogicはUI依存なし
- **既存活用**: 既存コンポーネントの最大限活用
- **テスト容易性**: モック注入による単体テスト

### 6.2 設計制約
- **既存API活用**: InputLogicの既存メソッドをそのまま使用
- **共通化**: InputDisplayViewImpl等の共通化を視野に入れる
- **パフォーマンス**: プレビュー表示の更新頻度に注意

### 6.3 拡張性考慮
- **モード切り替え**: 将来的な入力モード追加への対応
- **プレビュー表示**: 他の入力モードでの再利用可能性
- **エラー処理**: 統一されたエラー処理方式の採用

---

## 7. リスク・対策

### 7.1 技術的リスク
- **現在時刻の取得**: time(nullptr)の精度・タイミング
  - 対策: プレビュー更新時の時刻取得を最小限に抑制
- **パフォーマンス**: プレビュー表示の更新頻度
  - 対策: 入力値変更時のみ更新、不要な再計算を回避

### 7.2 設計的リスク
- **既存コードへの影響**: InputDisplayStateの拡張による影響
  - 対策: 既存機能のテストを維持、段階的な実装
- **共通化の遅れ**: InputDisplayViewImpl等の重複実装
  - 対策: 設計時に共通化を視野に入れる

---

## 8. 完了条件

### 8.1 機能要件
- [ ] メイン画面B短押しで相対値入力画面に遷移
- [ ] 相対値計算が正しく動作（現在時刻 + 入力値）
- [ ] プレビュー表示が正しく更新される
- [ ] C短押しでアラームが設定できる
- [ ] 日付跨ぎ処理が正しく動作する
- [ ] 不正値の補正が正しく動作する
- [ ] エラー処理が正しく動作する

### 8.2 品質要件
- [ ] カバレッジが品質ゲートを上回る
- [ ] 静的解析実行（Clang-Tidy）
- [ ] Unit Testが実装されている
- [ ] Integration Testが実装されている

### 8.3 設計要件
- [ ] 設計ドキュメントに準拠している
- [ ] 既存コンポーネントを適切に活用している
- [ ] 責務分離が明確である
- [ ] テスト容易性が確保されている

---

**作成日**: 2025年1月
**バージョン**: 1.0.0
**更新日**: 2025年1月 