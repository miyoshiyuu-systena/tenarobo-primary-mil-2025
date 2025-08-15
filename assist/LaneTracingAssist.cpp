#include "LaneTracingAssist.h"
#include "ColorSensor.h"
#include "Device.h"

IAssistGenerator laneTracingAssistGenerator(
    bool isRightSide,
    float kp,
    float ki,
    float kd,
    CalcErrorFunc calcError
) {
    return [isRightSide, kp, ki, kd, calcError](Device*& device) {
        return new LaneTracingAssist(device, isRightSide, kp, ki, kd, calcError);
    };
}

LaneTracingAssist::LaneTracingAssist(
    Device*& device,
    bool isRightSide,
    float kp,
    float ki,
    float kd,
    CalcErrorFunc calcError
):
    IAssist()
    , mDevice(device)
    , mIsRightSide(isRightSide)
    , mKp(kp)
    , mKi(ki)
    , mKd(kd)
    , mCalcError(calcError)
    , mErrorHistory(new float[mTotalHistory]())
    , mErrorHistoryIndex(0)
    , mErrorIntegral(0)
{
}

LaneTracingAssist::~LaneTracingAssist()
{
    delete[] mErrorHistory;
}

void LaneTracingAssist::init()
{
}

void LaneTracingAssist::correct(float* speeds)
{
    /**
    * 青白線の境界線からの誤差を計算する
    */
    ColorSensor::HSV hsv;
    mDevice->colorSensor.getHSV(hsv, true);
    float error = mCalcError(hsv.h, hsv.s, hsv.v);
    
    /**
     * 過去のエラー値を集計する
     * - もっとも古いデータを差し引いて
     * - 新しいデータを追加する
     */
    mErrorIntegral -= mErrorHistory[mErrorHistoryIndex];
    mErrorIntegral += error;
    
    float errorIntegral = mErrorIntegral;
    
    if (errorIntegral > mIntegralLimit) {           // 上限飽和の防止
        errorIntegral = mIntegralLimit;
    } else if (errorIntegral < -mIntegralLimit) {   // 下限飽和の防止
        errorIntegral = -mIntegralLimit;
    }    
    
    /**
     * 前回のエラーと比較する
     */
    int prevIndex = (mTotalHistory + mErrorHistoryIndex - 1) % mTotalHistory;
    float derivative = error - mErrorHistory[prevIndex];

    /**
     * HACK: ローパスフィルターによる微分値の緩和を適用する
     */
    
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
    float pidControl = mKp * error + mKi * errorIntegral + mKd * derivative;
    
    /**
     * 境界の左右どちらに沿って走行するか
     * 基本的には白色の床面に対して、黒や青のレーンが引いてあることを想定している
     * こう考えた時、mIsRightSide = trueの時は線の右縁に沿って走行すると想定する
     */
    const int sign = mIsRightSide ? 1 : -1;
    speeds[0] = speeds[0] - sign * pidControl;
    speeds[1] = speeds[1] + sign * pidControl;

    /**
     * 今回のエラーを履歴に加えて
     * 次回の添え字に更新
     */
    mErrorHistory[mErrorHistoryIndex] = error;
    mErrorHistoryIndex = (mErrorHistoryIndex + 1) % mTotalHistory;
}