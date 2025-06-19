#include "GoStraightAction.h"
#include "IAssist.h"
#include "ICloser.h"
#include "spikeapi.h"
#include "device/Device.h"
#include "device/PerceptionReport.h"
#include <vector>

ActionCall goStraightActionFactory(
    float speed,
    int detectInterval,
    std::vector<IAssistGenerator> assistPtrGenerators,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
    return [speed, detectInterval, assistPtrGenerators, closerPtrGenerators](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool isClosed = false;
        PerceptionReport report;

        float speeds[2] = {0.0f, 0.0f};
        
        // 複数のアシストオブジェクトを生成
        std::vector<IAssist*> assists;
        for (const auto& assistPtrGenerator : assistPtrGenerators) {
            IAssist* assist = assistPtrGenerator();
            assist->init();
            assists.push_back(assist);
        }
        
        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator();
            closer->init();
            closers.push_back(closer);
        }
        
        do {
            // 基準速度から開始
            speeds[0] = speed; // 左輪
            speeds[1] = speed; // 右輪

            // 知覚データを取得
            writePerceptionReport(
                device,
                report,
                (
                    // PERCEPTION_REPORT_MASK_ULTRASONIC |      //超音波使わない
                    // PERCEPTION_REPORT_MASK_FORCE |           //力センサー使わない
                    PERCEPTION_REPORT_MASK_COLOR |
                    // PERCEPTION_REPORT_MASK_IMAGE |           //画像使わない
                    0b00000000
                )
            );
            
            // 複数のアシストを順次適用
            for (IAssist* assist : assists) {
                assist->correct(speeds, &report);
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
                if (closer->isClosed(&report))
                    isClosed = true;
            }
        } while (!isClosed);

        // 全てのアシストオブジェクトを削除
        for (IAssist* assist : assists) {
            delete assist;
        }
        for (ICloser* closer : closers) {
            delete closer;
        }
    };
}