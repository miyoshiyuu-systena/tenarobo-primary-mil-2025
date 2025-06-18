#ifndef _HACHIKOU_ACTION_H_
#define _HACHIKOU_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 *  という感じのアクションを生成するファクトリー関数
 *
 * @param force_detect_threshold 圧力センサの検知閾値(N)
 * @return 忠犬ハチ公アクション
 */
std::function<void(ActionNode*&)> hachikouActionFactory(float force_detect_threshold);

#endif // _HACHIKOU_ACTION_H_