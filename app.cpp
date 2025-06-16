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

// ロガーインスタンス
Logger& logger = Logger::getInstance();

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
    logger.logInfo("左超信地回転開始");
    twinWheelDrive.leftPivotTurn(10000);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右超信地回転アクション
 */
void rightPivotTurnAction() {
    logger.logInfo("右超信地回転開始");
    twinWheelDrive.rightPivotTurn(10000);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 左信地回転アクション
 */
void leftSpinTurnAction() {
    logger.logInfo("左信地回転開始");
    twinWheelDrive.leftSpinTurn(10000);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右信地回転アクション
 */
void rightSpinTurnAction() {
    logger.logInfo("右信地回転開始");
    twinWheelDrive.rightSpinTurn(10000);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 左曲線走行アクション
 */
void leftCurveAction() {
    logger.logInfo("左曲線走行開始");
    twinWheelDrive.curveLeftSpeed(100, 500.0f);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 右曲線走行アクション
 */
void rightCurveAction() {
    logger.logInfo("右曲線走行開始");
    twinWheelDrive.curveRightSpeed(100, 500.0f);
    delay_ms(5000);  // 5秒実行
    twinWheelDrive.stop();
    logEncoderValues();
}

/**
 * 直進走行アクション
 */
void straightAction() {
    logger.logInfo("直進走行開始");
    twinWheelDrive.setPower(50, 50);
    delay_ms(3000);  // 3秒実行
    twinWheelDrive.brake();
    logEncoderValues();
}

/**
 * 前腕モーター動作アクション
 */
void frontArmAction() {
    logger.logInfo("前腕モーター動作開始");
    frontArm.setPower(50);
    delay_ms(2000);  // 2秒実行
    frontArm.stop();
    logger.logInfo("前腕モーター停止");
}

/**
 * センサー値確認アクション
 */
void sensorCheckAction() {
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
    logger.logInfo("全アクション完了");
    // 最終的なエンコーダ値の出力
    logEncoderValues();
}

/**
 * ActionChainの実行関数
 */
void executeActionChain(ActionChain* firstAction) {
    ActionChain* currentAction = firstAction;
    
    while (currentAction != nullptr) {
        logger.logInfo("アクション実行中...");
        currentAction->execute();
        
        // アクションが終了するまで待機
        while (!currentAction->isEnd()) {
            delay_ms(100);
        }
        
        // 次のアクションに移動（mNextActionはprivateなので直接アクセス不可）
        // ActionChainの設計上、各アクションは個別に実行する必要がある
        break;
    }
}

/**
 * ActionChainクラスを拡張して、次のアクションを取得するメソッドを追加するためのヘルパー関数
 * setNext()で設定されたアクションチェーンを再帰的に実行する
 */
void executeActionChainWithRecursion(ActionChain* action) {
    if (action == nullptr) {
        return;
    }
    
    logger.logInfo("アクション実行中...");
    action->execute();
    
    // アクションが終了するまで待機
    while (!action->isEnd()) {
        delay_ms(100);
    }
    
    // 次のアクションを再帰的に実行
    executeActionChainWithRecursion(action->getNext());
}

/**
 * ActionChainを使用したより効率的な実行関数
 */
void executeActionChainOptimized(ActionChain* firstAction) {
    ActionChain* currentAction = firstAction;
    
    while (currentAction != nullptr) {
        logger.logInfo("アクション実行中...");
        currentAction->execute();
        
        // アクションが終了するまで待機
        while (!currentAction->isEnd()) {
            delay_ms(100);
        }
        
        // 次のアクションに移動
        currentAction = currentAction->getNext();
    }
}

/**
 * メイン処理（方法1：個別実行）
 * @param   exinf     拡張情報
 */
void    main_task_method1(intptr_t exinf)   {
    logger.logInfo("ActionChainサンプルプログラム開始（方法1：個別実行）");
    
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);
    
    // ActionChainの作成と実行
    ActionChain action1(&twinWheelDrive, &frontArm, perceptionDataAccess, leftPivotTurnAction, "左超信地回転");
    action1.execute();
    while (!action1.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action2(&twinWheelDrive, &frontArm, perceptionDataAccess, rightPivotTurnAction, "右超信地回転");
    action2.execute();
    while (!action2.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action3(&twinWheelDrive, &frontArm, perceptionDataAccess, leftSpinTurnAction, "左信地回転");
    action3.execute();
    while (!action3.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action4(&twinWheelDrive, &frontArm, perceptionDataAccess, rightSpinTurnAction, "右信地回転");
    action4.execute();
    while (!action4.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action5(&twinWheelDrive, &frontArm, perceptionDataAccess, leftCurveAction, "左曲線走行");
    action5.execute();
    while (!action5.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action6(&twinWheelDrive, &frontArm, perceptionDataAccess, rightCurveAction, "右曲線走行");
    action6.execute();
    while (!action6.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action7(&twinWheelDrive, &frontArm, perceptionDataAccess, straightAction, "直進走行");
    action7.execute();
    while (!action7.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action8(&twinWheelDrive, &frontArm, perceptionDataAccess, frontArmAction, "前腕モーター動作");
    action8.execute();
    while (!action8.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action9(&twinWheelDrive, &frontArm, perceptionDataAccess, sensorCheckAction, "センサー値確認");
    action9.execute();
    while (!action9.isEnd()) {
        delay_ms(100);
    }
    
    ActionChain action10(&twinWheelDrive, &frontArm, perceptionDataAccess, finishAction, "終了処理");
    action10.execute();
    while (!action10.isEnd()) {
        delay_ms(100);
    }
    
    // 知覚タスクの停止
    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    logger.logInfo("ActionChainサンプルプログラム終了");
}

/**
 * メイン処理（方法2：チェーン作成）
 * @param   exinf     拡張情報
 */
void    main_task_method2(intptr_t exinf)   {
    logger.logInfo("ActionChainサンプルプログラム開始（方法2：チェーン作成）");
    
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);
    
    // ActionChainの作成
    ActionChain action1(&twinWheelDrive, &frontArm, perceptionDataAccess, leftPivotTurnAction, "左超信地回転");
    ActionChain action2(&twinWheelDrive, &frontArm, perceptionDataAccess, rightPivotTurnAction, "右超信地回転");
    ActionChain action3(&twinWheelDrive, &frontArm, perceptionDataAccess, leftSpinTurnAction, "左信地回転");
    ActionChain action4(&twinWheelDrive, &frontArm, perceptionDataAccess, rightSpinTurnAction, "右信地回転");
    ActionChain action5(&twinWheelDrive, &frontArm, perceptionDataAccess, leftCurveAction, "左曲線走行");
    ActionChain action6(&twinWheelDrive, &frontArm, perceptionDataAccess, rightCurveAction, "右曲線走行");
    ActionChain action7(&twinWheelDrive, &frontArm, perceptionDataAccess, straightAction, "直進走行");
    ActionChain action8(&twinWheelDrive, &frontArm, perceptionDataAccess, frontArmAction, "前腕モーター動作");
    ActionChain action9(&twinWheelDrive, &frontArm, perceptionDataAccess, sensorCheckAction, "センサー値確認");
    ActionChain action10(&twinWheelDrive, &frontArm, perceptionDataAccess, finishAction, "終了処理");
    
    // ActionChainの連結
    action1.setNext(&action2);
    action2.setNext(&action3);
    action3.setNext(&action4);
    action4.setNext(&action5);
    action5.setNext(&action6);
    action6.setNext(&action7);
    action7.setNext(&action8);
    action8.setNext(&action9);
    action9.setNext(&action10);
    
    // ActionChainの実行（手動でチェーンを実行）
    std::vector<ActionChain*> actionList = {&action1, &action2, &action3, &action4, &action5, &action6, &action7, &action8, &action9, &action10};
    
    for (ActionChain* action : actionList) {
        logger.logInfo("アクション実行中...");
        action->execute();
        while (!action->isEnd()) {
            delay_ms(100);
        }
    }
    
    // 知覚タスクの停止
    stp_cyc(PERC_CYC);
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    logger.logInfo("ActionChainサンプルプログラム終了");
}

/**
 * メイン処理（方法3：真のActionChain実行）
 * @param   exinf     拡張情報
 */
void    main_task_method3(intptr_t exinf)   {
    logger.logInfo("ActionChainサンプルプログラム開始（方法3：真のActionChain実行）");
    
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);
    
    // ActionChainの作成
    ActionChain action1(&twinWheelDrive, &frontArm, perceptionDataAccess, leftPivotTurnAction, "左超信地回転");
    ActionChain action2(&twinWheelDrive, &frontArm, perceptionDataAccess, rightPivotTurnAction, "右超信地回転");
    ActionChain action3(&twinWheelDrive, &frontArm, perceptionDataAccess, leftSpinTurnAction, "左信地回転");
    ActionChain action4(&twinWheelDrive, &frontArm, perceptionDataAccess, rightSpinTurnAction, "右信地回転");
    ActionChain action5(&twinWheelDrive, &frontArm, perceptionDataAccess, leftCurveAction, "左曲線走行");
    ActionChain action6(&twinWheelDrive, &frontArm, perceptionDataAccess, rightCurveAction, "右曲線走行");
    ActionChain action7(&twinWheelDrive, &frontArm, perceptionDataAccess, straightAction, "直進走行");
    ActionChain action8(&twinWheelDrive, &frontArm, perceptionDataAccess, frontArmAction, "前腕モーター動作");
    ActionChain action9(&twinWheelDrive, &frontArm, perceptionDataAccess, sensorCheckAction, "センサー値確認");
    ActionChain action10(&twinWheelDrive, &frontArm, perceptionDataAccess, finishAction, "終了処理");
    
    // ActionChainの連結
    action1.setNext(&action2);
    action2.setNext(&action3);
    action3.setNext(&action4);
    action4.setNext(&action5);
    action5.setNext(&action6);
    action6.setNext(&action7);
    action7.setNext(&action8);
    action8.setNext(&action9);
    action9.setNext(&action10);
    
    // ActionChainの実行（効率的なチェーン実行）
    executeActionChainOptimized(&action1);
    
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
    // 方法1を使用（個別実行）
    // main_task_method1(exinf);
    
    // 方法2を使用（チェーン作成 + 手動実行）
    // main_task_method2(exinf);
    
    // 方法3を使用（真のActionChain実行） - 推奨
    main_task_method3(exinf);
    
    //  タスク終了
    ext_tsk();
}