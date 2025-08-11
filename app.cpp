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

    ActionNode* action0 = new ActionNode(
        "action0: Hachikou",
        &device,
        hachikouActionFactory(
            1.0f,
            10
        ),
        0
    );

    // ActionNode* action1 = new ActionNode(
    //     "action1: 右エッジ直進",
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
    //             blueFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action0->setNext(action1);

    // ActionNode* action2 = new ActionNode(
    //     "action2: 右エッジ直進(青)",
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
    //                 calcBlueWhiteBorderError
    //             )
    //         },
    //         {
    //             blackFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action1->setNext(action2);

    // ActionNode* action3 = new ActionNode(
    //     "action3: 右エッジ曲進",
    //     &device,
    //     goCurveActionFactory(
    //         250.0f,
    //         400.0f,
    //         true,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 true,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             ),
    //             slowlyAccelerateAssistGenerator(
    //                 10,
    //                 10
    //             )
    //         },
    //         {
    //             blueFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action2->setNext(action3);

    ActionNode* action4 = new ActionNode(
        "action4: 右エッジ曲進(青)",
        &device,
        goCurveActionFactory(
            250.0f,
            400.0f,
            true,
            10,
            {
                laneTracingAssistGenerator(
                    true,
                    100.0f,
                    1.0f,
                    10.0f,
                    calcBlueWhiteBorderError
                )
            },
            {
                blackFloorCloserGenerator()
            }
        ),
        0
    );
    action0->setNext(action4);

    ActionNode* action4_5 = new ActionNode(
        "action4.5: 停止",
        &device,
        stopActionFactory(),
        1000
    );
    action4->setNext(action4_5);

    ActionNode* action5 = new ActionNode(
        "action5: 車線変更（左）",
        &device,
        simpleLaneChangeActionFactory(false),
        0
    );
    action4_5->setNext(action5);

    ActionNode* action5_5 = new ActionNode(
        "action5.5: 停止",
        &device,
        stopActionFactory(),
        1000
    );
    action5->setNext(action5_5);

    ActionNode* action6 = new ActionNode(
        "action6: 左エッジ曲進",
        &device,
        goCurveActionFactory(
            150.0f,
            200.0f,
            false,
            10,
            {
                laneTracingAssistGenerator(
                    false,
                    100.0f,
                    1.0f,
                    10.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                blueFloorCloserGenerator()
            }
        ),
        0
    );
    action5_5->setNext(action6);

    // ActionNode* action7 = new ActionNode(
    //     "action7: 左エッジ曲進(青)",
    //     &device,
    //     goCurveActionFactory(
    //         150.0f,
    //         200.0f,
    //         false,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 false,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlueWhiteBorderError
    //             )
    //         },
    //         {
    //             blackFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action6->setNext(action7);

    // ActionNode* action8 = new ActionNode(
    //     "action8: 車線変更（右）",
    //     &device,
    //     simpleLaneChangeActionFactory(true),
    //     0
    // );
    // action7->setNext(action8);

    // ActionNode* action9 = new ActionNode(
    //     "action9: 右エッジ曲進",
    //     &device,
    //     goCurveActionFactory(
    //         250.0f,
    //         400.0f,
    //         true,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 true,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             ),
    //             slowlyAccelerateAssistGenerator(
    //                 10,
    //                 10
    //             )
    //         },
    //         {
    //             blueFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action8->setNext(action9);

    // ActionNode* action10 = new ActionNode(
    //     "action10: 右エッジ曲進(青)",
    //     &device,
    //     goCurveActionFactory(
    //         250.0f,
    //         400.0f,
    //         true,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 true,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlueWhiteBorderError
    //             )
    //         },
    //         {
    //             blackFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action9->setNext(action10);

    // ActionNode* action11 = new ActionNode(
    //     "action11: 車線変更（左）",
    //     &device,
    //     simpleLaneChangeActionFactory(false),
    //     0
    // );
    // action10->setNext(action11);

    // ActionNode* action12 = new ActionNode(
    //     "action12: 左エッジ直進",
    //     &device,
    //     goStraightActionFactory(
    //         500.0f,
    //         10,
    //         {
    //             slowlyAccelerateAssistGenerator(
    //                 4,
    //                 10
    //             ),
    //             laneTracingAssistGenerator(
    //                 false,
    //                 100.0f,
    //                 1.0f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             )
    //         },
    //         {
    //             blueFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // action11->setNext(action12);

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