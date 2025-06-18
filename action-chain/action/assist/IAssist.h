#ifndef _I_ASSIST_H_
#define _I_ASSIST_H_

#include    "TwinWheelDrive.h"
#include    "FrontArmDrive.h"
#include    "Perception.h"

/**
 * 走行補助インターフェース
 * 走行補助を行うクラスはこのインターフェースを継承して実装する
 */
class IAssist
{
    public:
        virtual IAssist(
            TwinWheelDrive* twinWheelDrive,
            FrontArmDrive* frontArmDrive,
            Perception* perception
        ) = default;
        virtual ~IAssist() = default;

        /**
         * 走行補助を初期化する
         * @param baseLeftSpeed 左モーターの基準速度[mm/s]
         * @param baseRightSpeed 右モーターの基準速度[mm/s]
         */
        virtual void init(float baseLeftSpeed, float baseRightSpeed) = 0;

        /**
         * 走行補助を行う
         * @param speeds 次の走行速度[mm/s]（構造体を更新することで次の速度を渡す）
         */
        virtual void do(float* speeds) = 0;
};

#endif // _I_ASSIST_H_