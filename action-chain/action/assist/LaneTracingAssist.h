#ifndef LANE_TRACING_ASSIST_H
#define LANE_TRACING_ASSIST_H

#include "IAssist.h"
#include "drive/TwinWheelDrive.h"
#include "drive/FrontArmDrive.h"
#include "perception/Perception.h"

/**
 * 青白線に沿って走行する補助クラス
 * 青白線の境界を検出し、その境界に沿って走行する
 */
class LaneTracingAssist : public IAssist
{
    public:
        LaneTracingAssist(
            TwinWheelDrive* twinWheelDrive,
            FrontArmDrive* frontArmDrive,
            Perception* perception);
        ~LaneTracingAssist();

        void init(float baseLeftSpeed, float baseRightSpeed) override;

        /**
         * 青白線に沿って走行する
         * @param speeds 走行速度[°/s](構造体の中身を更新することで次の速度を渡す)
         */
        void correct(float* speeds) override;

    private:
        TwinWheelDrive* mTwinWheelDrive;
        FrontArmDrive* mFrontArmDrive;
        Perception* mPerc;
        float mBaseLeftSpeed;
        float mBaseRightSpeed;
        float mErrorIntegral;
        float mPreviousError;
        float mKp;
        float mKi;
        float mKd;
};

#endif // LANE_TRACING_ASSIST_H