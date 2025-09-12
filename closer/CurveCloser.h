#ifndef _CURVE_CLOSER_H_
#define _CURVE_CLOSER_H_

#include    "ICloser.h"
#include    "ICloserGenerator.h"
#include    "Device.h"

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
        CurveCloser(Device*& device);
        ~CurveCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 判定ヒストリー
         */
        static const int mTotalHistory = 6; // とりあえず過去6回分
        int* mIsCurveHistory;               // 0:曲がり角ではない
                                            // 1:曲がり角
                                            // mTotalHistory回分のisCurveの履歴
        int mIsCurveHistoryIndex;
        int mIsCurveCount;                  // 計算の効率化のため前回の値を記憶
};

#endif  // _CURVE_CLOSER_H_