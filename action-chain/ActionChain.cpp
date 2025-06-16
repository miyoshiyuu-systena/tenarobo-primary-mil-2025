#include "ActionChain.h"
#include "Logger.h"

using   namespace   spikeapi;

ActionChain::ActionChain(
    TwinWheelDrive* twinWheelDrive,
    Motor* frontArm,
    PerceptionDataAccess& percDataAccess,
    void (*actionCall)(void),
    std::string actionName
)
    : mTwinWheelDrive(twinWheelDrive)
    , mFrontArm(frontArm)
    , mPercDataAccess(&percDataAccess)
    , mActionCall(actionCall)
    , mActionName(actionName)
    , mNextAction(nullptr)
    , mIsEnd(false)
{}

ActionChain::~ActionChain()
{
    Logger::getInstance().logInfo("ActionChain %s 終了", mActionName.c_str());
}

void ActionChain::execute()
{
    mActionCall();
    mIsEnd = true;
}

bool ActionChain::isEnd()
{
    return mIsEnd;
}

void ActionChain::setNext(ActionChain* nextAction)
{
    mNextAction = nextAction;
}

void ActionChain::deleteNext()
{
    if (mNextAction == nullptr)
    {
        return;
    }
    mNextAction->deleteNext();
    delete mNextAction;
    mNextAction = nullptr;
}