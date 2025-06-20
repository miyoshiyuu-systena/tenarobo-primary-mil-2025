#ifndef _I_ASSIST_H_
#define _I_ASSIST_H_

#include "PerceptionReport.h"

/**
 * 走行補助インターフェース
 * 走行補助を行うクラスはこのインターフェースを継承して実装する
 */
class IAssist
{
    public:
        IAssist();

        virtual ~IAssist() = default;

        /**
         * 走行補助を初期化する
         */
        virtual void init() = 0;
        
        /**
         * 走行補助を行う
         * @param speeds 次の走行速度[mm/s]（構造体を更新することで次の速度を渡す）
         * @param report 知覚データ
         */
        virtual void correct(float* speeds, PerceptionReport* report) = 0;
};

#endif // _I_ASSIST_H_