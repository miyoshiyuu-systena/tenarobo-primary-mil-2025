#ifndef _STRAIGHT_STRICT_CLOSER_H_
#define _STRAIGHT_STRICT_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * 直線を検知したら終了判定を出すクラスのファクトリー関数
 * 正面に直線が続いているところで正確に終了するために、ガウシアンフィルターの縦を強化して、縦線の基準を厳しくしている。
 * @return 直線を検知したら終了判定を出すクラス
 */
ICloserGenerator straightStrictCloserGenerator();


/**
 * 正面に直線を検知したら終了条件を出すクラス
 */
class StraightStrictCloser : public ICloser
{
    public:
        StraightStrictCloser();
        ~StraightStrictCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

        /**
         * 知覚データのうち画像が必要
         */
        static const uint8_t mask = PERCEPTION_REPORT_MASK_IMAGE;

    private:
        /**
         * 正面の直線が連続して観測された回数
         */
        int mSeqCountIsStraight;
        
        /**
         * 設定ファイルから連続して直線と判定する最大回数を取得
         */
        int getSeqCountIsStraightMax();
};

#endif