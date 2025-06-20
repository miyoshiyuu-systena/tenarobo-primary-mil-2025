#ifndef _CURVE_CLOSER_H_
#define _CURVE_CLOSER_H_

#include    "ICloser.h"
#include    "ICloserGenerator.h"
#include    "PerceptionReport.h"

/**
 * 曲がり角に差し掛かったら終了判定を出すクラスのファクトリー関数
 * 正面に直線がなくなったことを曲がり角に差し掛かったとみなす
 * @return 曲がり角に差し掛かったら終了判定を出すクラス
 */
ICloserGenerator curveCloserGenerator();


/**
 * 曲がり角に差し掛かったら終了判定を出すクラス
 */
class CurveCloser : public ICloser
{
    public:
        CurveCloser();
        ~CurveCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

    private:
        /**
         * 正面の直線を連続して見失った回数
         */
        int mSeqCountIsCurve;
        static const int SEQ_COUNT_IS_CURVE_MAX = 10;
};

#endif  // _CURVE_CLOSER_H_