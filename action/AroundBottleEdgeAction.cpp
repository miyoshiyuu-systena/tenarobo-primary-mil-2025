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
 * @param vacation_time 休憩時間[ms]
 * @return 円弧を描きながら移動するアクション
 */
std::function<void(ActionNode*&)> around_bottle_edge_action(
    float radius,
    float angle,
    int duration,
    int vacation_time
)
{
    return [radius, angle, duration, vacation_time](ActionNode*& next_ptr) {
        // 命令を送る時間の間隔[ms]
        const float FIXED_INTERVAL = 100.0f;

        // 累積の角度[°]
        float accumulated_angle = 0.0f;

        // 単位時間当たりの角度の変化 = 回転速度[°/s]
        const float speed = angle / (duration / 1000);

        // 円弧を描きながら移動する
        while (true) {
            twinWheelDrive.curveRightSpeed(speed, radius);

            // 累積の角度を更新
            // 今回のインターバルで走行した実際の角度を加算
            const float left_angle = perceptionDataAccess.leftMotorSpeed * (FIXED_INTERVAL / 1000) * WHEEL_DIAMETER_MM / (2 * radius + WHEEL_TREAD_MM);
            const float right_angle = perceptionDataAccess.rightMotorSpeed * (FIXED_INTERVAL / 1000) * WHEEL_DIAMETER_MM / (2 * radius - WHEEL_TREAD_MM);
            accumulated_angle += (right_angle + left_angle) / 2.0f;

            // 累積の角度が目標の角度を超えたらループを抜ける
            if (accumulated_angle >= angle) {
                break;
            }

            // 残りの角度から、完了までの残り時間を計算する[ms]
            const float remaining_time_ms = (angle - accumulated_angle) / speed * 1000.0f;

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

        // 休憩時間
        dly_tsk(vacation_time * 1000);
    };
}
