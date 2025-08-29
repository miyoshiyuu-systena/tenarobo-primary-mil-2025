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
    float straightSpeed = 250.0f;

    // カーブ時の走行速度
    float curveSpeed = 150.0f;

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
            straightSpeed,
            10,
            {
                laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
                    is_right,                           //線の右縁にそう
                    150.0f,                             //比例ゲイン
                    0.1f,                               //積分ゲイン
                    10.0f,                              //微分ゲイン
                    calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                runDistanceCloserGenerator(3000.0f),    // 大きめに設定（基本的にはペットボトル検知で終了する想定）
                obstacleCloserGenerator(150)            // ペットボトル検知：150[mm]未満になったら次action
            }
        ),
        0
    );

    root->setNext(action1);

    // カーブまでそのまま直進
    ActionNode* action2 = new ActionNode(
        "action2: カーブまでそのまま直進",
        &device,
        goStraightActionFactory(
            straightSpeed,
            10,
            {
                laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
                    is_right,                           //線の右縁にそう
                    150.0f,                             //比例ゲイン
                    0.1f,                               //積分ゲイン
                    10.0f,                              //微分ゲイン
                    calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                runDistanceCloserGenerator(2000.0f),    // カーブまでの走行距離。値は仮。
                curveCloserGenerator()                  // カーブに差し掛かったら次action
            }
        ),
        0
    );
    action1->setNext(action2);

    // コース外までそのまま直進
    ActionNode* action3 = new ActionNode(
        "action3: コース外までそのまま直進",
        &device,
        goStraightActionFactory(
            straightSpeed,
            10,
            {},
            {
                runDistanceCloserGenerator(200.0f)      // カーブからコース外まで200[mm]と想定
            }
        ),
        0
    );
    action2->setNext(action3);

    // 停止（コース外にペットボトルを配置）
    ActionNode* action4 = new ActionNode(
        "action4: 停止（コース外にペットボトルを配置）",
        &device,
        stopActionFactory(),
        0
    );
    action3->setNext(action4);

    // 後進
    ActionNode* action5 = new ActionNode(
        "action5: 後進",
        &device,
        goStraightActionFactory(
            -straightSpeed,
            10,
            {},
            {
                runDistanceCloserGenerator(200.0f),     // カーブからコース外まで前進した距離と同じだけ後進する。
                blackFloorCloserGenerator()             // 黒線に差し掛かったら次action
            }
        ),
        0
    );
    action4->setNext(action5);
    
    // 黒線検知場所から少しだけ後進してコース復帰
    ActionNode* action6 = new ActionNode(
        "action6: 黒線検知場所から少しだけ後進してコース復帰",
        &device,
        goStraightActionFactory(
            -straightSpeed,
            10,
            {},
            {
                runDistanceCloserGenerator(20.0f)       // もしかしたら移動距離をマイナスにしないといけないかも
            }
        ),
        0
    );
    action5->setNext(action6);

    // 停止（コース復帰）
    ActionNode* action7 = new ActionNode(
        "action7: 停止（コース復帰）",
        &device,
        stopActionFactory(),
        0
    );
    action6->setNext(action7);

    // 曲線走行（カーブ侵入）
    ActionNode* action8 = new ActionNode(
        "action8: 曲線走行（カーブ侵入）",
        &device,
        goCurveActionFactory(
            curveSpeed,
            50.0f,                                  // 半径は仮。計測して設定
            is_right,                               // 最初のカーブを曲がる方向
            10,
            {
                laneTracingAssistGenerator(         //足元にガイド線がある場合はそれを活用する
                    is_right,                       //線の右縁にそう
                    100.0f,                         //比例ゲイン
                    0.1f,                           //積分ゲイン
                    10.0f,                          //微分ゲイン
                    calcBlackWhiteBorderError       //誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                straightCloserGenerator()           // 直線検知
            }
        ),
        0
    );
    action7->setNext(action8);

    // 直線走行
    ActionNode* action9 = new ActionNode(
        "action9: 直線走行",
        &device,
        goStraightActionFactory(
            straightSpeed,
            10,
            {
                laneTracingAssistGenerator(             //足元にガイド線がある場合はそれを活用する
                    is_right,                           //線の右縁にそう
                    150.0f,                             //比例ゲイン
                    0.1f,                               //積分ゲイン
                    10.0f,                              //微分ゲイン
                    calcBlackWhiteBorderError           //誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                runDistanceCloserGenerator(3000.0f),    // 走行距離は仮。実際に計測して決める。
                curveCloserGenerator()                  // 次カーブ検知で次action
            }
        ),
        0
    );
    action8->setNext(action9);

    // 二つ目のカーブを検知したら停止する
    ActionNode* action10 = new ActionNode(
        "action10: 停止する",
        &device,
        stopActionFactory(),
        0
    );
    action9->setNext(action10);

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
    
    //  タスク終了
    ext_tsk();
}