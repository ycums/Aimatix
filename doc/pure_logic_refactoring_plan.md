# 純粋ロジック完全分離・モック環境完全分離リファクタリングプラン

## 目的
- lib/aimatix_lib/src/配下の全ロジックからM5Stack.h, EEPROM.h等の外部依存を排除し、mockやDI（依存性注入）でテスト可能な設計にする
- テスト時はmock/スタブのみでnative環境で全ロジックの単体テストが可能な状態を目指す

## 1. 外部依存の現状棚卸し（2024/06/09時点）
- alarm.cpp, button_manager.cpp, debounce_manager.cpp, settings.h等でM5Stack.h, EEPROM.h, mock_m5stack.hへの依存が多数
- input.cppはM5Stack.h依存をコメントアウトしmockで代替しているが、他は未分離

## 2. リファクタリング方針
### (1) 依存注入（DI）方式への移行
- M5Stack依存の関数・型（例：M5.Lcd, M5.Speaker, M5.update, EEPROM等）は、抽象インターフェース（純粋仮想クラス）を定義し、
  - 本番: M5Stack実装を注入
  - テスト: mock実装を注入
- 例: IDisplay, ISpeaker, IEEPROM, IButtonManager など

### (2) ヘッダ依存の排除
- alarm.h, button_manager.h, debounce_manager.h, settings.h からM5Stack.h, EEPROM.h等の直接includeを削除
- 必要な型・関数はインターフェース経由で取得

### (3) テスト用mock/スタブの整備
- test/mocks/配下に各インターフェースのmock実装を用意
- テスト時はmockを注入してnative環境でビルド・テスト

### (4) 既存テストコードの修正
- mock_m5stack.h, mock_settings.cpp等のmockを新インターフェースに準拠させる
- テストコードのDI対応

## 3. 実施ステップ
1. 依存抽象化インターフェースの設計・追加（lib/aimatix_lib/include/など）
2. alarm, button_manager, debounce_manager, settings, input等のロジックをインターフェース経由に書き換え
3. 既存mockの新インターフェース対応
4. テストコードのDI化・mock注入化
5. native環境で全テストが通ることを確認

## 4. 期待効果
- 純粋ロジックの完全分離・再利用性向上
- テストの安定化・自動化
- LDFやPlatformIOの制約に依存しない柔軟な開発体制

---

【備考】
- DI/インターフェース設計例や、既存コードの具体的な修正案は別途詳細設計ドキュメントで提示予定 