#ifndef _I_CLOSER_H_
#define _I_CLOSER_H_

#include "PerceptionReport.h"

/**
 * 終了判定インターフェース
 * 終了判定を行うクラスはこのインターフェースを継承して実装する
 */
class ICloser
{
    public:
        ICloser();
        virtual ~ICloser() = default;
        virtual void init() = 0;

        /**
         * 終了判定を行う
         * @param report 知覚データ
         * @return 終了判定（終了したらtrue, 終了しない場合はfalse）
         */
        virtual bool isClosed(PerceptionReport* report) = 0;

        static const uint8_t mask = 0b11111111;
};

#endif // _I_CLOSER_H_