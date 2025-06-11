#include    <string.h>
#include    "syssvc/syslog.h"
#include    "ColorSensor.h"
#include    "PerceptionTask.h"

using   namespace   spikeapi;

// カラーセンサの取得
//                          SPIKE_port
ColorSensor colorSensor(    EPort::PORT_F   );

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

    syslog(LOG_WARNING, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

    ColorSensor::RGB rgb;
    ColorSensor::HSV hsv;
    char rgb_buffer[50], hsv_buffer[50];

    colorSensor.getRGB(&rgb);
    colorSensor.getHSV(&hsv);
    
    sprintf(rgb_buffer, "RGB: %d, %d, %d", rgb.r, rgb.g, rgb.b);
    syslog(LOG_WARNING, rbg_buffer);

    sprintf(hsv_buffer, "HSV: %d, %d, %d", hsv.h, hsv.s, hsv.v);
    syslog(LOG_WARNING, hsv_buffer);

    syslog(LOG_WARNING, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");


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