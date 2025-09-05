#include    "CurveCloser.h"
#include    "config.h"
#include    "ImageAnalysisServer.h"
#include    "spikeapi.h"
#include    <cmath>

ICloserGenerator curveCloserGenerator() {
    return [](Device*& device) {
        return new CurveCloser(device);
    };
}

CurveCloser::CurveCloser(
    Device*& device
)
    : ICloser()
    , mDevice(device)
    , mIsCurveHistory(new int[mTotalHistory]())
    , mIsCurveCount(0)
    , mIsCurveHistoryIndex(0)
{
}

CurveCloser::~CurveCloser()
{
}

void CurveCloser::init()
{
    ImageAnalysisServer::getInstance().request(FRONT_STRAIGHT);
    dly_tsk(50 * 1000);// サーバーが切り替わるまで1サイクル待ち
}

bool CurveCloser::isClosed()
{
    /**
     * ここで画像分析サーバーから分析結果を受け取る
     */
    bool isFrontStraight = ImageAnalysisServer::getInstance().responseFrontStraight();

    mIsCurveCount -= mIsCurveHistory[mIsCurveHistoryIndex];
    mIsCurveCount += (int)isFrontStraight;
    mIsCurveHistory[mIsCurveHistoryIndex] = (int)isFrontStraight;
    mIsCurveHistoryIndex = (mIsCurveHistoryIndex + 1) % mTotalHistory;

    return (float)mIsCurveCount > (float)mIsCurveCount * 0.75;
}