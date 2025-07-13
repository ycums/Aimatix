下記の仕様と作業手順に従ってプロダクトを作成してください。
**タスク:**
- 下記設計に基づき、M5Stack Fire (ESP32) 用の Arduino スケッチ (`.ino` または `.cpp` / `.h`) を作成してください。
- 基本的な状態管理（`enum` でモード定義、`loop()` でモードに応じた描画・処理・遷移）、ボタン入力処理、LCD描画関数、Wi-Fi/NTP、タイマーロジックなどを構造化して記述してください。
- 各モードの描画関数とボタン処理を明確に分離し、読みやすいコードを心がけてください。
- 主要な色定義（アンバー、フラッシュオレンジ、濃いグレーなど）を定数として定義してください。
- ファイルは管理しやすいように適切に分割してください。
- 可能な限り Unit Test を書いてください。
- 適宜ビルド可能して実機動作確認が可能なポイントを設け、ユーザーに確認するように促してください。
- 作業に先んじて、作業計画書を作成し、 `H:\Documents\Aimatix\doc\` ディレクトリに配置してください。
- 作業計画書に基づいて具体的にAIに指示を出すためのAIプロンプトを`.prompt.md` として作成してください。
- 上記を順次実行する形で作業を進めてください。

# 仕様書
**プロジェクト名:** M5Stack 集中タイマー
**デバイス:** M5Stack Fire (ESP32)

**概要:**
M5Stack Fire を使った、レトロなアンバーCRT風 UI の集中タイマーです。
Wi-Fi と NTP を利用して正確な時刻を維持し、設定した時刻に音とバイブレーションでアラートを鳴らします。
UI はシンプルさと視認性を最優先し、直感的な操作を目指します。

**UI/UX デザイン原則:**
- **デザインテーマ:** アンバーCRT風（黒/濃いグレー背景、アンバー系テキスト、フラッシュオレンジの警戒色、ネガポジ反転ハイライト）。
- **フォント:** M5Stack の標準フォントを `Font7` (最重要), `Font4` (重要), `Font2` (補助/ヒント) として使用。
- **レイアウト:** 全体的にグリッドレイアウトを意識し、要素を整然と配置。
- **データ永続化:**
    - SSID/パスワード、LCD明るさ、音/バイブレーション設定はFlashメモリに永続化。
    - **鳴動時刻データは永続化せず、電源オフでリセットされる揮発性データとする。**

---

**共通UI要素:**

1.  **タイトルバー/ステータスバー:**
    * **位置:** 画面の最上部。
    * **表示内容:** 左に**現在のモード名**（例: "MAIN", "SET TIME"）、右に**バッテリー残量アイコンとパーセンテージ**（例: "🔋 75%"）。充電中は充電アイコンを表示。
    * **色:** 補助的なアンバー。バッテリー残量20%以下でフラッシュオレンジに変化。
    * **サイズ:** Font2。

2.  **ボタンヒント:**
    * **位置:** 画面の最下部、それぞれの物理ボタンの真上。
    * **形式:** `[A: 〇〇] [B: 〇〇] [C: 〇〇]`。
    * **色:** 補助的なアンバー。
    * **サイズ:** Font2。

---

**主要機能と画面モード:**

**1. メイン表示:待機 (C)**
   * **目的:** 次の鳴動時刻までの残り時間を直感的に把握し、集中を促す。
   * **表示要素:**
        * 上部: 現在日時（`YYYY/MM/DD (ddd)` Font2, `HH:MM:SS` Font4）。
        * 中央: 最も大きく `-HH:MM:SS` (Font7) で一番近い鳴動時刻までの残り時間。秒まで常時表示、形式固定。
        * 残り時間直下: 進捗バー。未進捗部分がアンバー、進捗済み部分が濃いグレー。基準点は「直前の鳴動解除時」から「次の鳴動開始時」までを100%。
        * 進捗バー下: `NEXT: HH:MM` (Font4) で一番近い鳴動時刻。
        * 画面下部: 以降の鳴動時刻リスト（最大5件、`HH:MM`, Font2）。
   * **ボタン操作と遷移:**
        * `Aボタン（短押し）`: `D(Abs_Time_Input)` へ遷移（絶対時刻入力）。
        * `Bボタン（短押し）`: `G(Rel_Plus_Time_Input)` へ遷移（相対値加算入力）。
        * `Bボタン（長押し）`: `I(Rel_Minus_Time_Input)` へ遷移（相対値減算入力）。
        * `Cボタン（短押し）`: `S(Schedule_Select_Mode)` へ遷移（予定選択モード）。
        * `Cボタン（長押し）`: `SETTINGS` サブメニューへ遷移。

**2. NTP 時刻同期中 (B)**
   * **目的:** 時刻同期中であることをユーザーに明確に伝える。
   * **表示要素:** 画面中央上部に `SYNCING TIME...` (Font4)。画面中央下部に同期状況を示すアニメーション（アンバー）。
   * **遷移:** 同期完了またはタイムアウト後に自動的に「メイン表示 (C)」へ遷移。失敗時はエラーメッセージ表示。

**3. 入力モード (D, G, I)** (絶対時刻入力、相対値加算、相対値減算)
   * **目的:** ユーザーが目的の時刻や値を正確に入力できるようにする。
   * **共通表示要素:**
        * 画面上部: 各モードのタイトル（例: `SET ABS TIME`, `ADD REL TIME`, `SUB REL TIME`, Font4）。
        * 画面中央: 入力中の数字 (Font7)。未入力は`_`。カーソル桁はネガポジ反転。
        * 入力中の数字直下: `->YYYY/MM/DD HH:MM` (Font2) で予測される結果時刻。
   * **操作ロジック:** `A`ボタンで数字変更、`B`ボタンで桁送り、`C`ボタン短押しで確定、`C`ボタン長押しでキャンセル。

**4. 鳴動時刻選択モード (S)**
   * **目的:** 登録済み鳴動時刻の確認、個別削除、設定へのアクセス。
   * **表示要素:**
        * 画面の大部分: 鳴動時刻リスト（`HH:MM`, Font4）。最大5件表示。
        * リスト最下部: `[ SETTINGS ]` (Font4) を配置。
   * **操作ロジック:** `A/B`ボタンでリストを上下に移動、選択項目はネガポジ反転。
        * `Cボタン（短押し）`: 選択中の鳴動時刻を削除（削除時簡易メッセージ表示）。または「SETTINGS」項目選択時に設定サブメニューへ遷移。

**5. 鳴動状態 (M)**
   * **目的:** ユーザーに次の行動への「区切り」を明確に知らせる。
   * **表示要素:** 画面中央上部: `TIME UP!` (Font4)。画面中央下部: 鳴動時刻からのオーバータイム `+HH:MM:SS` (Font7)。
   * **フィードバック:**
        * 画面全体がフラッシュオレンジと黒の点滅。メッセージとオーバータイムもフラッシュオレンジ。
        * 内蔵スピーカーから音 ~とバイブレーションを同時作動~。
        * ~~バイブレーション仕様: 0.5秒作動、0.5秒OFFを5回繰り返す（合計5秒間）。音も同様に5秒間鳴動。~~
   * **解除:** 任意のボタン押下で鳴動停止し、メイン表示 (C) へ戻る。

**6. SETTINGS サブメニュー（新設）**
   * **目的:** デバイス全体の動作に関する設定変更と、全データの消去。
   * **アクセス:** メイン画面 `Cボタン（長押し）`。
   * **表示要素:**
        * 画面上部: `ALARM SETTINGS` (Font4) タイトル。
        * リスト項目 (Font4、選択時はネガポジ反転):
            * `SOUND: ON/OFF`
            * `LCD BRIGHTNESS` (輝度調整)
            * `ALL CLEAR` (全鳴動時刻の削除)
            * `INFO` (ファームウェアバージョン、著作権、MACアドレスなどを表示する情報画面への遷移)
   * **操作ロジック:** `A/B`ボタンで項目選択、`C`ボタンで設定変更/実行。
        * **「ALL CLEAR」選択時:** `ARE YOU SURE? Y/N` の確認ダイアログを表示し、二重確認。
   * **設定永続化:** `SOUND`, `LCD BRIGHTNESS` の設定値はFlashメモリに永続化。

---

**システムレベル実装要件:**

* **ライブラリ:** `M5Stack.h`, `WiFi.h`, `time.h`, `Preferences.h` など、必要に応じて。
* **電源管理:**
    * 起動時NTP同期後、Wi-FiはOFFにする。
    * メインモードでは1Hz動作を基本とし、不要な処理は極力減らし低消費電力化。
    * LCD明るさ設定は永続化。
    * ディープスリープモードの検討（一定時間アイドル後）。
* **Wi-Fi 設定:** SmartConfig 方式で実装。受信したSSID/パスワードはFlashメモリに永続化。
* **NTP 時刻同期:** 起動時にNTPで時刻同期し、RTCへ設定。
* **エラーハンドリング:** 各種エラー（Wi-Fi接続失敗など）の表示と適切な処理。
* **開発環境:** Arduino IDE または PlatformIO 環境でのビルドを想定。

---



# 擬似コード
ここからは擬似コードです。 実際には適切なファイルを作成し、そこに記載してください。
```cpp
#include <M5Stack.h>
#include <WiFi.h>
#include <time.h>
#include <Preferences.h> // 設定の永続化用

