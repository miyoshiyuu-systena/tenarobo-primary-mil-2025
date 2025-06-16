#include    "app.h"
#include    <stdio.h>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "logging/Logger.h"
#include    "Motor.h"
#include    "ForceSensor.h"
#include    "ColorSensor.h"
#include    "UltrasonicSensor.h"

using namespace spikeapi;

// モータのグローバル変数宣言（初期化はmain_task内で行う）
Motor* rightMotor = nullptr;  // 右モータ（一旦無効化）
Motor* leftMotor = nullptr;   // 左モータ（一旦無効化）
Motor* armMotor = nullptr;    // アームモータ
ForceSensor* forceSensor = nullptr;      // 力学センサ
ColorSensor* colorSensor = nullptr;      // カラーセンサ
UltrasonicSensor* ultrasonicSensor = nullptr;  // 超音波センサ

/**
 * 超信地旋回（左旋回）
 * @param power 回転パワー値（0-100）
 * @param duration_usec 旋回時間（マイクロ秒）
 */
void pivotTurnLeft(int power, int duration_usec) {
    Logger& logger = Logger::getInstance();
    logger.logInfo("超信地旋回開始（左旋回）- パワー: " + std::to_string(power) + ", 時間: " + std::to_string(duration_usec) + "usec");
    
    // 左モータを後進、右モータを前進に設定
    leftMotor->setPower(-power);
    rightMotor->setPower(power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftMotor->stop();
    rightMotor->stop();
    
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
    leftMotor->setPower(power);
    rightMotor->setPower(-power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftMotor->stop();
    rightMotor->stop();
    
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
    leftMotor->setPower(power);
    rightMotor->setPower(power);
    
    // 指定時間待機
    tslp_tsk(duration_usec);
    
    // モータを停止
    leftMotor->stop();
    rightMotor->stop();
    
    logger.logInfo("前進完了");
}

/**
 * 全モータ停止
 */
void stopAllMotors() {
    if (leftMotor) leftMotor->stop();
    if (rightMotor) rightMotor->stop();
    if (armMotor) armMotor->stop();
}

/**
 * モーターポート診断
 * 各ポートにモーターが接続されているかを確認する
 */
void diagnoseMotorPorts() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("=== モーターポート診断開始 ===");
    
    // グローバル変数として宣言されたモーターの状態を確認
    logger.logInfo("=== グローバルモーター状態確認 ===");
    logger.logInfo("右モーター (PORT_A): " + std::string(rightMotor ? (rightMotor->hasError() ? "エラー" : "正常") : "未初期化"));
    logger.logInfo("左モーター (PORT_B): " + std::string(leftMotor ? (leftMotor->hasError() ? "エラー" : "正常") : "未初期化"));
    
    // 実際にモーターを動作させて接続確認
    logger.logInfo("=== モーター動作確認 ===");
    if (rightMotor && !rightMotor->hasError()) {
        logger.logInfo("右モーター動作テスト開始");
        rightMotor->setPower(20);
        tslp_tsk(500000); // 0.5秒待機（500,000マイクロ秒）
        rightMotor->stop();
        logger.logInfo("右モーター動作テスト完了");
    }
    
    if (leftMotor && !leftMotor->hasError()) {
        logger.logInfo("左モーター動作テスト開始");
        leftMotor->setPower(20);
        tslp_tsk(500000); // 0.5秒待機（500,000マイクロ秒）
        leftMotor->stop();
        logger.logInfo("左モーター動作テスト完了");
    }
    
    logger.logInfo("=== モーターポート診断完了 ===");
}

/**
 * 全ポート診断
 * 各ポートで何のデバイスが検出されるかを確認する
 */
void diagnoseAllPorts() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("=== 全ポート診断開始 ===");
    logger.logInfo("接続されているデバイスを検出中...");
    
    // 使用可能なポートの配列を定義
    EPort ports[] = {EPort::PORT_A, EPort::PORT_B, EPort::PORT_C, EPort::PORT_D, EPort::PORT_E, EPort::PORT_F};
    const char* portNames[] = {"A", "B", "C", "D", "E", "F"};
    
    // 各ポートでデバイス検出を試行
    for (int i = 0; i < 6; i++) {
        EPort currentPort = ports[i];
        const char* portName = portNames[i];
        
        logger.logInfo("=== Port " + std::string(portName) + " 診断 ===");
        
        // デバイス検出カウンター
        int detectedDevices = 0;
        
        // モーター検出を試行
        Motor* testMotor = nullptr;
        try {
            testMotor = new Motor(currentPort, Motor::EDirection::CLOCKWISE, true);
            if (testMotor && !testMotor->hasError()) {
                logger.logInfo("✓ Port " + std::string(portName) + " - モーター検出成功");
                detectedDevices++;
            } else {
                logger.logInfo("✗ Port " + std::string(portName) + " - モーター検出失敗");
            }
        } catch (...) {
            logger.logInfo("✗ Port " + std::string(portName) + " - モーター検出エラー");
        }
        if (testMotor) {
            delete testMotor;
            testMotor = nullptr;
        }
        
        // 力学センサー検出を試行
        ForceSensor* testForce = nullptr;
        try {
            testForce = new ForceSensor(currentPort);
            if (testForce && !testForce->hasError()) {
                logger.logInfo("✓ Port " + std::string(portName) + " - 力学センサー検出成功");
                float force = testForce->getForce();
                logger.logInfo("   力: " + std::to_string(force) + "N");
                detectedDevices++;
            } else {
                logger.logInfo("✗ Port " + std::string(portName) + " - 力学センサー検出失敗");
            }
        } catch (...) {
            logger.logInfo("✗ Port " + std::string(portName) + " - 力学センサー検出エラー");
        }
        if (testForce) {
            delete testForce;
            testForce = nullptr;
        }
        
        // カラーセンサー検出を試行
        ColorSensor* testColor = nullptr;
        try {
            testColor = new ColorSensor(currentPort);
            if (testColor && !testColor->hasError()) {
                logger.logInfo("✓ Port " + std::string(portName) + " - カラーセンサー検出成功");
                ColorSensor::RGB rgb;
                testColor->getRGB(rgb);
                logger.logInfo("   RGB: (" + std::to_string(rgb.r) + ", " + std::to_string(rgb.g) + ", " + std::to_string(rgb.b) + ")");
                detectedDevices++;
            } else {
                logger.logInfo("✗ Port " + std::string(portName) + " - カラーセンサー検出失敗");
            }
        } catch (...) {
            logger.logInfo("✗ Port " + std::string(portName) + " - カラーセンサー検出エラー");
        }
        if (testColor) {
            delete testColor;
            testColor = nullptr;
        }
        
        // 超音波センサー検出を試行
        UltrasonicSensor* testUltrasonic = nullptr;
        try {
            testUltrasonic = new UltrasonicSensor(currentPort);
            if (testUltrasonic && !testUltrasonic->hasError()) {
                logger.logInfo("✓ Port " + std::string(portName) + " - 超音波センサー検出成功");
                int32_t distance = testUltrasonic->getDistance();
                logger.logInfo("   距離: " + std::to_string(distance) + "cm");
                detectedDevices++;
            } else {
                logger.logInfo("✗ Port " + std::string(portName) + " - 超音波センサー検出失敗");
            }
        } catch (...) {
            logger.logInfo("✗ Port " + std::string(portName) + " - 超音波センサー検出エラー");
        }
        if (testUltrasonic) {
            delete testUltrasonic;
            testUltrasonic = nullptr;
        }
        
        // 結果まとめ
        if (detectedDevices == 0) {
            logger.logWarning("⚠️  Port " + std::string(portName) + " - 何も検出されませんでした");
        } else {
            logger.logInfo("Port " + std::string(portName) + " - " + std::to_string(detectedDevices) + "個のデバイスが検出されました");
        }
        
        logger.logInfo("Port " + std::string(portName) + " 診断完了");
    }
    
    logger.logInfo("=== 全ポート診断完了 ===");
}

