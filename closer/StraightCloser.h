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
        StraightCloser(Device*& device);
        ~StraightCloser();

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
        int* mIsStraightHistory;            // 0:直線ではない
                                            // 1:直線
                                            // mTotalHistory回分のisStraightの履歴
        int mIsStraightHistoryIndex;
        int mIsStraightCount;               // 計算の効率化のため前回の値を記憶
};

#endif // _STRAIGHT_CLOSER_H_