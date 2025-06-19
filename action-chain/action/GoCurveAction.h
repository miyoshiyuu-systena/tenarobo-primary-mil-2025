#ifndef _GO_CURVE_ACTION_H_
#define _GO_CURVE_ACTION_H_

#include "ActionNode.h"
#include "IAssist.h"
#include "ICloser.h"
#include "IAssistGenerator.h"
#include "ICloserGenerator.h"
#include <functional>

/**
 * HACK:
 *  アシスト失敗した時に、アクションを中断して、次のアクションをリカバリーアクションにする
 *  引数としてIExceptionalを実装する？？
 *  そこではアシスト失敗の判定と次以降のノードのリフレッシュと次のノードのインスタンス化を行う
 */

/**
 * 曲線走行アクションのファクトリー関数
 * @param speed 速度[mm/s]
 * @param radius 半径[mm]
 * @param isGoRight 右に曲がるかどうか
 * @param detectInterval 検知間隔[ms]
 * @param assistPtrGenerator 走行補助
 * @param closerPtrGenerator 終了判定
 * @return 曲線走行アクション
 */
ActionCall goCurveActionFactory(
    float speed,
    float radius,
    bool isGoRight,
    int detectInterval,
    IAssistGenerator assistPtrGenerator,
    ICloserGenerator closerPtrGenerator
);

#endif // _GO_CURVE_ACTION_H_