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
        /**
         * 短く信地回転を交互に繰り返すことで車線変更する
         * 左輪を前に出す、同じ分右輪を出す、を繰り返すと、左右並行移動する
         */
        if (go_right_lane) {
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
        } else {
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.rightSpinTurn(180);
            dly_tsk(50 * 1000);
            device->twinWheelDrive.leftSpinTurn(180);
            dly_tsk(50 * 1000);
        }

        device->twinWheelDrive.stop();
    };
}