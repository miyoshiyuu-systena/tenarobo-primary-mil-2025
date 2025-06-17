#include "spikeapi.h"
#include "LineTraceAction.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"
#include <functional>

using   namespace   spikeapi;

/**
 * ライントレースアクションを生成するファクトリー関数
 * @note
 *      この関数で作成したアクションでは、ロボットが黒いラインの右縁に沿って移動する
 */
std::function<void(ActionNode*&)> line_trace_action(
    float speed, 
    int duration, 
    int threshold,
    float Kp, 
    std::function<bool()> judge
)
{
    return [speed, duration, threshold, Kp, judge](ActionNode*& next_ptr) {
        do {
            /**
             * P制御による駆動指示
             * errorは、実測した輝度が閾値よりもどのくらい明るかったかを表す。
             * (負数のときは閾値よりも暗かったことを表している。)
             * 
             * errorが正数のときは目標よりも白いところを通っているため、右輪を加速して左方向に移動する
             * errorが負数のときは目標よりも黒いところを通っているため、左輪を加速して右方向に移動する
             * この細かい調整により、黒い線の右縁を左右中心に移動するようにロボットの動きを最適化している
             * 
             * @note 
             *      P制御とは
             *      制御対象の目標値と現在の値の偏差（誤差）に比例した操作量を出す制御方式
             *      今回の場合、比例係数Kpを使用している
             */
            const float error = perceptionDataAccess.color[2] - threshold;
            twinWheelDrive.setSpeed(
                speed - Kp * error,     //  左輪
                speed + Kp * error      //  右輪
            );

            // 判定周期
            dly_tsk(duration * 1000);
        } while (!judge());

        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}

/**
 * 青線上にいるかどうかを判定する
 * @return 青線上にいるかどうか
 */
bool is_on_blue_line(void)
{
    const int H_UPPER_THRESHOLD = 220;
    const int H_LOWER_THRESHOLD = 180;
    const int S_UPPER_THRESHOLD = 200;
    const int S_LOWER_THRESHOLD = 50;
    const int V_UPPER_THRESHOLD = 100;
    const int V_LOWER_THRESHOLD = 50;

    return (
        (H_LOWER_THRESHOLD <= perceptionDataAccess.color[0] && perceptionDataAccess.color[0] <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= perceptionDataAccess.color[1] && perceptionDataAccess.color[1] <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= perceptionDataAccess.color[2] && perceptionDataAccess.color[2] <= V_UPPER_THRESHOLD)
    );
}