// --- グローバル変数と定数定義 ---

// 画面モード定義
enum AppMode {
    MODE_SYNCING_TIME,     // NTP 時刻同期中 (B)
    MODE_MAIN_DISPLAY,     // メイン表示:待機 (C)
    MODE_ABS_TIME_INPUT,   // 絶対時刻入力 (D)
    MODE_REL_PLUS_INPUT,   // 相対値加算入力 (G)
    MODE_REL_MINUS_INPUT,  // 相対値減算入力 (I)
    MODE_SCHEDULE_SELECT,  // 鳴動時刻選択モード (S)
    MODE_ALARM_RINGING,    // 鳴動状態 (M)
    MODE_SETTINGS,         // SETTINGS サブメニュー
    MODE_INFO_DISPLAY      // INFO サブメニュー（SETTINGSから遷移）
    // ... 必要に応じて他のモードを追加
};

AppMode currentMode = MODE_SYNCING_TIME; // 初期モードは時刻同期

// カラーパレット定義
#define COLOR_BG_DARK_GRAY     0x2104 // 背景の濃いグレー (例)
#define COLOR_AMBER            0xFDC0 // M5.Lcd.color565(255, 191, 0) // アンバー
#define COLOR_AUX_AMBER        0xCCAA77 // M5.Lcd.color565(204, 170, 119) // 補助的なアンバー
#define COLOR_FLASH_ORANGE     0xFD20 // M5.Lcd.color565(255, 127, 0) // フラッシュオレンジ
#define COLOR_BLACK            0x0000 // 黒

