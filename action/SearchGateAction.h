#ifndef SEARCH_GATE_ACTION_H
#define SEARCH_GATE_ACTION_H

#include "ActionNode.h"

/**
 * ゲートを検索するアクションのファクトリー関数
 * ゲートを正面に見つけるまでその場で回転する
 * @return ゲートを検索するアクションのファクトリー関数
 */
ActionCall searchGateActionFactory();

#endif