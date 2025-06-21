#include "LaneChangeAction.h"
#include "SpinTurnAction.h"
#include "StopAction.h"
#include "TimedCloser.h"
#include "LaneChangeCompleteJudge.h"
#include "spikeapi.h"

ActionCall laneChangeActionFactory(
    bool go_right_lane,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
    return [
        go_right_lane,
        closerPtrGenerators
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        /**
         * 次のアクションは一旦保留とする
         */
        ActionNode* next_ptr_wait = next_ptr;

        ActionNode* action0 = new ActionNode(
            "sub-action0: 信地回転",
            device,
            spinTurnActionFactory(
                100.0f,
                !go_right_lane,
                10,
                {
                    timedCloserGenerator(25)
                }
            ),
            0
        );
        next_ptr = action0;

        std::vector<ICloserGenerator> closerPtrGenerators_1 = closerPtrGenerators;
        closerPtrGenerators_1.push_back(timedCloserGenerator(25));
        ActionNode* action1 = new ActionNode(
            "sub-action1: 信地回転 - 逆",
            device,
            spinTurnActionFactory(
                100.0f,
                go_right_lane,
                10,
                closerPtrGenerators_1
            ),
            0
        );
        action0->setNext(action1);

        ActionNode* action2 = new ActionNode(
            "sub-action2: 停止",
            device,
            stopActionFactory(),
            0
        );
        action1->setNext(action2);

        ActionNode* action3 = new ActionNode(
            "sub-action3: 車線変更完了判定",
            device,
            laneChangeCompleteJudgeFactory(
                go_right_lane,
                closerPtrGenerators
            ),
            0
        );
        action2->setNext(action3);

        /**
         * 次のアクションは車線変更の後に実行する
         */
        action3->setNext(next_ptr_wait);
    };
}