// フォント定義 (M5Stackの標準フォントを仮定)
// M5Stack.h には M5.Lcd.setTextFont(x) があるので、このまま指定
#define FONT_AUXILIARY 2 // Font2 (Height 16)
#define FONT_IMPORTANT 4 // Font4 (Height 26)
#define FONT_MAIN      7 // Font7 (Height 48)

// 鳴動時刻構造体 (永続化しない)
struct AlarmTime {
    byte hour;
    byte minute;
};
std::vector<AlarmTime> alarmSchedules; // 鳴動時刻リスト (最大5件)
const int MAX_ALARMS = 5;

// 設定値の永続化用インスタンス
Preferences preferences;

// Wi-Fi設定用変数
char ssid[32];
char password[64];
bool wifiConnected = false;

// 設定項目
bool soundEnabled = true;
bool vibrateEnabled = true;
int lcdBrightness = 150; // 0-255

// タイマー関連変数
unsigned long nextAlarmMillis = 0; // 次のアラームまでのミリ秒（内部計算用）
AlarmTime nextAlarmTime; // 次のアラーム時刻
time_t currentRtcTime; // 現在のRTC時刻

// 鳴動状態関連変数
unsigned long alarmRingStartTime = 0; // アラーム開始時刻

// 各モードの描画関数とボタン処理関数のプロトタイプ宣言
void setupMode(AppMode mode); // モード切り替え時の初期化
void drawMode(AppMode mode);  // 各モードの描画
void handleButtonPress(AppMode mode); // 各モードのボタン処理

