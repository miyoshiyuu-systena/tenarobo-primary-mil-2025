#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    <cmath>
#include    <string>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "logging/Logger.h"
#include    "Motor.h"
#include    "ForceSensor.h"
#include    "ColorSensor.h"
#include    "UltrasonicSensor.h"
#include    "IMU.h"

#include    "organ/TwinWheelDrive.h"

using namespace spikeapi;

/**
 * 指定時間待機する関数
 * @param ms 待機時間（ミリ秒）
 */
void delay_ms(int ms) {
    dly_tsk(ms * 1000);  // マイクロ秒単位で待機
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    
    logger.logInfo("TwinWheelDriveデモンストレーション開始");
    
    sta_cyc(PERC_CYC);

    twinWheelDrive.leftPivotTurn(10000);
    logger.logInfo("左超信地回転開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    std::string left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    std::string right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.rightPivotTurn(10000);
    logger.logInfo("右超信地回転開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.leftSpinTurn(10000);
    logger.logInfo("左信地回転開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.rightSpinTurn(10000);
    logger.logInfo("右信地回転開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.curveLeftSpeed(100, 1000.0f);
    logger.logInfo("左曲線走行開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.curveRightSpeed(100, 1000.0f);
    logger.logInfo("右曲線走行開始");
    delay_ms(10000);
    twinWheelDrive.stop();

    left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    twinWheelDrive.setPower(100, 100);
    logger.logInfo("パワー設定開始");
    delay_ms(500);
    twinWheelDrive.brake();

    left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());

    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());

    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    //  タスク終了
    ext_tsk();
}