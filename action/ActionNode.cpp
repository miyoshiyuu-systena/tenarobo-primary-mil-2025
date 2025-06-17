#include "spikeapi.h"
#include "ActionNode.h"
#include "Logger.h"

using   namespace   spikeapi;

ActionNode::ActionNode(
    TwinWheelDrive* twinWheelDrive,
    Motor* frontArm,
    PerceptionDataAccess& percDataAccess,
    std::function<void(ActionNode*&)> actionCall,
    std::string actionName,
    int vacationTime = 500
)
    : mTwinWheelDrive(twinWheelDrive)
    , mFrontArm(frontArm)
    , mPercDataAccess(&percDataAccess)
    , mActionCall(actionCall)
    , mActionName(actionName)
    , mNextAction(nullptr)
    , mIsEnd(false)
    , mVacationTime(vacationTime)
{}

ActionNode::~ActionNode()
{
    Logger::getInstance().logInfo("ActionNode " + mActionName + " çµ‚äº?");
}

void ActionNode::execute()
{
    mActionCall(mNextAction);
    mIsEnd = true;
    dly_tsk(mVacationTime * 1000);
}

bool ActionNode::isEnd()
{
    return mIsEnd;
}

void ActionNode::setNext(ActionNode* nextAction)
{
    mNextAction = nextAction;
}

ActionNode* ActionNode::getNext()
{
    return mNextAction;
}

void ActionNode::deleteNext()
{
    if (mNextAction == nullptr)
    {
        return;
    }
    mNextAction->deleteNext();
    delete mNextAction;
    mNextAction = nullptr;
}