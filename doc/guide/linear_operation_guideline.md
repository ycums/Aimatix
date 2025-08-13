# Linear 運用規約（ラベル/優先度/依存/同期）

AIM-35: Guideline: Linear 運用規約（ラベル/優先度/依存/同期） に基づくドキュメント。Linearをソース・オブ・トゥルースとし、GitHubと同期しつつ運用の迷いを減らすためのガイドライン。

## 目的
Linearをソース・オブ・トゥルースとし、GitHubと同期しつつ運用の迷いを減らす。

## ラベル/優先度/領域
- 優先度: P0=Urgent, P1=High, P2=Medium（Linearのpriorityに対応）
- 領域: ui/input/time/network/storage/ci（必要に応じてラベル整備）

## ステータス
Backlog → Spike → Ready → In progress（WIP<=2）→ Review → Done

## 依存の明記
- Blocks/Blocked by を本文先頭に記載し相互リンク

## GitHub同期
- GitHub Issue/PRは必ずLinearにリンク
- スパイクは親→子（実装/テスト/ドキュメント/CI）に分割

## 品質ゲート/CI
- coverage>=80%（strict=false）
- Clang-Tidy: 中重要度<=19

## テンプレ
- 本プロジェクトの Template: SPIKE / 実装タスク をコピーして使用

---

参照: AIM-35 `https://linear.app/aimatix/issue/AIM-35/guideline-linear-運用規約（ラベル優先度依存同期）`
