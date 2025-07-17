#ifndef _LINE_TRACE_ACTION_H_
#define _LINE_TRACE_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * ライントレースアクションのファクトリー関数（PID制御）
 * @param speed 大体の速度[mm/s]
 * @param duration 判定周期[ms]
 * @param threshold 閾値
 * @param Kp 比例係数
 * @param Ki 積分係数
 * @param Kd 微分係数
 * @param judge ライントレース終了の判定
 * @return ライントレースアクション
 */
std::function<void(ActionNode*&)> line_trace_action(
    float speed,
    int duration,
    int threshold,
    /**
     * 比例定数
     * @note
     *      比例定数を大きくすると
     *      ラインから外れかけたときに鋭く補正がかかる
     *      ただし、大きく方向が変わるなどの問題もあり得る
     */
    float Kp,
    /**
     * 積分定数
     * @note
     *      積分定数を使用することで定常誤差を減らし、より精密なライントレースが可能になる
     *      値を大きくすると過去のエラーの累積効果が強くなるが、
     *      過度に大きくすると振動やオーバーシュートを引き起こす可能性がある
     */
    float Ki,
    /**
     * 微分定数
     * @note
     *      微分定数を使用することでエラーの変化率に基づく予測的制御が可能になる
     *      オーバーシュートを抑制し、より滑らかな応答を実現する
     *      ただし、値を大きくしすぎるとノイズに敏感になり不安定になる可能性がある
     */
    float Kd,
    std::function<bool()> judge
);

/**
 * 青線上にいるかどうかを判定する
 * @return 青線上にいるかどうか
 */
bool is_on_blue_line(void);

#endif // _LINE_TRACE_ACTION_H_