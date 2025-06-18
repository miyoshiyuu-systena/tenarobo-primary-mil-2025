#include "app.h"
#include "spikeapi.h"
#include "Logger.h"
#include "ActionNode.h"
#include "Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/closer/BlueFloorCloser.h" 

using namespace spikeapi;

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

    /**
     * ----------------------------
     * アクションチェーンの作成
     * ----------------------------
     */
    ActionNode* actionNode0 = new ActionNode(
        "背中のボタンが押されるまでハチ公モード！！",
        &twinWheelDrive,
        &frontArmDrive,
        &perc,
        hachikouActionFactory(
            1.0f,   // 圧力センサの検知閾値[N]
            1000    // 圧力センサの検知間隔[ms]
        ),
        500
    );

    ActionNode* actionNode1 = new ActionNode(
        "レーントレーシングアシストで白黒の境界に沿って正面を走行し、青床を検出すると停止",
        &twinWheelDrive,
        &frontArmDrive,
        &perc,
        goFrontActionFactory(
            100,   // 速度[mm/s]
            100,  // 検知間隔[ms]
            new LaneTracingAssist(&twinWheelDrive, &frontArmDrive, &perc),
            new BlueFloorCloser(&perc)
        ),
        1000
    );
    actionNode0->setNext(actionNode1);

    actionNode0->execute();
    ActionNode* currentNode = actionNode0;
    while (currentNode != nullptr) {
        currentNode->execute();
        currentNode = currentNode->getNext();
    }

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}