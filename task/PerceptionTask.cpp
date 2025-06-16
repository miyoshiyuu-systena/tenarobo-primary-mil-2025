#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "share/PerceptionDataAccess.h"
#include    "share/ModuleAccess.h"
#include    <string>

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
    ColorSensor::HSV hsv;
    colorSensor.getColor(hsv, true);
    perceptionDataAccess.color[0] = hsv.h;
    perceptionDataAccess.color[1] = hsv.s;
    perceptionDataAccess.color[2] = hsv.v;

    /**
     * 超音波センサデータの取得
     */
    perceptionDataAccess.distance = ultrasonicSensor.getDistance();

    /**
     * 力センサデータの取得
     */
    perceptionDataAccess.force = forceSensor.getForce();

    /**
     * モータデータの取得
     */
    perceptionDataAccess.leftMotor = twinWheelDrive.getLeftCount();
    perceptionDataAccess.rightMotor = twinWheelDrive.getRightCount();

    std::string sensorsStr = "Sensors - Force:" + std::to_string(perceptionDataAccess.force) + "N Distance:" + 
                            std::to_string(perceptionDataAccess.distance) + "mm Color:" + 
                            std::to_string(perceptionDataAccess.color[0]) + "," + 
                            std::to_string(perceptionDataAccess.color[1]) + "," + 
                            std::to_string(perceptionDataAccess.color[2]);
    syslog(LOG_WARNING, sensorsStr.c_str());
    
    std::string motorsStr = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotor) + 
                           " Right:" + std::to_string(perceptionDataAccess.rightMotor);
    syslog(LOG_WARNING, motorsStr.c_str());

    //  タスク終了
    ext_tsk();
}