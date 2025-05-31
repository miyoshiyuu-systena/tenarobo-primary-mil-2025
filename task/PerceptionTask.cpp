#include    "syssvc/syslog.h"
#include    "ForceSensor.h"
#include    "PerceptionTask.h"

using   namespace   spikeapi;

// 力学センサーの宣言
//                          SPIKE_port
ForceSensor forceSensor(    EPort::PORT_D      );

/**
 * 知覚処理
 * @param   exinf     拡張情報
 */
void    perc_task(intptr_t exinf)   {
    /**
     * カメラデータの取得
     */
    //  TODO    miyoshi: カメラデータの取得

    /**
     * 力学センサデータの取得
     */
    syslog(LOG_WARNING, "~~~~~~~~~~~~~~~~~~");
    
    float force, distance;
    bool isPressed, isTouched;
    force = forceSensor.getForce();
    syslog(LOG_WARNING, "force %06.3f [N]", force);

    distance = forceSensor.getDistance();
    syslog(LOG_WARNING, "distance %06.3f [mm]", distance);

    isPressed = forceSensor.isPressed(0.05);
    syslog(LOG_WARNING, "press?? %d", isPressed);

    isTouched = forceSensor.isTouched();
    syslog(LOG_WARNING, "touch?? %d", isTouched);

    syslog(LOG_WARNING, "~~~~~~~~~~~~~~~~~~");

    /**
     * カラーセンサデータの取得
     */
    //  TODO    miyoshi: カラーセンサデータの取得

    /**
     * 超音波センサデータの取得
     */
    //  TODO    miyoshi: 超音波センサデータの取得

    syslog(LOG_WARNING, "Perception_Task is running");

    //  タスク終了
    ext_tsk();
}