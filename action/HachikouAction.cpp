#include    "HachikouAction.h"
#include    "spikeapi.h"


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
        Device*& device
    ) {     
        // 圧力センサが押されるまで暇つぶし
        while (device->forceSensor.getForce() <= forceDetectThreshold) {
            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        }
    };
}
