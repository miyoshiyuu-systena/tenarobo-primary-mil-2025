#ifndef ON_LEFT_EDGE_CLOSER_H
#define ON_LEFT_EDGE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * 右端に差し掛かったら終了判定を出すクラスのファクトリー関数
 * @return 右端に差し掛かったら終了判定を出すクラス
 */
ICloserGenerator onLeftEdgeCloserGenerator();

/**
 * 線縁よりも右側に車体が出たら終了判定を出すクラス
 * 右車線（エッジ）に移るときに使用する
 */
class OnLeftEdgeCloser : public ICloser {
    public:
        OnLeftEdgeCloser(Device*& device);
        ~OnLeftEdgeCloser();

        void init() override;
        bool isClosed() override;

    private:
        Device*& mDevice;
};

#endif