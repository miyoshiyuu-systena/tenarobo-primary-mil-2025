#ifndef PIVOT_TURN_ACTION_H
#define PIVOT_TURN_ACTION_H

#include "ActionNode.h"
#include "ICloserGenerator.h"
#include <vector>

/**
 * その場で回転アクションのファクトリー関数
 * @return 回転アクション
 */
ActionCall pivotTurnActionFactory(
    /**
     * その場回転速度[°/s]
     */
    float angular_speed,

    /**
     * 回転方向（時計回りか反時計回りか）
     */
    bool is_clockwise,

    /**
     * 検知間隔[ms]
     */
    int detectInterval,

    /**
     * 終了判定ジェネレータのリスト（複数の終了判定を順次適用）
     * 何を基準にして動作を止めるか
     */
    std::vector<ICloserGenerator> closerPtrGenerators
);

#endif