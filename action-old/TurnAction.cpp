#include "TurnAction.h"
#include "spikeapi.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"

/**
 * 回転アクションのファクトリー
 * @param angular 回転角度[°]
 * @param duration 回転時間[ms]
 * @param is_right 右回転か左回転か
 * @return 回転アクション
 */
std::function<void(ActionNode*&)> turn_action(int angular, int duration, bool is_right)
{
    return [angular, duration, is_right](ActionNode*& next_ptr) {
        const float angular_speed = angular / (duration / 1000);
        if (is_right) {
            twinWheelDrive.leftPivotTurn(angular_speed);
        } else {
            twinWheelDrive.rightPivotTurn(angular_speed);
        }
        dly_tsk(duration * 1000);
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}