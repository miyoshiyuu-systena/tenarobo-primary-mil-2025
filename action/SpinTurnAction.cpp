#include "SpinTurnAction.h"
#include "spikeapi.h"

ActionCall spinTurnActionFactory(
    float angular_speed,
    bool is_clockwise,
    int detectInterval,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
    return [
        angular_speed,
        is_clockwise,
        detectInterval,
        closerPtrGenerators
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool isClosed = false;

        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator();
            closer->init();
            closers.push_back(closer);
        }

        do {
            if (is_clockwise) {
                device->twinWheelDrive.leftSpinTurn(angular_speed);
            } else {
                device->twinWheelDrive.rightSpinTurn(angular_speed);
            }

            dly_tsk(detectInterval * 1000);

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