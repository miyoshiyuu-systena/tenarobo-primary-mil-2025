#include "GoFrontAction.h"

std::function<ActionCall> goFrontActionFactory(
    float speed,
    int detectInterval,
    Assist* assist,
    Closer* closerPtr
)
{
    return [speed, detectInterval, assist, closerPtr](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        TwinWheelDrive*& twinWheelDrive,
        FrontArmDrive*& frontArmDrive,
        Perception*& perc
    ) {
        float speeds[2] = {0.0f, 0.0f};
        assist->init(speed);
        closerPtr->init();

        do {
            assist->do(speeds);
            twinWheelDrive->setSpeed(speeds[0], speeds[1]);

            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        } while (!closerPtr->isClosed());

        twinWheelDrive->stop();
        twinWheelDrive->resetCount();
        frontArmDrive->stop();
    }
}