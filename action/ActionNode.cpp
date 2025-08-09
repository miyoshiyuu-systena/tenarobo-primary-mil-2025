#include "spikeapi.h"
#include "ActionNode.h"
#include "logger/Logger.h"

using   namespace   spikeapi;

ActionNode::ActionNode(
    std::string actionName,
    Device* device,
    ActionCall actionCall,
    int vacationTime
)
    : mActionName(actionName)
    , mNextAction(nullptr)
    , mDevice(device)
    , mActionCall(actionCall)
    , mVacationTime(vacationTime)
{
    Logger::getInstance().logInfo("ActionNode: " + mActionName + " 生成");
}

ActionNode::~ActionNode()
{
    Logger::getInstance().logInfo("ActionNode: " + mActionName + " メモリ解放");
}

void ActionNode::execute()
{
    Logger::getInstance().logInfo("ActionNode: " + mActionName + " 実行");
    ActionNode* curr_ptr = this;
    ActionNode*& curr_ptr_ref = curr_ptr;
    mActionCall(
        curr_ptr_ref,           // 現在のノード(必ず自分自身のポインタとなる)
        mNextAction,        // 次のノード(自分自身に登録されている次のノードのポインタを渡す)
        mDevice             // デバイス類のポインタ
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