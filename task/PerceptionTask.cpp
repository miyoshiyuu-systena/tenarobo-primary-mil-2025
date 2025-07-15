#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "share/PerceptionDataAccess.h"
#include    "share/ModuleAccess.h"
#include    "logging/Logger.h"
#include    <string>

/**
 * 知覚データをログ出力するかどうか
 * デバッグ用
 */
static const bool is_logging_enable = false;

/**
 * 知覚タスクの回数
 * オーバーフローしてもよい
 */
static int count = 0;

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
     * @note
     *      一回のタスクでカラーセンサから複数のデータを取得しようとすると、メインタスクが停止する（原因不明）
     * 　　　そのため知覚タスクのサイクルを2回セットにして別々の値を受ける
     */
    if (count % 2 == 0) {
        /**
         * HSV値の取得（青床の判別に有効）
         */
        ColorSensor::HSV hsv;
        colorSensor.getHSV(hsv, true);
        perceptionDataAccess.color[0] = hsv.h;
        perceptionDataAccess.color[1] = hsv.s;
        perceptionDataAccess.color[2] = hsv.v;
    } else {
        /**
         * 反射値の取得（白黒床の判別に有効）
         */
        perceptionDataAccess.brightness = colorSensor.getReflection();
    }

    
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
    // perceptionDataAccess.leftMotorRotationCount = twinWheelDrive.getLeftCount();
    // perceptionDataAccess.rightMotorRotationCount = twinWheelDrive.getRightCount();
    perceptionDataAccess.leftMotorSpeed = twinWheelDrive.getLeftSpeed();
    perceptionDataAccess.rightMotorSpeed = twinWheelDrive.getRightSpeed();
    // perceptionDataAccess.leftMotorPower = twinWheelDrive.getLeftPower();
    // perceptionDataAccess.rightMotorPower = twinWheelDrive.getRightPower();
    
    if (is_logging_enable) {
        std::string sensorsStr = "Sensors - Force:" + std::to_string(perceptionDataAccess.force) + "N Distance:" + 
        std::to_string(perceptionDataAccess.distance) + "mm Color: " + 
        std::to_string(perceptionDataAccess.color[0]) + "," + 
        std::to_string(perceptionDataAccess.color[1]) + "," + 
        std::to_string(perceptionDataAccess.color[2]);
        Logger::getInstance().logInfoWithoutConsoleLog(sensorsStr);
    }
    
    
    if (is_logging_enable) {
        // std::string motorsStr = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotorRotationCount) + 
        //                        " Right:" + std::to_string(perceptionDataAccess.rightMotorRotationCount);
        // Logger::getInstance().logWarning(motorsStr);
        
        std::string motorsStr2 = "Motor Speed - Left:" + std::to_string(perceptionDataAccess.leftMotorSpeed) + 
        " Right:" + std::to_string(perceptionDataAccess.rightMotorSpeed);
        Logger::getInstance().logInfoWithoutConsoleLog(motorsStr2);
        
        // std::string motorsStr3 = "Motors - Left:" + std::to_string(perceptionDataAccess.leftMotorPower) + 
        //                        " Right:" + std::to_string(perceptionDataAccess.rightMotorPower);
        // Logger::getInstance().logWarning("Power （-100 ~ 100）");
        // Logger::getInstance().logWarning(motorsStr3);
    }
    
    
    //  タスク終了
    ext_tsk();
}