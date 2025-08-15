#include "GateFrontCloser.h"
#include "ImageAnalysisServer.h"
#include "config.h"

ICloserGenerator gateFrontCloserGenerator() {
    return [](Device*& device) {
        return new GateFrontCloser(device);
    };
}

GateFrontCloser::GateFrontCloser(Device*& device)
: ICloser()
, mDevice(device)
{
    mSeqCountIsGate = 0;
}

GateFrontCloser::~GateFrontCloser()
{
}

void GateFrontCloser::init()
{
    ImageAnalysisServer::getInstance().request(AnalysisCommand::GATE_IN_FRONT);
}

int GateFrontCloser::getSeqCountIsGateMax()
{
    return config.getIntValue("seqCountIsGateMax", 10);
}

bool GateFrontCloser::isClosed()
{
    /**
     * ここで画像分析サーバーから分析結果を受け取る
     */
    bool isGateInFront = ImageAnalysisServer::getInstance().responseGateInFront();

    if (isGateInFront) {
        mSeqCountIsGate++;
    } else {
        mSeqCountIsGate = 0;
    }

    return mSeqCountIsGate > getSeqCountIsGateMax();
}
