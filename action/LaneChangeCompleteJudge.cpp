#include "LaneChangeCompleteJudge.h"
#include "LaneChangeAction.h"
#include "spikeapi.h"
#include "PerceptionReporter.h"
#include "PerceptionMask.h"

ActionCall laneChangeCompleteJudgeFactory(
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
        PerceptionReporter::getInstance().update(10, PERCEPTION_REPORT_MASK_COLOR);

        bool is_closed = false;
        for (auto closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator();
            if (closer->isClosed()) {
                is_closed = true;
                break;
            }
        }

        if (is_closed) {
            /** 何もしない */
            return;
        }

        /**
         * 車線変更をもう一度行う
         * 元々の次のアクションは一旦保留とする
         */
        ActionNode* next_ptr_wait = next_ptr;

        ActionNode* action = new ActionNode(
            "sub-action: 車線変更 - もう一度",
            device,
            laneChangeActionFactory(
                go_right_lane,
                closerPtrGenerators
            ),
            0
        );
        next_ptr = action;
        action->setNext(next_ptr_wait);
    };
}