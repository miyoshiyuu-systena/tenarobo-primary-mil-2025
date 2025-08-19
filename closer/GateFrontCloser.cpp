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
}

GateFrontCloser::~GateFrontCloser()
{
}

void GateFrontCloser::init()
{
    ImageAnalysisServer::getInstance().request(AnalysisCommand::GATE_IN_FRONT);
}

bool GateFrontCloser::isClosed()
{
    /**
     * ここで画像分析サーバーから分析結果を受け取る
     */
    return ImageAnalysisServer::getInstance().responseGateInFront();
}
