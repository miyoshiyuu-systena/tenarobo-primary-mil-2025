#include "action/LineTraceTimedAction.h"
#include "logic/CalcPidControl.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"

std::function<void(ActionNode*&)> line_trace_timed_action(
    float* speed,
    bool is_right_side,
    int duration,
    float Kp,
    float Ki,
    float Kd,
    int stop_time,
    std::function<float(int h, int s, int v)> calc_error
)
{
    return [speed, is_right_side, duration, Kp, Ki, Kd, stop_time, calc_error](ActionNode*& next_ptr) {
        float integral = 0.0f;  // 積分項の累積値
        float previous_error = 0.0f;  // 前回のエラー値（微分項計算用）
        const float integral_limit = 2.0f;  // 積分飽和防止用の制限値
        
        const int side_symbol = is_right_side ? 1 : -1;
        
        int time_count = 0;
        do {
            const float error = calc_error(perceptionDataAccess.color[0], perceptionDataAccess.color[1], perceptionDataAccess.color[2]);
            const float control_output = calc_pid_control(error, integral, previous_error, Kp, Ki, Kd, integral_limit);
            twinWheelDrive.setSpeed(
                speed[0] - side_symbol * control_output,     //  左輪
                speed[1] + side_symbol * control_output      //  右輪
            );
            time_count += duration;
            dly_tsk(duration);
        } while (time_count < stop_time);

        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}