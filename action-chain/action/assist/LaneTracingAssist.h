#ifndef LANE_TRACING_ASSIST_H
#define LANE_TRACING_ASSIST_H

#include "IAssist.h"
#include "device/Device.h"
#include "CalcErrorFunc.h"
#include "IAssistGenerator.h"

/**
 * 青白線に沿って走行する補助クラスのファクトリー関数
 * @param device デバイス
 * @param isRightSide 右側に走行するかどうか
 * @param kp 比例ゲイン
 * @param ki 積分ゲイン
 * @param kd 微分ゲイン
 * @param calcError 誤差を計算する関数
 * @return 青白線に沿って走行する補助クラスのファクトリー関数
 */
IAssistGenerator laneTracingAssistGenerator(
    Device* device,
    bool isRightSide,
    float kp,
    float ki,
    float kd,
    CalcErrorFunc calcError
);

/**
 * 青白線に沿って走行する補助クラス
 * 青白線の境界を検出し、その境界に沿って走行する
 */
class LaneTracingAssist : public IAssist
{
    public:
        LaneTracingAssist(
            Device* device,

            /**
             * 右側に走行するかどうか
             * @note
             *  true: 右側に走行する
             *  false: 左側に走行する
             */
            bool isRightSide,

            /**
             * 比例ゲイン
             */
            float kp,

            /**
             * 積分ゲイン
             */
            float ki,

            /**
             * 微分ゲイン
             */
            float kd,

            /**
             * 誤差を計算する関数
             */
            CalcErrorFunc calcError
        );
        ~LaneTracingAssist();

        void init() override;

        /**
         * 青白線に沿って走行する
         * @param speeds 走行速度[°/s](構造体の中身を更新することで次の速度を渡す)
         * @note
         *  speeds[0]: 左輪の速度[°/s]
         *  speeds[1]: 右輪の速度[°/s]
         */
        void correct(float* speeds) override;

    private:
        bool mIsRightSide;
        float mKp;
        float mKi;
        float mKd;
        CalcErrorFunc mCalcError;

        /**
         * エラーヒストリー
         */
        static const int mTotalHistory = 5; // とりあえず過去5回分
        float* mErrorHistory;
        int mErrorHistoryIndex;
        float mErrorIntegral;   // 計算の効率化のため前回の値を記憶
                                // 毎回mErrorHistoryをfor足し算すると効率が悪いかもしれない
};

#endif // LANE_TRACING_ASSIST_H