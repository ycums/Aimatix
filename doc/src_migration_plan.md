# src移行計画書

## 目的・背景
- lib/aimatix_libの純粋ロジック分離・DI化が完了した後、src/配下（UI/ハード制御層）の責務分離・設計最適化・mock化を段階的に実施する。
- UI/UXガイドラインやAmber CRTテーマ、デバウンス・ボタン管理の設計原則をsrc/層にも徹底し、保守性・テスト容易性・拡張性を高める。

## 適用範囲
- src/配下のmain.cpp, ui.cpp/h, time_sync.cpp/h, wifi_manager.cpp/h, state_transition/以下 など
- M5Stack依存部、UI描画、物理ボタン・スピーカー・LCD制御、WiFi/時刻同期等のハード制御

## 設計方針
- **責務分離**: UI描画・入力・状態遷移・ハード制御を明確に分離
- **DI設計**: lib/aimatix_libのIButtonManager/IDisplay/ISpeaker/IEEPROM等をsrc/層で注入・利用
- **mock化**: テスト時はmock実装を注入し、native環境でUI/ハード層のロジックもテスト可能に
- **UI/UX準拠**: Amber CRTテーマ、フォント階層、グリッドレイアウト、警告色・選択反転等のガイドライン遵守
- **デバウンス/ボタン管理**: 階層化デバウンス・統一ButtonManager設計をsrc/層でも徹底

## 実施ステップ
1. **現状分析**: src/配下の現状責務・依存関係・重複ロジックの棚卸し
2. **責務分離設計**: UI/入力/状態遷移/ハード制御の分離案を設計
3. **DI/インターフェース適用**: lib/aimatix_libの各インターフェースをsrc/層で注入・利用する形にリファクタ
4. **mock対応**: テスト用にmock_xxx.hを注入できる設計に変更
5. **UI/UXガイドライン適用**: Amber CRTテーマ・フォント・レイアウト・警告色等の適用
6. **テスト整備**: src/層のロジックもmockでnativeテスト可能な構造に
7. **段階的移行・動作確認**: 各ステップごとにビルド・テスト・実機確認

## 注意点
- lib/aimatix_lib層のテストがグリーンであることを前提とする
- src/層の責務分離・DI化は段階的に進め、常にビルド・テストが通る状態を維持
- UI/UXガイドライン・設計原則との整合性を常に確認
- 既存main.cpp等の肥大化・責務混在部は優先的に分離
- 実装・テストの進行はpure_logic_refactoring_plan.md完了後、**別チャットで実行**すること

## 関連ドキュメント
- pure_logic_refactoring_plan.md（lib/aimatix_lib層の純粋ロジック分離計画）
- interface_design_step1.md（DI用インターフェース設計）
- dependency_inventory_report.md（依存関係棚卸し）
- platformio_best_practice_plan.md（ビルド・テスト運用指針）
- UI/UX仕様: spec.md, phase0_2_detailed_spec.md など

---

※本計画書はlib/aimatix_lib層のリファクタ完了後、src/層の移行・最適化を別チャットで段階的に実行するための指針・設計方針をまとめたものです。 