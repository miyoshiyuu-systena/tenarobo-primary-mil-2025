#include "ObstacleCloser.h"

ICloserGenerator obstacleCloserGenerator(int detectDistance) {
    return [detectDistance](Device*& device) {
        return new ObstacleCloser(device, detectDistance);
    };
}

ObstacleCloser::ObstacleCloser(Device*& device, int detectDistance)
    : ICloser()
    , mDevice(device)
    , mDetectDistance(detectDistance)
{
}

ObstacleCloser::~ObstacleCloser()
{
}

void ObstacleCloser::init()
{
}

bool ObstacleCloser::isClosed()
{
    int distance = mDevice->ultrasonicSensor.getDistance();
    return (0 < distance) && (distance < mDetectDistance);//超音波センサは何も検知しなかったときに負数を返すため
}