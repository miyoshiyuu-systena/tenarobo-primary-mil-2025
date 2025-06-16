#include "ActionChain.h"
#include "Logger.h"

using   namespace   spikeapi;

ActionChain::ActionChain(
    TwinWheelDrive* twinWheelDrive,
    Motor* frontArm,
    PerceptionDataAccess& percDataAccess,
    void (*actionCall)(ActionChain*& next_ptr),
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
    Logger::getInstance().logInfo("ActionChain " + mActionName + " 終了");
}

void ActionChain::execute()
{
    mActionCall(mNextAction);
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

ActionChain* ActionChain::getNext()
{
    return mNextAction;
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