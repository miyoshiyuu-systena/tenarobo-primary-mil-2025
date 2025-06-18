#ifndef _I_CLOSER_H_
#define _I_CLOSER_H_

#include    "Perception.h"

/**
 * 終了判定インターフェース
 * 終了判定を行うクラスはこのインターフェースを継承して実装する
 */
class ICloser
{
    public:
        virtual ICloser(Perception* perc) = default;
        virtual ~ICloser() = default;
        virtual void init() = 0;

        /**
         * 終了判定を行う
         * @return 終了判定（終了したらtrue, 終了しない場合はfalse）
         */
        virtual bool isClosed() = 0;
};

#endif // _I_CLOSER_H_