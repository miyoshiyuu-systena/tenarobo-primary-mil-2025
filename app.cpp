#include "app.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoStraightAction.h"
#include "perception/Perception.h"

using namespace spikeapi;

/**
 * 知覚タスク
 * @param exinf 拡張情報
 */
void perc_task(intptr_t exinf) {
    perception.update();
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // カメラ初期化は知覚タスクで行うため、ここでは削除
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    /**
     * ----------------------------
     * アクションチェーンの作成
     * ----------------------------
     */
    ActionNode* actionNode0 = new ActionNode(
        "背中のボタンが押されるまでハチ公モード！！",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00010000,
        hachikouActionFactory(
            1.0f,   // 圧力センサの検知閾値[N]
            100    // 圧力センサの検知間隔[ms]
        ),
        1000
    );

    IAssist* assist1 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        50.0f,
        25.0f,
        0.5f,
        calcBlackWhiteBorderError
    );
    ICloser* closer1 = new TimedCloser(&perception, 800);
    ActionNode* actionNode1 = new ActionNode(
        "白黒の境界(右縁)に沿って正面を走行し、8秒後に停止する",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goStraightActionFactory(
            500.0f,   // 速度[mm/s]
            10,  // 検知間隔[ms]
            assist1,
            closer1
        ),
        100
    );
    actionNode0->setNext(actionNode1);

    IAssist* assist2 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        25.0f,
        1.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer2 = new TimedCloser(&perception, 800);
    ActionNode* actionNode2 = new ActionNode(
        "白黒の境界(右縁)に沿って右まがりに走行し、8秒後に停止する",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            100.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist2,
            closer2
        ),
        100
    );
    actionNode1->setNext(actionNode2);

    IAssist* assist3 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        50.0f,
        25.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer3 = new BlueFloorCloser(&perception);
    ActionNode* actionNode3 = new ActionNode(
        "白黒の境界(右縁)に沿って正面を走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goStraightActionFactory(
            500.0f,   // 速度[mm/s]
            10,  // 検知間隔[ms]
            assist3,
            closer3
        ),
        100
    );
    actionNode2->setNext(actionNode3);

    IAssist* assist4 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        25.0f,
        1.0f,
        0.5f,
        calcBlueWhiteBorderError
    );
    ICloser* closer4 = new BlackFloorCloser(&perception);
    ActionNode* actionNode4 = new ActionNode(
        "白青の境界(右縁)に沿って右まがりに走行し、黒床を検出すると停止(ダブルループ開始)",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            400.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist4,
            closer4
        ),
        100
    );
    actionNode3->setNext(actionNode4);

    IAssist* assist5 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        50.0f,
        25.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer5 = new BlueFloorCloser(&perception);
    ActionNode* actionNode5 = new ActionNode(
        "白黒の境界(右縁)に沿って右まがりに走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            500.0f,   // 速度[mm/s]
            400.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist5,
            closer5
        ),
        100
    );
    actionNode4->setNext(actionNode5);

    IAssist* assist6 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        false,
        25.0f,
        1.0f,
        0.5f,
        calcBlueWhiteBorderError
    );
    ICloser* closer6 = new BlackFloorCloser(&perception);
    ActionNode* actionNode6 = new ActionNode(
        "白青の境界(左縁)に沿って左まがりに走行し、黒床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            200.0f,   // 半径[mm]
            false,
            10,  // 検知間隔[ms]
            assist6,
            closer6
        ),
        100
    );
    actionNode5->setNext(actionNode6);

    IAssist* assist7 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        false,
        25.0f,
        1.0f,
        0.5f,
        calcBlackWhiteBorderError
    );
    ICloser* closer7 = new BlueFloorCloser(&perception);
    ActionNode* actionNode7 = new ActionNode(
        "白黒の境界(左縁)に沿って左まがりに走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            250.0f,   // 速度[mm/s]
            200.0f,   // 半径[mm]
            false,
            10,  // 検知間隔[ms]
            assist7,
            closer7
        ),
        100
    );
    actionNode6->setNext(actionNode7);

    IAssist* assist8 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        25.0f,
        1.0f,
        0.5f,
        calcBlueWhiteBorderError
    );
    ICloser* closer8 = new BlackFloorCloser(&perception);
    ActionNode* actionNode8 = new ActionNode(
        "白青の境界(右縁)に沿って右まがりに走行し、黒床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            400.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist8,
            closer8
        ),
        100
    );
    actionNode7->setNext(actionNode8);

    IAssist* assist9 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        50.0f,
        25.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer9 = new BlueFloorCloser(&perception);
    ActionNode* actionNode9 = new ActionNode(
        "白黒の境界(右縁)に沿って右まがりに走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            500.0f,   // 速度[mm/s]
            400.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist9,
            closer9
        ),
        100
    );
    actionNode8->setNext(actionNode9);

    IAssist* assist10 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        false,
        25.0f,
        1.0f,
        0.5f,
        calcBlueWhiteBorderError
    );
    ICloser* closer10 = new BlackFloorCloser(&perception);
    ActionNode* actionNode10 = new ActionNode(
        "白青の境界(左縁)に沿って右まがりに走行し、黒床を検出すると停止（ダブルループ終了）",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            200.0f,   // 半径[mm]
            false,
            10,  // 検知間隔[ms]
            assist10,
            closer10
        ),
        100
    );
    actionNode9->setNext(actionNode10);

    IAssist* assist11 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        false,
        50.0f,
        25.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer11 = new TimedCloser(&perception, 200);
    ActionNode* actionNode11 = new ActionNode(
        "白黒の境界(左縁)に沿って正面を走行し、2秒後に停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goStraightActionFactory(
            500.0f,   // 速度[mm/s]
            10,  // 検知間隔[ms]
            assist11,
            closer11
        ),
        100
    );
    actionNode10->setNext(actionNode11);

    IAssist* assist12 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        25.0f,
        1.0f,
        0.5f,
        calcBlackWhiteBorderError
    );
    ICloser* closer12 = new TimedCloser(&perception, 800);
    ActionNode* actionNode12 = new ActionNode(
        "白黒の境界(左縁)に沿って右まがりに走行し、8秒後に停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goCurveActionFactory(
            100.0f,   // 速度[mm/s]
            100.0f,   // 半径[mm]
            true,
            10,  // 検知間隔[ms]
            assist12,
            closer12
        ),
        100
    );
    actionNode11->setNext(actionNode12);

    IAssist* assist13 = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        50.0f,
        25.0f,
        0.05f,
        calcBlackWhiteBorderError
    );
    ICloser* closer13 = new BlueFloorCloser(&perception);
    ActionNode* actionNode13 = new ActionNode(
        "白黒の境界(左縁)に沿って正面を走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goStraightActionFactory(
            500.0f,   // 速度[mm/s]
            10,  // 検知間隔[ms]
            assist13,
            closer13
        ),
        100
    );
    actionNode12->setNext(actionNode13);


    /** 
     * ----------------------------
     * アクションチェーンの実行
     * ----------------------------
     */
    ActionNode* currentNode = actionNode0;
    ActionNode* previousNode = nullptr;
    while (currentNode != nullptr) {
        currentNode->execute();
        previousNode = currentNode;
        currentNode = currentNode->getNext();
        delete previousNode;
    }

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}