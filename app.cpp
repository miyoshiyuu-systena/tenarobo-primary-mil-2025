#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    <cmath>
#include    <string>
#include    <vector>
#include    "syssvc/syslog.h"
#include    "logging/Logger.h"
#include    "PerceptionTask.h"
#include    "organ/TwinWheelDrive.h"
#include    "action/ActionChain.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"

using namespace spikeapi;

/**
 * 指定時間待機する関数
 * @param ms 待機時間（ミリ秒）
 */
void delay_ms(int ms) {
    dly_tsk(ms * 1000);  // マイクロ秒単位で待機
}

/**
 * エンコーダ値を出力する関数
 */
void logEncoderValues() {
    Logger& logger = Logger::getInstance();
    std::string left_count = "左エンコーダ値: " + std::to_string(twinWheelDrive.getLeftCount());
    std::string right_count = "右エンコーダ値: " + std::to_string(twinWheelDrive.getRightCount());
    logger.logInfo(left_count.c_str());
    logger.logInfo(right_count.c_str());
}

// ActionChain用のアクション関数群

/**
 * 左超信地回転アクション
 */
void leftPivotTurnAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("左超信地回転開始");
    twinWheelDrive.leftPivotTurn(100);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右超信地回転アクション
 */
void rightPivotTurnAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("右超信地回転開始");
    twinWheelDrive.rightPivotTurn(100);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 左信地回転アクション
 */
void leftSpinTurnAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("左信地回転開始");
    twinWheelDrive.leftSpinTurn(100);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右信地回転アクション
 */
void rightSpinTurnAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("右信地回転開始");
    twinWheelDrive.rightSpinTurn(100);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 左曲線走行アクション
 */
void leftCurveAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("左曲線走行開始");
    twinWheelDrive.curveLeftSpeed(100, 150.0f);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右曲線走行アクション
 */
void rightCurveAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("右曲線走行開始");
    twinWheelDrive.curveRightSpeed(100, 150.0f);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 直進走行アクション
 */
void straightAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("直進走行開始");
    twinWheelDrive.setPower(50, 50);
    delay_ms(3000);  // 3秒実行
    twinWheelDrive.brake();
    logEncoderValues();
}


/**
 * センサー値確認アクション
 */
void sensorCheckAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("センサー値確認開始");
    
    // 力センサー値
    std::string force_value = "力センサー値: " + std::to_string(forceSensor.getForce());
    logger.logInfo(force_value.c_str());
    
    // 超音波センサー値
    std::string distance_value = "超音波センサー値: " + std::to_string(ultrasonicSensor.getDistance());
    logger.logInfo(distance_value.c_str());
    
    // カラーセンサー値
    // auto color = colorSensor.getColor();
    // std::string color_value = "カラーセンサー値 - R:" + std::to_string(color.r) + 
    //                           " G:" + std::to_string(color.g) + 
    //                           " B:" + std::to_string(color.b);
    // logger.logInfo(color_value.c_str());
    
    delay_ms(1000);  // 1秒待機
}

/**
 * 終了アクション
 */
void finishAction() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("全アクション完了");
    // 最終的なエンコーダ値の出力
    logEncoderValues();
}

/**
 * ActionChainを使用したより効率的な実行関数
 */
void executeActionChainOptimized(ActionChain* firstAction) {
    Logger& logger = Logger::getInstance();
    ActionChain* currentAction = firstAction;
    ActionChain* previousAction = nullptr;
    
    while (currentAction != nullptr) {
        logger.logInfo("アクション実行中...");
        currentAction->execute();
        
        // アクションが終了するまで待機
        // while (!currentAction->isEnd()) {
        //     delay_ms(100);
        // }

        if (!currentAction->isEnd()) {
            logger.logError("アクションが終了していないにもかかわらず、次のアクションに移動しました");
            currentAction->deleteNext();
            delete currentAction;
            currentAction = nullptr;
            break;
        }
        
        // 次のアクションに移動
        previousAction = currentAction;
        currentAction = currentAction->getNext();
        delete previousAction;
    }
}

/**
 * メイン処理（方法3：真のActionChain実行）
 * @param   exinf     拡張情報
 */
void    main_task_method3(intptr_t exinf)   {
    Logger& logger = Logger::getInstance();
    logger.logInfo("ActionChainサンプルプログラム開始（方法3：真のActionChain実行）");
    
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);
    
    // ActionChainの動的作成（newを使用してヒープ上に作成）
    ActionChain* action1 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, leftPivotTurnAction, "左超信地回転");
    ActionChain* action2 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, rightPivotTurnAction, "右超信地回転");
    ActionChain* action3 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, leftSpinTurnAction, "左信地回転");
    ActionChain* action4 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, rightSpinTurnAction, "右信地回転");
    ActionChain* action5 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, leftCurveAction, "左曲線走行");
    ActionChain* action6 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, rightCurveAction, "右曲線走行");
    ActionChain* action7 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, straightAction, "直進走行");
    // ActionChain* action8 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, frontArmAction, "前腕モーター動作");
    ActionChain* action9 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, sensorCheckAction, "センサー値確認");
    ActionChain* action10 = new ActionChain(&twinWheelDrive, &frontArm, perceptionDataAccess, finishAction, "終了処理");
    
    // ActionChainの連結
    action1->setNext(action2);
    action2->setNext(action3);
    action3->setNext(action4);
    action4->setNext(action5);
    action5->setNext(action6);
    action6->setNext(action7);
    // action7->setNext(action8);
    action7->setNext(action9);
    action9->setNext(action10);
    
    // ActionChainの実行（動的メモリ管理で効率的なチェーン実行）
    executeActionChainOptimized(action1);
    
    // 知覚タスクの停止
    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    logger.logInfo("ActionChainサンプルプログラム終了");
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {   
    // 方法3を使用（真のActionChain実行） - 推奨
    main_task_method3(exinf);
    
    //  タスク終了
    ext_tsk();
}