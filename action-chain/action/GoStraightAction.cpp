#include "GoStraightAction.h"
#include "IAssist.h"
#include "ICloser.h"
#include "spikeapi.h"
#include "device/Device.h"

ActionCall goStraightActionFactory(
    float speed,
    int detectInterval,
    IAssistGenerator assistPtrGenerator,
    ICloser* closerPtr
)
{
    return [speed, detectInterval, assistPtrGenerator, closerPtr](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        float speeds[2] = {0.0f, 0.0f};
        IAssist* assist = assistPtrGenerator();
        
        assist->init(speed, speed);
        closerPtr->init();
        
        do {
            assist->correct(speeds);
            device->twinWheelDrive.setSpeed(speeds[0], speeds[1]);

            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        } while (!closerPtr->isClosed());

        delete assist;
        delete closerPtr;
    };
}