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
#include    "action/ActionNode.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"
#include    "action/AroundBottleEdgeAction.h"
#include    "action/StartOnPressureSensorAction.h"
#include    "action/GenerateInfinityWanderAroundAction.h"

using namespace spikeapi;

/**
 * ActionChainを手繰り寄せながら、順繰りに実行する
 */
void execute_and_haul_action_chain(ActionNode* firstAction) {
    Logger& logger = Logger::getInstance();
    ActionNode* currentAction = firstAction;
    ActionNode* previousAction = nullptr;
    
    while (currentAction != nullptr) {
        logger.logInfo("アクション実行中...");
        currentAction->execute();

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
 * ActionChainの処理
 * 生成から実行まで
 * @param   exinf     拡張情報
 */
void    main_task_action_chain(intptr_t exinf)   {  
    /**
     * アクションチェーンの形成
     */
    ActionNode* action0 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        start_on_pressure_sensor_action,
        "ボタンが押されるまでハチ公モード！！"
    );

    ActionNode* action1 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        generate_infinity_wander_around_action,
        "無限にうろうろするよー"
    );
    action0->setNext(action1);

    // ActionChainの実行
    execute_and_haul_action_chain(action0);
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    // アクションチェーンの処理
    twinWheelDrive.curveLeftSpeed(90, 300);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}