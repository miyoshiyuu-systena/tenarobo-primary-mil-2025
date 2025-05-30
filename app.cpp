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

    while (true) {

        device.twinWheelDrive.leftSpinTurn(90);
        dly_tsk(250 * 1000);
        
        device.twinWheelDrive.rightSpinTurn(90);
        dly_tsk(250 * 1000);

        device.twinWheelDrive.rightSpinTurn(90);
        dly_tsk(250 * 1000);
        
        device.twinWheelDrive.leftSpinTurn(90);
        dly_tsk(250 * 1000);

        device.twinWheelDrive.stop();
        dly_tsk(1000 * 1000);

    }

    // bool is_clockwise = true;

    // ActionNode* root = new ActionNode(
    //     "action_root: 背中のボタンを押すまで忠犬ハチ公！！！",
    //     &device,
    //     hachikouActionFactory(
    //         1.0f,
    //         10
    //     ),
    //     0
    // );

    // ActionNode* action1 = new ActionNode(
    //     "action1: ゲート検知",
    //     &device,
    //     changeDirectionActionFactory(
    //         is_clockwise,
    //         {
    //             gateFrontCloserGenerator()
    //         }
    //     ),
    //     0
    // );
    // root->setNext(action1);

    // ActionNode* current = root;
    // ActionNode* next = nullptr;

    // do {
    //     current->execute();
    //     next = current->getNext();
    //     delete current;
    //     current = next;
    // } while (current != nullptr);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}