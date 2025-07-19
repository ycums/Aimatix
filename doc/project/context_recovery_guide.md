# Aimatix 開発コンテキスト復帰ガイド（Phase 2以降）

---

## 1. 現在の開発フェーズ
- **Phase 2: 純粋ロジックと本体実装の統合**
  - test/pure/配下のロジック（TimeLogic, AlarmLogic, ButtonManager等）をmain.cppや状態遷移システムと実際に接続・統合中
  - DI/アダプター層で物理層と純粋ロジック層をIButtonManager等で抽象化・接続
  - 状態遷移システム・UIでIButtonManager等の抽象型を利用
  - 統合テスト・結合テストでAPI設計の有用性・問題点を実証的に検証
  - 統合・運用結果をもとにAPI設計や責務分離の見直し・改善を予定

## 2. 品質ゲート・カバレッジ目標
- **品質ゲートは「現状カバレッジ（61%）維持」**
- 80%目標は一時保留中
- coverage_config.jsonのthresholdも61.0

## 3. 責務分担の原則
- **ButtonManager**: ボタン状態の一元管理（押下/長押し/リリース等）
- **main.cpp**: システム初期化・ループ・UI/状態遷移の起点
- **StateTransitionManager**: 状態遷移ロジックの抽象化
- **UI層**: 画面描画・ユーザーインターフェース
- **アダプター層**: 物理層（M5Stack API）と純粋ロジック層の橋渡し

## 4. デバッグ・切り分けの鉄則
1. **「何がどうおかしいか」を記録**（再現手順・現象・エラー・シリアル出力）
2. **最小単位で「どこまで正常か」を切り分け**
   - main.cppのsetup/loopだけ動かす
   - ButtonManager単体で動作確認
   - 物理API直参照で動作確認
3. **責務ごとにロールバック・段階的再統合**
4. **シリアル出力・デバッグログを最大活用**
5. **「最小限の再現コード」で問題箇所を特定**
6. **設計ドキュメント（integrated_work_plan.md等）と照合し、責務分離・型一致を再確認**

## 5. 進捗・現状メモ（2024/07時点）
- test/pure/のロジックはmain.cpp等の本体実装と未接続→Phase 2で統合中
- main.cppのボタン管理はButtonManager経由にリファクタ済み
- ビルドは正常に通る状態
- 今後は実機/シミュレータでの動作確認・統合テスト設計・API有用性検証が主タスク

## 6. 参考ドキュメント
- doc/project/integrated_work_plan.md（全体進捗・品質ゲート・Phase構成）
- coverage_config.json（カバレッジ品質ゲート設定）
- src/main.cpp, src/m5stack_adapters.h, lib/libaimatix/src/button_manager.h など

---

**このガイドを見れば、どのフェーズで何をしていたか・品質ゲート・責務分担・切り分け方針が即座に復元できます。** 