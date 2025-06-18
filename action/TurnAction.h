#ifndef TURN_ACTION_H
#define TURN_ACTION_H

#include "ActionNode.h"

/**
 * 回転アクションのファクトリー
 * @param angular 回転角度[°]
 * @param duration 回転時間[ms]
 * @param is_right 右回転か左回転か
 * @return 回転アクション
 */
std::function<void(ActionNode*&)> turn_action(int angular, int duration, bool is_right);

#endif // TURN_ACTION_H