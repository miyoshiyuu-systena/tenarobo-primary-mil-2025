#include    "CurveCloser.h"
#include    "config.h"
#include    "ImageAnalysisServer.h"
#include    <cmath>

ICloserGenerator curveCloserGenerator() {
    return [](Device*& device) {
        return new CurveCloser(device);
    };
}

CurveCloser::CurveCloser(Device*& device)
: ICloser()
, mDevice(device)
{
    mSeqCountIsCurve = 0;
}

CurveCloser::~CurveCloser()
{
}

void CurveCloser::init()
{
    ImageAnalysisServer::getInstance()->request(FRONT_STRAIGHT);
}

int CurveCloser::getSeqCountIsCurveMax()
{
    return config.getIntValue("seqCountIsCurveMax", 10);
}

bool CurveCloser::isClosed()
{
    /**
     * ここで画像分析サーバーから分析結果を受け取る
     */
    bool isFrontStraight = ImageAnalysisServer::getInstance()->responseFrontStraight();

    if (!isFrontStraight) {
        mSeqCountIsCurve++;
    } else {
        mSeqCountIsCurve = 0;
    }

    return mSeqCountIsCurve > getSeqCountIsCurveMax();
}