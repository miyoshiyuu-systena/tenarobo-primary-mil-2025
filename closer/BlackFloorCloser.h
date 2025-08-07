#ifndef _BLACK_FLOOR_CLOSER_H_
#define _BLACK_FLOOR_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * 黒色の床に近づいたら終了判定を出すクラスのファクトリー関数
 * @return 黒色の床に近づいたら終了判定を出すクラスのファクトリー関数
 */
ICloserGenerator blackFloorCloserGenerator();

/**
 * 黒色の床に近づいたら終了判定を出すクラス
 */
class BlackFloorCloser : public ICloser
{
    public:
        BlackFloorCloser();
        ~BlackFloorCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

        /**
         * 知覚データのうちカラーセンサ値が必要
         */
        static const uint8_t mask = PERCEPTION_REPORT_MASK_COLOR;
};

#endif // _BLACK_FLOOR_CLOSER_H_