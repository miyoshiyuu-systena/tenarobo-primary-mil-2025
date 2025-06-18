#include "spikeapi.h"
#include "LineTraceAction.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"
#include "logic/CalcPidControl.h"
#include <functional>

using   namespace   spikeapi;

/**
 * ライントレースアクションを生成するファクトリー関数
 * @note
 *      この関数で作成したアクションでは、ロボットが黒いラインの右縁に沿って移動する
 *      PID制御により高精度なライントレースを実現する
 */
std::function<void(ActionNode*&)> line_trace_action(
    float* speed,
    bool is_right_side,
    int duration, 
    float Kp,
    float Ki,
    float Kd,  // 微分係数を追加
    std::function<bool()> judge,
    std::function<float(int h, int s, int v)> calc_error
)
{
    return [speed, is_right_side, duration, Kp, Ki, Kd, judge, calc_error](ActionNode*& next_ptr) {
        float integral = 0.0f;  // 積分項の累積値
        float previous_error = 0.0f;  // 前回のエラー値（微分項計算用）
        const float integral_limit = 5.0f;  // 積分飽和防止用の制限値
        
        const int side_symbol = is_right_side ? 1 : -1;
        
        do {
            /**
             * PID制御による駆動指示
             * errorは、実測した輝度が閾値よりもどのくらい明るかったかを表す。
             * (負数のときは閾値よりも暗かったことを表している。)
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
             * 
             * @note 
             *      PID制御とは
             *      P制御（比例項）：現在の誤差に比例した制御量
             *      I制御（積分項）：過去の誤差の累積に比例した制御量
             *      D制御（微分項）：誤差の変化率に比例した制御量
             *      これにより定常誤差を減らし、応答性とオーバーシュート抑制を両立した安定したライントレースが可能になる
             */
            const float error = calc_error(perceptionDataAccess.color[0], perceptionDataAccess.color[1], perceptionDataAccess.color[2]);
            
            // PID制御による制御量計算
            const float control_output = calc_pid_control(error, integral, previous_error, Kp, Ki, Kd, integral_limit);
            
            twinWheelDrive.setSpeed(
                speed[0] - side_symbol * control_output,     //  左輪
                speed[1] + side_symbol * control_output      //  右輪
            );

            // 判定周期
            dly_tsk(duration * 1000);
        } while (!judge());

        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}
