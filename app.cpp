#include "app.h"
#include "spikeapi.h"
#include "config.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "assist/LaneTracingAssist.h"
#include "assist/CalcBlackWhiteBorderError.h"
#include "assist/CalcBlueWhiteBorderError.h"
#include "assist/IAssistGenerator.h"
#include "assist/SlowlyAccelerateAssist.h"
#include "closer/BlueFloorCloser.h"
#include "closer/BlackFloorCloser.h"
#include "closer/StraightCloser.h"
#include "closer/CurveCloser.h"
#include "closer/TimedCloser.h"
#include "action-chain/HachikouAction.h"
#include "action-chain/GoStraightAction.h"
#include "action-chain/GoCurveAction.h"
#include "action-chain/StopAction.h"
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

    ActionNode* action1 = new ActionNode(
        "action1: 白黒の直線に沿って走行し、曲がり角に到達したら終了",
        &device,
        goStraightActionFactory(
            500.0f,
            10,
            {
                laneTracingAssistGenerator(
                    false,
                    100.0f,
                    0.5f,
                    10.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                curveCloserGenerator()
            }
        ),
        0
    );
    action0->setNext(action1);

    ActionNode* action2 = new ActionNode(
        "action2: 白黒の曲線に沿って徐行し、直線を検知したら終了",
        &device,
        goStraightActionFactory(
            150.0f,
            10,
            {
                laneTracingAssistGenerator(
                    false,
                    50.0f,
                    50.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                straightCloserGenerator()
            }
        ),
        0
    );
    action1->setNext(action2);

    ActionNode * action3 = new ActionNode(
        "action3: とまる",
        &device,
        stopActionFactory(),
        0
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

    // カメラマネージャの終了
    CameraManager::getInstance().shutdownCamera();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}