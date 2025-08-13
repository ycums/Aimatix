#pragma once
#include "ITimeService.h"
#include "PartialInputLogic.h"
#include <string>
#include <ctime>

class TimePreviewLogic {
public:
    struct PreviewResult {
        std::string preview;
        bool isValid;
    };
    
    /**
     * プレビュー文字列を生成する
     * @param digits 入力された数字配列
     * @param entered 入力済みフラグ配列
     * @param timeProvider 時刻プロバイダー
     * @param isRelativeMode 相対値入力モードかどうか
     * @return プレビュー結果
     */
    static PreviewResult generatePreview(
        const int* digits, 
        const bool* entered, 
        ITimeService* timeService,
        bool isRelativeMode
    );
    
    /**
     * 相対値プレビューを生成する
     * @param relativeTime 相対時刻
     * @param timeProvider 時刻プロバイダー
     * @return プレビュー結果
     */
    static PreviewResult generateRelativePreview(
        time_t relativeTime,
        ITimeService* timeService
    );
    
private:
    /**
     * 絶対時刻を計算する
     * @param digits 入力された数字配列
     * @param entered 入力済みフラグ配列
     * @param timeProvider 時刻プロバイダー
     * @return 計算された絶対時刻（失敗時は-1）
     */
    static time_t calculateAbsoluteTime(
        const int* digits, 
        const bool* entered, 
        ITimeService* timeService
    );
    
    /**
     * プレビュー文字列をフォーマットする
     * @param time 時刻
     * @param timeProvider 時刻プロバイダー
     * @param isRelativeMode 相対値入力モードかどうか
     * @return フォーマットされたプレビュー文字列
     */
    static std::string formatPreview(
        time_t time, 
        ITimeService* timeService,
        bool isRelativeMode
    );
    
    /**
     * 日付跨ぎ判定を行う
     * @param targetTime 対象時刻
     * @param currentTime 現在時刻
     * @param timeProvider 時刻プロバイダー
     * @return 日数差（0の場合は同日）
     */
    static int calculateDayDifference(
        time_t targetTime,
        time_t currentTime,
        ITimeService* timeService
    );
}; 