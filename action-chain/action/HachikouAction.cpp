#include    "HachikouAction.h"
#include    "spikeapi.h"
#include    "ActionNode.h"

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 */
ActionCall hachikouActionFactory(
    float forceDetectThreshold,
    int detectInterval
)
{
    return [forceDetectThreshold, detectInterval](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        TwinWheelDrive*& twinWheelDrive,
        FrontArmDrive*& frontArmDrive,
        Perception*& perc
    ) {     
        // 圧力センサが押されるまで暇つぶし
        while (perc->getForce() <= forceDetectThreshold) {
            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        }
    };
}
