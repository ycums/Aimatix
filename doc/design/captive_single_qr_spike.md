# SoftAP Captive誘導で二段QRを一段に短縮（SPIKE結果）

## 概要
- 目的: iOSでWi‑Fi接続後に自動で時刻同期ページ(`/sync?t=...`)を開かせ、二段QRを一段に短縮
- 対象: iOS 18 / iPhone SE2 / Wi‑Fiアシスト ON
- 結果: 3/3で CNA 自動表示 → `/sync` → JS 自動 `POST /time/set` → 適用 → AP 停止 を確認（安定）

## 実装ポイント（SPIKE）
- `DNSServer` を起動し、任意ホストを `192.168.4.1` に解決
- `WebServer` に接続性チェック系パスを追加し、302で `/sync?t=...` へ誘導
  - `/hotspot-detect.html`, `/success.txt`, `/success.html`, `/ncsi.txt`, `/generate_204`, `/` , `onNotFound`
- `/sync` は既存どおり（JSで `Date.now()` と `getTimezoneOffset()` を取得し `POST /time/set`）
- 適用成功/タイムアウトで `dnsServer.stop(); server.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF);` を実施
- AP停止の二重発火防止に、ウィンドウタイマー(esp_timer)の stop/delete と短い待機を追加（冪等）

## 検証ログ要点
- CNA起動: `GET /hotspot-detect.html` (Host=captive.apple.com, UA=CaptiveNetworkSupport)
- 誘導: 302 → `GET /sync?t=...` (CNA UA)
- 反映: `POST /time/set` (Referer/Origin=`http://captive.apple.com/...`) → `200 Time applied`
- 3回の試行で同様のシーケンスを確認

## 判断（Go/No-Go）
- Go（iOS向けは二段目URL QRを省略可能）。他OSは既存フローをフォールバックとして継続。

## 影響/非影響
- 影響: `src/SoftApTimeSyncController.cpp`（DNS/ルート追加、停止処理の冪等化）
- 非影響: `lib/libaimatix/src/**`（純粋ロジックは非改変）

## 今後のタスク
- UI分岐の方針反映（iOSはStep2省略、その他は既存案内）
- 追加の挙動マトリクス（環境差, セルラー/アシスト条件）を追記
- 成功後のAP再起動ログの完全抑止（必要なら待機時間や再入ガードの微調整）