// グローバルな描画ヘルパー関数
void drawStatusBar(const char* modeName);
void drawButtonHints(const char* hintA, const char* hintB, const char* hintC);
void drawBatteryStatus();
void drawDateTime(int x, int y, int font, int color, bool showDate, bool showSeconds);
void drawProgressbar(int x, int y, int width, int height, float progress, int barColor, int bgColor);
void drawInvertedText(int x, int y, const char* text, int font);


// --- setup() と loop() 関数 ---
void setup() {
    M5.begin();
    M5.Speaker.mute(); // 初期はミュート
    M5.Lcd.setBrightness(lcdBrightness); // 初期明るさ設定
    M5.Lcd.fillScreen(COLOR_BLACK);

    // 設定を読み込む
    preferences.begin("m5timer", false);
    soundEnabled = preferences.getBool("soundEn", true);
    vibrateEnabled = preferences.getBool("vibrateEn", true);
    lcdBrightness = preferences.getInt("lcdBright", 150);
    M5.Lcd.setBrightness(lcdBrightness);
    preferences.end();

    // 初期モード設定
    setupMode(MODE_SYNCING_TIME);
}

void loop() {
    M5.update(); // ボタン状態の更新

    drawMode(currentMode);
    handleButtonPress(currentMode);

    // 1Hz 動作の基本（メインモード以外でも描画更新は必要だが、処理頻度を調整）
    if (currentMode == MODE_MAIN_DISPLAY || currentMode == MODE_ALARM_RINGING) {
        delay(1000); // 1秒ごとに更新
    } else {
        delay(100); // 他のモードではより頻繁に更新（入力応答性のため）
    }

    // 全モード共通のアラート監視
    if (currentMode != MODE_ALARM_RINGING) {
        // 次のアラーム時刻が現在時刻を超えたら鳴動状態へ遷移
        time_t now;
        time(&now);
        if (alarmSchedules.size() > 0) {
            // ここで一番近い未来のアラームを計算し、nextAlarmTimeとnextAlarmMillisを更新
            // ... (計算ロジック)
            if (now >= mktime(&nextAlarmTime)) { // 実際は、アラーム時間から少し余裕を持たせるか、過去のアラームは無視するロジックが必要
                setupMode(MODE_ALARM_RINGING);
            }
        }
    }
}

// --- 各モードの描画と処理関数 ---

void setupMode(AppMode newMode) {
    currentMode = newMode;
    M5.Lcd.fillScreen(COLOR_BLACK); // 画面クリア
    // モードごとの初期設定があればここに記述
    switch (currentMode) {
        case MODE_SYNCING_TIME:
            // SmartConfigの開始
            // ...
            WiFi.mode(WIFI_AP_STA); // AP_STAモードでSmartConfigを待機
            WiFi.beginSmartConfig();
            break;
        case MODE_MAIN_DISPLAY:
            // WiFiはオフにする
            WiFi.mode(WIFI_OFF);
            M5.Lcd.setBrightness(lcdBrightness); // 明るさ設定を適用
            break;
        case MODE_ALARM_RINGING:
            alarmRingStartTime = millis(); // アラーム開始時刻を記録
            M5.Speaker.setVolume(255); // スピーカー音量最大
            if (soundEnabled) M5.Speaker.tone(1000, 500); // 初期音
            break;
        case MODE_SETTINGS:
            // Preferencesから最新の設定を読み込む
            preferences.begin("m5timer", false);
            soundEnabled = preferences.getBool("soundEn", true);
            lcdBrightness = preferences.getInt("lcdBright", 150);
            preferences.end();
            break;
        default:
            break;
    }
}

