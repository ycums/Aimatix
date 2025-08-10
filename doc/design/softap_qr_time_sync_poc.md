# AIM-36 SoftAP + QR 二段階方式 時刻反映 SPIKE 設計方針書

## 目的 / スコープ
- iPhone等のスマートフォンから、アプリ配布なしで M5Stack へ「ローカル時刻を一方向反映」する実用手段の検証（PoC）。
- 本資料は SPIKE（PoC）段階の設計方針。純粋ロジック層は汚染せず、`src/` 層内のアダプタ実装に限定。

## 前提・制約（PoC）
- SoftAP は 60秒の公開時間窓、先着1クライアント許可。成功または時間窓切れで停止。
- トークン・PSK はランダム生成（PoCでは64bit相当）。
- ドリフト閾値チェックは PoC では無制限（全許容）。
- HTTPサーバは Arduino `WebServer` を使用（最速優先）。

## ユーザーフロー（二段QR）
1. 装置側で SoftAP を起動（SSID/PSK をランダム生成）。
2. 画面に Wi‑Fi接続用QR を表示（`WIFI:T:WPA;S:<SSID>;P:<PSK>;H:false;;`）。
3. スマホが AP に接続すると、自動で URL QR に切替（`http://192.168.4.1/sync?t=<token>`）。
4. `/sync` HTML が `Date.now()` と `getTimezoneOffset()` を取得し `POST /time/set` を自動送信。
5. 受理・適用成功で AP を停止し、実装では即時に `MAIN_DISPLAY` へ復帰（成功画面の固定表示は行わない）。

## QR 仕様
- Wi‑Fi QR: `WIFI:T:WPA;S:<SSID>;P:<PSK>;H:false;;`
  - エスケープ: SSID/PSK に `\\` `;` `,` `:` を含む場合はバックスラッシュでエスケープ。
- URL QR: `http://192.168.4.1/sync?t=<token>`
- 表示: `ricmoo/QRCode` で QR を生成し、M5GFX でドット描画。画面サイズに応じ自動スケール。

### PoC 表示設定（最終）
- ECC: LOW（誤り訂正7%）
- quiet zone: 2 モジュール（大きさ優先）
- 画面余白: 上端 10px を見出し用に確保
- 配色: 黒背景 + 橙ドット（可読性よりサイズ優先の選定）
- バージョン目安: Wi‑Fi=7、URL=5（本実装でも同目安を採用）
- 切替: APクライアント接続イベントで Wi‑Fi → URL に自動切替

## API 仕様
- `GET /sync`
  - 入力: `?t=<token>`（ワンタイム）
  - 出力: HTML（JSが `POST /time/set` を自動実行）
- `POST /time/set`
  - 入力(JSON): `{ "epochMs": <int>, "tzOffsetMin": <int>, "token": "<hex>" }`
  - 受理条件（PoC）:
    - `token` 一致
    - AP時間窓内（<= 60秒）
    - レート制限（1回/起動）
    - ドリフト閾値: 無制限（PoC）
  - 入力バリデーション（本実装方針）:
    - `epochMs` 下限: `2025-01-01 00:00:00 UTC`（`1735689600000`）以上
    - `epochMs` 上限: `sizeof(time_t)==4` の場合 `2038-01-19 03:14:07 UTC`（`2147483647000`）以下、`sizeof(time_t)==8` の場合 `2100-01-01 00:00:00 UTC`（`4102444800000`）以下
    - `tzOffsetMin` 範囲: `[-14*60, +14*60]` 分（`[-840, +840]`）
  - 出力:
    - 200: `Time applied`
    - 400: `reason: "time_out_of_range" | "tz_offset_out_of_range" | "invalid_token"`
    - 4xx/5xx: 理由文字列

## セキュリティ要件（PoC → 本実装での強化方針）
- PoC:
  - トークン: 64bit相当（16桁Hex）
  - PSK: 64bit相当（16桁Hex）
  - 最大接続: 1、露出: 60秒、レート制限: 1回/窓
- 本実装の推奨:
  - トークン: 128bit以上
  - PSK: 128bit相当以上、記号を含む高強度
  - 接続性警告の緩和（下記「既知事象」参照）
  - 監査ログ/失敗回数に応じたバックオフ

## 既知事象 / 運用上の注意
- スマホ側に「ネットワークに接続できませんでした」等の警告が出る場合がある。
  - 理由: SoftAP はインターネットに出られないため、OSの接続性チェックで失敗と判定されやすい。
  - 回避: 一時的にセルラー無効化 or Wi‑Fiアシスト無効化、または `http://192.168.4.1/sync?t=...` を直接開く。
  - 改善（任意）: `/generate_204`（204応答）や `hotspot-detect` 互換パスを用意し、接続性チェックを満たして警告を抑止。

## 実装メモ（PoC）
- 環境: `env:m5stack-core2-aim36-spike`（`upload_port=COM5`）。
- エントリ: `src/spikes/aim36/main.cpp`
- 画面: Amber CRTトーン、Wi‑Fi QR → URL QR 切替は APクライアント接続イベントで自動。
- `/time/set` 成功後は `Local` と `TZ` の情報を Serial ログに出力し AP 停止（本実装では画面固定表示は行わず即時復帰）。

## UI文言（本実装差分）
- タイトル区切り記号: `|` → `>` に変更
- Step1 タイトル: "TIME SYNC > JOIN AP"
- Step2 タイトル: "TIME SYNC > OPEN URL"
- 成功: "TIME SYNC > DONE"（固定画面は出さずログのみ）
- エラー: "TIME SYNC > ERROR"
- ボタンヒント: A="REISSUE" / B=未使用 / C="EXIT"
- エラー時の動作: SoftAP 停止 → エラーメッセージを2秒表示 → `SETTINGS_MENU`

## DoD（Issue）
- 実機での一連フローが再現可能（接続QR → 同期URL QR → 時刻反映 → 自動クローズ）。
- セキュリティ要件（短時間露出・ワンタイムトークン・レート制限）を方針として明文化。
- 本実装のタスク分解と概算見積りを提示。
- No-Go時の代替（NTP＋擬似RTC）方針を明記。
- PoC動作証跡（ログ/写真）を保存。

## 後続Issue（案）
1. 本実装化: `SoftApSyncAdapter` の `src/` 統合、UI/音/警告の整備。
2. 接続性警告の緩和: `/generate_204` 等の実装、必要なら `AsyncWebServer` 検討。
3. セキュリティ強化: トークン/PSK長の拡張、監査ログ、失敗時バックオフ。
4. ドキュメント: 運用手順、トラブルシュート、QRエスケープ表。
5. テスト: 実機手順、ユニット化可能部（トークン/時間窓検証など）の切り出し。

## 参考
- BLE CTS 代替の結論: AIM-32
- 設計原則/アーキテクチャ: `doc/design/*`, `doc/spec/system_overview.md`


