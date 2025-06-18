#include "LaneTracingAssist.h"
#include "IAssist.h"

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
    Perception* perc,
    bool isRightSide,
    float kp,
    float ki,
    float kd
): IAssist(twinWheelDrive, frontArmDrive, perc)
{
    mIsRightSide = isRightSide;
    mKp = kp;
    mKi = ki;
    mKd = kd;
}

LaneTracingAssist::~LaneTracingAssist()
{
}

void LaneTracingAssist::init(float baseLeftSpeed, float baseRightSpeed)
{
    mBaseLeftSpeed = baseLeftSpeed;
    mBaseRightSpeed = baseRightSpeed;
    mErrorIntegral = 0.0f;
    mPreviousError = 0.0f;
}

void LaneTracingAssist::correct(float* speeds)
{
    /**
    * 青白線の境界線からの誤差を計算する
    */
    float error = (float)((mPerc->getColorV() - BLACK_WHITE_BORDER_V_IDEAL) / 100.0f);

    mErrorIntegral += error;
    if (mErrorIntegral > INTEGRAL_LIMIT) {
        mErrorIntegral = INTEGRAL_LIMIT;
    } else if (mErrorIntegral < -INTEGRAL_LIMIT) {
        mErrorIntegral = -INTEGRAL_LIMIT;
    }
    
    float derivative = error - mPreviousError;
    mPreviousError = error;
    
    /**
     * PID制御による駆動指示
     * errorは、実測した輝度が閾値よりもどのくらい明るかったかを表す。
     * (負数のときは閾値よりも暗かったことを表している。)
     * 
     * @note 
     *      PID制御とは
     *      P制御（比例項）：現在の誤差に比例した制御量
     *      I制御（積分項）：過去の誤差の累積に比例した制御量
     *      D制御（微分項）：誤差の変化率に比例した制御量
     *      これにより定常誤差を減らし、応答性とオーバーシュート抑制を両立した安定したライントレースが可能になる
     * 
     * P制御項：
     * errorが正数のときは目標よりも白いところを通っているため、右輪を加速して左方向に移動する
     * errorが負数のときは目標よりも黒いところを通っているため、左輪を加速して右方向に移動する
     * 
     * I制御項：
     * 過去のエラーの累積により定常誤差を減らし、より正確にラインの中央を追従する
     * 
     * D制御項：
     * エラーの変化率に基づいて予測的に制御し、オーバーシュートを抑制してより滑らかな動作を実現する
     */
    float pidControl = mKp * error + mKi * mErrorIntegral + mKd * derivative;
    
    /**
     * 境界の左右どちらに沿って走行するか
     * 基本的には白色の床面に対して、黒や青のレーンが引いてあることを想定している
     * こう考えた時、mIsRightSide = trueの時は線の右縁に沿って走行すると想定する
     */
    const int sign = mIsRightSide ? 1 : -1;
    speeds[0] = mBaseLeftSpeed - sign * pidControl;
    speeds[1] = mBaseRightSpeed + sign * pidControl;
}