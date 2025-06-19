#include "app.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/assist/CalcBlueWhiteBorderError.h"
#include "action-chain/action/assist/IAssistGenerator.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/BlackFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoStraightAction.h"
#include "action-chain/action/GoCurveAction.h"
#include "action-chain/action/StopAction.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    ActionNode* action0 = new ActionNode(
        "action0: 圧力センサを押すまで忠犬ハチ公！！",
        &device,
        hachikouActionFactory(
            1.0f,
            10
        ),
        0
    );

    IAssistGenerator assist1 = laneTracingAssistGenerator(
        &device,
        true,
        150.0f,
        10.0f,
        10.0f,
        calcBlackWhiteBorderError
    );
    std::vector<IAssistGenerator> assistGenerators1 = {
        assist1
    };
    ICloserGenerator closer1 = blueFloorCloserGenerator(&device);
    std::vector<ICloserGenerator> closerGenerators1 = {
        closer1
    };
    ActionNode* action1 = new ActionNode(
        "action1: 白黒の直線に沿って走行し、青色の床に到達したら終了",
        &device,
        goStraightActionFactory(
            150,
            10,
            assistGenerators1,
            closerGenerators1
        ),
        0
    );
    action0->setNext(action1);

    IAssistGenerator assist2 = laneTracingAssistGenerator(
        &device,
        true,
        150.0f,
        10.0f,
        10.0f,
        calcBlueWhiteBorderError
    );
    std::vector<IAssistGenerator> assistGenerators2 = {
        assist2
    };
    ICloserGenerator closer2 = blackFloorCloserGenerator(&device);
    std::vector<ICloserGenerator> closerGenerators2 = {
        closer2
    };
    ActionNode* action2 = new ActionNode(
        "action2: 白青の直線に沿って走行し、黒い床に到達したら終了",
        &device,
        goStraightActionFactory(
            150,
            10,
            assistGenerators2,
            closerGenerators2
        ),
        0
    );
    action1->setNext(action2);

    ActionNode* action3 = new ActionNode(
        "action3: 停止",
        &device,
        stopActionFactory(),
        1000
    );
    action2->setNext(action3);

    ActionNode* prevAction = nullptr;
    ActionNode* currentAction = action0;
    while (currentAction != nullptr) {
        currentAction->execute();
        prevAction = currentAction;
        currentAction = currentAction->getNext();
        delete prevAction;
    }

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}