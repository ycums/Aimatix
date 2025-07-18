# 純粋ロジック同期・Aimatix本体再開計画（2024/06）

## 目的
- test_minimal_project配下のlib（pureロジック）をAimatix本体のlib/libaimatixに反映し、今後の開発を本体リポジトリで再開する。
- 本体libがdirtyな状態を解消し、全体の品質・保守性・テスト性を向上させる。
- 純粋ロジックを「別プロジェクト化」するか「lib/libaimatixに留める」かの方針も検討・提案する。

## 現状整理
- test_minimal_project/lib/libaimatix/src/：PlatformIO/Unity/LDFベストプラクティスに準拠したpureロジック群（テスト済み、全GREEN）
- Aimatix本体/lib/libaimatix/：過去の移植・リファクタの影響でdirtyな状態（未整理・未統一・テスト不整合あり）
- 本体のlib配下をpureな状態に戻すことで、今後の開発・保守・CI/CDが容易になる

## 同期・再開のための具体的手順
1. **現状バックアップ**
    - Aimatix本体のlib/libaimatix/を一時バックアップ（lib/libaimatix_backup等）
2. **pureロジックの反映**
    - test_minimal_project/lib/libaimatix/src/配下の全ファイルをlib/libaimatix/src/に上書きコピー
    - 必要に応じてinclude/やmock等も同期
3. **テストディレクトリの整理**
    - test/pure/配下のテストも本体test/pure/に反映（必要に応じて）
    - テストが本体でGREENになることを確認
4. **platformio.ini等の設定見直し**
    - build_flagsやlib_deps、LDF設定がpureロジックに最適化されているか再確認
5. **CI/CD・カバレッジ等の再整備**
    - テスト自動化・カバレッジ測定・レポート生成の再確認
6. **ドキュメント・運用ルールの更新**
    - platformio_best_practice_plan.md等に新運用ルールを追記

## 方針比較：pureロジックを「別プロジェクト化」or「lib/libaimatixに留める」

### 1. 別プロジェクト化（サブモジュール/独立リポジトリ）
#### メリット
- 完全な責務分離・再利用性向上（他プロジェクトでも流用しやすい）
- テスト・CI/CD・リリース管理が独立して行える
- 本体リポジトリの肥大化防止
#### デメリット
- サブモジュール管理やバージョン同期の運用コスト
- 本体とロジックのAPI変更時に連携作業が発生
- PlatformIOのLDFや依存解決で追加設定が必要な場合あり

### 2. lib/libaimatixに留める（現状維持・整理型）
#### メリット
- PlatformIOのLDFが自動で依存解決・ビルドしてくれる
- 本体・テスト・ロジックの一元管理が容易
- サブモジュール管理不要、運用がシンプル
#### デメリット
- 本体リポジトリが肥大化しやすい
- 他プロジェクトへの流用時は手動コピーが必要
- 責務分離の観点ではやや弱い

### 推奨方針
- **現時点では「lib/libaimatixに留める」運用を推奨**
    - Aimatix本体の開発・保守・テスト運用が最優先
    - LDF/PlatformIOベストプラクティスに最も適合
    - 将来的に他プロジェクト展開やOSS化を本格化する場合は、サブモジュール化・独立リポジトリ化を再検討

## 今後の運用・メンテナンス指針
- 本体lib/libaimatix配下は常にpureな状態を維持（テストGREEN・依存最小・DI設計）
- テスト・CI/CD・カバレッジ測定を継続的に実施
- ドキュメント・運用ルールも随時アップデート
- 必要に応じてtest_minimal_projectでの検証・実験を継続

---

（本ドキュメントはAimatix本体の純粋ロジック再統合・再開のための運用指針として作成） 