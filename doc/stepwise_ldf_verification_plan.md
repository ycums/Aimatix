# lib/testペア段階移植によるLDF動作検証プラン

## 目的
Aimatix本体のlib/aimatix_lib/src/とtest/pure/配下の各ペア（例：button_manager, alarm, debounce_manager等）を、test_minimal_projectに段階的に移動し、どのタイミング・組み合わせでビルドやテストが壊れるかを検証する。

## 検証手順

1. **初期状態の確認**
    - test_minimal_projectは最小構成（add関数サンプル等）でテストが通ることを確認

2. **1ペアずつ移植・検証**
    - 例：
        1. lib/aimatix_lib/src/button_manager.* と test/pure/test_button_manager_pure/ を test_minimal_project/lib/your_lib/src/ および test_minimal_project/test/ にコピー
        2. include記法・ディレクトリ構成は本体と同じにする
        3. `pio test` でビルド・テスト
        4. テストが通れば次のペアへ進む
        5. テストが失敗・ビルドエラーの場合、その時点の構成・エラー内容・include記法・依存関係をレポートファイルに記録して、巻き戻す。

3. **他ペアも同様に追加し、都度ビルド・テスト**
    - alarm, debounce_manager, input, settings, time_logic など順次追加
    - 追加ごとに `pio test` で検証

4. **壊れたタイミングでの詳細調査**
    - 依存関係の循環、includeパスの問題、他ファイルへの依存などを重点的に確認
    - 必要に応じてlib/構成やinclude記法を修正し、再度テスト

5. **原因特定後、本体の是正策にフィードバック**
    - 問題点・解決策をplatformio_best_practice_plan.md等に追記

## ポイント
- コピー時はディレクトリ構成（lib/aimatix_lib/src/xxx.h など）を厳密に再現する
- テストコードのinclude記法も本体と同じにしておく
- 1ペアずつ追加し、壊れたらその直前の差分を記録
- 必要に応じて`pio run --target clean`でキャッシュクリア

## 参考
- LDF仕様: https://docs.platformio.org/en/latest/librarymanager/ldf.html
- 是正策: platformio_best_practice_plan.md 