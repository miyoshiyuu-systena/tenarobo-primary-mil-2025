#include "app.h"
#include "spikeapi.h"
#include "config.h"
#include "Logger.h"
#include "ActionNode.h"
#include "HachikouAction.h"
#include "GoStraightAction.h"
#include "GoCurveAction.h"
#include "PivotTurnAction.h"
#include "SpinTurnAction.h"
#include "LaneChangeAction.h"
#include "SinpleLaneChangeAction.h"
#include "StopAction.h"
#include "Device.h"
#include "LaneTracingAssist.h"
#include "CalcBlackWhiteBorderError.h"
#include "CalcBlueWhiteBorderError.h"
#include "IAssistGenerator.h"
#include "SlowlyAccelerateAssist.h"
#include "BlueFloorCloser.h"
#include "BlackFloorCloser.h"
#include "StraightCloser.h"
#include "StraightStrictCloser.h"
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "CameraManager.h"

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

    cv::Mat image;
    CameraManager::getInstance().captureImageNow(image);
    CameraManager::getInstance().saveImage(image);
    
    device.frontArmDrive.resetCount();
    device.frontArmDrive.setSpeed(200);
    while (true) {
        if (device.frontArmDrive.getCount() > 60) {
            break;
        }
        dly_tsk(10 * 1000);
    }
    device.frontArmDrive.hold();
    CameraManager::getInstance().captureImageNow(image);
    CameraManager::getInstance().saveImage(image);

    // ActionNode* action0 = new ActionNode(
    //     "action0: Hachikou",
    //     &device,
    //     hachikouActionFactory(
    //         1.0f,
    //         10
    //     ),
    //     0
    // );

    // ActionNode* action1 = new ActionNode(
    //     "action1: 左エッジ直進",
    //     &device,
    //     goStraightActionFactory(
    //         250.0f,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 false,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             )
    //         },
    //         {
    //             timedCloserGenerator(
    //                 200
    //             )
    //         }
    //     ),
    //     0
    // );
    // action0->setNext(action1);

    // ActionNode* action2 = new ActionNode(
    //     "action2: 車線変更",
    //     &device,
    //     simpleLaneChangeActionFactory(true),
    //     0
    // );
    // action1->setNext(action2);

    // ActionNode* action3 = new ActionNode(
    //     "action3: 右エッジ直進",
    //     &device,
    //     goStraightActionFactory(
    //         250.0f,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 true,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             )
    //         },
    //         {
    //             timedCloserGenerator(
    //                 200
    //             )
    //         }
    //     ),
    //     0
    // );
    // action2->setNext(action3);

    // ActionNode* action4 = new ActionNode(
    //     "action4: 止まる",
    //     &device,
    //     stopActionFactory(),
    //     0
    // );
    // action3->setNext(action4);

    // ActionNode* prevAction = nullptr;
    // ActionNode* currentAction = action0;
    // while (currentAction != nullptr) {
    //     currentAction->execute();
    //     prevAction = currentAction;
    //     currentAction = currentAction->getNext();
    //     delete prevAction;
    // }

    // カメラマネージャの終了
    CameraManager::getInstance().shutdownCamera();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}