#include "LaneChangeAction.h"
#include "SpinTurnAction.h"
#include "StopAction.h"
#include "TimedCloser.h"
#include "LaneChangeCompleteJudge.h"
#include "spikeapi.h"

ActionCall laneChangeActionFactory(
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
        /**
         * 次のアクションは一旦保留とする
         */
        ActionNode* next_ptr_wait = next_ptr;

        /**
         * 信地回転: 一方の車輪を固定して、もう一方の車輪を回転させる
         * これによって車体の方向を変えることができる
         * これを左右逆に同程度の角度繰り返すと、車体の方向はそのままに正中線だけずらすことができる
         */
        ActionNode* action0 = new ActionNode(
            "sub-action0: 信地回転",
            device,
            spinTurnActionFactory(
                120.0f,
                go_right_lane,
                10,
                {
                    timedCloserGenerator(50)
                }
            ),
            0
        );
        next_ptr = action0;

        ActionNode* action1 = new ActionNode(
            "sub-action1: 信地回転 - 逆",
            device,
            spinTurnActionFactory(
                120.0f,
                !go_right_lane,
                10,
                {
                    timedCloserGenerator(50)
                }
            ),
            0
        );
        action0->setNext(action1);

        ActionNode* action2 = new ActionNode(
            "sub-action2: 止まる",
            device,
            stopActionFactory(),
            0
        );
        action1->setNext(action2);

        ActionNode* action3 = new ActionNode(
            "sub-action3: 車線変更完了判定",
            device,
            laneChangeCompleteJudgeFactory(
                go_right_lane
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