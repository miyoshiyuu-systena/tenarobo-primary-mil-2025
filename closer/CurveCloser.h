#ifndef _CURVE_CLOSER_H_
#define _CURVE_CLOSER_H_

#include    "ICloser.h"
#include    "ICloserGenerator.h"

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
        bool isClosed() override;

    private:
        /**
         * 正面の直線を連続して見失った回数
         * @note
         *  ○回連続して正面に直線がなかったら曲がり角に差し掛かったとみなす
         *  1回だけだと誤検出しやすいから短い時間にある程度連続して、検出されないことを基準とする
         */
        int mSeqCountIsCurve;
        
        /**
         * 設定ファイルから連続してカーブと判定する最大回数を取得
         */
        int getSeqCountIsCurveMax();
};

#endif  // _CURVE_CLOSER_H_