#include "app.h"
#include "spikeapi.h"
#include "config.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/assist/CalcBlueWhiteBorderError.h"
#include "action-chain/action/assist/IAssistGenerator.h"
#include "action-chain/action/assist/SlowlyAccelerateAssist.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/BlackFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoStraightAction.h"
#include "action-chain/action/GoCurveAction.h"
#include "action-chain/action/StopAction.h"
#include "web-camera/CameraManager.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // 設定ファイルの読み込みと設定情報のログ出力
    std::cout << "=== メインタスク開始 ===" << std::endl;
    
    if (config.loadConfig()) {
        std::cout << "設定ファイルの読み込みに成功しました" << std::endl;
        config.printConfig();  // 設定情報をコンソールに出力
    } else {
        std::cout << "設定ファイルの読み込みに失敗しました。診断情報を表示します。" << std::endl;
        config.printDiagnostics();  // 診断情報を表示
        std::cout << "デフォルト値を使用して続行します。" << std::endl;
        config.printConfig();  // デフォルト値での設定情報を表示
    }

    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    
    // カメラマネージャの起動
    CameraManager::getInstance().initializeCamera();

    ActionNode* action0 = new ActionNode(
        "action0: 圧力センサを押すまで忠犬ハチ公！！",
        &device,
        hachikouActionFactory(
            1.0f,
            10
        ),
        0
    );

    std::vector<IAssistGenerator> assistGenerators1 = {
        laneTracingAssistGenerator(
            false,
            150.0f,
            10.0f,
            10.0f,
            calcBlackWhiteBorderError
        ),
        slowlyAccelerateAssistGenerator(
            10,
            10
        )
    };
    std::vector<ICloserGenerator> closerGenerators1 = {
        blueFloorCloserGenerator()
    };
    ActionNode* action1 = new ActionNode(
        "action1: 白黒の直線に沿って走行し、青色の床に到達したら終了",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            assistGenerators1,
            closerGenerators1
        ),
        0
    );
    action0->setNext(action1);

    // std::vector<IAssistGenerator> assistGenerators2 = {
    //     laneTracingAssistGenerator(
    //         true,
    //         50.0f,
    //         5.0f,
    //         5.0f,
    //         calcBlueWhiteBorderError
    //     ),
    //     slowlyAccelerateAssistGenerator(
    //         10,
    //         1
    //     )
    // };
    // std::vector<ICloserGenerator> closerGenerators2 = {
    //     blackFloorCloserGenerator()
    // };
    // ActionNode* action2 = new ActionNode(
    //     "action2: 白青の直線に沿って走行し、黒い床に到達したら終了",
    //     &device,
    //     goCurveActionFactory(
    //         150.0f,
    //         350.0f,
    //         false,
    //         10,
    //         assistGenerators2,
    //         closerGenerators2
    //     ),
    //     0
    // );
    // action1->setNext(action2);

    // std::vector<IAssistGenerator> assistGenerators3 = {
    //     laneTracingAssistGenerator(
    //         true,
    //         25.0f,
    //         1.0f,
    //         1.0f,
    //         calcBlackWhiteBorderError
    //     ),
    //     slowlyAccelerateAssistGenerator(
    //         100,
    //         1
    //     )
    // };
    // std::vector<ICloserGenerator> closerGenerators3 = {
    //     blueFloorCloserGenerator()
    // };
    // ActionNode* action3 = new ActionNode(
    //     "action3: 白黒の曲線に沿って走行し、青色の床に到達したら終了",
    //     &device,
    //     goCurveActionFactory(
    //         100.0f,
    //         150.0f,
    //         true,
    //         10,
    //         assistGenerators3,
    //         closerGenerators3
    //     ),
    //     0
    // );
    // action2->setNext(action3);

    // ActionNode* action4 = new ActionNode(
    //     "action4: 停止",
    //     &device,
    //     stopActionFactory(),
    //     0
    // );
    // action3->setNext(action4);

    ActionNode* prevAction = nullptr;
    ActionNode* currentAction = action0;
    while (currentAction != nullptr) {
        currentAction->execute();
        prevAction = currentAction;
        currentAction = currentAction->getNext();
        delete prevAction;
    }

    // カメラマネージャの終了
    CameraManager::getInstance().shutdownCamera();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}