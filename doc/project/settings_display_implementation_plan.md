# 設定画面（SettingsDisplay）実装計画書 - 3-0-12フェーズ

## 概要
本ドキュメントは、Aimatixプロダクトの設定画面（SettingsDisplay）の実装計画書です。
3-0-12フェーズでは、設定画面の雛形追加・遷移機能を実装し、SettingsLogicのstub実装と設定値の表示・編集雛形を作成します。

---

## 1. 実装範囲（3-0-12フェーズ）

### 1.1 実装対象機能
- [x] 設定画面への遷移（メイン画面C長押し）
- [x] 項目選択モード（上下移動）
- [x] Set Date/Time画面遷移
- [x] Info画面遷移
- [x] メイン画面への遷移（C長押し）

### 1.2 実装対象外機能（後続フェーズ）
- [ ] 値変更モード（Sound設定）
- [ ] 値変更モード（LCD Brightness設定）
- [ ] 設定の永続化
- [ ] WARNING COLOR TEST機能
- [ ] All Clear機能

---

## 2. 設計方針

### 2.1 アーキテクチャ原則
- **責務分離**: SettingsLogicは純粋ロジックのみ担当、UI遷移は行わない
- **依存性注入**: IEEPROM等のインターフェース経由で副作用を管理
- **StateManager統合**: SettingsDisplayStateとしてIStateを実装
- **共通化**: アラーム管理画面と同様のUI/UX設計

### 2.2 ファイル構成
```
lib/libaimatix/src/
├── SettingsLogic.h          # 設定ロジックインターフェース
├── SettingsLogic.cpp        # 設定ロジック実装（stub）
├── SettingsDisplayState.h   # 設定画面状態管理
├── SettingsDisplayState.cpp # 設定画面状態実装
├── ISettingsDisplayView.h   # 設定画面表示インターフェース
└── SettingsDisplayViewImpl.h # 設定画面表示実装

src/
├── SettingsDisplayViewImpl.h # M5Stack依存表示実装
└── main.cpp                 # StateManager登録・遷移処理
```

---

## 3. 実装詳細

### 3.1 SettingsLogic（stub実装）

#### 3.1.1 インターフェース定義
```cpp
// 設定項目の論理的な識別子
enum class SettingsItem {
    SOUND = 0,
    LCD_BRIGHTNESS = 1,
    SET_DATE_TIME = 2,
    INFO = 3
};

class ISettingsLogic {
public:
    virtual ~ISettingsLogic() {}
    
    // 設定値のgetter/setter
    virtual int getLcdBrightness() const = 0;
    virtual void setLcdBrightness(int value) = 0;
    virtual bool isSoundEnabled() const = 0;
    virtual void setSoundEnabled(bool enabled) = 0;
    
    // 設定項目の管理（論理的識別子ベース）
    virtual SettingsItem getSelectedItem() const = 0;
    virtual void setSelectedItem(SettingsItem item) = 0;
    virtual int getItemCount() const = 0;
    virtual SettingsItem getItemByIndex(int index) const = 0;
    virtual int getIndexByItem(SettingsItem item) const = 0;
    
    // 設定項目の表示名取得
    virtual std::string getItemDisplayName(SettingsItem item) const = 0;
    virtual std::string getItemValueString(SettingsItem item) const = 0;
    
    // モード管理
    virtual bool isValueEditMode() const = 0;
    virtual void setValueEditMode(bool editMode) = 0;
    
    // 永続化（stub実装）
    virtual void loadSettings(IEEPROM* eeprom) = 0;
    virtual void saveSettings(IEEPROM* eeprom) const = 0;
    virtual void resetSettings() = 0;
    virtual bool validateSettings() const = 0;
};
```

#### 3.1.2 stub実装内容
- 初期値設定（Sound: ON, LCD Brightness: 150）
- 設定値の一時的な保持（EEPROM非依存）
- 項目選択・値編集モードの状態管理
- バリデーション（範囲チェック等）
- 論理的識別子とIndexの変換機構
- 設定項目の表示名・値文字列の管理

### 3.2 SettingsDisplayState

#### 3.2.1 状態管理
- **項目選択モード**: 設定項目の上下移動（論理的識別子ベース）
- **値変更モード**: 選択項目の値編集
- **遷移管理**: StateManager経由での画面遷移
- **論理的識別子管理**: SettingsItemによる項目識別

#### 3.2.2 ボタン操作
- **A短押し**: 前項目（上に移動）
- **B短押し**: 次項目（下に移動）
- **C短押し**: 選択確定（値変更モードに入る）
- **C長押し**: メイン画面に戻る

#### 3.2.3 表示内容
- タイトル: "SETTINGS"
- 設定項目リスト（論理的識別子ベース）:
  1. "SOUND: ON" (SettingsItem::SOUND)
  2. "LCD BRIGHTNESS: 150" (SettingsItem::LCD_BRIGHTNESS)
  3. "SET DATE/TIME" (SettingsItem::SET_DATE_TIME)
  4. "INFO" (SettingsItem::INFO)
- ボタンヒント: "UP" "DOWN" "SELECT"

### 3.3 SettingsDisplayViewImpl

#### 3.3.1 表示仕様
- **レイアウト**: アラーム管理画面と同一
- **フォント**: Font4（設定項目）
- **色**: アンバー色（#FB20）
- **選択表示**: 背景色反転（アンバー⇔黒）
- **表示位置**: グリッドセル(0,2)-(15,6)

#### 3.3.2 画面遷移
- **Set Date/Time**: 日時入力画面への遷移（後続フェーズ）
- **Info**: 情報表示画面への遷移（後続フェーズ）

---

## 4. 実装手順

### 4.1 Step 1: インターフェース定義
1. `ISettingsLogic.h`の作成
2. `ISettingsDisplayView.h`の作成
3. `SettingsLogic.h`の作成

