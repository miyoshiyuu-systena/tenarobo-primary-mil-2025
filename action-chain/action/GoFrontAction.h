#ifndef _GO_FRONT_ACTION_H_
#define _GO_FRONT_ACTION_H_

#include    "ActionNode.h"
#include    "IAssist.h"
#include    "ICloser.h"
#include    <functional>

/**
 * HACK: 
 *  アシスト失敗時の挙動を新しい引数として入れて良いかも
 *  引数としてIExceptionalを実装する？？
 */

/**
 * 前方に走るアクションのファクトリー関数
 * @param speed 速度[mm/s]
 * @param detectInterval 検知間隔[ms]
 * @param assistPtr 走行補助
 * @param closerPtr 終了判定
 * @return 前方に走るアクション
 */
std::function<ActionCall> goFrontActionFactory(
    float speed,
    int detectInterval,
    IAssist* assistPtr,
    ICloser* closerPtr
);

#endif // _GO_FRONT_ACTION_H_