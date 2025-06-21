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
#include "WhiteFloorAndStraightStrictCloser.h"
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "CameraManager.h"

#include "PerceptionMask.h"
#include "PerceptionReporter.h"

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

    PerceptionReporter::getInstance().update(10, PERCEPTION_REPORT_MASK_IMAGE);
    ICloser* closer = onRightEdgeCloserGenerator()();
    bool isClosed = closer->isClosed();
    std::cout << "isClosed: " << isClosed << std::endl;

    // ActionNode* action0 = new ActionNode(
    //     "action0: 圧力センサを押すまで忠犬ハチ公！！",
    //     &device,
    //     hachikouActionFactory(
    //         1.0f,
    //         10
    //     ),
    //     0
    // );

    // ActionNode* action1 = new ActionNode(
    //     "action1: 車線変更",
    //     &device,
    //     laneChangeActionFactory(
    //         true,
    //         {}
    //     ),
    //     0
    // );
    // action0->setNext(action1);
    
    // ActionNode* action2 = new ActionNode(
    //     "action2: 車線変更",
    //     &device,
    //     laneChangeActionFactory(
    //         false,
    //         {
    //             whiteFloorAndStraightStrictGenerator()
    //         }
    //     ),
    //     0
    // );
    // action1->setNext(action2);

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