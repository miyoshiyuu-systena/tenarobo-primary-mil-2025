#include "StraightGoAction.h"
#include "spikeapi.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"

std::function<void(ActionNode*&)> straight_go_action(int speed, int duration, std::function<bool(void)> judge)
{
    return [speed, duration, judge](ActionNode*& next_ptr) {
        twinWheelDrive.setSpeed(speed, speed);
        while (!judge()) {
            dly_tsk(duration);
        }
        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}