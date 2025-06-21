#include "LaneChangeAction.h"
#include "SpinTurnAction.h"
#include "TimedCloser.h"
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
                150.0f,
                !go_right_lane,
                10,
                {
                    timedCloserGenerator(50)
                }
            ),
            0
        );
        next_ptr = action0;

        std::vector<ICloserGenerator> closerPtrGenerators_ = closerPtrGenerators;
        closerPtrGenerators_.push_back(timedCloserGenerator(50));
        ActionNode* action1 = new ActionNode(
            "sub-action1: 車線変更",
            device,
            spinTurnActionFactory(
                150.0f,
                go_right_lane,
                10,
                closerPtrGenerators_
            ),
            0
        );
        action0->setNext(action1);

        /**
         * 次のアクションは車線変更の後に実行する
         */
        action1->setNext(next_ptr_wait);
    };
}