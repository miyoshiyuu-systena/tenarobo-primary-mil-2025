#include "RunDistanceCloser.h"
#include "spikeapi.h"
#include "TwinWheelConst.h"
#include "config.h"
#include <cmath>

#include "Logger.h"

using namespace spikeapi;

ICloserGenerator runDistanceCloserGenerator(float distance) {
    return [distance](Device*& device) {
        return new RunDistanceCloser(device, distance);
    };
}

RunDistanceCloser::RunDistanceCloser(Device*& device, float distance)
: ICloser()
, mDevice(device)
, mStartEncodeLeft(0)
, mStartEncodeRight(0)
{
    mTargetEncode = static_cast<int>(((distance * 360.0f) / (M_PI * WHEEL_DIAMETER_MM)) * getEncodeCorrect());
    Logger::getInstance().logDebug(":::" + std::to_string(getEncodeCorrect()));
}

RunDistanceCloser::~RunDistanceCloser()
{
}

void RunDistanceCloser::init()
{
    // XXX ここで勝手にリセットするのは本当はまずい
    // XXX 他の走行補助やクローザーでエンコードを使用しているときは狂いが発生するかも
    // XXX とはいえ、エンコード値が繰り上がるのも考慮できない・・・
    mDevice->twinWheelDrive.resetCount();
    mStartEncodeLeft = mDevice->twinWheelDrive.getLeftEncode();
    mStartEncodeRight = mDevice->twinWheelDrive.getRightEncode();
}

bool RunDistanceCloser::isClosed()
{
    int currentEncodeLeft = mDevice->twinWheelDrive.getLeftEncode();
    int currentEncodeRight = mDevice->twinWheelDrive.getRightEncode();
    int encodeLeft = currentEncodeLeft - mStartEncodeLeft;
    int encodeRight = currentEncodeRight - mStartEncodeRight;

    int currentDistance = (encodeLeft + encodeRight) / 2;
    return currentDistance >= mTargetEncode;
}

float RunDistanceCloser::getEncodeCorrect() const
{
    return config.getFloatValue("encodeCorrect", 1.0f);
}