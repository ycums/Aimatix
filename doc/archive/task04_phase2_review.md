1. `drawMainDisplay()`のグリッド配置修正
   1. 指摘なし
2. `drawSettingsMenu()`のグリッド配置修正
   1. 共通ロジックに 1px の margin top を追加
3. `drawScheduleSelect()`のグリッド配置修正
   1. 各項目をグリッドひとつ分上に上げた方が良い。
4. `drawInputMode()`のグリッド配置修正
   1. 入力カーソル時の文字幅が入力後の文字幅の半分しかなく、反転領域が細いので違和感がある。入力前後で文字幅が合うようにしたい。