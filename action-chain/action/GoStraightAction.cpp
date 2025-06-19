#include "GoStraightAction.h"
#include "IAssist.h"
#include "ICloser.h"
#include "spikeapi.h"
#include "device/Device.h"
#include <vector>

ActionCall goStraightActionFactory(
    float speed,
    int detectInterval,
    std::vector<IAssistGenerator> assistPtrGenerators,
    ICloserGenerator closerPtrGenerator
)
{
    return [speed, detectInterval, assistPtrGenerators, closerPtrGenerator](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        float speeds[2] = {0.0f, 0.0f};
        
        // 複数のアシストオブジェクトを生成
        std::vector<IAssist*> assists;
        for (const auto& assistPtrGenerator : assistPtrGenerators) {
            assists.push_back(assistPtrGenerator());
        }
        
        ICloser* closer = closerPtrGenerator();
        
        // 全てのアシストを初期化
        for (IAssist* assist : assists) {
            assist->init(speed, speed);
        }
        closer->init();
        
        do {
            // 基準速度から開始
            speeds[0] = speed; // 左輪
            speeds[1] = speed; // 右輪
            
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

        device->twinWheelDrive.stop();

        // 全てのアシストオブジェクトを削除
        for (IAssist* assist : assists) {
            delete assist;
        }
        delete closer;
    };
}