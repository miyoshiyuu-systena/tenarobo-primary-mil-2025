#include    "spikeapi.h"
#include    "RunUntilWallDetectAction.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionChain.h"

/**
 * 壁を見つけるまで猪突猛進
 */
void run_until_wall_detect_action(ActionChain*& next_ptr)
{
    // 壁の接近を検知する距離
    static const int DETECT_DISTANCE_THRESHOLD = 500;

    // 壁の接近を検知する間隔時間（ms）
    static const int DETECT_INTERVAL = 200;

    // モーターの速度出力
    static const float SPEED = 1000.0f;

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

    // 休憩時間（1s）
    static const int VACATION_TIME = 1000;

    dly_tsk(VACATION_TIME * 1000);
}