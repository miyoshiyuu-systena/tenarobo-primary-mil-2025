#ifndef _STRAIGHT_CLOSER_H_
#define _STRAIGHT_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "PerceptionReport.h"

/**
 * 直線を検知したら終了判定を出すクラスのファクトリー関数
 * @return 直線を検知したら終了判定を出すクラス
 */
ICloserGenerator straightCloserGenerator();

/**
 * 正面に直線を検知したら終了条件を出すクラス
 */
class StraightCloser : public ICloser
{
    public:
        StraightCloser();
        ~StraightCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

    private:
        /**
         * 正面の直線が連続して観測された回数
         */
        int mSeqCountIsStraight;
        static const int SEQ_COUNT_IS_STRAIGHT_MAX = 10;
}

#endif // _STRAIGHT_CLOSER_H_