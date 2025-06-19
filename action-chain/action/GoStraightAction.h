#ifndef _GO_STRAIGHT_ACTION_H_
#define _GO_STRAIGHT_ACTION_H_

#include    "ActionNode.h"
#include    "IAssist.h"
#include    "ICloser.h"
#include    "IAssistGenerator.h"
#include    "ICloserGenerator.h"
#include    <functional>
#include    <vector>

/**
 * HACK: 
 *  アシスト失敗時の挙動を新しい引数として入れて良いかも
 *  引数としてIExceptionalを実装する？？
 */

/**
 * 直線走行アクションのファクトリー関数
 * @param speed 速度[mm/s]
 * @param detectInterval 検知間隔[ms]
 * @param assistGenerators 走行補助ジェネレータのリスト（複数のアシストを順次適用）
 * @param closerPtrGenerator 終了判定
 * @return 直線走行アクション
 */
ActionCall goStraightActionFactory(
    float speed,
    int detectInterval,
    std::vector<IAssistGenerator> assistGenerators,
    ICloserGenerator closerPtrGenerator
);

#endif // _GO_STRAIGHT_ACTION_H_