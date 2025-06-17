#ifndef _START_ON_PRESSURE_SENSOR_ACTION_H_
#define _START_ON_PRESSURE_SENSOR_ACTION_H_

#include    "action/ActionNode.h"

/**
 * 圧力センサを押すまで忠犬ハチ公！！
 * 圧力センサを押したら次のタスクを始めるよ
 */
void start_on_pressure_sensor_action(ActionNode*& next_ptr);

#endif // _START_ON_PRESSURE_SENSOR_ACTION_H_