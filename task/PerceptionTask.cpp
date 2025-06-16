#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "share/PerceptionDataAccess.h"
#include    "share/ModuleAccess.h"
#include    "logging/Logger.h"
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
    colorSensor.getHSV(hsv, true);
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
    perceptionDataAccess.leftMotorRotationCount = twinWheelDrive.getLeftCount();
    perceptionDataAccess.rightMotorRotationCount = twinWheelDrive.getRightCount();
    perceptionDataAccess.leftMotorSpeed = twinWheelDrive.getLeftSpeed();
    perceptionDataAccess.rightMotorSpeed = twinWheelDrive.getRightSpeed();
    perceptionDataAccess.leftMotorPower = twinWheelDrive.getLeftPower();
    perceptionDataAccess.rightMotorPower = twinWheelDrive.getRightPower();

    // // ログ出力
    std::string sensorsStr = "Sensors - Force:" + std::to_string(perceptionDataAccess.force) + "N Distance:" + 
                            std::to_string(perceptionDataAccess.distance) + "mm Color:" + 
                            std::to_string(perceptionDataAccess.color[0]) + "," + 
                            std::to_string(perceptionDataAccess.color[1]) + "," + 
                            std::to_string(perceptionDataAccess.color[2]);
    Logger::getInstance().logWarning(sensorsStr);
    
    // std::string motorsStr = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotorRotationCount) + 
    //                        " Right:" + std::to_string(perceptionDataAccess.rightMotorRotationCount);
    // Logger::getInstance().logWarning(motorsStr);
    
    std::string motorsStr2 = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotorSpeed + " °/秒") + 
                           " Right:" + std::to_string(perceptionDataAccess.rightMotorSpeed + " °/秒");
    Logger::getInstance().logWarning(motorsStr2);
    
    // std::string motorsStr3 = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotorPower) + 
    //                        " Right:" + std::to_string(perceptionDataAccess.rightMotorPower);
    // Logger::getInstance().logWarning("Power （-100 ~ 100）");
    // Logger::getInstance().logWarning(motorsStr3);

    //  タスク終了
    ext_tsk();
}