#include "StraightCloser.h"
#include "config.h"
#include <cmath>
#include "ImageAnalysisServer.h"

ICloserGenerator straightCloserGenerator() {
    return [](Device*& device) {
        return new StraightCloser(device);
    };
}

StraightCloser::StraightCloser(Device*& device)
: ICloser()
, mDevice(device)
{
    mSeqCountIsStraight = 0;
}

StraightCloser::~StraightCloser()
{
}

void StraightCloser::init()
{
}

int StraightCloser::getSeqCountIsStraightMax()
{
    return config.getIntValue("seqCountIsStraightMax", 10);
}

bool StraightCloser::isClosed()
{
    ImageAnalysisServer::getInstance().request(FRONT_STRAIGHT);
    bool isFrontStraight = ImageAnalysisServer::getInstance().responseFrontStraight();

    if (isFrontStraight) {
        mSeqCountIsStraight++;
    } else {
        mSeqCountIsStraight = 0;
    }

    return mSeqCountIsStraight > getSeqCountIsStraightMax();
}