#ifndef UI_LOGIC_H
#define UI_LOGIC_H

// メニューインデックスを循環させるロジック
inline int nextMenuIndex(int current, int itemCount) {
    return (current + 1) % itemCount;
}

inline int prevMenuIndex(int current, int itemCount) {
    return (current - 1 + itemCount) % itemCount;
}

// アラーム管理画面などリスト系のインデックス移動ロジック
inline int nextAlarmIndex(int current, int count) {
    return (current + 1) % count;
}
inline int prevAlarmIndex(int current, int count) {
    return (current - 1 + count) % count;
}

#endif // UI_LOGIC_H 