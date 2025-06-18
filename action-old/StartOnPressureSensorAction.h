#ifndef _START_ON_PRESSURE_SENSOR_ACTION_H_
#define _START_ON_PRESSURE_SENSOR_ACTION_H_

#include    "action/ActionNode.h"
#include    <functional>

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 */
std::function<void(ActionNode*&)> start_on_pressure_sensor_action(float force_detect_threshold);

#endif // _START_ON_PRESSURE_SENSOR_ACTION_H_