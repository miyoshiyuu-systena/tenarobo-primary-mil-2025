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
#include "ChangeDirectionAction.h"
#include "FineChangeDirectionAction.h"
#include "FineChangeDirectionLineAction.h"
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
#include "GateFrontCloser.h"
#include "TargetInImageCloser.h"
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "ObstacleCloser.h"
#include "ObstacleNearCloser.h"
#include "NoObstacleCloser.h"
#include "RunDistanceCloser.h"
#include "OraOraAction.h"
#include "LaneChangeAssist.h"
#include "ImageAnalysisServer.h"

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

    bool is_right = config.isLCourse(); // Lコースの場合ラインの右側、Rコースの場合ラインの左側を走行

    ActionNode* root = new ActionNode(
        "action0: 背中のボタンを押すまで忠犬ハチ公！！！",
        &device,
        hachikouActionFactory(
            1.0f,
            10
        ),
        0
    );

    ActionNode* action1 = new ActionNode(
        "action1: 直線走行",
        &device,
        goStraightActionFactory(
            500.0f,
            50,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    50.0f,
                    0.0f,
                    0.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                obstacleCloserGenerator(80) // 障害物がロボットの筐体にベッタリくっついている状態のとき距離75mm程度になる
            }
        ),
        0
    );

    ActionNode* action2 = new ActionNode(
        "action2: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action3 = new ActionNode(
        "action3: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            50,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    50.0f,
                    0.0f,
                    0.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                curveCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action4 = new ActionNode(
        "action4: 曲がる",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(400)
            }
        ),
        0
    );

    ActionNode* action5 = new ActionNode(
        "action5: 曲がる",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
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

    ActionNode* action6 = new ActionNode(
        "action6: 直進する",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(200)
            }
        ),
        0
    );

    ActionNode* action7 = new ActionNode(
        "action7: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action8 = new ActionNode(
        "action8: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            50,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    100.0f,
                    0.1f,
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

    ActionNode* action9 = new ActionNode(
        "action9: 曲がる",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(400)
            }
        ),
        0
    );

    ActionNode* action10 = new ActionNode(
        "action10: 曲がる",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
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

    ActionNode* action11 = new ActionNode(
        "action11: 直進する",
        &device,
        goStraightActionFactory(
            200.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    200.0f,
                    0.1f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(200)
            }
        ),
        0
    );

    ActionNode* action12 = new ActionNode(
        "action12: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    100.0f,
                    0.1f,
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

    ActionNode* action13 = new ActionNode(
        "action13: 少しだけ前進する",
        &device,
        goStraightActionFactory(
            500.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(40)
            }
        ),
        0
    );

    ActionNode* action14 = new ActionNode(
        "action14: その場で右90度を向く",
        &device,
        pivotTurnActionFactory(
            90.0f,
            is_right,
            10,
            {
                timedCloserGenerator(100)
            }
        ),
        0
    );

    ActionNode* action15 = new ActionNode(
        "action15: 直進する",
        &device,
        goStraightActionFactory(
            750.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(150)
            }
        ),
        0
    );

    ActionNode* action16 = new ActionNode(
        "action16: 後退する",
        &device,
        goStraightActionFactory(
            -250.0f,
            10,
            {},
            {
                blueFloorCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action17 = new ActionNode(
        "action17: その場で左45度を向く",
        &device,
        pivotTurnActionFactory(
            90.0f,
            !is_right,
            10,
            {
                timedCloserGenerator(30)//ペットボトルが無くなった分帰りの方がよく回る
                                        //その滑りの程度は環境に大きく依存するため、後続の追加回転アクションでカバーする
            }
        ),
        0
    );

    ActionNode* action18 = new ActionNode(
        "action18: その場で左に回転して正面に直線を検知する",
        &device,
        fineChangeDirectionLineActionFactory(!is_right),
        0
    );

    ActionNode* action19 = new ActionNode(
        "action19: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    root->setNext(action1);
    action1->setNext(action2);
    action2->setNext(action3);
    action3->setNext(action4);
    action4->setNext(action5);
    action5->setNext(action6);
    action6->setNext(action7);
    action7->setNext(action8);
    action8->setNext(action9);
    action9->setNext(action10);
    action10->setNext(action11);
    action11->setNext(action12);
    action12->setNext(action13);
    action13->setNext(action14);
    action14->setNext(action15);
    action15->setNext(action16);
    action16->setNext(action17);
    action17->setNext(action18);
    action18->setNext(action19);

    ActionNode* current = root;
    ActionNode* next = nullptr;

    do {
        current->execute();
        next = current->getNext();
        delete current;
        current = next;
    } while (current != nullptr);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    // タスク終了
    ext_tsk();
}