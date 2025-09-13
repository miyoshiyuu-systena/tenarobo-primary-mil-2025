#include "TargetInImageCloser.h"
#include "Logger.h"
#include "spikeapi.h"

ICloserGenerator targetInImageCloserGenerator(enum AnalysisCommand command) {
    return [command](Device*& device) {
        return new TargetInImageCloser(device, command);
    };
}

TargetInImageCloser::TargetInImageCloser(Device*& device, enum AnalysisCommand command)
: ICloser()
, mDevice(device)
, mCommand(command)
{}

TargetInImageCloser::~TargetInImageCloser()
{
    ImageAnalysisServer::getInstance().request(DO_NOTHING);
}

void TargetInImageCloser::init()
{
    switch (mCommand) {
        case BLUE_BOTTLE_IN_FRONT:
            ImageAnalysisServer::getInstance().request(mCommand);
            mIsInImageCallback = ImageAnalysisServer::responseBlueBottleInFrontStatic;
            break;
        case TARGET_CIRCLE_IN_DISPLAY:
            ImageAnalysisServer::getInstance().request(mCommand);
            mIsInImageCallback = ImageAnalysisServer::responseTargetCircleInDisplayStatic;
            break;
        default:
            return;
    }
    dly_tsk(100 * 1000);
}

bool TargetInImageCloser::isClosed()
{
    if (mIsInImageCallback == nullptr) {
        Logger::getInstance().logWarning("正しく画像分析タスクが実行されていません。");
        return true;
    }

    return mIsInImageCallback() ;
}