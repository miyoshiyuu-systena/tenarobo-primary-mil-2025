#ifndef _STRAIGHT_CLOSER_H_
#define _STRAIGHT_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"

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
        bool isClosed() override;

    private:
        /**
         * 正面の直線が連続して観測された回数
         * @note
         *  ○回連続して正面に直線がなかったら曲がり角に差し掛かったとみなす
         *  1回だけだと誤検出しやすいから短い時間にある程度連続して、検出されないことを基準とする
         */
        int mSeqCountIsStraight;
        
        /**
         * 設定ファイルから連続して直線と判定する最大回数を取得
         */
        int getSeqCountIsStraightMax();
};

#endif // _STRAIGHT_CLOSER_H_