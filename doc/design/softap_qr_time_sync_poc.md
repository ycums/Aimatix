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

## 変更（Issue #93 SPIKE・一段化）
- DNS全リダイレクト（`DNSServer`）を追加し、任意ホストを `192.168.4.1` に解決。
- 接続性チェック系/既知パスを 302 で `/sync?t=...` へ誘導。
  - `/hotspot-detect.html`, `/success.txt`, `/success.html`, `/ncsi.txt`, `/generate_204`, `/` , `onNotFound`
- iOS では CNA が安定発火し、`/sync` → 自動 `POST /time/set` まで無操作で到達。

## 既知事象 / 運用上の注意
- 成功直後のAP再起動ログが1回出ることがある → 停止処理にタイマー停止/待機を入れて抑制（残存時は今後微調整）。

## DoD（Issue）
- 実機での一連フロー（接続→自動表示→反映→自動クローズ）が再現可能。
- セキュリティ要件（短時間露出・ワンタイムトークン・レート制限）を方針として明文化。
- 本実装のタスク分解と概算見積りを提示。
- PoC動作証跡（ログ/写真）を保存。
