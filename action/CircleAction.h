#ifndef CIRCLE_ACTION_H
#define CIRCLE_ACTION_H

#include "ActionNode.h"

/**
 * 円弧を描きながら移動するアクションのファクトリー
 * @param radius 円の半径[mm]
 * @param angular 円を描く角度[°]
 * @param duration 円を描く時間[ms]
 * @param is_right 右回転か左回転か
 * @return 円を描くアクション
 */
std::function<void(ActionNode*&)> circle_action(int radius, int angular, int duration, bool is_right);

#endif // CIRCLE_ACTION_H