/**
 * アームモーターとセンサーの診断
 * 各デバイスが正常に初期化されているかを確認する
 */
void diagnoseArmAndSensors() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("=== アームモーター・センサー診断開始 ===");
    
    // アームモーターの状態確認
    logger.logInfo("=== アームモーター状態確認 ===");
    logger.logInfo("アームモーター (PORT_C): " + std::string(armMotor ? (armMotor->hasError() ? "エラー" : "正常") : "未初期化"));
    
    // センサーの状態確認
    logger.logInfo("=== センサー状態確認 ===");
    logger.logInfo("力学センサー: " + std::string(forceSensor ? (forceSensor->hasError() ? "エラー" : "正常") : "未初期化"));
    logger.logInfo("カラーセンサー: " + std::string(colorSensor ? (colorSensor->hasError() ? "エラー" : "正常") : "未初期化"));
    logger.logInfo("超音波センサー: " + std::string(ultrasonicSensor ? (ultrasonicSensor->hasError() ? "エラー" : "正常") : "未初期化"));
    
    // 実際にデバイスを動作させて接続確認
    logger.logInfo("=== デバイス動作確認 ===");
    
    // アームモーター動作テスト
    if (armMotor && !armMotor->hasError()) {
        logger.logInfo("アームモーター動作テスト開始");
        armMotor->setPower(15);
        tslp_tsk(500000); // 0.5秒待機（500,000マイクロ秒）
        armMotor->stop();
        logger.logInfo("アームモーター動作テスト完了");
    }
    
    // 力学センサーテスト
    if (forceSensor && !forceSensor->hasError()) {
        logger.logInfo("力学センサーテスト開始");
        
        // 複数回測定してみる
        for (int i = 0; i < 3; i++) {
            float force = forceSensor->getForce();
            float distance = forceSensor->getDistance();
            bool isPressed = forceSensor->isPressed(5.0f);
            bool isTouched = forceSensor->isTouched();
            
            logger.logInfo("測定 " + std::to_string(i + 1) + " - 力: " + std::to_string(force) + "N, 距離: " + std::to_string(distance) + "mm");
            logger.logInfo("測定 " + std::to_string(i + 1) + " - 押下: " + std::string(isPressed ? "Yes" : "No") + ", 接触: " + std::string(isTouched ? "Yes" : "No"));
            
            // 短時間待機
            tslp_tsk(100000); // 0.1秒待機
        }
        
        // 力が0Nの場合は警告を表示
        float finalForce = forceSensor->getForce();
        float finalDistance = forceSensor->getDistance();
        if (finalForce == 0.0f && finalDistance == 0.0f) {
            logger.logWarning("⚠️  力学センサーが0値を返しています - 接続を確認してください");
            logger.logWarning("   デバッグ情報: センサーが初期化されているが、データが更新されていない可能性があります");
        }
        
        logger.logInfo("力学センサーテスト完了");
    } else {
        logger.logError("❌ 力学センサーエラー");
        if (forceSensor) {
            logger.logError("   センサーにエラーが発生しています");
        } else {
            logger.logError("   センサーが初期化されていません");
        }
    }
    
    // カラーセンサーテスト
    if (colorSensor && !colorSensor->hasError()) {
        logger.logInfo("カラーセンサーテスト開始");
        ColorSensor::RGB rgb;
        colorSensor->getRGB(rgb);
        
        int32_t reflection = colorSensor->getReflection();
        int32_t ambient = colorSensor->getAmbient();
        
        logger.logInfo("カラーセンサー値 - RGB: (" + std::to_string(rgb.r) + ", " + std::to_string(rgb.g) + ", " + std::to_string(rgb.b) + ")");
        logger.logInfo("カラーセンサー値 - 反射: " + std::to_string(reflection) + "%, 環境光: " + std::to_string(ambient) + "%");
        logger.logInfo("カラーセンサーテスト完了");
    }
    
    // 超音波センサーテスト
    if (ultrasonicSensor && !ultrasonicSensor->hasError()) {
        logger.logInfo("超音波センサーテスト開始");
        int32_t distance = ultrasonicSensor->getDistance();
        bool presence = ultrasonicSensor->isPresence();
        
        logger.logInfo("超音波センサー値 - 距離: " + std::to_string(distance) + "cm, 検出: " + std::string(presence ? "Yes" : "No"));
        logger.logInfo("超音波センサーテスト完了");
    }
    
    logger.logInfo("=== アームモーター・センサー診断完了 ===");
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    logger.logInfo("=== ロボット制御システム開始 ===");
    logger.logInfo("システム初期化を開始");

    /**
     * センサーデータ取得インスタンスの生成
     */
    //  TODO miyoshi: センサーの初期化
    logger.logInfo("センサーシステムの初期化中...");
    // 実際の初期化処理が追加される際のログ例
    // logger.logDebug("カラーセンサー初期化完了");
    // logger.logDebug("超音波センサー初期化完了");
    // logger.logDebug("ジャイロセンサー初期化完了");
    logger.logInfo("センサーシステム初期化完了");

    /**
     * シナリオチェーンの構成
     */
    //  TODO miyoshi: シナリオチェーンの構成
    logger.logInfo("シナリオチェーンの構成中...");
    // 実際の構成処理が追加される際のログ例
    // logger.logDebug("ライントレースシナリオ登録");
    // logger.logDebug("障害物回避シナリオ登録");
    // logger.logDebug("ゴール検出シナリオ登録");
    logger.logInfo("シナリオチェーン構成完了");

    /**
     * 駆動系インスタンスの生成
     */
    logger.logInfo("駆動系システムの初期化中...");
    
    // 左右のモーター初期化は一旦無効化
    logger.logInfo("左右のモーター初期化を一旦スキップ");
    
    // アームモーター初期化
    logger.logInfo("アームモーター初期化を開始");
    armMotor = new Motor(EPort::PORT_C, Motor::EDirection::CLOCKWISE, true);
    
    // センサー初期化（固定ポート割り当て）
    logger.logInfo("センサー初期化を開始");
    
    // 力学センサー - PORT_D固定
    logger.logInfo("力学センサー初期化 (PORT_D)");
    forceSensor = new ForceSensor(EPort::PORT_D);
    if (!forceSensor->hasError()) {
        logger.logInfo("力学センサー初期化成功 - PORT_D");
    } else {
        logger.logError("力学センサー初期化失敗 - PORT_D");
        delete forceSensor;
        forceSensor = nullptr;
        
        // 代替ポートで再試行
        logger.logInfo("代替ポートでの力学センサー初期化を試行...");
        for (int portIndex = 0; portIndex < 6; portIndex++) {
            EPort altPorts[] = {EPort::PORT_A, EPort::PORT_B, EPort::PORT_C, EPort::PORT_D, EPort::PORT_E, EPort::PORT_F};
            const char* portNames[] = {"A", "B", "C", "D", "E", "F"};
            
            logger.logInfo("PORT_" + std::string(portNames[portIndex]) + "で力学センサー初期化を試行");
            ForceSensor* testSensor = new ForceSensor(altPorts[portIndex]);
            if (!testSensor->hasError()) {
                logger.logInfo("PORT_" + std::string(portNames[portIndex]) + "で力学センサー初期化成功！");
                forceSensor = testSensor;
                break;
            } else {
                logger.logInfo("PORT_" + std::string(portNames[portIndex]) + "で力学センサー初期化失敗");
                delete testSensor;
            }
        }
    }
    
    // カラーセンサー - PORT_E固定
    logger.logInfo("カラーセンサー初期化 (PORT_E)");
    colorSensor = new ColorSensor(EPort::PORT_E);
    if (!colorSensor->hasError()) {
        logger.logInfo("カラーセンサー初期化成功 - PORT_E");
    } else {
        logger.logError("カラーセンサー初期化失敗 - PORT_E");
        delete colorSensor;
        colorSensor = nullptr;
    }
    
    // 超音波センサー - PORT_F固定
    logger.logInfo("超音波センサー初期化 (PORT_F)");
    ultrasonicSensor = new UltrasonicSensor(EPort::PORT_F);
    if (!ultrasonicSensor->hasError()) {
        logger.logInfo("超音波センサー初期化成功 - PORT_F");
    } else {
        logger.logError("超音波センサー初期化失敗 - PORT_F");
        delete ultrasonicSensor;
        ultrasonicSensor = nullptr;
    }
    
    // アームモーターとセンサーの初期化結果確認
    logger.logInfo("=== 初期化結果確認 ===");
    logger.logInfo("アームモーター: " + std::string(armMotor && !armMotor->hasError() ? "正常" : "エラー"));
    logger.logInfo("力学センサー: " + std::string(forceSensor && !forceSensor->hasError() ? "正常" : "エラー"));
    logger.logInfo("カラーセンサー: " + std::string(colorSensor && !colorSensor->hasError() ? "正常" : "エラー"));
    logger.logInfo("超音波センサー: " + std::string(ultrasonicSensor && !ultrasonicSensor->hasError() ? "正常" : "エラー"));
    
    // 診断機能を初期化後に実行
    logger.logInfo("=== 包括的診断開始 ===");
    diagnoseAllPorts();
    diagnoseArmAndSensors();
    logger.logInfo("=== 包括的診断完了 ===");
    
    logger.logInfo("駆動系システム初期化完了");

    syslog(LOG_NOTICE, "INIT FINISH - Its start!!!");
    logger.logInfo("=== システム初期化完了 - 制御開始 ===");

    sta_cyc(PERC_CYC);
    logger.logInfo("周期タスク開始: PERC_CYC");

    // 初期化完了時にログファイルを一度書き込み
    logger.writeLogsToFile();

    // アームモーターとセンサーのデモンストレーション
    logger.logInfo("センサー監視システム開始");
    logger.logInfo("=== 使用可能なセンサー ===");
    if (forceSensor && !forceSensor->hasError()) {
        logger.logInfo("✓ 力学センサー (PORT_D) - 正常");
    }
    if (colorSensor && !colorSensor->hasError()) {
        logger.logInfo("✓ カラーセンサー (PORT_E) - 正常");
    }
    if (ultrasonicSensor && !ultrasonicSensor->hasError()) {
        logger.logInfo("✓ 超音波センサー (PORT_F) - 正常");
    }
    
    // 力学センサーが利用可能な場合のみ待機
    if (forceSensor && !forceSensor->hasError()) {
        logger.logInfo("=== 待機中 ===");
        logger.logInfo("圧力センサーを押してください...");
        
        bool systemStarted = false;
        int waitCount = 0;
        float pressureThreshold = 5.0f;  // 初期閾値5.0N
        
        // 圧力センサーが押されるまで待機ループ
        while (!systemStarted) {
        if (forceSensor && !forceSensor->hasError()) {
            // 現在の力の値を取得
            float currentForce = forceSensor->getForce();
            float currentDistance = forceSensor->getDistance();
            bool isTouched = forceSensor->isTouched();
            
            // 閾値を段階的に下げる（デバッグ用）
            if (waitCount == 30) {
                pressureThreshold = 1.0f;
                logger.logInfo("🔧 デバッグ: 閾値を1.0Nに下げました");
            } else if (waitCount == 60) {
                pressureThreshold = 0.1f;
                logger.logInfo("🔧 デバッグ: 閾値を0.1Nに下げました");
            } else if (waitCount == 90) {
                pressureThreshold = 0.01f;
                logger.logInfo("🔧 デバッグ: 閾値を0.01Nに下げました");
            }
            
            // 圧力センサーをチェック（動的な閾値で判定）
            if (forceSensor->isPressed(pressureThreshold) || forceSensor->isTouched() || (currentForce > pressureThreshold)) {
                logger.logInfo("=== 圧力センサーが押されました！ ===");
                logger.logInfo("現在の力: " + std::to_string(currentForce) + "N (閾値: " + std::to_string(pressureThreshold) + "N)");
                logger.logInfo("センサー監視を開始します...");
                systemStarted = true;
                break;
            }
            
            // 1秒ごとに現在の力の値を表示（デバッグ用）
            logger.logInfo("現在の力: " + std::to_string(currentForce) + "N, 距離: " + std::to_string(currentDistance) + "mm, 接触: " + std::string(isTouched ? "Yes" : "No") + " (閾値: " + std::to_string(pressureThreshold) + "N)");
            
            // 力が常に0Nの場合の警告
            if (waitCount > 5 && currentForce == 0.0f && currentDistance == 0.0f) {
                logger.logWarning("⚠️  力学センサーが常に0値を返しています。");
                logger.logWarning("   センサーの接続を確認してください。");
                logger.logWarning("   - PORT_Dに力学センサーが接続されていますか？");
                logger.logWarning("   - ケーブルの接続は正しいですか？");
                logger.logWarning("   - 別のポートに接続されている可能性はありませんか？");
            }
        } else {
            logger.logError("❌ 力学センサーが利用できません");
            if (!forceSensor) {
                logger.logError("   センサーが初期化されていません");
            } else {
                logger.logError("   センサーにエラーが発生しています");
            }
        }
        
        // 10秒ごとに待機中メッセージを表示
        if (waitCount % 10 == 0) {
            logger.logInfo("待機中... (圧力センサーを押してください)");
        }
        
            waitCount++;
            tslp_tsk(1000000); // 1秒待機（1,000,000マイクロ秒）
        }
        
        // システム開始後の連続監視
        logger.logInfo("=== センサー値の連続監視開始 ===");
        logger.logInfo("Ctrl+C でプログラムを終了してください");
    } else {
        logger.logWarning("❌ 力学センサーが利用できません");
        logger.logWarning("圧力センサーによる起動待機をスキップして、直接監視を開始します");
        logger.logInfo("=== センサー値の連続監視開始 ===");
        logger.logInfo("Ctrl+C でプログラムを終了してください");
    }
    
    int monitoringCount = 0;
    
    while (true) {
        logger.logInfo("=== 監視サイクル " + std::to_string(monitoringCount + 1) + " ===");
        
        // 力学センサーの値を取得・表示
        if (forceSensor && !forceSensor->hasError()) {
            float force = forceSensor->getForce();
            float distance = forceSensor->getDistance();
            bool isTouched = forceSensor->isTouched();
            
            logger.logInfo("💪 力学センサー:");
            logger.logInfo("   力: " + std::to_string(force) + "N");
            logger.logInfo("   距離: " + std::to_string(distance) + "mm");
            logger.logInfo("   接触: " + std::string(isTouched ? "検出" : "未検出"));
        }
        
        // カラーセンサーの値を取得・表示
        if (colorSensor && !colorSensor->hasError()) {
            ColorSensor::RGB rgb;
            colorSensor->getRGB(rgb);
            
            int32_t reflection = colorSensor->getReflection();
            int32_t ambient = colorSensor->getAmbient();
            
            logger.logInfo("🎨 カラーセンサー:");
            logger.logInfo("   RGB: (" + std::to_string(rgb.r) + ", " + std::to_string(rgb.g) + ", " + std::to_string(rgb.b) + ")");
            logger.logInfo("   反射率: " + std::to_string(reflection) + "%");
            logger.logInfo("   環境光: " + std::to_string(ambient) + "%");
        }
        
        // 超音波センサーの値を取得・表示
        if (ultrasonicSensor && !ultrasonicSensor->hasError()) {
            int32_t distance = ultrasonicSensor->getDistance();
            bool presence = ultrasonicSensor->isPresence();
            
            logger.logInfo("📡 超音波センサー:");
            logger.logInfo("   距離: " + std::to_string(distance) + "cm");
            logger.logInfo("   物体検出: " + std::string(presence ? "検出" : "未検出"));
            
            // 距離に応じた警告
            if (distance > 0 && distance < 10) {
                logger.logInfo("   ⚠️  警告: 物体が非常に近い！");
            } else if (distance > 0 && distance < 20) {
                logger.logInfo("   ⚠️  注意: 物体が近づいています");
            }
        }
        
                 // 圧力センサーの現在状態も表示
         if (forceSensor && !forceSensor->hasError()) {
             float force = forceSensor->getForce();
             float distance = forceSensor->getDistance();
             bool isPressed = forceSensor->isPressed(5.0f);
             bool isTouched = forceSensor->isTouched();
            
            logger.logInfo("👆 圧力センサー:");
            logger.logInfo("   力: " + std::to_string(force) + "N");
            logger.logInfo("   距離: " + std::to_string(distance) + "mm");
            logger.logInfo("   状態: " + std::string(isPressed ? "押下中" : (isTouched ? "接触中" : "なし")));
        }
        
        logger.logInfo("--------------------------------");
        
        monitoringCount++;
        
        // 3秒間隔で監視
        tslp_tsk(3000000); // 3秒待機（3,000,000マイクロ秒）
        
        // 100回監視したら一旦ログファイルに書き込み
        if (monitoringCount % 100 == 0) {
            logger.writeLogsToFile();
            logger.logInfo("ログファイルを更新しました (監視回数: " + std::to_string(monitoringCount) + ")");
        }
    }

    // 通常はここに到達しませんが、プログラム終了時の処理
    logger.logInfo("プログラム終了処理開始");
    
    // 最終的にモータを停止
    stopAllMotors();
    
    stp_cyc(PERC_CYC);
    logger.logInfo("周期タスク停止完了");
    
    logger.logInfo("=== ロボット制御システム正常終了 ===");
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    // メモリ解放
    if (leftMotor) {
        delete leftMotor;
        leftMotor = nullptr;
    }
    if (rightMotor) {
        delete rightMotor;
        rightMotor = nullptr;
    }
    if (armMotor) {
        delete armMotor;
        armMotor = nullptr;
    }
    if (forceSensor) {
        delete forceSensor;
        forceSensor = nullptr;
    }
    if (colorSensor) {
        delete colorSensor;
        colorSensor = nullptr;
    }
    if (ultrasonicSensor) {
        delete ultrasonicSensor;
        ultrasonicSensor = nullptr;
    }
    logger.logInfo("メモリ解放完了");
    
    //  タスク終了
    ext_tsk();
}