### 4.2 Step 2: stub実装
1. `SettingsLogic.cpp`の作成（stub実装）
2. 初期値設定・状態管理の実装
3. バリデーション機能の実装
4. 論理的識別子とIndexの変換機構の実装
5. 設定項目の表示名・値文字列管理の実装

### 4.3 Step 3: 状態管理実装
1. `SettingsDisplayState.h`の作成
2. `SettingsDisplayState.cpp`の作成
3. ボタン操作・状態遷移の実装
4. 論理的識別子ベースの項目選択実装

### 4.4 Step 4: 表示実装
1. `SettingsDisplayViewImpl.h`の作成
2. 表示レイアウトの実装
3. 選択表示・ボタンヒントの実装
4. 論理的識別子ベースの表示名・値文字列表示

### 4.5 Step 5: 統合・テスト
1. `main.cpp`でのStateManager登録
2. 画面遷移の動作確認
3. 静的解析実行（Clang-Tidy）

---

## 5. 不明点・確認事項

### 5.1 設定項目の詳細仕様

#### 5.1.1 LCD Brightness設定
**質問**: LCD Brightnessの初期値は100ですか、それとも150ですか？
- [ ] 100
- [x] 150（質問票で確認済み）
- [ ] その他: _________

#### 5.1.2 設定項目の表示順序
**質問**: 設定項目の表示順序は以下で正しいですか？
1. Sound（音ON/OFF）
2. LCD Brightness（明度調整）
3. Set Date/Time（日時設定）
4. Info（情報表示）

- [x] 正しい（質問票で確認済み）
- [ ] 修正が必要: _________

### 5.2 画面遷移の詳細仕様

#### 5.2.1 Set Date/Time画面遷移
**質問**: 3-0-12フェーズでSet Date/Time画面遷移を実装する場合、どの程度の機能を実装しますか？
- [x] 画面遷移のみ（日時入力機能は後続フェーズ）
- [ ] 基本的な日時入力機能も含める
- [ ] その他: _________

#### 5.2.2 Info画面遷移
**質問**: 3-0-12フェーズでInfo画面遷移を実装する場合、どの程度の機能を実装しますか？
- [x] 画面遷移のみ（情報表示機能は後続フェーズ）
- [ ] 基本的な情報表示機能も含める
- [ ] その他: _________

### 5.3 エラー処理

#### 5.3.1 無効な値の処理
**質問**: 3-0-12フェーズで値変更モードを実装しない場合、無効な値の処理は不要ですか？
- [x] 不要（値変更モードは後続フェーズ）
- [ ] 基本的なバリデーションは実装する
- [ ] その他: _________

### 5.4 テスト方針

#### 5.4.1 論理的識別子の設計
**質問**: SettingsItemの設計は以下で正しいですか？
- [x] enum classを使用して型安全性を確保
- [x] 各設定項目に論理的な識別子を割り当て
- [x] Indexと論理的識別子の変換機構を提供
- [x] 表示名・値文字列の取得機能を提供

- [x] 正しい
- [ ] 修正が必要: _________

#### 5.4.2 テスト範囲
**質問**: 3-0-12フェーズで実装するテストの範囲は以下で正しいですか？
- [x] 画面遷移のテスト
- [x] 項目選択のテスト（論理的識別子ベース）
- [x] 論理的識別子とIndexの変換テスト
- [ ] 値変更のテスト（後続フェーズ）
- [ ] 設定保存のテスト（後続フェーズ）

- [x] 正しい
- [ ] 修正が必要: _________

#### 5.4.3 論理的識別子の拡張性
**質問**: 将来的に設定項目が追加される場合の対応は以下で正しいですか？
- [x] SettingsItemに新しい項目を追加
- [x] getItemCount()の戻り値を更新
- [x] getItemDisplayName()に新しい項目の表示名を追加
- [x] getItemValueString()に新しい項目の値文字列を追加

- [x] 正しい
- [ ] 修正が必要: _________

---

## 6. 品質基準

### 6.1 カバレッジ目標
- **目標**: 品質ゲート基準を上回る
- **測定**: `scripts/test_coverage.py`を使用
- **基準**: 詳細は`doc/operation/quality_gates.md`を参照

### 6.2 静的解析
- **Clang-Tidy**: 実装完了後に実行
- **対象**: 新規作成ファイルすべて
- **基準**: 警告・エラーの解消

### 6.3 実機テスト
- **画面遷移**: メイン画面⇔設定画面の遷移確認
- **項目選択**: 上下移動の動作確認
- **ボタン操作**: 各ボタンの動作確認

---

## 7. 後続フェーズとの連携

### 7.1 3-0-13: 日時手動設定画面
- Set Date/Time画面の詳細実装
- 日時入力機能の追加

### 7.2 3-0-14: 値変更モード
- Sound設定のON/OFF切り替え
- LCD Brightness設定の値変更

### 7.3 3-0-15: 設定保存・復元
- EEPROM連携の実装
- 設定の永続化機能

---

## 8. リスク・課題

### 8.1 技術的リスク
- **StateManager統合**: 既存の状態管理との整合性
- **画面遷移**: 複数画面間の遷移管理
- **メモリ使用量**: 新規コンポーネントの追加による影響
- **論理的識別子管理**: SettingsItemの拡張性・保守性

### 8.2 対応策
- **段階的実装**: 最小限の機能から開始
- **テスト駆動**: 各ステップでの動作確認
- **コードレビュー**: 設計原則の遵守確認
- **論理的識別子設計**: 拡張性を考慮したenum設計

---

**作成日**: 2025年1月
**バージョン**: 1.0.0
**目的**: 設定画面（SettingsDisplay）3-0-12フェーズ実装計画 