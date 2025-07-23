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

    IAssist* assist = new LaneTracingAssist(
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        true,
        0.0f,
        0.0f,
        0.0f,
        calcBlackWhiteBorderError
    );
    ICloser* closer = new TimedCloser(&perception, 300);
    ActionNode* actionNode1 = new ActionNode(
        "レーントレーシングアシストで白黒の境界(右縁)に沿って正面を走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perception,
        0b00100000,
        goStraightActionFactory(
            750,   // 速度[mm/s]
            10,  // 検知間隔[ms]
            assist,
            closer
        ),
        100
    );
    actionNode0->setNext(actionNode1);

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