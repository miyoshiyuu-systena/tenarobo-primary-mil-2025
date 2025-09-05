#include "StraightCloser.h"
#include "config.h"
#include <cmath>
#include "ImageAnalysisServer.h"

ICloserGenerator straightCloserGenerator() {
    return [](Device*& device) {
        return new StraightCloser(device);
    };
}

StraightCloser::StraightCloser(
    Device*& device
)
    : ICloser()
    , mDevice(device)
    , mIsStraightHistory(new int[mTotalHistory]())
    , mIsStraightCount(0)
    , mIsStraightHistoryIndex(0)
{
}

StraightCloser::~StraightCloser()
{
    ImageAnalysisServer::getInstance().request(DO_NOTHING);
}

void StraightCloser::init()
{
    ImageAnalysisServer::getInstance().request(FRONT_STRAIGHT);
    dly_tsk(500 * 1000);// サーバーが切り替わるまで10サイクル待ち
}

bool StraightCloser::isClosed()
{
    /**
     * ここで画像分析サーバーから分析結果を受け取る
     */
    bool isFrontStraight = ImageAnalysisServer::getInstance().responseFrontStraight();

    mIsStraightCount -= mIsStraightHistory[mIsStraightHistoryIndex];
    mIsStraightCount += (int)(isFrontStraight);
    mIsStraightHistory[mIsStraightHistoryIndex] = (int)(isFrontStraight);
    mIsStraightHistoryIndex = (mIsStraightHistoryIndex + 1) % mTotalHistory;

    return (float)mIsStraightCount > (float)mTotalHistory * 0.75f;
}