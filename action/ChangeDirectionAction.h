#ifndef CHANGE_DIRECTION_ACTION_H
#define CHANGE_DIRECTION_ACTION_H

#include "ActionNode.h"
#include "ICloserGenerator.h"
#include <vector>

/**
 * 方向転換アクションのファクトリー関数
 * 方向転換する
 */
ActionCall changeDirectionActionFactory(
    /**
     * 方向転換方向（時計回りか反時計回りか）
     */
    bool is_clockwise,

    /**
     * 終了判定ジェネレータのリスト（複数の終了判定を順次適用）
     */
    std::vector<ICloserGenerator> closerPtrGenerators
);

#endif