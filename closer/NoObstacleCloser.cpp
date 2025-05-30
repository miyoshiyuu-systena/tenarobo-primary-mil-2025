#include "NoObstacleCloser.h"

ICloserGenerator noObstacleCloserGenerator(int detectDistance) {
    return [detectDistance](Device*& device) {
        return new NoObstacleCloser(device, detectDistance);
    };
}

NoObstacleCloser::NoObstacleCloser(Device*& device, int detectDistance)
    : ICloser()
    , mDevice(device)
    , mDetectDistance(detectDistance)
{
}

NoObstacleCloser::~NoObstacleCloser()
{
}

void NoObstacleCloser::init()
{
}

bool NoObstacleCloser::isClosed()
{
    int distance = mDevice->ultrasonicSensor.getDistance();
    return (distance > mDetectDistance) || (distance == -1);//超音波センサは何も検知しなかったときに-1を返すため
}