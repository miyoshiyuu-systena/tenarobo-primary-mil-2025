#ifndef _HACHIKOU_ACTION_H_
#define _HACHIKOU_ACTION_H_

#include    "ActionNode.h"
#include    <functional>

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 *  という感じのアクションを生成するファクトリー関数
 *
 * @param forceDetectThreshold 圧力センサの検知閾値[N]
 * @param detectInterval 圧力センサの検知間隔[ms]
 * @return 忠犬ハチ公アクション
 */
std::function<ActionCall> hachikouActionFactory(
    float forceDetectThreshold,
    int detectInterval
);

#endif // _HACHIKOU_ACTION_H_