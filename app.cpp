#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    <cmath>
#include    <string>
#include    <vector>
#include    <unistd.h>
#include    "spikeapi.h"
#include    "syssvc/syslog.h"
#include    "logging/Logger.h"
#include    "PerceptionTask.h"
#include    "organ/TwinWheelDrive.h"
#include    "organ/TwinWheelLogic.h"
#include    "action/ActionNode.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"
#include    "action/AroundBottleEdgeAction.h"
#include    "action/StartOnPressureSensorAction.h"
#include    "action/GenerateInfinityWanderAroundAction.h"
#include    "action/LineTraceAction.h"
#include    "action/LineTraceTimedAction.h"
#include    "action/TurnAction.h"
#include    "action/CircleAction.h"
#include    "action/StraightGoAction.h"
#include    "judge/IsOnBlue.h"
#include    "judge/IsOnBlack.h"
#include    "logic/CalcErrorOnBlackWhiteBorder.h"
#include    "logic/CalcErrorOnBlueWhiteBorder.h"
#include    "webcamera/CameraManager.h"

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
        start_on_pressure_sensor_action(3.0f),
        "ボタンが押されるまでハチ公モード！！",
        0
    );

    float speeds[2];
    calcStraightSpeed(600.0f, speeds);
    ActionNode* action1 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_timed_action(
            speeds,
            false,
            3,
            100.0f,
            1.5f,
            0.25f,
            4000,
            calc_error_on_black_white_border
        ),
        "黒直線の左側を走行する、青色の床面に差し当たったら停止する",
        0
    );
    action0->setNext(action1);

    float speeds2[2];
    calcCurveSpeeds(30.0f, true, 350.0f, speeds2);
    ActionNode* action2 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            speeds2,
            false,
            3,
            100.0f,
            10.0f,
            0.25f,
            is_on_blue,
            calc_error_on_black_white_border
        ),
        "黒曲線の左側を走行する、青色の床面に差し当たったら停止する",
        0
    );
    action1->setNext(action2);




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

    // カメラ初期化は知覚タスクで行うため、ここでは削除
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    // アクションチェーンの処理
    main_task_action_chain(0);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}