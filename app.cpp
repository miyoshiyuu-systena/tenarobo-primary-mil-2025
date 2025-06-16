#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "logging/Logger.h"
#include    "Motor.h"
#include    "ForceSensor.h"
#include    "ColorSensor.h"
#include    "UltrasonicSensor.h"

using namespace spikeapi;

// 周辺モジュールのインスタンスを宣言
/**                 SPIKE_port          Rotation_direction                      Count_reset */
Motor rightWheel(   EPort::PORT_A,      Motor::EDirection::CLOCKWISE,           true        );
Motor leftWheel (   EPort::PORT_B,      Motor::EDirection::COUNTERCLOCKWISE,    true        );
Motor frontArm  (   EPort::PORT_C,      Motor::EDirection::CLOCKWISE,           true        );

/**                                 SPIKE_port       */
ForceSensor forceSensor(            EPort::PORT_D   );
ColorSensor colorSensor(            EPort::PORT_E   );
UltrasonicSensor ultrasonicSensor(  EPort::PORT_F   );

/**
 * 超信地旋回（左旋回）
 * @param power 回転パワー値（0-100）
 * @param duration_usec 旋回時間（マイクロ秒）
 */
void pivotTurnLeft(int power, int duration_usec) {
    Logger& logger = Logger::getInstance();
    logger.logInfo("超信地旋回開始（左旋回）- パワー: " + std::to_string(power) + ", 時間: " + std::to_string(duration_usec) + "usec");
    
    // 左モータを後進、右モータを前進に設定
    leftWheel.setPower(-power);
    rightWheel.setPower(power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftWheel.stop();
    rightWheel.stop();
    
    logger.logInfo("超信地旋回完了（左旋回）");
}

/**
 * 超信地旋回（右旋回）
 * @param power 回転パワー値（0-100）
 * @param duration_usec 旋回時間（マイクロ秒）
 */
void pivotTurnRight(int power, int duration_usec) {
    Logger& logger = Logger::getInstance();
    logger.logInfo("超信地旋回開始（右旋回）- パワー: " + std::to_string(power) + ", 時間: " + std::to_string(duration_usec) + "usec");
    
    // 左モータを前進、右モータを後進に設定
    leftWheel.setPower(power);
    rightWheel.setPower(-power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftWheel.stop();
    rightWheel.stop();
    
    logger.logInfo("超信地旋回完了（右旋回）");
}

/**
 * 前進
 * @param power 前進パワー値（0-100）
 * @param duration_usec 前進時間（マイクロ秒）
 */
void moveForward(int power, int duration_usec) {
    Logger& logger = Logger::getInstance();
    logger.logInfo("前進開始 - パワー: " + std::to_string(power) + ", 時間: " + std::to_string(duration_usec) + "usec");
    
    // 両モータを前進に設定
    leftWheel.setPower(power);
    rightWheel.setPower(power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftWheel.stop();
    rightWheel.stop();
    
    logger.logInfo("前進完了");
}

/**
 * 全モータ停止
 */
void stopAllMotors() {
    leftWheel.stop();
    rightWheel.stop();
    frontArm.stop();
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    
    // 乱数生成器の初期化
    srand((unsigned int)time(NULL));
    
    // アームモーターとセンサーの初期化結果確認
    logger.logInfo("=== 初期化結果確認 ===");
    logger.logInfo("右モーター: " + std::string(rightWheel.hasError() ? "エラー" : "正常"));
    logger.logInfo("左モーター: " + std::string(leftWheel.hasError() ? "エラー" : "正常"));
    logger.logInfo("アームモーター: " + std::string(frontArm.hasError() ? "エラー" : "正常"));
    logger.logInfo("力学センサー: " + std::string(forceSensor.hasError() ? "エラー" : "正常"));
    logger.logInfo("カラーセンサー: " + std::string(colorSensor.hasError() ? "エラー" : "正常"));
    logger.logInfo("超音波センサー: " + std::string(ultrasonicSensor.hasError() ? "エラー" : "正常"));

    sta_cyc(PERC_CYC);
    
    logger.logInfo("=== メインループ開始 ===");
    
    while (true) {
        // 超音波センサーによる距離測定
        int32_t distance = ultrasonicSensor.getDistance();
        logger.logInfo("超音波センサー - 距離: " + std::to_string(distance) + "cm");
        
        // 力学センサーによる押下判定
        float force = forceSensor.getForce();
        bool isPressed = forceSensor.isPressed(1.0f);  // 1.0N以上で押下と判定
        bool isTouched = forceSensor.isTouched();
        logger.logInfo("力学センサー - 力: " + std::to_string(force) + "N, 押下: " + 
                      (isPressed ? "Yes" : "No") + ", 接触: " + (isTouched ? "Yes" : "No"));
        
        // カラーセンサーによるRGB値測定
        ColorSensor::RGB rgb;
        colorSensor.getRGB(rgb);
        logger.logInfo("カラーセンサー - R: " + std::to_string(rgb.r) + 
                      ", G: " + std::to_string(rgb.g) + 
                      ", B: " + std::to_string(rgb.b));
        
        // 右モーター、左モーターのランダムなPWM制御
        int rightPower = (rand() % 201) - 100;  // -100から+100の範囲
        int leftPower = (rand() % 201) - 100;   // -100から+100の範囲
        
        rightWheel.setPower(rightPower);
        leftWheel.setPower(leftPower);
        
        logger.logInfo("モーター制御 - 右: " + std::to_string(rightPower) + 
                      ", 左: " + std::to_string(leftPower));
        
        // 3秒待機（3,000,000マイクロ秒）
        tslp_tsk(3000000);
    }

    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    //  タスク終了
    ext_tsk();
}