#ifndef NO_OBSTACLE_CLOSER_H
#define NO_OBSTACLE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"

ICloserGenerator noObstacleCloserGenerator(int detectDistance);

class NoObstacleCloser : public ICloser
{
    public:
        NoObstacleCloser(Device*& device, int detectDistance);
        ~NoObstacleCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 障害物検知距離
         */
        int mDetectDistance;
};

#endif