```bash
pio run -t upload # upload
pio device list # シリアルポートの確認
pio device monitor --baud 115200
pio test -e test-m5stack-fire -f test_button_manager #テストを実行
```