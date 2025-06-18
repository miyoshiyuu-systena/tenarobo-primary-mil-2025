#ifndef STRAIGHT_GO_ACTION_H
#define STRAIGHT_GO_ACTION_H

#include "ActionNode.h"

/**
 * 直線走行アクションのファクトリー
 * @param speed 大体の速度[mm/s]
 * @param duration 判定周期[ms]
 * @param judge 直線走行終了の判定
 * @return 直線走行アクション
 */
std::function<void(ActionNode*&)> straight_go_action(
    int speed,
    int duration,
    std::function<bool(void)> judge
);

#endif // STRAIGHT_GO_ACTION_H