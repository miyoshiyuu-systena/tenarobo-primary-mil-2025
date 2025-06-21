#include "LaneChangeCompleteJudge.h"
#include "LaneChangeAction.h"
#include "spikeapi.h"

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
        PerceptionReport report;

        /**
         * XXXX やばい
         * わんちゃんカメラの画像が取得できないサイクルがあって実行時エラーを起こしうる
         * writePerceptionReport自体を諦めるのが良いかもしれない（ちゃんとクラスにする、最新のレポートは持っており、新しく取得しない方式）
         */
        writePerceptionReport(
            device,
            report,
            10,
            0b11111111
        );

        bool is_closed = false;
        for (auto closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator();
            if (closer->isClosed(&report)) {
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