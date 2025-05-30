#ifndef _GATE_FRONT_CLOSER_H_
#define _GATE_FRONT_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"

/**
 * 正面にゲートがあることを終了判定を出すクラスのファクトリー関数
 * @return 正面にゲートがあることを終了判定を出すクラス
 */
ICloserGenerator gateFrontCloserGenerator();

/**
 * 正面にゲートがあることを終了判定を出すクラス
 */
class GateFrontCloser : public ICloser
{
    public:
        GateFrontCloser(Device*& device);
        ~GateFrontCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 正面のゲートを連続して検出した回数
         * @note
         *  ○回連続して正面にゲートがあったらゲート検出とみなす
         */
        int mSeqCountIsGate;

        /**
         * 設定ファイルから連続してゲートと判定する最大回数を取得
         */
        int getSeqCountIsGateMax();
};

#endif