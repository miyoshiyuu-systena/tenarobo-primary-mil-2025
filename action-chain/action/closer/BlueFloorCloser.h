#ifndef _BLUE_FLOOR_CLOSER_H_
#define _BLUE_FLOOR_CLOSER_H_

#include    "ICloser.h"

/**
 * 青色の床に近づいたら終了判定を出すクラス
 */
class BlueFloorCloser : public ICloser
{
    public:
        BlueFloorCloser(Perception* perc);
        ~BlueFloorCloser();

        void init() override;
        bool isClosed() override;

    private:
        Perception* mPerc;
};

#endif // _BLUE_FLOOR_CLOSER_H_