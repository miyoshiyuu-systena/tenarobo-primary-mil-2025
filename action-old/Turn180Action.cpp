#include    "Turn180Action.h"
#include    "spikeapi.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"

/**
 * 180度回転
 * @param duration 180度回転するのにかける時間（ms）
 */
std::function<void(ActionNode*&)> turn_180_action(int duration)
{
    return [duration](ActionNode*& next_ptr) {
        // その場で180°回転（引数は1s当たり何°回転するか）
        twinWheelDrive.leftPivotTurn(180 / (duration / 1000));

        // TOPPERS/ASP3インターフェースの待機関数（マイクロ秒）
        dly_tsk(duration * 1000);

        // モーターを停止
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}