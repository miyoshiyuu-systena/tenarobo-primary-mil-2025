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
        bool isClosed(PerceptionReport* report) override;;
};

#endif // _BLUE_FLOOR_CLOSER_H_