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
                !go_right_lane,
                10,
                {
                    timedCloserGenerator(50)
                }
            ),
            0
        );
        next_ptr = action0;

        std::vector<ICloserGenerator> closerPtrGenerators_2 = closerPtrGenerators;
        closerPtrGenerators_2.push_back(timedCloserGenerator(50));
        ActionNode* action2 = new ActionNode(
            "sub-action2: 信地回転 - 逆",
            device,
            spinTurnActionFactory(
                120.0f,
                go_right_lane,
                10,
                closerPtrGenerators_2
            ),
            0
        );
        action0->setNext(action2);

        ActionNode* action4 = new ActionNode(
            "sub-action4: 車線変更完了判定",
            device,
            laneChangeCompleteJudgeFactory(
                go_right_lane,
                closerPtrGenerators
            ),
            0
        );
        action2->setNext(action4);

        /**
         * 次のアクションは車線変更の後に実行する
         */
        action4->setNext(next_ptr_wait);
    };
}