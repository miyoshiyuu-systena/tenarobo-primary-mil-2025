#ifndef _I_ASSIST_H_
#define _I_ASSIST_H_

#include    "device/Device.h"

/**
 * 走行補助インターフェース
 * 走行補助を行うクラスはこのインターフェースを継承して実装する
 */
class IAssist
{
    public:
        IAssist(
            Device* device
        );

        virtual ~IAssist() = default;
        
        /**
         * 走行補助を行う
         * @param speeds 次の走行速度[mm/s]（構造体を更新することで次の速度を渡す）
         */
        virtual void correct(float* speeds) = 0;

    protected:
        Device* mDevice;
};

#endif // _I_ASSIST_H_