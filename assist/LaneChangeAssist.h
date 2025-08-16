#ifndef LANE_CHANGE_ASSIST_H
#define LANE_CHANGE_ASSIST_H

#include "IAssist.h"
#include "IAssistGenerator.h"
#include "Device.h"

/**
 * 車線変更補助クラスのファクトリー関数
 * @param isRightSide 右側に走行するかどうか
 * @return 車線変更補助クラスのファクトリー関数
 */
IAssistGenerator laneChangeAssistGenerator(
    bool isRightSide
);

/**
 * 車線変更補助クラス
 * 車線変更を補助する
 */
class LaneChangeAssist : public IAssist
{
    public:
        LaneChangeAssist(
            /**
             * 右側に走行するかどうか
             * どの方向に左右並行移動するか
             */
            bool isRightSide
        );
        ~LaneChangeAssist();

        void init() override;

        /**
         * 左右に並行移動する
         * @param speeds 左右に並行移動する速度[°/s]
         * @note
         *  speeds[0]: 左輪の速度[°/s]
         *  speeds[1]: 右輪の速度[°/s]
         */
        void correct(float* speeds) override;

    private:
        bool mIsRightSide;

        /**
         * 判定回数
         */
        int mDetectCount;
};

#endif