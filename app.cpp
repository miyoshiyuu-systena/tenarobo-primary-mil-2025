#include "app.h"
#include "spikeapi.h"
#include "config.h"
#include "Logger.h"
#include "ActionNode.h"
#include "HachikouAction.h"
#include "HachikouColorAction.h"
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

    bool is_right = config.isRCourse();

    Logger::getInstance().logInfo("最初のコーナーを黒い線の外縁でライントレース走行するようにロボットを配置してください。");

    ActionNode* action0 = new ActionNode(
        "action0: 黒い床の上に配置して、背中のボタンを押すと、床の色を取得します",
        &device,
        hachikouColorActionFactory(
            1.0f,
            10
        ),
        1000
    );

    ActionNode* action1 = new ActionNode(
        "action1: 白い床の上に配置して、背中のボタンを押すと、床の色を取得します",
        &device,
        hachikouColorActionFactory(
            1.0f,
            10
        ),
        1000
    );

    ActionNode* action2 = new ActionNode(
        "action2: 青い床の上に配置して、背中のボタンを押すと、床の色を取得します",
        &device,
        hachikouColorActionFactory(
            1.0f,
            10
        ),
        1000
    );

    ActionNode* action3 = new ActionNode(
        "action2: 白と黒の床のちょうど真ん中に配置して、背中のボタンを押すと、床の色を取得します",
        &device,
        hachikouColorActionFactory(
            1.0f,
            10
        ),
        1000
    );

    ActionNode* action4 = new ActionNode(
        "action2: 白と青の床のちょうど真ん中に配置して、背中のボタンを押すと、床の色を取得します",
        &device,
        hachikouColorActionFactory(
            1.0f,
            10
        ),
        1000
    );

    action0->setNext(action1);
    action1->setNext(action2);
    action2->setNext(action3);
    action3->setNext(action4);

    ActionNode* current = action0;
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