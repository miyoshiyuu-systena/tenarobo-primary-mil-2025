#include "CircleAction.h"
#include "spikeapi.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"

std::function<void(ActionNode*&)> circle_action(int radius, int angular, int duration, bool is_right)
{
    return [radius, angular, duration, is_right](ActionNode*& next_ptr) {
        const float angular_speed = angular / (duration / 1000);
        if (is_right) {
            twinWheelDrive.curveRightSpeed(angular_speed, radius);
        } else {
            twinWheelDrive.curveLeftSpeed(angular_speed, radius);
        }

        dly_tsk(duration * 1000);
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}