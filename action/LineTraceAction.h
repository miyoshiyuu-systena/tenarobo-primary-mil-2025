#ifndef _LINE_TRACE_ACTION_H_
#define _LINE_TRACE_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * ライントレースアクションのファクトリー関数
 * @param speed 大体の速度[mm/s]
 * @param duration 判定周期[ms]
 * @param judge ライントレース終了の判定
 * @return ライントレースアクション
 */
std::function<void(ActionNode*&)> line_trace_action(
    float speed,
    int duration,
    std::function<bool()> judge
);

/**
 * 青線上にいるかどうかを判定する
 * @return 青線上にいるかどうか
 */
bool is_on_blue_line(void);

#endif // _LINE_TRACE_ACTION_H_