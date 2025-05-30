#include "ChangeDirectionAction.h"
#include "spikeapi.h"

ActionCall changeDirectionActionFactory(
    bool is_clockwise,
    std::vector<ICloserGenerator> closerPtrGenerators
)
{
    return [
        is_clockwise,
        closerPtrGenerators
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        bool isClosed = false;

        std::vector<ICloser*> closers;
        for (const auto& closerPtrGenerator : closerPtrGenerators) {
            ICloser* closer = closerPtrGenerator(device);
            closer->init();
            closers.push_back(closer);
        }

        do {
            /**
             * 回転速度45°/sで200ms回転する
             * これにより
             * 45°/s * 0.2s = 9°
             * 9°刻みでゲートを分析する
             */
            if (is_clockwise) {
                device->twinWheelDrive.leftPivotTurn(45);
            } else {
                device->twinWheelDrive.rightPivotTurn(45);
            }
            dly_tsk(150 * 1000);

            /**
             * 一時停止し、カメラの手ぶれなしで画像を取得する機会を与える
             */
            device->twinWheelDrive.stop();
            dly_tsk(150 * 1000);

            /**
             * XXX
             * 1サイクル400msなので、十分画像処理可能
             * このスピードで撮像して、手ぶれがないかどうかは要検証
             * 手ぶれが全くないならばstopしなくていいかもしれない
             */

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