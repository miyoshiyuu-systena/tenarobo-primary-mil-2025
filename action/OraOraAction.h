#ifndef ORAORA_ACTION_H
#define ORAORA_ACTION_H

#include "ActionNode.h"
#include "ICloserGenerator.h"
#include <vector>

/**
 * おらおらアクションのファクトリー関数
 * @return おらおらアクション
 * @note
 *  どけどけ！！！ロボットのお通りだ！！！
 *  前方にある軽い障害物を跳ね除けながら前進する
 */
ActionCall oraOraActionFactory(
    /**
     * 最初に首を振る方向
     */
    bool is_clockwise,

    /**
     * 終了判定ジェネレータのリスト（複数の終了判定を順次適用）
     */
    std::vector<ICloserGenerator> closerPtrGenerators
);


#endif