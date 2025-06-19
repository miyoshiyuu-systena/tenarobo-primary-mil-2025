#ifndef _BLACK_FLOOR_CLOSER_H_
#define _BLACK_FLOOR_CLOSER_H_

#include "ICloser.h"
#include "device/Device.h"

/**
 * 黒色の床に近づいたら終了判定を出すクラス
 */
class BlackFloorCloser : public ICloser
{
    public:
        BlackFloorCloser(Device* device);
        ~BlackFloorCloser();

        void init() override;
        bool isClosed() override;
};

#endif // _BLACK_FLOOR_CLOSER_H_