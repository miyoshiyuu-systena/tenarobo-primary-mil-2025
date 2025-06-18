#include "CalcPidControl.h"

/**
 * PID制御による補正値を計算する
 * @param error 現在のエラー値
 * @param integral 積分項の累積値（参照渡しで更新）
 * @param previous_error 前回のエラー値（参照渡しで更新）
 * @param Kp 比例係数
 * @param Ki 積分係数
 * @param Kd 微分係数
 * @param integral_limit 積分飽和防止用の制限値
 * @return PID制御による補正値
 */
 float calc_pid_control(
    float error,
    float& integral,
    float& previous_error,
    float Kp,
    float Ki,
    float Kd,
    float integral_limit
)
{
    // 積分項を更新（積分飽和防止のため制限を設ける）
    integral += error;
    if (integral > integral_limit) {
        integral = integral_limit;
    } else if (integral < -integral_limit) {
        integral = -integral_limit;
    }
    
    // 微分項を計算（エラーの変化率）
    const float derivative = error - previous_error;
    previous_error = error;  // 次回計算のため現在のエラーを保存
    
    // PID制御による制御量計算
    return Kp * error + Ki * integral + Kd * derivative;
}