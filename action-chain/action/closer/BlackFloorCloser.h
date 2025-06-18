#ifndef _BLACK_FLOOR_CLOSER_H_
#define _BLACK_FLOOR_CLOSER_H_

#include "ICloser.h"

/**
 * 黒色の床に近づいたら終了判定を出すクラス
 */
class BlackFloorCloser : public ICloser
{
    public:
        BlackFloorCloser(Perception* perc);
        ~BlackFloorCloser();

        void init() override;
        bool isClosed() override;

    private:
        Perception* mPerc;
};

#endif // _BLACK_FLOOR_CLOSER_H_