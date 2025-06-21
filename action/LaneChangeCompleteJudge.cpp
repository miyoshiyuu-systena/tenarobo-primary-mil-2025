#include "LaneChangeCompleteJudge.h"
#include "LaneChangeAction.h"
#include "spikeapi.h"
#include "PerceptionReporter.h"
#include "PerceptionMask.h"
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
        // XXX 一旦マスクは画像
        // 画像を最新にするために初期化する
        PerceptionReporter::getInstance().init();
        PerceptionReporter::getInstance().update(10, PERCEPTION_REPORT_MASK_IMAGE);

        bool is_closed = false;
        if (go_right_lane) {
            is_closed = onRightEdgeCloserGenerator()()->isClosed();
        } else {
            is_closed = onLeftEdgeCloserGenerator()()->isClosed();
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