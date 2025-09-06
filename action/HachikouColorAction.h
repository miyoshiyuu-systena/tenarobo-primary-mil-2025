#ifndef _HACHIKOU_COLOR_ACTION_H_
#define _HACHIKOU_COLOR_ACTION_H_

#include "ActionNode.h"
#include <functional>

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押した直後に床の色を取得します
 */
ActionCall hachikouColorActionFactory(
    float forceDetectThreshold,
    int detectInterval
);

#endif // _HACHIKOU_COLOR_ACTION_H_