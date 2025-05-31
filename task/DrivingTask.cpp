#include    <math.h>
#include    "syssvc/syslog.h"
#include    "Motor.h"
#include    "DrivingTask.h"

using   namespace   spikeapi;

// モーターの宣言
// TODO: Motor::hasErrorでインスタンスの可否を判定できるので、初期化時に確認する
/**                 SPIKE_port          Rotation_direction                      Count_reset */
Motor rightWheel(   EPort::PORT_A,      Motor::EDirection::CLOCKWISE,           true        );
Motor leftWheel (   EPort::PORT_B,      Motor::EDirection::COUNTERCLOCKWISE,    true        );
Motor frontArm  (   EPort::PORT_C,      Motor::EDirection::CLOCKWISE,           true        );

const int8_t num = 50;
int8_t count = 0;

/**
 * 駆動指示
 * @param   exinf     拡張情報
 */
void    driv_task(intptr_t exinf)   {
    count++;

    /**
     * 右モーターへの指示
     */
    //  TODO miyoshi: 右モーターへの指示
    rightWheel.setPower(num);

    /**
     * 左モーターへの指示
     */
    //  TODO miyoshi: 左モーターへの指示
    leftWheel.setSpeed(num);

    /**
     * アームモータへの指示
     */
    //  TODO miyoshi: アームモータへの指示
    frontArm.setPower(num * pow(-1, count));

    /**
     * 待機
     */
    syslog(LOG_ERROR, "Driving_Task is running");

    //  タスク終了
    ext_tsk();
}