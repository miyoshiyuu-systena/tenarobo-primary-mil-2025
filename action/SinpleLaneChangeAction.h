#ifndef SIMPLE_LANE_CHANGE_ACTION_H
#define SIMPLE_LANE_CHANGE_ACTION_H

#include "ActionNode.h"

/**
 * 簡単な車線変更アクションのファクトリー関数
 * 信地回転を行って車線変更を行う
 * 特に車線変更成否の判定を実施せず、1回左右に信地回転をして、正中線をずらす
 * 
 * @note
 *  このアクションを作成した経緯として、
 *  元々は車線変更の成功/失敗を判定するつもりだったが、
 *  何を持って成功とするか、失敗とするかが難しいのと
 *  2度の信地回転によって必ずしも左右並行移動できるわけではなかった（通信のわずかなずれとか）ため
 *  今回は諦めて判定をしないことにした
 * 
 * @return 簡単な車線変更アクションのファクトリー関数
 */
ActionCall simpleLaneChangeActionFactory(
    /**
     * 車線変更方向
     * true: 右車線への車線変更
     * false: 左車線への車線変更
     */
    bool go_right_lane
);

#endif