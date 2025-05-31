#include    "syssvc/syslog.h"
#include    "ForceSensor.h"
#include    "PerceptionTask.h"

using   namespace   spikeapi;

// 力学センサーの宣言
//                          SPIKE_port
ForceSensor forceSensor(    EPort::PORT_D      );

/**
 * 知覚�?��?
 * @param   exinf     拡張�?報
 */
void    perc_task(intptr_t exinf)   {
    /**
     * カメラ�?ータの取�?
     */
    //  TODO    miyoshi: カメラ�?ータの取�?

    /**
     * 力学センサ�?ータの取�?
     */
    syslog(LOG_INFO, "~~~~~~~~~~~~~~~~~~");
    
    float force, distance;
    bool isPress, isTouch;
    force = forceSensor.getForce();
    syslog(LOG_INFO, "force %f [N]", force);

    distance = forceSensor.getDistance();
    syslog(LOG_INFO, "distance %f [mm]", distance);

    isPress = forceSensor.isPressed(0.05);
    syslog(LOG_INFO, "press?? %d", isPress);

    isTouch = forceSensor.isTouched();
    syslog(LOG_INFO, "touch?? %d", isTouch);

    syslog(LOG_INFO, "~~~~~~~~~~~~~~~~~~");

    /**
     * カラーセンサ�?ータの取�?
     */
    //  TODO    miyoshi: カラーセンサ�?ータの取�?

    /**
     * �?音波センサ�?ータの取�?
     */
    //  TODO    miyoshi: �?音波センサ�?ータの取�?

    /**
     * �?�?
     */
    syslog(LOG_WARNING, "Perception_Task is running");

    //  タスク終�?
    ext_tsk();
}