void drawMode(AppMode mode) {
    // 共通UI要素の描画
    drawStatusBar("CURRENT_MODE_NAME"); // 例: "MAIN", "SET TIME"
    M5.Lcd.setTextFont(FONT_AUXILIARY);
    M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);

    switch (mode) {
        case MODE_SYNCING_TIME:
            // NTP 時刻同期中 (B) の描画
            M5.Lcd.setCursor(0, M5.Lcd.getFontHeight() * 2); // 適当な位置
            M5.Lcd.setTextDatum(MC_DATUM); // 中央揃え
            M5.Lcd.drawString("SYNCING TIME...", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - 20);
            // アニメーションを描画 (例: ドットアニメーション)
            // ...
            M5.Lcd.drawString("[ o O . ]", M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 20);
            break;

        case MODE_MAIN_DISPLAY:
            // メイン表示:待機 (C) の描画
            // 現在日時
            drawDateTime(0, M5.Lcd.getFontHeight() * 1.5, FONT_AUXILIARY, COLOR_AUX_AMBER, true, false); // 日付
            drawDateTime(M5.Lcd.width()/2 - M5.Lcd.textWidth("00:00")/2, M5.Lcd.getFontHeight() * 3, FONT_IMPORTANT, COLOR_AUX_AMBER, false, false); // 時刻
            
            // 残り時間（Font7）
            M5.Lcd.setTextFont(FONT_MAIN);
            M5.Lcd.setTextColor(COLOR_AMBER, COLOR_BLACK);
            char remainTimeStr[10];
            // sprintf(remainTimeStr, "-%02d:%02d:%02d", H, M, S); // 次のアラームまでの時間を計算して表示
            M5.Lcd.drawString("-00:25:30", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - M5.Lcd.getFontHeight() / 2);

            // 進捗バー
            // drawProgressbar(x, y, width, height, progress, COLOR_AMBER, COLOR_BG_DARK_GRAY);
            M5.Lcd.fillRect(40, M5.Lcd.height()/2 + 30, M5.Lcd.width() - 80, 10, COLOR_AMBER); // 仮でバーを描画
            
            // NEXT時刻
            M5.Lcd.setTextFont(FONT_IMPORTANT);
            M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);
            M5.Lcd.drawString("NEXT: 18:55", M5.Lcd.width()/2, M5.Lcd.height()/2 + 50);

            // 以降の鳴動時刻リスト
            M5.Lcd.setTextFont(FONT_AUXILIARY);
            M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);
            M5.Lcd.drawString("19:00", M5.Lcd.width()/2, M5.Lcd.height()/2 + 80);
            M5.Lcd.drawString("20:00", M5.Lcd.width()/2, M5.Lcd.height()/2 + 95);

            drawButtonHints("[A: ABS]", "[B: REL+]", "[C: SEL]");
            break;

        case MODE_ABS_TIME_INPUT:
            // 入力モード (D, G, I) の描画（絶対時刻入力の例）
            M5.Lcd.setTextFont(FONT_IMPORTANT);
            M5.Lcd.setTextColor(COLOR_AMBER, COLOR_BLACK);
            M5.Lcd.drawString("SET ABS TIME", M5.Lcd.width() / 2, M5.Lcd.height() / 4);

            // 入力中の数字 (ネガポジ反転は drawInvertedText で)
            drawInvertedText(M5.Lcd.width() / 2 - M5.Lcd.textWidth("1_ : _ _") / 2, M5.Lcd.height() / 2 - 20, "1_ : _ _", FONT_MAIN);
            
            // 予測結果時刻
            M5.Lcd.setTextFont(FONT_AUXILIARY);
            M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);
            M5.Lcd.drawString("-> 2025/07/06 18:00", M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 40);

            drawButtonHints("[A: +1/5]", "[B: NEXT]", "[C: OK/CANCEL]");
            break;

        case MODE_SCHEDULE_SELECT:
            // 鳴動時刻選択モード (S) の描画
            M5.Lcd.setTextFont(FONT_IMPORTANT);
            M5.Lcd.setTextColor(COLOR_AMBER, COLOR_BLACK);
            
            // 仮のリスト表示（選択中の項目をネガポジ反転）
            drawInvertedText(M5.Lcd.width() / 2 - M5.Lcd.textWidth("15:00") / 2, M5.Lcd.height() / 2 - 20, "15:00", FONT_IMPORTANT); // 選択中
            M5.Lcd.drawString("10:00", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - 50);
            M5.Lcd.drawString("11:30", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - 80);
            M5.Lcd.drawString("13:45", M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 10);
            M5.Lcd.drawString("17:15", M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 40);
            
            M5.Lcd.drawString("[ SETTINGS ]", M5.Lcd.width() / 2, M5.Lcd.height() - 50); // SETTINGS項目

            drawButtonHints("[A: UP]", "[B: DOWN]", "[C: SEL/DEL]");
            break;

        case MODE_ALARM_RINGING:
            // 鳴動状態 (M) の描画
            unsigned long elapsedRingTime = millis() - alarmRingStartTime;
            if ((elapsedRingTime / 500) % 2 == 0) { // 0.5秒ごとに背景を反転
                 M5.Lcd.fillScreen(COLOR_FLASH_ORANGE);
            } else {
                 M5.Lcd.fillScreen(COLOR_BLACK);
            }
            
            M5.Lcd.setTextFont(FONT_IMPORTANT);
            M5.Lcd.setTextColor(COLOR_FLASH_ORANGE, M5.Lcd.color565(0,0,0)); // テキスト色は常にフラッシュオレンジ
            M5.Lcd.drawString("TIME UP!", M5.Lcd.width() / 2, M5.Lcd.height() / 2 - 40);

            char overTimeStr[10];
            // sprintf(overTimeStr, "+%02d:%02d:%02d", H_over, M_over, S_over); // オーバータイムを計算して表示
            M5.Lcd.drawString("+00:00:15", M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 20);
            break;

        case MODE_SETTINGS:
            // SETTINGS サブメニューの描画
            M5.Lcd.setTextFont(FONT_IMPORTANT);
            M5.Lcd.setTextColor(COLOR_AMBER, COLOR_BLACK);
            M5.Lcd.drawString("ALARM SETTINGS", M5.Lcd.width() / 2, M5.Lcd.height() / 4);

            // 各設定項目の表示（選択中の項目はネガポジ反転）
            // 例: drawInvertedText(x, y, "SOUND: ON", FONT_IMPORTANT);
            M5.Lcd.drawString("SOUND: ON", M5.Lcd.width()/2, M5.Lcd.height()/2 - 30);
            M5.Lcd.drawString("VIBRATE: OFF", M5.Lcd.width()/2, M5.Lcd.height()/2);
            M5.Lcd.drawString("LCD BRIGHTNESS", M5.Lcd.width()/2, M5.Lcd.height()/2 + 30);
            M5.Lcd.drawString("ALL CLEAR", M5.Lcd.width()/2, M5.Lcd.height()/2 + 60);
            M5.Lcd.drawString("INFO", M5.Lcd.width()/2, M5.Lcd.height()/2 + 90);

            drawButtonHints("[A: UP]", "[B: DOWN]", "[C: OK/BACK]");
            break;
        
        // ... 他のモード (MODE_INFO_DISPLAY など) の描画も追加

        default:
            break;
    }
}

