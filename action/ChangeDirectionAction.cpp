#include "ChangeDirectionAction.h"
#include "spikeapi.h"

ActionCall changeDirectionActionFactory(
    bool is_clockwise,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
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
            if (is_clockwise) {
                device->twinWheelDrive.leftSpinTurn(90);
            } else {
                device->twinWheelDrive.rightSpinTurn(90);
            }
            dly_tsk(500 * 1000);
            
            device->twinWheelDrive.stop();
            dly_tsk(500 * 1000);

            for (ICloser* closer : closers) {
                if (closer->isClosed())
                    isClosed = true;
            }
        } while (!isClosed);

        // 全ての終了判定オブジェクトを削除
        for (ICloser* closer : closers) {
            delete closer;
        }
    };
}