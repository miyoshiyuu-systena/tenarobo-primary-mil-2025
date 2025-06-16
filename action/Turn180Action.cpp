#include    "Turn180Action.h"
#include    "spikeapi.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"

/**
 * 180度回転
 */
void turn_180_action()
{
    // 180°回転するのにかける時間（s）
    static const int TURN_180_DURATION = 1;

    // その場で180°回転（引数は1s当たり何°回転するか）
    twinWheelDrive.leftPivotTurn(180 / TURN_180_DURATION);

    // TOPPERS/ASP3インターフェースの待機関数（マイクロ秒）
    dly_tsk(TURN_180_DURATION * 1000 * 1000);

    // モーターを停止
    twinWheelDrive.stop();
}