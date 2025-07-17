# src/ディレクトリ移行計画書

## 目的・背景
- lib/libaimatix配下の純粋ロジック分離・DI化が完了した後、src/配下をM5Stack依存部のみに整理する。
- 純粋ロジックとハードウェア依存部の明確な分離により、テスト可能性と保守性を向上させる。
- PlatformIOのベストプラクティスに従ったディレクトリ構成を実現する。

## 移行対象ファイル

### src/配下に残すファイル（M5Stack依存部）
- main.cpp - メイン制御、モード管理
- ui.cpp/h - UI描画、画面制御
- wifi_manager.cpp/h - WiFi接続管理
- time_sync.cpp/h - NTP時刻同期
- state_transition/ - 状態遷移制御
- types.h - 共通型定義

### lib/libaimatix/src/配下に移動済み（純粋ロジック）
- button_manager.cpp/h - ボタン管理ロジック
- debounce_manager.cpp/h - デバウンス管理ロジック
- alarm.cpp/h - アラーム管理ロジック
- input.cpp/h - 入力処理ロジック
- settings.cpp/h - 設定管理ロジック
- time_logic.cpp/h - 時刻管理ロジック

## 移行手順

1. **依存関係の確認**
    - src/配下のファイルがlib/libaimatix/src/配下の純粋ロジックを正しく参照しているか確認
    - include記法の統一（LDFを活用した記法に統一）

2. **インターフェース実装の確認**
    - IDisplay, ISpeaker, IEEPROM, IButtonManager等のインターフェース実装
    - M5Stack依存部でのインターフェース実装状況確認

3. **テスト構成の確認**
    - **PlatformIOテスト構成のベストプラクティスに従った構造確認**
    - test/pure/配下の各テストが独立したディレクトリに配置されているか確認
    - test/integration/配下の統合テストが適切に配置されているか確認

4. **ビルド・テスト確認**
    - `pio run` でM5Stack環境でのビルド確認
    - `pio test` でnative環境でのテスト確認
    - `pio test -e test-m5stack-fire` で実機統合テスト確認

## PlatformIOテスト構成のベストプラクティス

### テストディレクトリ構造
```
test/
├── pure/                      # native環境向け純粋ロジックテスト
│   ├── test_button_manager_pure/     # 独立したディレクトリ
│   │   └── test_main.cpp            # ButtonManager専用テスト
│   ├── test_alarm_logic_pure/       # 独立したディレクトリ
│   │   └── test_main.cpp            # Alarm専用テスト
│   └── ...
├── integration/               # 実機向け統合テスト
│   ├── test_integration.cpp
│   └── ...
└── mocks/                    # native用モック
    ├── mock_button_manager.h
    └── ...
```

### 重要なポイント
- **各テストは独立したディレクトリに配置**: 複数のtest_main.cppを同じディレクトリに配置しない
- **ディレクトリ名は`test_`プレフィックスで開始**: PlatformIOが自動検出するため
- **各test_main.cppは単一の責務**: 特定のモジュール/機能のテストのみ

## 期待される効果

1. **責務の明確な分離**
    - 純粋ロジックとハードウェア依存部の明確な分離
    - テスト可能性の向上

2. **保守性の向上**
    - モジュール間の依存関係の明確化
    - 変更影響範囲の限定

3. **開発効率の向上**
    - 純粋ロジックの単体テスト実行
    - ハードウェア非依存でのデバッグ

## 注意点
- 移行時は段階的に実施し、各段階でビルド・テストを確認
- PlatformIOのテスト構成ベストプラクティスに従った構造維持
- LDFの動作確認とinclude記法の統一

## 関連ドキュメント
- pure_logic_refactoring_plan.md（純粋ロジック分離計画）
- platformio_best_practice_plan.md（PlatformIOベストプラクティス）
- native_test_verification_plan.md（native環境テスト検証計画）

---

※本計画書はlib/libaimatix層のリファクタ完了後、src/配下の整理を実施するための指針・手順をまとめたものです。 