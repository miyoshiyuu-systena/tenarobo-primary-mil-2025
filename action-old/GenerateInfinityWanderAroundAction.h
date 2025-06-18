#ifndef _GENERATE_INFINITY_WANDER_AROUND_H_
#define _GENERATE_INFINITY_WANDER_AROUND_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 壁を見つけるまで猪突猛進 ->
 * その場でくるりん
 * を繰り返す
 */
std::function<void(ActionNode*&)> generate_infinity_wander_around_action();

#endif // _GENERATE_INFINITY_WANDER_AROUND_H_

