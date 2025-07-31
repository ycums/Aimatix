## 修正完了: BA操作での入力値表示問題

### 問題の概要
前提: 絶対値入力画面 初期状態、現在時刻 `00:00`
操作: BA (Button B → Button A)
    操作の意味
    0. (初期状態: `__:__` )
    1. 無効な桁送りを試行 (Button B)
    2. 数値を+1 (Button A)

### 修正前の問題
- Expected: プレビューの表示 `00:01`
- Actual: プレビューの表示 `00:10`

### 根本原因
`PartialInputLogic::parsePartialInput` メソッドで、分一桁のみ入力時に分十桁として解釈していた。
```cpp
// 修正前: 分一桁のみ入力時に分十桁として解釈
minute = digits[3] * HOURS_10 + 0; // 1 * 10 + 0 = 10
```

### 修正内容
分一桁のみの入力時は、直感的に分一桁として解釈するよう変更。
```cpp
// 修正後: 分一桁のみ入力時に分一桁として解釈
minute = 0 * HOURS_10 + digits[3]; // 0 * 10 + 1 = 1
```

### 修正ファイル
- `lib/libaimatix/src/PartialInputLogic.cpp`: 解釈ロジック修正
- `lib/libaimatix/src/PartialInputLogic.h`: コメント更新
- `test/pure/test_partial_input_logic_pure/test_main.cpp`: テストケース更新
- `test/pure/test_alarm_logic_pure/test_main.cpp`: テストケース更新
- `test/pure/test_input_display_pure/test_main.cpp`: バグ再現テスト追加

### 検証結果
- 全110テストケース成功
- BA操作で期待される `00:01` が正しく表示される
- 既存機能への影響なし

### 修正日時
2025年1月