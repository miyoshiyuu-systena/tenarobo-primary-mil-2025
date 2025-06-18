#include "spikeapi.h"
#include "ActionNode.h"
#include "logger/Logger.h"

using   namespace   spikeapi;

ActionNode::ActionNode(
    std::string actionName,
    TwinWheelDrive* twinWheelDrive,
    FrontArmDrive* frontArmDrive,
    Perception* perc,
    ActionCall actionCall,
    int vacationTime
)
    : mActionName(actionName)
    , mTwinWheelDrive(twinWheelDrive)
    , mFrontArmDrive(frontArmDrive)
    , mPerc(perc)
    , mActionCall(actionCall)
    , mVacationTime(vacationTime)
{}

ActionNode::~ActionNode()
{
    Logger::getInstance().logInfo("ActionNode: " + mActionName + " メモリ解放");
}

void ActionNode::execute()
{
    Logger::getInstance().logInfo("ActionNode: " + mActionName + " 実行");
    ActionNode* curr_ptr = this;
    mActionCall(
        curr_ptr,           // 現在のノード(必ず自分自身のポインタとなる)
        mNextAction,        // 次のノード(自分自身に登録されている次のノードのポインタを渡す)
        mTwinWheelDrive,    // ツインホイールドライブ
        mFrontArmDrive,     // フロントアームドライブ
        mPerc               // 知覚データ
    );
    dly_tsk(mVacationTime * 1000);
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
    /**
     * 再帰的に次のノードを削除する
     * これを実行するまでにdelete命令してしまうと、「次の次以降のノード」は他のノードとの関連性を失ったまま、ヒープ領域に残留してしまう
     * そのため、再帰的にdelete命令を実行することで、「次の次以降のノード」も含めてメモリを解放する
     */
    mNextAction->deleteNext();

    /**
     * 次のノードを削除する
     * @note
     *   C++でこの命令が実行できるインスタンスは、ヒープ領域に確保されたインスタンスのみである
     *   ヒープ領域でインスタンス化するには、new命令を使用する必要がある
     */
    delete mNextAction;
    mNextAction = nullptr;
}