void handleButtonPress(AppMode mode) {
    // ボタンの長押し判定用の変数
    static unsigned long buttonCPressStartTime = 0;
    const unsigned long LONG_PRESS_THRESHOLD = 500; // 500ms
    bool buttonCLongPressed = false;

    if (M5.BtnC.isPressed()) {
        if (buttonCPressStartTime == 0) {
            buttonCPressStartTime = millis();
        }
    } else {
        if (buttonCPressStartTime != 0) {
            if (millis() - buttonCPressStartTime >= LONG_PRESS_THRESHOLD) {
                buttonCLongPressed = true;
            }
            buttonCPressStartTime = 0;
        }
    }

    switch (mode) {
        case MODE_MAIN_DISPLAY:
            if (M5.BtnA.wasPressed()) { setupMode(MODE_ABS_TIME_INPUT); }
            if (M5.BtnB.wasPressed()) { setupMode(MODE_REL_PLUS_INPUT); }
            // M5.BtnB.wasHold() で長押しを判定
            // if (M5.BtnB.wasHold()) { setupMode(MODE_REL_MINUS_INPUT); } // M5StackライブラリのwasHoldはリリース時判定なので注意
            if (M5.BtnC.wasPressed() && !buttonCLongPressed) { setupMode(MODE_SCHEDULE_SELECT); }
            if (buttonCLongPressed) { setupMode(MODE_SETTINGS); } // C長押しでSETTINGSへ
            break;

        case MODE_SYNCING_TIME:
            // SmartConfig完了/失敗で自動遷移、ボタン操作なし
            if (WiFi.smartConfigDone()) {
                // WiFi.smartConfigDone() は一度trueを返したら終わり。
                // SSIDとパスワードを取得し、Preferencesに保存
                preferences.begin("m5timer", false);
                preferences.putString("ssid", WiFi.SSID());
                preferences.putString("password", WiFi.psk());
                preferences.end();
                setupMode(MODE_MAIN_DISPLAY);
            } else if (WiFi.smartConfigTimeout()) { // タイムアウト処理（仮）
                // エラー表示
                M5.Lcd.fillScreen(COLOR_BLACK);
                M5.Lcd.setTextFont(FONT_IMPORTANT);
                M5.Lcd.setTextColor(COLOR_FLASH_ORANGE, COLOR_BLACK);
                M5.Lcd.drawString("SYNC FAILED", M5.Lcd.width()/2, M5.Lcd.height()/2);
                delay(2000); // 2秒表示
                setupMode(MODE_MAIN_DISPLAY); // メイン画面に戻る
            }
            break;

        case MODE_ALARM_RINGING:
            // 任意のボタン押下でアラーム停止
            if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed()) {
                M5.Speaker.mute();
                setupMode(MODE_MAIN_DISPLAY);
            }
            // 5秒経過で自動停止（バイブレーション、音）
            if (millis() - alarmRingStartTime >= 5000) {
                M5.Speaker.mute();
            }
            // スピーカーの間欠作動も同様に実装可能

            break;

        case MODE_ABS_TIME_INPUT:
            // 入力ロジック (A:数字, B:桁送り, C:確定/キャンセル)
            // ...
            if (M5.BtnC.wasPressed()) { setupMode(MODE_MAIN_DISPLAY); } // 仮の遷移
            break;

        case MODE_SCHEDULE_SELECT:
            // リスト選択ロジック (A:UP, B:DOWN, C:SEL/DEL)
            // ...
            if (M5.BtnC.wasPressed()) {
                // 選択中の項目がSETTINGSならSETTINGSモードへ
                // それ以外なら削除
                setupMode(MODE_MAIN_DISPLAY); // 仮の遷移
            }
            break;
        
        case MODE_SETTINGS:
            // 設定項目選択と変更ロジック
            // ...
            if (M5.BtnC.wasPressed()) { setupMode(MODE_SCHEDULE_SELECT); } // 仮の遷移（選択モードに戻る）
            break;

        // ... 他のモードのボタン処理も追加

        default:
            break;
    }
}

