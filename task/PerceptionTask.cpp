#include    <string.h>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "UltrasonicSensor.h"

using   namespace   spikeapi;
// 超音波センサーの宣言
UltrasonicSensor    ultraSonicSencer(   EPort::PORT_F   );

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
     * カラーセンサデータの取得
     */
    //  TODO    miyoshi: カラーセンサデータの取得

    /**
     * 超音波センサデータの取得
     */
    //  TODO    miyoshi: 超音波センサデータの取得
    int32_t Distance;
    char distance_buffer[50];
    Distance = ultraSonicSencer.getDistance();
    sprintf(distance_buffer, "distance %d [cm]", Distance);
    syslog(LOG_WARNING, distance_buffer);     

    /**
     * 待機
     */
    syslog(LOG_WARNING, "Perception_Task is running");

    //  タスク終了
    ext_tsk();
}