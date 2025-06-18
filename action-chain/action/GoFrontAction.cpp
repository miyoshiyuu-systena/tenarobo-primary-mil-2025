#include "GoFrontAction.h"
#include "IAssist.h"
#include "ICloser.h"
#include "spikeapi.h"
#include "logger/Logger.h"

ActionCall goFrontActionFactory(
    float speed,
    int detectInterval,
    IAssist* assist,
    ICloser* closerPtr
)
{
    return [speed, detectInterval, assist, closerPtr](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        TwinWheelDrive*& twinWheelDrive,
        FrontArmDrive*& frontArmDrive,
        Perception*& perc
    ) {
        Logger::getInstance().logInfo("GoFrontAction: 実行中");
        float speeds[2] = {0.0f, 0.0f};
        assist->init(speed, speed);
        closerPtr->init();
        
        do {
            Logger::getInstance().logInfo("before assist: speeds: " + std::to_string(speeds[0]) + ", " + std::to_string(speeds[1]));
            assist->correct(speeds);
            Logger::getInstance().logInfo("after assist: speeds: " + std::to_string(speeds[0]) + ", " + std::to_string(speeds[1]));
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

        delete assist;
        delete closerPtr;
    };
}