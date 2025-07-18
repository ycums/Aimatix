# native環境テスト検証 実施計画書

## 目的・背景
- lib/libaimatix配下の純粋ロジック分離・DI化が完了した後、native環境（PC上）で全テストがグリーンになることを確認する。
- LDF（Library Dependency Finder）の挙動による依存解決・includeパス問題を回避し、安定したテスト基盤を構築する。
- 過去のstepwise_ldf_verification_plan/reportの知見を活かし、段階的なペア移植＋一括テストで進める。
- **PlatformIOのテスト構成ベストプラクティス**に従い、各テストを独立したディレクトリに配置する。

## 現在の進行状況

### ✅ 完了済み項目
- [x] **test_minimal_project環境の構築**
- [x] **button_managerペアの段階的移植テスト**
  - [x] lib/libaimatix/src/button_manager.cpp/h のコピー
  - [x] test/pure/test_button_manager_pure/test_main.cpp のコピー
  - [x] 依存関係の解決（mock_time.h、M5Stack.h等）
  - [x] DI化（millis()関数ポインタ注入）
  - [x] ビルド・テスト成功確認
- [x] **alarmペアの段階的移植テスト**
  - [x] lib/libaimatix/src/alarm.cpp/h のコピー
  - [x] test/pure/test_alarm_logic_pure/test_main.cpp のコピー
  - [x] 依存関係の解決（ISpeaker.h、settings.h、IEEPROM.h等）
  - [x] テストコードの全面リファクタ（time(NULL)固定化）
  - [x] ビルド・テスト成功確認
- [x] **debounce_managerペアの段階的移植テスト**
  - [x] lib/libaimatix/src/debounce_manager.cpp/h のコピー
  - [x] test/pure/test_debounce_manager_pure/test_main.cpp のコピー
  - [x] 依存関係の解決（mock_m5stack.h、M5Stack.h等）
  - [x] テストコードの全面リファクタ（mock_time.h削除）
  - [x] **テスト項目の整理・最適化**
    - [x] 非現実的なテストの削除（8個 → 4個）
    - [x] 現実的なユーザー操作パターンに基づくテスト設計
    - [x] 連打防止テストの実装
  - [x] ビルド・テスト成功確認

### 🔄 進行中項目
- [x] **input_logicペアの段階的移植テスト**
- [x] **settings_logicペアの段階的移植テスト**
- [x] **time_logicペアの段階的移植テスト**

### ⏳ 未着手項目
- [x] **本体プロジェクトでの一括テスト**
- [x] **LDF/PlatformIOのキャッシュクリア・再ビルド**
- [x] **レポート・フィードバック**

## 実施手順
1. **LDF仕様・依存関係の再確認**
    - 各ロジック（button_manager, alarm, debounce_manager, input, settings, time_logic等）の依存関係・include記法・ディレクトリ構成を棚卸し
    - **PlatformIOテスト構成の確認**: 各テストが独立したディレクトリに配置されているか確認

2. **段階的ペア移植テスト（stepwise方式）**
    - test_minimal_project等の最小構成プロジェクトにlib/libaimatix/src/とtest/pure/のペアを1組ずつコピー
    - **重要**: テストディレクトリ構造をPlatformIOのベストプラクティスに従って配置
    - 例：button_manager.cpp/h＋test_button_manager_pure/test_main.cpp → ビルド・テスト
    - テストが通れば次のペアを追加。壊れたらエラー内容・依存関係・include記法を記録し、修正

3. **本体プロジェクトでの一括テスト**
    - lib/libaimatix配下の全ロジック・mock・テストを本体プロジェクトで一括ビルド・テスト
    - `pio test` でnative環境の全テストがグリーンになることを確認
    - 失敗時はstepwise検証の知見をもとに修正

4. **LDF/PlatformIOのキャッシュクリア・再ビルド**
    - `pio run --target clean` でキャッシュクリアし、LDFの再走査を強制
    - 依存解決の不整合がないか再度テスト

5. **レポート・フィードバック**
    - すべてのテストがグリーンになったら、その時点の構成・工夫点・LDF対策・include記法のベストプラクティスをplatformio_best_practice_plan.md等に記録・共有

## PlatformIOテスト構成のベストプラクティス

### テストディレクトリ構造
```
test/
├── pure/
│   ├── test_button_manager_pure/     # 独立したディレクトリ
│   │   └── test_main.cpp            # ButtonManager専用テスト
│   ├── test_alarm_logic_pure/       # 独立したディレクトリ
│   │   └── test_main.cpp            # Alarm専用テスト
│   └── ...
└── mocks/
    ├── mock_button_manager.h
    └── ...
```

### 重要なポイント
- **各テストは独立したディレクトリに配置**: 複数のtest_main.cppを同じディレクトリに配置しない
- **ディレクトリ名は`test_`プレフィックスで開始**: PlatformIOが自動検出するため
- **各test_main.cppは単一の責務**: 特定のモジュール/機能のテストのみ

### よくある問題と解決策
- **リンカーエラー「multiple definition of main」**: 複数のtest_main.cppが同じディレクトリにある
- **テストが検出されない**: ディレクトリ名が`test_`で始まっていない
- **モックが見つからない**: `test/mocks/`に配置し、`-Itest/mocks`でインクルード

## 注意点
- LDFの仕様・挙動はPlatformIOのバージョンや設定で変わる場合があるため、都度公式ドキュメントも参照
- include記法（<> or ""）やディレクトリ構成は本体とテストで統一
- テストが壊れた場合は、stepwise_ldf_verification_report.mdのよ原因・修正内容を必ず記録
- **PlatformIOのテスト構成ベストプラクティスに従い、各テストを独立したディレクトリに配置**

## 関連ドキュメント
- pure_logic_refactoring_plan.md（lib/libaimatix層の純粋ロジック分離計画）
- stepwise_ldf_verification_plan.md（段階的LDF検証手順）
- stepwise_ldf_verification_report.md（LDF検証レポート）
- platformio_best_practice_plan.md（ビルド・テスト運用指針）
- dependency_inventory_report.md（依存関係棚卸し）

---

※本計画書はlib/libaimatix層のリファクタ完了後、native環境での全テスト検証を別チャットで段階的に実施するための指針・手順をまとめたものです。 