// --- グローバルヘルパー関数の実装 ---

void drawStatusBar(const char* modeName) {
    M5.Lcd.setTextFont(FONT_AUXILIARY);
    M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);
    M5.Lcd.setTextDatum(TL_DATUM); // 左上揃え
    M5.Lcd.drawString(modeName, 5, 5); // 左端にモード名

    drawBatteryStatus(); // バッテリー表示は専用関数で
}

void drawBatteryStatus() {
    // M5.Power.getBatteryLevel() や M5.Power.isCharging() を使う
    // M5Stack FireはM5.Powerオブジェクトを使う
    int batteryLevel = M5.Power.getBatteryLevel(); // 0-100%
    bool isCharging = M5.Power.isCharging();

    char batteryStr[10];
    sprintf(batteryStr, "%s %d%%", isCharging ? "⚡" : "🔋", batteryLevel);

    // 残量に応じて色を変更
    int batteryColor = COLOR_AUX_AMBER;
    if (batteryLevel <= 20 && !isCharging) {
        batteryColor = COLOR_FLASH_ORANGE;
    }

    M5.Lcd.setTextFont(FONT_AUXILIARY);
    M5.Lcd.setTextColor(batteryColor, COLOR_BLACK);
    M5.Lcd.setTextDatum(TR_DATUM); // 右上揃え
    M5.Lcd.drawString(batteryStr, M5.Lcd.width() - 5, 5);
}

