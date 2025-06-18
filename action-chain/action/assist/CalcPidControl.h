#ifndef CALC_PID_CONTROL_H
#define CALC_PID_CONTROL_H

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
);

#endif // CALC_PID_CONTROL_H