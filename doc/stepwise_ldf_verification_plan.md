# lib/testペア段階移植によるLDF動作検証プラン

## 目的
Aimatix本体のlib/libaimatix/src/とtest/pure/配下の各ペア（例：button_manager, alarm, debounce_manager等）を、test_minimal_projectに段階的に移動し、どのタイミング・組み合わせでビルドやテストが壊れるかを検証する。

## 検証手順

1. **初期状態の確認**
    - test_minimal_projectは最小構成（add関数サンプル等）でテストが通ることを確認
    - **PlatformIOテスト構成の確認**: 各テストが独立したディレクトリに配置されているか確認

2. **1ペアずつ移植・検証**
    - 例：
        1. lib/libaimatix/src/button_manager.* と test/pure/test_button_manager_pure/ を test_minimal_project/lib/your_lib/src/ および test_minimal_project/test/ にコピー
        2. **重要**: テストディレクトリ構造をPlatformIOのベストプラクティスに従って配置
        3. include記法・ディレクトリ構成は本体と同じにする
        4. `pio test` でビルド・テスト
        5. テストが通れば次のペアへ進む
        6. テストが失敗・ビルドエラーの場合、その時点の構成・エラー内容・include記法・依存関係をレポートファイルに記録して、巻き戻す。

3. **他ペアも同様に追加し、都度ビルド・テスト**
    - alarm, debounce_manager, input, settings, time_logic など順次追加
    - 追加ごとに `pio test` で検証

4. **壊れたタイミングでの詳細調査**
    - 依存関係の循環、includeパスの問題、他ファイルへの依存などを重点的に確認
    - 必要に応じてlib/構成やinclude記法を修正し、再度テスト

5. **原因特定後、本体の是正策にフィードバック**
    - 問題点・解決策をplatformio_best_practice_plan.md等に追記

## PlatformIOテスト構成のベストプラクティス

### テストディレクトリ構造
```
test_minimal_project/test/
├── test_button_manager_pure/     # 独立したディレクトリ
│   └── test_main.cpp            # ButtonManager専用テスト
├── test_alarm_logic_pure/       # 独立したディレクトリ
│   └── test_main.cpp            # Alarm専用テスト
└── mocks/
    ├── mock_button_manager.h
    └── ...
```

### 重要なポイント
- **各テストは独立したディレクトリに配置**: 複数のtest_main.cppを同じディレクトリに配置しない
- **ディレクトリ名は`test_`プレフィックスで開始**: PlatformIOが自動検出するため
- **各test_main.cppは単一の責務**: 特定のモジュール/機能のテストのみ

### よくある問題と解決策
- **リンカーエラー「multiple definition of main」**: 複数のtest_main.cppが同じディレクトリにある
- **テストが検出されない**: ディレクトリ名が`test_`で始まっていない
- **モックが見つからない**: `test/mocks/`に配置し、`-Itest/mocks`でインクルード

## ポイント
- コピー時はディレクトリ構成（lib/libaimatix/src/xxx.h など）を厳密に再現する
- **テストディレクトリ構造はPlatformIOのベストプラクティスに従う**
- テストコードのinclude記法も本体と同じにしておく
- 1ペアずつ追加し、壊れたらその直前の差分を記録
- 必要に応じて`pio run --target clean`でキャッシュクリア

## 参考
- LDF仕様: https://docs.platformio.org/en/latest/librarymanager/ldf.html
- PlatformIOテスト仕様: https://docs.platformio.org/en/latest/advanced/unit-testing/index.html
- 是正策: platformio_best_practice_plan.md 