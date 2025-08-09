#ifndef SPIN_TURN_ACTION_H
#define SPIN_TURN_ACTION_H

#include "ActionNode.h"
#include "ICloserGenerator.h"
#include <vector>

ActionCall spinTurnActionFactory(
    /**
     * 回転速度[°/s]
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