void drawButtonHints(const char* hintA, const char* hintB, const char* hintC) {
    M5.Lcd.setTextFont(FONT_AUXILIARY);
    M5.Lcd.setTextColor(COLOR_AUX_AMBER, COLOR_BLACK);
    M5.Lcd.setTextDatum(BL_DATUM); // 左下揃え

    int padding = 10;
    int buttonWidth = (M5.Lcd.width() - padding * 4) / 3; // 左右とボタン間のパディング

    // ボタンAヒント
    M5.Lcd.drawString(hintA, padding + buttonWidth / 2, M5.Lcd.height() - padding);
    M5.Lcd.setTextDatum(BC_DATUM); // 中央揃え
    // ボタンBヒント
    M5.Lcd.drawString(hintB, M5.Lcd.width() / 2, M5.Lcd.height() - padding);
    // ボタンCヒント
    M5.Lcd.setTextDatum(BR_DATUM); // 右下揃え
    M5.Lcd.drawString(hintC, M5.Lcd.width() - padding, M5.Lcd.height() - padding);
}

void drawDateTime(int x, int y, int font, int color, bool showDate, bool showSeconds) {
    struct tm timeinfo;
    getLocalTime(&timeinfo);

    M5.Lcd.setTextFont(font);
    M5.Lcd.setTextColor(color, COLOR_BLACK);
    M5.Lcd.setTextDatum(MC_DATUM); // 中央揃え

    char timeStr[20];
    if (showDate) {
        // 例: 2025/07/06 (SUN)
        char wday_names[][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        sprintf(timeStr, "%04d/%02d/%02d (%s)", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, wday_names[timeinfo.tm_wday]);
        M5.Lcd.drawString(timeStr, M5.Lcd.width()/2, y);
    } else {
        // 例: HH:MM または HH:MM:SS
        if (showSeconds) {
            sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        } else {
            sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        }
        M5.Lcd.drawString(timeStr, M5.Lcd.width()/2, y);
    }
}

void drawProgressbar(int x, int y, int width, int height, float progress, int barColor, int bgColor) {
    // progressは0.0(0%)から1.0(100%)
    M5.Lcd.fillRect(x, y, width, height, bgColor); // 背景色で全体を塗る
    int filledWidth = (int)(width * progress);
    M5.Lcd.fillRect(x, y, filledWidth, height, barColor); // プログレス部分を塗る
}

void drawInvertedText(int x, int y, const char* text, int font) {
    M5.Lcd.setTextFont(font);
    M5.Lcd.setTextDatum(MC_DATUM); // 中央揃え
    M5.Lcd.setTextColor(COLOR_BLACK, COLOR_AMBER); // 反転色
    M5.Lcd.drawString(text, x, y);
    M5.Lcd.setTextColor(COLOR_AMBER, COLOR_BLACK); // デフォルト色に戻す
}

// 時刻同期関数
void obtainTime() {
    // NTPサーバー設定
    configTime(9 * 3600, 0, "ntp.nict.jp", "time.google.com", "pool.ntp.org"); // 日本時間JST=UTC+9
    
    // 時刻取得まで待機
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) { // Unixエポックからの時間が短すぎる場合（つまり同期前）
        delay(500);
        time(&now);
        M5.Lcd.drawString(".", M5.Lcd.width()/2 + (now%3)*10, M5.Lcd.height()/2 + 20); // 簡単なアニメーション
        if (M5.BtnC.isPressed()) { // デバッグ用：同期中にCボタンでスキップ
            Serial.println("Skipping time sync...");
            break;
        }
    }
}
```


