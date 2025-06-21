#ifndef LANE_CHANGE_ACTION_H
#define LANE_CHANGE_ACTION_H

#include "ActionNode.h"
#include "ICloserGenerator.h"
#include <vector>

/**
 * 車線変更アクションのファクトリー関数
 * 線の右側を走るか、左側を走るかをスイッチできる
 * 俗にいうエッジ切り替え
 * @return 車線変更アクション
 */
ActionCall laneChangeActionFactory(
    /**
     * 車線変更方向
     * 右車線を走るか、左車線を走るか
     */
    bool is_right_lane,
    
    /**
     * 終了判定ジェネレータのリスト（複数の終了判定を順次適用）
     * 何を基準にして動作を止めるか
     */
    std::vector<ICloserGenerator> closerPtrGenerators
);

#endif