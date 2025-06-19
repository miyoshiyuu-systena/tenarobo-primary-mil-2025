#include "GoCurveAction.h"
#include "spikeapi.h"
#include "CalcCurveDriveSpeed.h"
#include "device/Device.h"

ActionCall goCurveActionFactory(
    float speed,
    float radius,
    bool isGoRight,
    int detectInterval,
    IAssistGenerator assistPtrGenerator,
    ICloserGenerator closerPtrGenerator
)
{
    return [speed, radius, isGoRight, detectInterval, assistPtrGenerator, closerPtrGenerator](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        float speeds[2] = {0.0f, 0.0f};
        IAssist* assist = assistPtrGenerator();
        ICloser* closer = closerPtrGenerator();

        float baseSpeed[2] = {0.0f, 0.0f};
        calcCurveSpeedsByLinearSpeed(speed, radius, baseSpeed);
        if (isGoRight) {
            /**
             * - calcCurveSpeedsByLinearSpeed: 返り値の[0]が外輪、[1]が内輪
             * - init: 第一引数が左輪、第二引数が右輪
             * - 右に曲がる場合は、左が外輪、右が内輪である
             */
            assist->init(baseSpeed[0], baseSpeed[1]);
        } else {
            /**
             * - calcCurveSpeedsByLinearSpeed: 返り値の[0]が外輪、[1]が内輪
             * - init: 第一引数が左輪、第二引数が右輪
             * - 左に曲がる場合は、左が内輪、右が外輪である
             */
            assist->init(baseSpeed[1], baseSpeed[0]);
        }
        closer->init();

        do {
            assist->correct(speeds);
            device->twinWheelDrive.setSpeed(speeds[0], speeds[1]);

            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        } while (!closer->isClosed());

        delete assist;
        delete closer;
    };
}