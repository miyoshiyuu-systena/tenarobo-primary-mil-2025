#ifndef ON_RIGHT_EDGE_CLOSER_H
#define ON_RIGHT_EDGE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * 右端に差し掛かったら終了判定を出すクラスのファクトリー関数
 * @return 右端に差し掛かったら終了判定を出すクラス
 */
ICloserGenerator onRightEdgeCloserGenerator();

/**
 * 線縁よりも右側に車体が出たら終了判定を出すクラス
 * 右車線（エッジ）に移るときに使用する
 */
class OnRightEdgeCloser : public ICloser {
    public:
        OnRightEdgeCloser();
        ~OnRightEdgeCloser();

        void init() override;
        bool isClosed() override;
};

#endif