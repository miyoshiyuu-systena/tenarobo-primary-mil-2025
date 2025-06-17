#include    "StartOnPressureSensorAction.h"
#include    "spikeapi.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 */
void start_on_pressure_sensor_action(ActionNode*& next_ptr)
{
    // 検知間隔時間(100ms)
    static const int DELAY_TIME = 100 * 1000;

    // 圧力検知の閾値（3.0N）
    static const float FORCE_DETECT_THRESHOLD = 3.0f;

    // 圧力センサが押されるまで暇つぶし
    while (perceptionDataAccess.force <= FORCE_DETECT_THRESHOLD) {
        dly_tsk(DELAY_TIME);
    }
}