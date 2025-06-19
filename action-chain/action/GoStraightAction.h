#ifndef _GO_STRAIGHT_ACTION_H_
#define _GO_STRAIGHT_ACTION_H_

#include    "ActionNode.h"
#include    "IAssist.h"
#include    "ICloser.h"
#include    "IAssistGenerator.h"
#include    <functional>

/**
 * HACK: 
 *  アシスト失敗時の挙動を新しい引数として入れて良いかも
 *  引数としてIExceptionalを実装する？？
 */

/**
 * 直線走行アクションのファクトリー関数
 * @param speed 速度[mm/s]
 * @param detectInterval 検知間隔[ms]
 * @param assistPtrGenerator 走行補助
 * @param closerPtr 終了判定
 * @return 直線走行アクション
 */
ActionCall goStraightActionFactory(
    float speed,
    int detectInterval,
    IAssistGenerator assistPtrGenerator,
    ICloser* closerPtr
);

#endif // _GO_STRAIGHT_ACTION_H_