#ifndef _LANE_CHANGE_COMPLETE_JUDGE_H_
#define _LANE_CHANGE_COMPLETE_JUDGE_H_

#include "ActionNode.h"
#include "ICloserGenerator.h"

/**
 * 車線変更完了判定のファクトリー関数
 * 車線変更が完了していることを確認し、できていなければもう一度車線変更を行う
 * @return 車線変更完了判定のファクトリー関数
 */
ActionCall laneChangeCompleteJudgeFactory(
    /**
     * 車線変更の方向
     * true: 右車線への車線変更
     * false: 左車線への車線変更
     */
    bool go_right_lane,
    
    /**
     * 終了判定ジェネレータのリスト（複数の終了判定を順次適用）
     * 何を基準にして車線変更が完了したとみなすか
     */
    std::vector<ICloserGenerator> closerPtrGenerators
);

#endif // _LANE_CHANGE_COMPLETE_JUDGE_H_