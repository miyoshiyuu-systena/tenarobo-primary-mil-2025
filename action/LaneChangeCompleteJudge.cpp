#include "LaneChangeCompleteJudge.h"
#include "LaneChangeAction.h"
#include "spikeapi.h"
#include "OnLeftEdgeCloser.h"
#include "OnRightEdgeCloser.h"

ActionCall laneChangeCompleteJudgeFactory(
    bool go_right_lane
)
{
    return [
        go_right_lane
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool is_closed = false;
        if (go_right_lane) {
            is_closed = onRightEdgeCloserGenerator()(device)->isClosed();
        } else {
            is_closed = onLeftEdgeCloserGenerator()(device)->isClosed();
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
                go_right_lane
            ),
            0
        );
        next_ptr = action;
        action->setNext(next_ptr_wait);
    };
}