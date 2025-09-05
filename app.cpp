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

    // 直進時の走行速度
    float straightSpeed = 750.0f;

    // カーブ時の走行速度
    float curveSpeed = 450.0f;

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
                // curveCloserGenerator()
                obstacleCloserGenerator(80)
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
                // straightCloserGenerator(),
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
        "action6: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action7 = new ActionNode(
        "action7: 直進する",
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

    ActionNode* action8 = new ActionNode(
        "action8: 停止する",
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

    // ActionNode* action1 = new ActionNode(
    //     "action1: 直線走行",
    //     &device,
    //     goStraightActionFactory(
    //         straightSpeed,
    //         10,
    //         {
    //             laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
    //                 is_right,                           //線の右縁にそう
    //                 120.0f,                             //比例ゲイン
    //                 0.1f,                               //積分ゲイン
    //                 10.0f,                              //微分ゲイン
    //                 calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
    //             )
    //         },
    //         {
    //             obstacleCloserGenerator(80)            // ペットボトル検知：80[mm]未満になったら次action
    //         }
    //     ),
    //     0
    // );

    // root->setNext(action1);

    // // カーブまでそのまま直進
    // ActionNode* action2 = new ActionNode(
    //     "action2: カーブまでそのまま直進",
    //     &device,
    //     goStraightActionFactory(
    //         straightSpeed,
    //         10,
    //         {
    //             laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
    //                 is_right,                           //線の右縁にそう
    //                 120.0f,                             //比例ゲイン
    //                 0.1f,                               //積分ゲイン
    //                 10.0f,                              //微分ゲイン
    //                 calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
    //             )
    //         },
    //         {
    //             curveCloserGenerator()                  // カーブに差し掛かったら次action
    //         }
    //     ),
    //     0
    // );
    // action1->setNext(action2);

    // // 曲線走行（カーブ侵入）
    // ActionNode* action8 = new ActionNode(
    //     "action8: 曲線走行（カーブ侵入）",
    //     &device,
    //     goCurveActionFactory(
    //         curveSpeed,
    //         155.0f,                                 // 半径
    //         is_right,                               // 最初のカーブを曲がる方向
    //         10,
    //         {
    //             laneTracingAssistGenerator(         //足元にガイド線がある場合はそれを活用する
    //                 is_right,                       //線の右縁にそう
    //                 100.0f,                         //比例ゲイン
    //                 0.1f,                           //積分ゲイン
    //                 10.0f,                          //微分ゲイン
    //                 calcBlackWhiteBorderError       //誤差計算関数(黒い線と白い線の境界を活用する)
    //             )
    //         },
    //         {
    //             straightCloserGenerator()           // 直線検知
    //         }
    //     ),
    //     0
    // );
    // action2->setNext(action8);

    // // 直線走行
    // ActionNode* action9 = new ActionNode(
    //     "action9: 直線走行",
    //     &device,
    //     goStraightActionFactory(
    //         straightSpeed,
    //         10,
    //         {
    //             laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
    //                 is_right,                           //線の右縁にそう
    //                 120.0f,                             //比例ゲイン
    //                 0.1f,                               //積分ゲイン
    //                 10.0f,                              //微分ゲイン
    //                 calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
    //             )
    //         },
    //         {
    //             curveCloserGenerator()                  //曲線検知
    //         }
    //     ),
    //     0
    // );
    // action8->setNext(action9);

    // // 二つ目のカーブを検知したら停止する
    // ActionNode* action10 = new ActionNode(
    //     "action10: 停止する",
    //     &device,
    //     stopActionFactory(),
    //     0
    // );
    // action9->setNext(action10);

    // ActionNode* action28_1 = new ActionNode(
    //     "action28_1: その場で回転して黒い線の方向を向く",
    //     &device,
    //     pivotTurnActionFactory(
    //         45.0f,
    //         is_right,
    //         10,
    //         {
    //             timedCloserGenerator(100)
    //         }
    //     ),
    //     0
    // );

    // ActionNode* action29 = new ActionNode(
    //     "action29: 直進する、黒い線に突き当たるまで",
    //     &device,
    //     goStraightActionFactory(
    //         500.0f,
    //         10,
    //         {},
    //         {
    //             blackFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );

    // ActionNode* action30 = new ActionNode(
    //     "action30: その場で左に回転する、正面に直線を検知するまで",
    //     &device,
    //     pivotTurnActionFactory(
    //         45.0f,
    //         !is_right,
    //         10,
    //         {
    //             straightCloserGenerator()
    //         }
    //     ),
    //     0
    // );

    // ActionNode* action31 = new ActionNode(
    //     "action31: 直線に沿って走行する、青い床に差し掛かるまで",
    //     &device,
    //     goStraightActionFactory(
    //         500.0f,
    //         10,
    //         {
    //             laneTracingAssistGenerator(
    //                 !is_right,
    //                 100.0f,
    //                 0.1f,
    //                 10.0f,
    //                 calcBlackWhiteBorderError
    //             ),
    //             slowlyAccelerateAssistGenerator(
    //                 5,
    //                 10
    //             )
    //         },
    //         {
    //             blueFloorCloserGenerator()
    //         }
    //     ),
    //     0
    // );

    // ActionNode* action32 = new ActionNode(
    //     "action32: 停止する",
    //     &device,
    //     stopActionFactory(),
    //     0
    // );

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