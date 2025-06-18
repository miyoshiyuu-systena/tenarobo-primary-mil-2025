#ifndef _I_CLOSER_H_
#define _I_CLOSER_H_

#include "perception/Perception.h"

/**
 * 終了判定インターフェース
 * 終了判定を行うクラスはこのインターフェースを継承して実装する
 */
class ICloser
{
    public:
        ICloser(Perception* perc);
        virtual ~ICloser() = default;
        virtual void init() = 0;

        /**
         * 終了判定を行う
         * @return 終了判定（終了したらtrue, 終了しない場合はfalse）
         */
        virtual bool isClosed() = 0;

    protected:
        Perception* mPerc;
};

#endif // _I_CLOSER_H_