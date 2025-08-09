#ifndef PERCEPTION_REPORTER_H
#define PERCEPTION_REPORTER_H

#include "PerceptionReport.h"
#include <cstdint>

/**
 * 知覚データを更新して、格納する
 */
class PerceptionReporter {
    public:
        /**
         * シングルトンインスタンス取得
         */
        static PerceptionReporter& getInstance();
        
        /**
        * 最新の観測結果を取得する
        * @return 最新の観測結果
        */
        PerceptionReport getLatest() const;

        /**
         * 最新の観測結果の画像を更新したかどうか
         * @return 最新の観測結果の画像を更新したかどうか
         */
        bool isImageUpdated() const;
        
        /**
        * 観測結果を更新する
        * @param detectInterval 検知間隔[ms]
        * @param mask 更新する情報を制限するマスク（必要なデータだけ取得する）
        */
        void update(int detectInterval, uint8_t mask);

        /**
         * 初期化
         * 全ての値を0に初期化する
         */
        void init();
        
    private:
        /**
         * プライベートコンストラクタ
         */
        PerceptionReporter();

        /**
         * コピーコンストラクタを削除
         */
        PerceptionReporter(const PerceptionReporter&) = delete;

        /**
         * 代入演算子を削除
         */
        PerceptionReporter& operator=(const PerceptionReporter&) = delete;

        /**
         * カメラの撮影に最適な間隔
         * 34 [ms] * 30 [フレーム / s] = 1.02 [フレーム]
         */
        static const int CAMERA_INTERVAL = 34;

        /**
         * 最新の観測結果
         */
        PerceptionReport mLatestReport;

        /**
         * 観測回数
         */
        int mCount;

        /**
         * 前回Webカメラ画像を取得したときのインデックス
         */
        int mPrevIndex;

        /**
         * Webカメラ画像を更新したかどうか
         */
        bool mIsImageUpdated;

        /**
         * ログ出力を行うかどうか
         */
        bool mIsLogOutput;
};

#endif