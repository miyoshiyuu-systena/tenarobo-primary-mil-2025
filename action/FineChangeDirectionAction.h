#ifndef FINE_CHANGE_DIRECTION_ACTION_H
#define FINE_CHANGE_DIRECTION_ACTION_H

#include "ActionNode.h"
#include "ImageAnalysisServer.h"

/**
 * 方向転換アクションのファクトリー関数
 * 視野内に存在することがわかっている対象物が、視野の正面に来るようにその場で回転する
 * 視野内に存在することは、ChangeDirectionActionなどを事前に使用し、保証しておくこと
 */
ActionCall fineChangeDirectionActionFactory(
    enum AnalysisCommand command
);

#endif