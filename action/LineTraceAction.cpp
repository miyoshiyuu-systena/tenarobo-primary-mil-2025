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
 *      PI制御により精度の高いライントレースを実現する
 */
std::function<void(ActionNode*&)> line_trace_action(
    float speed, 
    int duration, 
    int threshold,
    float Kp,
    float Ki,  // 積分係数を追加
    std::function<bool()> judge
)
{
    return [speed, duration, threshold, Kp, Ki, judge](ActionNode*& next_ptr) {
        float integral = 0.0f;  // 積分項の累積値
        const float integral_limit = 100.0f;  // 積分飽和防止用の制限値
        
        do {
            /**
             * PI制御による駆動指示
             * errorは、実測した輝度が閾値よりもどのくらい明るかったかを表す。
             * (負数のときは閾値よりも暗かったことを表している。)
             * 
             * P制御項：
             * errorが正数のときは目標よりも白いところを通っているため、右輪を加速して左方向に移動する
             * errorが負数のときは目標よりも黒いところを通っているため、左輪を加速して右方向に移動する
             * 
             * I制御項：
             * 過去のエラーの累積により定常誤差を減らし、より正確にラインを追従する
             * 
             * @note 
             *      PI制御とは
             *      P制御（比例項）：現在の誤差に比例した制御量
             *      I制御（積分項）：過去の誤差の累積に比例した制御量
             *      これにより定常誤差を減らし、より安定したライントレースが可能になる
             */
            const float error = perceptionDataAccess.color[2] - threshold;
            
            // 積分項を更新（積分飽和防止のため制限を設ける）
            integral += error;
            if (integral > integral_limit) {
                integral = integral_limit;
            } else if (integral < -integral_limit) {
                integral = -integral_limit;
            }
            
            // PI制御による制御量計算
            const float control_output = Kp * error + Ki * integral;
            
            twinWheelDrive.setSpeed(
                speed - control_output,     //  左輪
                speed + control_output      //  右輪
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