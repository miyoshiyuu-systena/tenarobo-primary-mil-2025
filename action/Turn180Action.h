#ifndef _TURN_180_ACTION_H_
#define _TURN_180_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 180度回転
 * @param duration 180度回転するのにかける時間（ms）
 */
std::function<void(ActionNode*&)> turn_180_action(int duration);

#endif // _TURN_180_ACTION_H_