#ifndef _RUN_UNTIL_WALL_DETECT_ACTION_H_
#define _RUN_UNTIL_WALL_DETECT_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 壁を見つけるまで猪突猛進するアクションのファクトリー関数
 * @param detect_distance_threshold 壁の接近を検知する距離
 * @param speed モーターの速度出力
 * @param detect_interval 壁の接近を検知する間隔時間（ms）
 * @return 壁を見つけるまで猪突猛進するアクション
 */
std::function<void(ActionNode*&)> run_until_wall_detect_action(
    int detect_distance_threshold,
    float speed,
    int detect_interval
);

#endif // _RUN_UNTIL_WALL_DETECT_ACTION_H_
