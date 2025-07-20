# モック実装方針（Aimatix標準）

本ドキュメントは、Aimatixプロジェクトにおけるモック実装の方針・設計・運用ルールを明文化するものです。

---

## 1. モックの目的
- 純粋ロジック層の単体テスト・統合テストで、ハード依存部や外部副作用を完全に排除し、テスト容易性・再現性を最大化する

## 2. モックの設計・配置
- すべてのハード依存インターフェース（IEEPROM, ISpeaker, IButtonManager, IDisplay等）に対し、Mockクラスを実装
- モッククラスは`test/mock/`ディレクトリに一元管理（例：`mock_eeprom.h/cpp`, `mock_speaker.h/cpp` など）
- テストごとに必要なモックを`test/pure/`や`test/integration/`からインクルードして利用

## 3. モックの実装例
- **MockEEPROMAdapter**: メモリ上の配列で永続化を模擬
- **MockSpeakerAdapter**: ログ出力やカウンタでビープ動作を模擬
- **MockButtonManagerAdapter**: 任意のボタン状態をプログラムから制御可能
- **MockDisplayAdapter**: 描画内容をバッファやログに記録

## 4. DIによる切替
- テスト時はmain.cppやテストセットアップで本番アダプタ→モックアダプタに差し替え
- 例：
```cpp
IEEPROM* eeprom = new MockEEPROMAdapter();
ISpeaker* speaker = new MockSpeakerAdapter();
```

## 5. モックの拡張・管理
- 新たなハード依存インターフェース追加時は、必ず対応するモックも`test/mock/`に実装
- モックの共通化・再利用性を重視し、テストごとに重複実装しない

## 6. テストコードでの利用例
- 各テストケースで必要なモックをインスタンス化し、DIでロジック層に注入
- テストごとにモックの状態や返り値を柔軟に制御

---

## 補足
- モック実装は「単一責任」「テスト容易性」「拡張性」を重視
- 実装・テスト段階でAPI追加・修正が必要な場合は本ドキュメントを随時更新 