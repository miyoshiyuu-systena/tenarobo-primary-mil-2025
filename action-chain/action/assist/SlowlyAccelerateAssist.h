#ifndef SLOWLY_ACCELERATE_ASSIST_H
#define SLOWLY_ACCELERATE_ASSIST_H

#include "IAssist.h"
#include "IAssistGenerator.h"

/**
 * ゆっくりと加速する補助のファクトリー関数
 */
IAssistGenerator slowlyAccelerateAssistGenerator(
    Device* device,
    int stepCount,
    int stepInterval
);

/**
 * ゆっくりと加速する補助
 * 走行中にゆっくり（段階的に）加速するようになる
 */
class SlowlyAccelerateAssist : public IAssist
{
    public:
        /**
         * ゆっくりと加速する補助のコンストラクタ
         * @param device デバイス
         * @param stepCount 加速の段階数
         * @param stepInterval 1ステップあたり判定周期回数
         * @note
         *  例えば、
         *  detectInterval = 10[ms]
         *  stepCount = 5
         *  stepInterval = 2
         *  とすると、
         *  
         *  detectInteval * stepInterval = 20msに一回ずつ加速する
         *  一回目の加速では入力された速度の1/5の速度にする
         *  二回目の加速では入力された速度の2/5の速度にする
         *  というふうに線形に速度を向上させる。
         */
        SlowlyAccelerateAssist(
            Device* device,

            /**
             * 加速の段階数
             */
            int stepCount,

            /**
             * 判定の間隔
             */
            int stepInterval
        );
        ~SlowlyAccelerateAssist();

        void init() override;

        void correct(float* speeds) override;

    private:
        int mStepCount;
        int mStepInterval;
        int mStepIndex;
};

#endif