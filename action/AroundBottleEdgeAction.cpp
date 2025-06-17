#include    "AroundBottleEdgeAction.h"
#include    "spikeapi.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"
#include    "TwinWheelConst.h"

/**
 * 円弧を描きながら移動するアクションのファクトリー関数
 * @param radius 円弧の半径[mm]
 * @param angle 円弧の角度[°]
 * @param duration 円弧を描く時間[ms](実際の到着時間にはルーズ、走路状況による、目標時間程度)
 * @return 円弧を描きながら移動するアクション
 */
std::function<void(ActionNode*&)> around_bottle_edge_action(
    float radius,
    float angle,
    int duration
)
{
    return [radius, angle, duration](ActionNode*& next_ptr) {
        // 命令を送る時間の間隔[ms]
        const float FIXED_INTERVAL = 100.0f;

        // 速度[°/s]
        const float speed = angle / (duration / 1000);

        // 残りの時間[ms]
        float remaining_time_ms = duration;

        // 円弧を描きながら移動する
        while (true) {
            twinWheelDrive.curveRightSpeed(speed, radius);

            // 残りの時間を更新
            remaining_time_ms -= FIXED_INTERVAL;

            if (remaining_time_ms < FIXED_INTERVAL) {
                dly_tsk(remaining_time_ms * 1000);
                break; // 最後の待機を終えたら、モーター停止に進む
            } else {
                dly_tsk(FIXED_INTERVAL * 1000);
            }
        }

        // モーターを停止
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}
