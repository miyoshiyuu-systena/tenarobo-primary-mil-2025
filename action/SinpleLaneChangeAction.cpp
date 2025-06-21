#include "SinpleLaneChangeAction.h"
#include "spikeapi.h"

ActionCall simpleLaneChangeActionFactory(
    bool go_right_lane
) {
    return [go_right_lane](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        if (go_right_lane) {
            device->twinWheelDrive.leftSpinTurn(180);
        } else {
            device->twinWheelDrive.rightSpinTurn(180);
        }
        dly_tsk(500 * 1000);

        if (go_right_lane) {
            device->twinWheelDrive.rightSpinTurn(180);
        } else {
            device->twinWheelDrive.leftSpinTurn(180);
        }
        dly_tsk(500 * 1000);
    };
}