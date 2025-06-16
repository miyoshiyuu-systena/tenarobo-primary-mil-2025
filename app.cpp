#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    <cmath>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "logging/Logger.h"
#include    "Motor.h"
#include    "ForceSensor.h"
#include    "ColorSensor.h"
#include    "UltrasonicSensor.h"
#include    "IMU.h"

#include    "organ/two_wheel_drive.h"

using namespace spikeapi;

// 駆動用のモーターインスタンス
// Motor leftDriveMotor(EPort::PORT_A, Motor::EDirection::CLOCKWISE, true);
// Motor rightDriveMotor(EPort::PORT_B, Motor::EDirection::CLOCKWISE, true);

// 2輪駆動システムのインスタンス
// TwoWheelDrive drive(&leftDriveMotor, &rightDriveMotor);

// 他の周辺モジュールのインスタンスを宣言
// NOTE: PORT_A、PORT_Bは2輪駆動システムで使用するため、アームのモーターを無効化
Motor rightMotor  (   EPort::PORT_A,      Motor::EDirection::CLOCKWISE,           true        );
Motor leftMotor   (   EPort::PORT_B,      Motor::EDirection::CLOCKWISE,           true        );
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
 * 2輪駆動システムのデモンストレーション
 */
void demonstrateTwoWheelDrive() {
    if (leftMotor.hasError() || rightMotor.hasError()) {
        syslog(LOG_ERROR, "モーターの初期化に失敗しました");
    } else {
        syslog(LOG_INFO, "モーターの初期化に成功しました");
    }
    
    // 初期化チェック
    // if (drive.hasError()) {
    //     syslog(LOG_ERROR, "2輪駆動システムの初期化に失敗しました");
    //     return;
    // }

    
    syslog(LOG_INFO, "=== 2輪駆動システムデモンストレーション開始 ===");
    
    // 1. 前進デモ
    syslog(LOG_INFO, "1. 前進（速度: 200°/s、3秒間）");
    // drive.leftForward(200);
    // drive.rightForward(200);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 2. 後進デモ
    syslog(LOG_INFO, "2. 後進（速度: 200°/s、3秒間）");
    // drive.leftReverse(200);
    // drive.rightReverse(200);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 3. 左曲線デモ
    syslog(LOG_INFO, "3. 左曲線（速度: 200°/s、半径: 300mm、3秒間）");
    // drive.curveLeft(200, 300.0f);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 4. 右曲線デモ
    syslog(LOG_INFO, "4. 右曲線（速度: 200°/s、半径: 300mm、3秒間）");
    // drive.curveRight(200, 300.0f);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 5. 左回転デモ（超信地旋回）
    syslog(LOG_INFO, "5. 左回転（速度: 150°/s、3秒間）");
    // drive.leftReverse(150);
    // drive.rightForward(150);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 6. 右回転デモ（超信地旋回）
    syslog(LOG_INFO, "6. 右回転（速度: 150°/s、3秒間）");
    // drive.leftForward(150);
    // drive.rightReverse(150);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 7. PWM制御デモ
    syslog(LOG_INFO, "7. PWM制御（左: 50%, 右: 80%, 3秒間）");
    // drive.setPower(50, 80);
    delay_ms(3000);
    // drive.stop();
    delay_ms(1000);
    
    // 8. ブレーキデモ
    syslog(LOG_INFO, "8. ブレーキテスト（前進後、急ブレーキ）");
    // drive.leftForward(300);
    // drive.rightForward(300);
    delay_ms(2000);
    // drive.brake();  // 急ブレーキ
    delay_ms(2000);
    
    // 9. エンコーダ値の表示
    syslog(LOG_INFO, "9. エンコーダ値の表示");
    // drive.resetLeftCount();
    // drive.resetRightCount();
    delay_ms(100);
    
    // 少し動かしてエンコーダ値を確認
    // drive.leftForward(200);
    // drive.rightForward(200);
    delay_ms(2000);
    // drive.stop();
    
    // int32_t leftCount = drive.getLeftCount();
    // int32_t rightCount = drive.getRightCount();
    // syslog(LOG_INFO, "左エンコーダ: %d°, 右エンコーダ: %d°", leftCount, rightCount);
    
    syslog(LOG_INFO, "=== 2輪駆動システムデモンストレーション完了 ===");
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    
    sta_cyc(PERC_CYC);
    
    // 2輪駆動システムのデモンストレーション実行
    demonstrateTwoWheelDrive();

    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    //  タスク終了
    ext_tsk();
}