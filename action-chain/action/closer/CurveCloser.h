#ifndef _CURVE_CLOSER_H_
#define _CURVE_CLOSER_H_

#include    "ICloser.h"
#include    "ICloserGenerator.h"
#include    "PerceptionReport.h"

/**
 * 曲線に近づいたら終了判定を出すクラスのファクトリー関数
 * @return 曲線に近づいたら終了判定を出すクラスのファクトリー関数
 */
ICloserGenerator curveCloserGenerator();


/**
 * 曲線に近づいたら終了判定を出すクラス
 */
class CurveCloser : public ICloser
{
    public:
        CurveCloser();
        ~CurveCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

    private:
        bool mPrevIsCurve;
};

#endif