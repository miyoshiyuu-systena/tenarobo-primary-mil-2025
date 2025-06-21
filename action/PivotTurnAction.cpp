#include "PivotTurnAction.h"
#include "spikeapi.h"

ActionCall pivotTurnActionFactory(
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
        PerceptionReport report;
        uint8_t mask = 0b00000000;

        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator();
            closer->init();
            mask |= closer->mask;
            closers.push_back(closer);
        }

        do {
            if (is_clockwise) {
                device->twinWheelDrive.leftPivotTurn(angular_speed);
            } else {
                device->twinWheelDrive.rightPivotTurn(angular_speed);
            }

            writePerceptionReport(
                device,
                report,
                detectInterval,
                mask
            );

            dly_tsk(detectInterval * 1000);
            
            for (ICloser* closer : closers) {
                if (closer->isClosed(&report))
                    isClosed = true;
            }
        } while (!isClosed);

        // 全ての終了判定オブジェクトを削除
        for (ICloser* closer : closers) {
            delete closer;
        }
    };
}