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
    syslog(LOG_INFO, "~~~~~~~~~~~~~~~~~~");
    
    float force, distance;
    bool isPress, isTouch;
    force = forceSensor.getForce();
    syslog(LOG_INFO, "force %f [N]", force);

    distance = forceSensor.getDistance();
    syslog(LOG_INFO, "distance %f [mm]", distance);

    isPress = forceSensor.isPress(0.05);
    syslog(LOG_INFO, "press?? %d", isPress);

    isTouch = forceSensor.isTouch();
    syslog(LOG_INFO, "touch?? %d", isTouch);

    syslog(LOG_INFO, "~~~~~~~~~~~~~~~~~~");

    /**
     * カラーセンサデータの取得
     */
    //  TODO    miyoshi: カラーセンサデータの取得

    /**
     * 超音波センサデータの取得
     */
    //  TODO    miyoshi: 超音波センサデータの取得

    /**
     * 待機
     */
    syslog(LOG_WARNING, "Perception_Task is running");

    //  タスク終了
    ext_tsk();
}