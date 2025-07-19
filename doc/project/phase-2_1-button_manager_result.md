#### 【進捗メモ】Phase 2.1 ButtonManager組み込み・現象まとめ（2024/xx/xx）

- **実施内容**:
  - main.cppにButtonManager（純粋ロジック）を組み込み、M5Stack物理ボタンAPIの直接利用をButtonManager経由に置換。
  - 状態遷移システム（handleButtons等）もButtonManagerベースのイベント生成に切り替え。
- **ビルド・アップロード**:
  - ビルド・アップロードは成功。
- **実機動作結果**:
  - 起動・画面表示は正常。
  - しかし、物理ボタンを押していないのに`Mode changed to: ...`が頻繁に出力され、モードが自動で切り替わる現象が発生。
  - ButtonManagerのisPressed/isLongPressedは常に0（押されていない）を返している。
- **デバッグ状況**:
  - handleButtons()内でButtonEvent生成・判定・状態遷移の各段階でシリアル出力を追加し、異常発生箇所を特定中。
  - createButtonEventFromButtonManager()やisValidButtonEvent()、状態遷移ロジックのいずれかで「何も押していないのにイベントが発生」している可能性。
- **課題・TODO**:
  - ButtonManager/イベント生成/状態遷移のどこで異常な値が発生しているかを切り分ける。
  - デバッグ出力を追加し、シリアルログから原因を特定する。
  - 必要に応じてButtonManagerやイベント生成ロジックの修正を行う。
- **備考**:
  - WiFiManager/TimeSync等は一時的に有効化・無効化しつつ、ButtonManager統合の影響範囲を最小化して検証中。

---