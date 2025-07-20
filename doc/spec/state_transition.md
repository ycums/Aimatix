| 現在モード               | ボタンイベント | アクション   | 次モード                |
|---------------------|---------|---------|---------------------|
| MAIN_DISPLAY        | A短押し    | NOP     | ABS_TIME_INPUT      |
| MAIN_DISPLAY        | A長押し    | NOP     | (遷移なし)              |
| MAIN_DISPLAY        | B短押し    | NOP     | REL_PLUS_TIME_INPUT |
| MAIN_DISPLAY        | B長押し    | NOP     | (遷移なし)              |
| MAIN_DISPLAY        | C短押し    | NOP     | ALARM_MANAGEMENT    |
| MAIN_DISPLAY        | C長押し    | NOP     | SETTINGS_MENU       |
|                     |         |         |                     |
| ABS_TIME_INPUT      | A短押し    | +1      | (遷移なし)              |
| ABS_TIME_INPUT      | A長押し    | +5      | (遷移なし)              |
| ABS_TIME_INPUT      | B短押し    | カーソルを次へ | (遷移なし)              |
| ABS_TIME_INPUT      | B長押し    | 入力クリア   | (遷移なし)              |
| ABS_TIME_INPUT      | C短押し    | 確定      | MAIN_DISPLAY        |
| ABS_TIME_INPUT      | C長押し    | NOP     | MAIN_DISPLAY        |
|                     |         |         |                     |
| REL_PLUS_TIME_INPUT | A短押し    | +1      | (遷移なし)              |
| REL_PLUS_TIME_INPUT | A長押し    | +5      | (遷移なし)              |
| REL_PLUS_TIME_INPUT | B短押し    | カーソルを次へ | (遷移なし)              |
| REL_PLUS_TIME_INPUT | B長押し    | 入力クリア   | (遷移なし)              |
| REL_PLUS_TIME_INPUT | C短押し    | 確定      | MAIN_DISPLAY        |
| REL_PLUS_TIME_INPUT | C長押し    | NOP     | MAIN_DISPLAY        |
|                     |         |         |                     |
| ALARM_MANAGEMENT    | A短押し    | 上へ      | (遷移なし)              |
| ALARM_MANAGEMENT    | A長押し    | 一番上へ    | (遷移なし)              |
| ALARM_MANAGEMENT    | B短押し    | 下へ      | (遷移なし)              |
| ALARM_MANAGEMENT    | B長押し    | 一番下へ    | (遷移なし)              |
| ALARM_MANAGEMENT    | C短押し    | 項目削除    | (遷移なし)              |
| ALARM_MANAGEMENT    | C長押し    | NOP     | MAIN_DISPLAY        |
|                     |         |         |                     |
| SETTINGS_MENU       | A短押し    | 上へ      | (遷移なし)              |
| SETTINGS_MENU       | A長押し    | 一番上へ    | (遷移なし)              |
| SETTINGS_MENU       | B短押し    | 下へ      | (遷移なし)              |
| SETTINGS_MENU       | B長押し    | 一番下へ    | (遷移なし)              |
| SETTINGS_MENU       | C短押し    | 値変更     | (遷移なし)              |
| SETTINGS_MENU       | C長押し    | NOP     | MAIN_DISPLAY        |
|                     |         |         |                     |
| ALARM_ACTIVE        | A短押し    | 鳴動解除    | MAIN_DISPLAY        |
| ALARM_ACTIVE        | A長押し    | 鳴動解除    | MAIN_DISPLAY        |
| ALARM_ACTIVE        | B短押し    | 鳴動解除    | MAIN_DISPLAY        |
| ALARM_ACTIVE        | B長押し    | 鳴動解除    | MAIN_DISPLAY        |
| ALARM_ACTIVE        | C短押し    | 鳴動解除    | MAIN_DISPLAY        |
| ALARM_ACTIVE        | C長押し    | 鳴動解除    | 23                  |