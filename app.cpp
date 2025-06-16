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
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"

using namespace spikeapi;

/**
 * 指定時間待機する関数
 * @param ms 待機時間（ミリ秒）
 */
void delay_ms(int ms) {
    dly_tsk(ms * 1000);  // マイクロ秒単位で待機
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

    ActionChain* acttion0 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        parceptionDateAccess,
        start_on_pressure_sensor_action,
        "ボタンが押されるまでハチ公モード！！"
    );
    
    /**
     * アクションチェーンの形成
     */
    ActionChain* action1 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action,
        "壁に激突するまで猪突猛進！！"
    );

    ActionChain* action2 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action,
        "その場でくるりん！！"
    );
    action1->setNext(action2);

    ActionChain* action3 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action,
        "壁に激突するまで猪突猛進！！"
    );
    action2->setNext(action3);

    ActionChain* action4 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action,
        "その場でくるりん！！"
    );
    action3->setNext(action4);

    ActionChain* action5 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action,
        "壁に激突するまで猪突猛進！！"
    );
    action4->setNext(action5);

    ActionChain* action6 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action,
        "その場でくるりん！！"
    );
    action5->setNext(action6);

    ActionChain* action7 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action,
        "壁に激突するまで猪突猛進！！"
    );
    action6->setNext(action7);

    ActionChain* action8 = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action,
        "その場でくるりん！！"
    );
    action7->setNext(action8);

    // ActionChainの実行
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