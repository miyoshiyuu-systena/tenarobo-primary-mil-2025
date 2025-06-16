#include    "spikeapi.h"
#include    "RunUntilWallDetectAction.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"

/**
 * 壁を見つけるまで猪突猛進
 */
void run_until_wall_detect_action()
{
    // 壁の接近を検知する距離
    static const int DETECT_DISTANCE_THRESHOLD = 100;

    // 壁の接近を検知する間隔時間（ms）
    static const int DETECT_INTERVAL = 100;

    // モーターの回転出力
    static const int POWER = 50;

    // 壁の接近を検知したか
    bool isDetectWall = false;

    do {
        // 前進する
        twinWheelDrive.setPower(POWER, POWER);

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
}