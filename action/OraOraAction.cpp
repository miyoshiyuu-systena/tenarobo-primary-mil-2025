#include "OraOraAction.h"

ActionCall oraOraActionFactory(
    bool is_clockwise,
    std::vector<ICloserGenerator> closerPtrGenerators
) {
    return [
        is_clockwise,
        closerPtrGenerators
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool isClosed = false;

        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator(device);
            closer->init();
            closers.push_back(closer);
        }

        do {
            // XXX 安定性をとるならばdly_tskは500 * 1000の方が良かった
            if (is_clockwise) {
                device->twinWheelDrive.leftSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.rightSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.rightSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.leftSpinTurn(90);
                dly_tsk(250 * 1000);
            } else {
                device->twinWheelDrive.rightSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.leftSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.leftSpinTurn(90);
                dly_tsk(250 * 1000);
                device->twinWheelDrive.rightSpinTurn(90);
                dly_tsk(250 * 1000);
            }
            device->twinWheelDrive.stop();
            dly_tsk(100 * 1000);

            for (ICloser* closer : closers) {
                if (closer->isClosed())
                    isClosed = true;
            }
        } while (!isClosed);
    }
}

