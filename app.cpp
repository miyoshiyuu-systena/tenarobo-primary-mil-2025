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
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "ObstacleCloser.h"
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
    
    bool is_clockwise = true;

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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(760.0f), // 走行距離は仮。実際に計測して決める。なくてもいいかも
                obstacleCloserGenerator(1)          // ペットボトル検知：1[cm]未満になったら次action
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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(760.0f), // カーブまでの走行距離は仮。実際に計測して決める。
                curveCloserGenerator()              // カーブに差し掛かったら次action
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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(50.0f)   // コース外までの走行距離は仮。実際に計測して決める。
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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(50.0f),  // もしかしたら移動距離をマイナスにしないといけないかも
                blackFloorCloserGenerator()         // 黒線に差し掛かったら次action
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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(20.0f)   // もしかしたら移動距離をマイナスにしないといけないかも
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
            100.0f,                                 // 速度は仮。ゆっくりの方がいい？
            50.0f,                                  // 半径は仮。計測して設定
            true,                                   // Lコース、Rコースを決めるパラメータ
            10,
            {},
            {
                straightCloserGenerator(),          // 直線検知
                gateFrontCloserGenerator()          // ゲート検知（距離が近すぎて意味ないかも）
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
            250.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(760.0f), // 走行距離は仮。実際に計測して決める。
                curveCloserGenerator()              // 次カーブ検知で次action
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