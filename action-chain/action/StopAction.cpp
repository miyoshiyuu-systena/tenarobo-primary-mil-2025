#include    "StopAction.h"
#include    "spikeapi.h"
#include    "ActionNode.h"
#include    "device/Device.h"

/**
 * 停止アクションのファクトリー関数
 */
ActionCall stopActionFactory()
{
    return [](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        device->twinWheelDrive.stop();
    };
}