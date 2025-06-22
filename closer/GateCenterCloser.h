#ifndef GATE_CENTER_CLOSER_H
#define GATE_CENTER_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * ゲートの中心に差し掛かったら終了判定を出すクラスのファクトリー関数
 * @return ゲートの中心に差し掛かったら終了判定を出すクラス
 */
ICloserGenerator gateCenterCloserGenerator();

/**
 * ゲートの中心に差し掛かったら終了判定を出すクラス
 */
class GateCenterCloser : public ICloser {
    public:
        GateCenterCloser();
        ~GateCenterCloser();

        void init() override;
        bool isClosed() override;
};

#endif