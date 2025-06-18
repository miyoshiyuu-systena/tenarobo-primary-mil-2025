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
#include    "action/LineTraceAction.h"

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
        "ボタンが押されるまでハチ公モード！！"
    );

    ActionNode* action1 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            250,// 速度[mm/s]
            true,// 右寄りか左寄りか
            5,// 判定周期[ms]
            125.0f,// Kp（比例係数）
            10.0f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_blue_line,
            calc_error_on_black_white_border
        ),
        "黒いラインの右側を走行し、青い線が見つかったら終了"
    );
    action0->setNext(action1);

    ActionNode* action2 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            false,// 右寄りか左寄りか
            5,// 判定周期[ms]
            75.0f,// Kp（比例係数）
            2.5f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_black_line,
            calc_error_on_blue_white_border
        ),
        "青いラインの右側を走行し、黒い線が見つかったら終了"
    );
    action1->setNext(action2);

    ActionNode* action3 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            250,// 速度[mm/s]
            false,// 右寄りか左寄りか
            5,// 判定周期[ms]
            125.0f,// Kp（比例係数）
            10.0f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_blue_line,
            calc_error_on_black_white_border
        ),
        "黒いラインの左側を走行し、青い線が見つかったら終了"
    );
    action2->setNext(action3);

    ActionNode* action4 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            true,// 右寄りか左寄りか
            5,// 判定周期[ms]
            75.0f,// Kp（比例係数）
            2.5f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_black_line,
            calc_error_on_blue_white_border
        ),
        "青いラインの右側を走行し、黒い線が見つかったら終了"
    );
    action3->setNext(action4);

    ActionNode* action5 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            true,// 右寄りか左寄りか
            3,// 判定周期[ms]
            100.0f,// Kp（比例係数）
            25.0f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_blue_line,
            calc_error_on_black_white_border
        ),
        "黒いラインの右側を走行し、青い線が見つかったら終了(ここから難所)"
    );
    action4->setNext(action5);

    ActionNode* action6 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            false,// 右寄りか左寄りか
            5,// 判定周期[ms]
            75.0f,// Kp（比例係数）
            2.5f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_black_line,
            calc_error_on_blue_white_border
        ),
        "青いラインの左側を走行し、黒い線が見つかったら終了"
    );
    action5->setNext(action6);

    ActionNode* action7 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            false,// 右寄りか左寄りか
            5,// 判定周期[ms]
            100.0f,// Kp（比例係数）
            25.0f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_blue_line,
            calc_error_on_black_white_border
        ),
        "黒いラインの左側を走行し、青い線が見つかったら終了(ここも難所)"
    );
    action6->setNext(action7);

    ActionNode* action8 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            150,// 速度[mm/s]
            true,// 右寄りか左寄りか
            5,// 判定周期[ms]
            75.0f,// Kp（比例係数）
            2.5f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_black_line,
            calc_error_on_blue_white_border
        ),
        "青いラインの右側を走行し、黒い線が見つかったら終了"
    );
    action7->setNext(action8);

    ActionNode* action9 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        line_trace_action(
            250,// 速度[mm/s]
            true,// 右寄りか左寄りか
            5,// 判定周期[ms]
            125.0f,// Kp（比例係数）
            10.0f,// Ki（積分係数）
            10.0f,// Kd（微分係数）
            is_on_blue_line,
            calc_error_on_black_white_border
        ),
        "黒いラインの右側を走行し、青い線が見つかったら終了"
    );
    action8->setNext(action9);

    // ダブルループのシナリオ
    // "黒い線の右側をライントレースして、青い線が見つかったら終了"
    // "青い線の右側をライントレースして、黒い線が見つかったら終了"
    // "右方向に回転する"
    // "旋回走行する、何メートル走ったら終了"
    // "直線走行する、黒い線が見つかったら終了"
    // "黒い線の右側をライントレースして、青い線が見つかったら終了"
    // "右方向に回転する"
    // "直進走行する、何メートル以上走って、かつ黒い線を見つける"
    // "黒い線の左側をライントレースして、青い線が見つかったら終了"
    // "左方向に回転する"
    // "直進走行する、なんメートル以上走って、かつ黒い線を見つける"
    // "黒い線の右側をライントレースする"
    // "右方向に回転する"
    // "直進走行する、なんメートル以上走って、かつ黒い線を見つける"
    // "黒い線の右側をライントレースする"


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
    main_task_action_chain(0);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}