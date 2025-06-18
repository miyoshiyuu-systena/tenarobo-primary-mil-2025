#include "LaneTracingAssist.h"

/**
 * XXX: debug
 * 三好家の床では60の方がうまくいく
 */
// static const float BLACK_WHITE_BORDER_V_IDEAL = 45.0f;
static const float BLACK_WHITE_BORDER_V_IDEAL = 60.0f;

// 積分の最大値（最小値）
// 飽和防止のための制限値
static const float INTEGRAL_LIMIT = 5.0f;

LaneTracingAssist::LaneTracingAssist(
    TwinWheelDrive* twinWheelDrive,
    FrontArmDrive* frontArmDrive,
    Perception* perception
)
    : mTwinWheelDrive(twinWheelDrive)
    , mFrontArmDrive(frontArmDrive)
    , mPerception(perception)
{}

LaneTracingAssist::~LaneTracingAssist()
{
}

void LaneTracingAssist::init(float baseLeftSpeed, float baseRightSpeed)
{
    mBaseLeftSpeed = baseLeftSpeed;
    mBaseRightSpeed = baseRightSpeed;
    mErrorIntegral = 0.0f;
    mPreviousError = 0.0f;
    mKp = 10.0f;
    mKi = 1.0f;
    mKd = 0.1f;
}

void LaneTracingAssist::do(float* speeds)
{
    /**
     * 青白線の境界線からの誤差を計算する
     */
    float error = (float)((mPerception->getColorV() - BLACK_WHITE_BORDER_V_IDEAL) / 100.0f);

    mErrorIntegral += error;
    if (mErrorIntegral > INTEGRAL_LIMIT) {
        mErrorIntegral = INTEGRAL_LIMIT;
    } else if (mErrorIntegral < -INTEGRAL_LIMIT) {
        mErrorIntegral = -INTEGRAL_LIMIT;
    }

    float derivative = error - mPreviousError;
    mPreviousError = error;

    float pidControl = mKp * error + mKi * mErrorIntegral + mKd * derivative;

    /**
     * HACK: 左右の速度の符号を反転する
     */
    speeds[0] = mBaseLeftSpeed - pidControl;
    speeds[1] = mBaseRightSpeed + pidControl;
}