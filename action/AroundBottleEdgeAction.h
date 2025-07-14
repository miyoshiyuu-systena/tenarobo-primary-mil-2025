#ifndef _AROUND_BOTTLE_EDGE_ACTION_H_
#define _AROUND_BOTTLE_EDGE_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 円弧を描きながら移動するアクションのファクトリー関数
 * @param radius 円弧の半径[mm]
 * @param angle 円弧の角度[°]
 * @param duration 円弧を描く時間[ms]
 * @param vacation_time 休憩時間[ms]
 * @return 円弧を描きながら移動するアクション
 * 
 * 名前の経緯：
 * もともとは障害物のペットボトルを円弧を描きながら避けるアクションだったが、
 * 汎用性を高めるために円弧を描きながら移動するアクションに変更した
 */
std::function<void(ActionNode*&)> around_bottle_edge_action(
    float radius,
    float angle,
    int duration,
    int vacation_time
);

#endif // _AROUND_BOTTLE_EDGE_ACTION_H_