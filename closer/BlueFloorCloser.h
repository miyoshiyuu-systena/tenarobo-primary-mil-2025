#ifndef _BLUE_FLOOR_CLOSER_H_
#define _BLUE_FLOOR_CLOSER_H_

#include    "ICloser.h"
#include    "ICloserGenerator.h"

/**
 * 青色の床に近づいたら終了判定を出すクラスのファクトリー関数
 * @return 青色の床に近づいたら終了判定を出すクラスのファクトリー関数
 */
ICloserGenerator blueFloorCloserGenerator();

/**
 * 青色の床に近づいたら終了判定を出すクラス
 */
class BlueFloorCloser : public ICloser
{
    public:
        BlueFloorCloser();
        ~BlueFloorCloser();

        void init() override;
        bool isClosed(PerceptionReport* report) override;

        /**
         * 知覚データのうちカラーセンサ値が必要
         */
        static const uint8_t mask = PERCEPTION_REPORT_MASK_COLOR;
};

#endif // _BLUE_FLOOR_CLOSER_H_