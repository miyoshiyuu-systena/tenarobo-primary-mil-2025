#include "GoCurveAction.h"
#include "spikeapi.h"
#include "CalcCurveDriveSpeed.h"
#include <vector>

#include "logger/Logger.h"

ActionCall goCurveActionFactory(
    float speed,
    float radius,
    bool isGoRight,
    int detectInterval,
    std::vector<IAssistGenerator> assistPtrGenerators,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
    return [speed, radius, isGoRight, detectInterval, assistPtrGenerators, closerPtrGenerators](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        int count = 0;
        bool isClosed = false;

        float speeds[2] = {0.0f, 0.0f};
        float baseSpeed[2] = {0.0f, 0.0f};
        calcCurveSpeedsByLinearSpeed(speed, radius, baseSpeed);
        
        // 複数のアシストオブジェクトを生成
        std::vector<IAssist*> assists;
        for (const auto& assistPtrGenerator : assistPtrGenerators) {
            IAssist* assist = assistPtrGenerator(device);
            assist->init();
            assists.push_back(assist);
        }
        
        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator(device);
            closer->init();
            closers.push_back(closer);
        }

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

            // 複数の終了判定を順次適用
            for (ICloser* closer : closers) {
                if (closer->isClosed())
                    isClosed = true;
            }
            count++;
        } while (!isClosed);

        // 全てのアシストオブジェクトを削除
        for (IAssist* assist : assists) {
            delete assist;
        }
        for (ICloser* closer : closers) {
            delete closer;
        }

        Logger::getInstance().logInfo("GoCurveAction: count = " + std::to_string(count));
    };
}