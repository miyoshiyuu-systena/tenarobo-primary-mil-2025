#ifndef _LINE_TRACE_ACTION_H_
#define _LINE_TRACE_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * ライントレースアクションのファクトリー関数
 * @param speed 大体の速度[mm/s]
 * @param duration 判定周期[ms]
 * @param threshold 閾値
 * @param Kp 比例係数
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
    std::function<bool()> judge
);

/**
 * 青線上にいるかどうかを判定する
 * @return 青線上にいるかどうか
 */
bool is_on_blue_line(void);

#endif // _LINE_TRACE_ACTION_H_