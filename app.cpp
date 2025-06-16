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

/**                                 ハブ内蔵IMU      */
IMU imu;

// 座標変換用の構造体
struct RobotAngularVelocity {
    float yaw;    // ロボットのZ軸回転（左右回転）
    float pitch;  // ロボットのY軸回転（前後傾斜）
    float roll;   // ロボットのX軸回転（左右傾斜）
};

/**
 * 改善された期待値計算関数
 */
 float calculateExpectedYawRate(int leftPower, int rightPower) {
    // 車輪間距離（実測値に基づいて調整）
    const float wheelbase = 115.0f; // mm
    // 車輪径（実測値に基づいて調整）
    const float wheelDiameter = 56.0f; // mm
    // モーターパワーから回転速度への変換係数
    const float powerToRpmFactor = 1.8f; // 実験的に決定
    
    // 左右車輪の回転速度差から角速度を計算
    float leftRpm = leftPower * powerToRpmFactor;
    float rightRpm = rightPower * powerToRpmFactor;
    
    // 角速度 = (右車輪速度 - 左車輪速度) / 車輪間距離
    float linearVelocityDiff = (rightRpm - leftRpm) * (wheelDiameter * M_PI / 60.0f);
    float expectedYawRate = linearVelocityDiff / wheelbase * 180.0f / M_PI; // deg/s
    
    return expectedYawRate;
}

/**
 * SPIKE角度（54.3146度）での座標変換テスト関数
 * 複数のパターンを試してロボット座標系での角速度を計算
 */
