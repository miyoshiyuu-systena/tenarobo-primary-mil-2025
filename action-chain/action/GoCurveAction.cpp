#include "GoCurveAction.h"
#include "spikeapi.h"
#include "CalcCurveDriveSpeed.h"
#include "device/Device.h"
#include <vector>

ActionCall goCurveActionFactory(
    float speed,
    float radius,
    bool isGoRight,
    int detectInterval,
    std::vector<IAssistGenerator> assistPtrGenerators,
    ICloserGenerator closerPtrGenerator
)
{
    return [speed, radius, isGoRight, detectInterval, assistPtrGenerators, closerPtrGenerator](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        float speeds[2] = {0.0f, 0.0f};
        float baseSpeed[2] = {0.0f, 0.0f};
        calcCurveSpeedsByLinearSpeed(speed, radius, baseSpeed);
        
        // 複数のアシストオブジェクトを生成
        std::vector<IAssist*> assists;
        for (const auto& assistPtrGenerator : assistPtrGenerators) {
            IAssist* assist = assistPtrGenerator();
            assist->init();
            assists.push_back(assist);
        }
        
        ICloser* closer = closerPtrGenerator();
        closer->init();

        do {
            // 基準速度から開始
            if (isGoRight) {
                speeds[0] = baseSpeed[0]; // 左輪（外輪）
                speeds[1] = baseSpeed[1]; // 右輪（内輪）
            } else {
                speeds[0] = baseSpeed[1]; // 左輪（内輪）
                speeds[1] = baseSpeed[0]; // 右輪（外輪）
            }
            
            // 複数のアシストを順次適用
            for (IAssist* assist : assists) {
                assist->correct(speeds);
            }
            
            device->twinWheelDrive.setSpeed(speeds[0], speeds[1]);

            /**
             * 検知間隔を待つ
             * @note
             *  dly_tskの引き数は[μs]であることに注意
             */
            dly_tsk(detectInterval * 1000);
        } while (!closer->isClosed());

        // 全てのアシストオブジェクトを削除
        for (IAssist* assist : assists) {
            delete assist;
        }
        delete closer;
    };
}