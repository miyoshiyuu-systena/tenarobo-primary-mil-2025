#include    "spikeapi.h"
#include    "RunUntilWallDetectAction.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"
#include    <functional>

/**
 * 壁を見つけるまで猪突猛進するアクションのファクトリー関数
 * @param detect_distance_threshold 壁の接近を検知する距離
 * @param speed モーターの速度出力
 * @param detect_interval 壁の接近を検知する間隔時間（ms）
 * @param vacation_time 休憩時間（ms）
 * @return 壁を見つけるまで猪突猛進するアクション
 */
std::function<void(ActionNode*&)> run_until_wall_detect_action(
    int detect_distance_threshold,
    float speed,
    int detect_interval
)
{
    return [detect_distance_threshold, speed, detect_interval, vacation_time](ActionNode*& next_ptr) {
        // 壁の接近を検知する距離
        const int DETECT_DISTANCE_THRESHOLD = detect_distance_threshold;

        // 壁の接近を検知する間隔時間（ms）
        const int DETECT_INTERVAL = detect_interval;

        // モーターの速度出力
        const float SPEED = speed;

        // 休憩時間（ms）
        const int VACATION_TIME = vacation_time;

        // 壁の接近を検知したか
        bool isDetectWall = false;

        do {
            // 前進する
            twinWheelDrive.setSpeed(SPEED, SPEED);

            // 壁の接近を検知したらループを抜ける、そのためにフラグをONにする
            int distance = perceptionDataAccess.distance;
            if (
                (distance != -1) &&                     // (何も検知していないときは-1を返すため)
                (distance < DETECT_DISTANCE_THRESHOLD)  // 壁に接近したら
            ) {
                isDetectWall = true;
            }

            // 壁の接近を検知する間隔時間だけ待機
            dly_tsk(DETECT_INTERVAL * 1000);
        } while (!isDetectWall);

        // モーターを停止
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}