void testAngularVelocityConversions(const IMU::AngularVelocity& imuAngVel, 
                                   int leftPower, int rightPower, Logger& logger) {
    
    const float SPIKE_ANGLE = 54.3146f * M_PI / 180.0f;  // ラジアンに変換
    const float cos_angle = cos(SPIKE_ANGLE);
    const float sin_angle = sin(SPIKE_ANGLE);
    
    // パターン1: Z軸周りの回転変換
    RobotAngularVelocity pattern1;
    pattern1.yaw   = imuAngVel.x * cos_angle - imuAngVel.y * sin_angle;
    pattern1.pitch = imuAngVel.x * sin_angle + imuAngVel.y * cos_angle;
    pattern1.roll  = imuAngVel.z;
    
    // パターン2: X軸周りの回転変換
    RobotAngularVelocity pattern2;
    pattern2.yaw   = imuAngVel.x;
    pattern2.pitch = imuAngVel.y * cos_angle - imuAngVel.z * sin_angle;
    pattern2.roll  = imuAngVel.y * sin_angle + imuAngVel.z * cos_angle;
    
    // パターン3: Y軸周りの回転変換
    RobotAngularVelocity pattern3;
    pattern3.yaw   = imuAngVel.x * cos_angle + imuAngVel.z * sin_angle;
    pattern3.pitch = imuAngVel.y;
    pattern3.roll  = -imuAngVel.x * sin_angle + imuAngVel.z * cos_angle;
    
    // パターン4: 複合回転変換（仮想的な組み合わせ）
    RobotAngularVelocity pattern4;
    pattern4.yaw   = (imuAngVel.x + imuAngVel.y + imuAngVel.z) / sqrt(3.0f);
    pattern4.pitch = (imuAngVel.x - imuAngVel.y) / sqrt(2.0f);
    pattern4.roll  = (imuAngVel.x - 2*imuAngVel.y + imuAngVel.z) / sqrt(6.0f);
    
    // パターン5: 単純な軸入れ替え
    RobotAngularVelocity pattern5;
    pattern5.yaw   = imuAngVel.z;  // Z軸をロボットのYaw（回転）とする
    pattern5.pitch = imuAngVel.y;  // Y軸をPitch
    pattern5.roll  = imuAngVel.x;  // X軸をRoll
    
    // 期待される角速度（改善された計算方法）
    float expected_yaw = calculateExpectedYawRate(leftPower, rightPower);
    
    // 各パターンの結果をログ出力
    logger.logInfo("=== 角速度変換テスト ===");
    logger.logInfo("生IMU - X:" + std::to_string(imuAngVel.x) + ", Y:" + std::to_string(imuAngVel.y) + ", Z:" + std::to_string(imuAngVel.z));
    logger.logInfo("モーター - 左:" + std::to_string(leftPower) + ", 右:" + std::to_string(rightPower));
    logger.logInfo("理論的期待値 - Yaw:" + std::to_string(expected_yaw) + ", Pitch:0.0, Roll:0.0 (水平移動)");
    
    logger.logInfo("パターン1(Z軸回転) - Yaw:" + std::to_string(pattern1.yaw) + ", Pitch:" + std::to_string(pattern1.pitch) + ", Roll:" + std::to_string(pattern1.roll));
    logger.logInfo("パターン2(X軸回転) - Yaw:" + std::to_string(pattern2.yaw) + ", Pitch:" + std::to_string(pattern2.pitch) + ", Roll:" + std::to_string(pattern2.roll));
    logger.logInfo("パターン3(Y軸回転) - Yaw:" + std::to_string(pattern3.yaw) + ", Pitch:" + std::to_string(pattern3.pitch) + ", Roll:" + std::to_string(pattern3.roll));
    logger.logInfo("パターン4(複合回転) - Yaw:" + std::to_string(pattern4.yaw) + ", Pitch:" + std::to_string(pattern4.pitch) + ", Roll:" + std::to_string(pattern4.roll));
    logger.logInfo("パターン5(軸入替) - Yaw:" + std::to_string(pattern5.yaw) + ", Pitch:" + std::to_string(pattern5.pitch) + ", Roll:" + std::to_string(pattern5.roll));
    
    // 物理的変換の複数パターン
    // SPIKE_ANGLE、cos_angle、sin_angleは関数の上部で既に宣言済み
    
    // 物理的変換パターン1: Z軸cosのみ
    RobotAngularVelocity physical1;
    physical1.yaw = imuAngVel.z * cos_angle;
    physical1.pitch = imuAngVel.y;
    physical1.roll = imuAngVel.x;
    
    // 物理的変換パターン2: Z軸cos + X軸sin
    RobotAngularVelocity physical2;
    physical2.yaw = imuAngVel.z * cos_angle + imuAngVel.x * sin_angle;
    physical2.pitch = imuAngVel.y;
    physical2.roll = imuAngVel.x;
    
    // 物理的変換パターン3: Z軸cos + Y軸sin
    RobotAngularVelocity physical3;
    physical3.yaw = imuAngVel.z * cos_angle + imuAngVel.y * sin_angle;
    physical3.pitch = imuAngVel.y;
    physical3.roll = imuAngVel.x;
    
    // 物理的変換パターン4: Z軸cos - X軸sin（符号逆転）
    RobotAngularVelocity physical4;
    physical4.yaw = imuAngVel.z * cos_angle - imuAngVel.x * sin_angle;
    physical4.pitch = imuAngVel.y;
    physical4.roll = imuAngVel.x;
    
    // 物理的変換パターン5: Z軸cos - Y軸sin（符号逆転）
    RobotAngularVelocity physical5;
    physical5.yaw = imuAngVel.z * cos_angle - imuAngVel.y * sin_angle;
    physical5.pitch = imuAngVel.y;
    physical5.roll = imuAngVel.x;
    
    logger.logInfo("物理的変換P1(Z*cos) - Yaw:" + std::to_string(physical1.yaw) + ", Pitch:" + std::to_string(physical1.pitch) + ", Roll:" + std::to_string(physical1.roll));
    logger.logInfo("物理的変換P2(Z*cos+X*sin) - Yaw:" + std::to_string(physical2.yaw) + ", Pitch:" + std::to_string(physical2.pitch) + ", Roll:" + std::to_string(physical2.roll));
    logger.logInfo("物理的変換P3(Z*cos+Y*sin) - Yaw:" + std::to_string(physical3.yaw) + ", Pitch:" + std::to_string(physical3.pitch) + ", Roll:" + std::to_string(physical3.roll));
    logger.logInfo("物理的変換P4(Z*cos-X*sin) - Yaw:" + std::to_string(physical4.yaw) + ", Pitch:" + std::to_string(physical4.pitch) + ", Roll:" + std::to_string(physical4.roll));
    logger.logInfo("物理的変換P5(Z*cos-Y*sin) - Yaw:" + std::to_string(physical5.yaw) + ", Pitch:" + std::to_string(physical5.pitch) + ", Roll:" + std::to_string(physical5.roll));
    
    // 各パターンの期待値との誤差を計算（Yaw, Pitch, Roll全て考慮）
    // 水平面を移動する2輪ロボットの理想的な角速度：
    // - Yaw (Z軸回転): 左右車輪の回転速度差による回転 → expected_yaw
    // - Pitch (Y軸回転): 前後傾斜 → 0 (水平移動のため)
    // - Roll (X軸回転): 左右傾斜 → 0 (水平移動のため)
    float error1 = fabs(pattern1.yaw - expected_yaw) + fabs(pattern1.pitch - 0.0f) + fabs(pattern1.roll - 0.0f);
    float error2 = fabs(pattern2.yaw - expected_yaw) + fabs(pattern2.pitch - 0.0f) + fabs(pattern2.roll - 0.0f);
    float error3 = fabs(pattern3.yaw - expected_yaw) + fabs(pattern3.pitch - 0.0f) + fabs(pattern3.roll - 0.0f);
    float error4 = fabs(pattern4.yaw - expected_yaw) + fabs(pattern4.pitch - 0.0f) + fabs(pattern4.roll - 0.0f);
    float error5 = fabs(pattern5.yaw - expected_yaw) + fabs(pattern5.pitch - 0.0f) + fabs(pattern5.roll - 0.0f);
    
    // 物理的変換パターンの誤差計算
    float errorPhysical1 = fabs(physical1.yaw - expected_yaw) + fabs(physical1.pitch - 0.0f) + fabs(physical1.roll - 0.0f);
    float errorPhysical2 = fabs(physical2.yaw - expected_yaw) + fabs(physical2.pitch - 0.0f) + fabs(physical2.roll - 0.0f);
    float errorPhysical3 = fabs(physical3.yaw - expected_yaw) + fabs(physical3.pitch - 0.0f) + fabs(physical3.roll - 0.0f);
    float errorPhysical4 = fabs(physical4.yaw - expected_yaw) + fabs(physical4.pitch - 0.0f) + fabs(physical4.roll - 0.0f);
    float errorPhysical5 = fabs(physical5.yaw - expected_yaw) + fabs(physical5.pitch - 0.0f) + fabs(physical5.roll - 0.0f);
    
    // 詳細な誤差分析
    logger.logInfo("=== 詳細誤差分析 ===");
    logger.logInfo("P1 - Yaw誤差:" + std::to_string(fabs(pattern1.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(pattern1.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(pattern1.roll)) + 
                  ", 総誤差:" + std::to_string(error1));
    logger.logInfo("P2 - Yaw誤差:" + std::to_string(fabs(pattern2.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(pattern2.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(pattern2.roll)) + 
                  ", 総誤差:" + std::to_string(error2));
    logger.logInfo("P3 - Yaw誤差:" + std::to_string(fabs(pattern3.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(pattern3.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(pattern3.roll)) + 
                  ", 総誤差:" + std::to_string(error3));
    logger.logInfo("P4 - Yaw誤差:" + std::to_string(fabs(pattern4.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(pattern4.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(pattern4.roll)) + 
                  ", 総誤差:" + std::to_string(error4));
    logger.logInfo("P5 - Yaw誤差:" + std::to_string(fabs(pattern5.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(pattern5.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(pattern5.roll)) + 
                  ", 総誤差:" + std::to_string(error5));
    logger.logInfo("物理的P1 - Yaw誤差:" + std::to_string(fabs(physical1.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(physical1.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(physical1.roll)) + 
                  ", 総誤差:" + std::to_string(errorPhysical1));
    logger.logInfo("物理的P2 - Yaw誤差:" + std::to_string(fabs(physical2.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(physical2.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(physical2.roll)) + 
                  ", 総誤差:" + std::to_string(errorPhysical2));
    logger.logInfo("物理的P3 - Yaw誤差:" + std::to_string(fabs(physical3.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(physical3.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(physical3.roll)) + 
                  ", 総誤差:" + std::to_string(errorPhysical3));
    logger.logInfo("物理的P4 - Yaw誤差:" + std::to_string(fabs(physical4.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(physical4.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(physical4.roll)) + 
                  ", 総誤差:" + std::to_string(errorPhysical4));
    logger.logInfo("物理的P5 - Yaw誤差:" + std::to_string(fabs(physical5.yaw - expected_yaw)) + 
                  ", Pitch誤差:" + std::to_string(fabs(physical5.pitch)) + 
                  ", Roll誤差:" + std::to_string(fabs(physical5.roll)) + 
                  ", 総誤差:" + std::to_string(errorPhysical5));
    
    // 最適パターンの特定
    float minError = error1;
    int bestPattern = 1;
    if (error2 < minError) { minError = error2; bestPattern = 2; }
    if (error3 < minError) { minError = error3; bestPattern = 3; }
    if (error4 < minError) { minError = error4; bestPattern = 4; }
    if (error5 < minError) { minError = error5; bestPattern = 5; }
    if (errorPhysical1 < minError) { minError = errorPhysical1; bestPattern = 6; }
    if (errorPhysical2 < minError) { minError = errorPhysical2; bestPattern = 7; }
    if (errorPhysical3 < minError) { minError = errorPhysical3; bestPattern = 8; }
    if (errorPhysical4 < minError) { minError = errorPhysical4; bestPattern = 9; }
    if (errorPhysical5 < minError) { minError = errorPhysical5; bestPattern = 10; }
    
    switch (bestPattern) {
        case 6:
            logger.logInfo("最適パターン: 物理的P1(Z*cos) (総誤差:" + std::to_string(minError) + ")");
            break;
        case 7:
            logger.logInfo("最適パターン: 物理的P2(Z*cos+X*sin) (総誤差:" + std::to_string(minError) + ")");
            break;
        case 8:
            logger.logInfo("最適パターン: 物理的P3(Z*cos+Y*sin) (総誤差:" + std::to_string(minError) + ")");
            break;
        case 9:
            logger.logInfo("最適パターン: 物理的P4(Z*cos-X*sin) (総誤差:" + std::to_string(minError) + ")");
            break;
        case 10:
            logger.logInfo("最適パターン: 物理的P5(Z*cos-Y*sin) (総誤差:" + std::to_string(minError) + ")");
            break;
        default:
            logger.logInfo("最適パターン: P" + std::to_string(bestPattern) + " (総誤差:" + std::to_string(minError) + ")");
            break;
    }
    
    // IMU データ品質の評価
    float imuMagnitude = sqrt(imuAngVel.x*imuAngVel.x + imuAngVel.y*imuAngVel.y + imuAngVel.z*imuAngVel.z);
    logger.logInfo("IMU角速度ベクトル大きさ: " + std::to_string(imuMagnitude) + "°/s");
    
    // データの信頼性チェック
    if (imuMagnitude > 100.0f) {
        logger.logInfo("警告: IMU角速度が異常に大きい値を示しています");
    } else if (imuMagnitude < 0.1f && (abs(leftPower) > 5 || abs(rightPower) > 5)) {
        logger.logInfo("警告: モーター動作中にIMU角速度が小さすぎます");
    } else {
        logger.logInfo("IMUデータ品質: 正常");
    }
}

/**
 * 物理的に正しい座標変換関数
 * SPIKEが54.3146度傾いているため、Z軸のcos成分とX/Y軸のsin成分を組み合わせる
 */
RobotAngularVelocity convertIMUToRobotAngularVelocity(const IMU::AngularVelocity& imuAngVel) {
    const float SPIKE_ANGLE = 54.3146f * M_PI / 180.0f;  // ラジアンに変換
    const float cos_angle = cos(SPIKE_ANGLE);
    const float sin_angle = sin(SPIKE_ANGLE);
    
    RobotAngularVelocity result;
    
    // SPIKEのZ軸センサーから実際のロボットのYaw角速度を取り出す
    // Z軸が傾いているため、cos成分のみが実際の垂直軸回転
    // さらに、X軸またはY軸のsin成分も垂直軸に寄与する可能性
    // テスト結果で以下のパターンから最適なものを選択：
    // P1: Z*cos, P2: Z*cos+X*sin, P3: Z*cos+Y*sin, P4: Z*cos-X*sin, P5: Z*cos-Y*sin
    result.yaw = imuAngVel.z * cos_angle + imuAngVel.x * sin_angle;  // 暫定：物理的P2パターン
    
    // 水平移動時は、PitchとRollは基本的に0に近い値になるはず
    // 微小な傾きや振動がある場合は、X, Y軸の読み値を使用
    result.pitch = imuAngVel.y;  // 前後傾斜
    result.roll = imuAngVel.x;   // 左右傾斜
    
    return result;
}

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
    logger.logInfo("IMU: " + std::string(imu.hasError() ? "エラー" : "正常"));

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
        
        // IMUによる角速度、加速度、温度測定
        IMU::AngularVelocity angVel;
        IMU::Acceleration accel;
        imu.getAngularVelocity(angVel);
        imu.getAcceleration(accel);
        float temperature = imu.getTemperature();
        
        logger.logInfo("IMU温度: " + std::to_string(temperature) + "℃");
        logger.logInfo("IMU加速度 - X: " + std::to_string(accel.x) + "mm/s², Y: " + 
                      std::to_string(accel.y) + "mm/s², Z: " + std::to_string(accel.z) + "mm/s²");
        
        // 角速度変換テスト用のモーター制御
        // 3つのパターンで回転テストを実施
        static int testPattern = 0;
        int rightPower, leftPower;
        
        switch(testPattern % 4) {
            case 0: // 静止テスト
                rightPower = 0;
                leftPower = 0;
                logger.logInfo("=== テストパターン: 静止 ===");
                break;
            case 1: // 右回転テスト
                rightPower = -30;
                leftPower = 30;
                logger.logInfo("=== テストパターン: 右回転 ===");
                break;
            case 2: // 左回転テスト
                rightPower = 30;
                leftPower = -30;
                logger.logInfo("=== テストパターン: 左回転 ===");
                break;
            case 3: // 前進テスト
                rightPower = 20;
                leftPower = 20;
                logger.logInfo("=== テストパターン: 前進 ===");
                break;
        }
        testPattern++;
        
        // 角速度変換テストを実行
        testAngularVelocityConversions(angVel, leftPower, rightPower, logger);
        
        // 変換後の角速度も表示（物理的変換P2パターン使用）
        RobotAngularVelocity robotAngVel = convertIMUToRobotAngularVelocity(angVel);
        logger.logInfo("変換後角速度(物理的P2) - Yaw: " + std::to_string(robotAngVel.yaw) + "°/s, Pitch: " + 
                      std::to_string(robotAngVel.pitch) + "°/s, Roll: " + std::to_string(robotAngVel.roll) + "°/s");
        
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