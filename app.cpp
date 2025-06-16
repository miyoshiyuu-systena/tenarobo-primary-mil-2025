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

// 他の周辺モジュールのインスタンスを宣言
TwinWheelDrive twinWheelDrive(EPort::PORT_B, EPort::PORT_A);

Motor frontArm    (   EPort::PORT_C,      Motor::EDirection::CLOCKWISE,           true        );

/**                                 SPIKE_port       */
ForceSensor forceSensor(            EPort::PORT_D   );
ColorSensor colorSensor(            EPort::PORT_E   );
UltrasonicSensor ultrasonicSensor(  EPort::PORT_F   );

/**                                 ハブ内蔵IMU      */
IMU imu;

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

    // 5. 左曲線走行 (速度200°/s, 半径300mm) - 2秒間
    logger.logInfo("左曲線走行開始");
    twinWheelDrive.curveLeft(200, 1000.0f);
    delay_ms(10000);
    twinWheelDrive.stop();
    
    sta_cyc(PERC_CYC);
    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    //  タスク終了
    ext_tsk();
}