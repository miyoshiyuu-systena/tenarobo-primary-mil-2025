#include "app.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/assist/CalcBlueWhiteBorderError.h"
#include "action-chain/action/assist/IAssistGenerator.h"
#include "action-chain/action/assist/SlowlyAccelerateAssist.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/BlackFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoStraightAction.h"
#include "action-chain/action/GoCurveAction.h"
#include "action-chain/action/StopAction.h"
#include "web-camera/CameraManager.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // カメラインスタンスの取得
    CameraManager& cameraManager = CameraManager::getInstance();

    // カメラの初期化
    cameraManager.initializeCamera();

    // カメラの終了処理
    cv::Mat img = cv::imread("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img/19_知覚データを取得する2.jpg");
    cameraManager.saveImage(img);

    cv::Mat img2;
    cv::cvtColor(img, img2, cv::COLOR_BGR2GRAY);
    cameraManager.saveImage(img2);

    cv::Mat img2_2;
    cv::GaussianBlur(img2, img2_2, cv::Size(31, 101), 0);
    cameraManager.saveImage(img2_2);

    cv::Mat img3;
    cv::threshold(img2_2, img3, 100, 255, cv::THRESH_BINARY);
    cameraManager.saveImage(img3);

    cv::Mat img4;
    cv::Canny(img3, img4, 100, 200);

    cameraManager.saveImage(img4);

    // ActionNode* action0 = new ActionNode(
    //     "action0: 圧力センサを押すまで忠犬ハチ公！！",
    //     &device,
    //     hachikouActionFactory(
    //         1.0f,
    //         10
    //     ),
    //     0
    // );

    // std::vector<IAssistGenerator> assistGenerators1 = {
    //     laneTracingAssistGenerator(
    //         &device,
    //         false,
    //         150.0f,
    //         10.0f,
    //         10.0f,
    //         calcBlackWhiteBorderError
    //     ),
    //     slowlyAccelerateAssistGenerator(
    //         &device,
    //         100,
    //         1
    //     )
    // };
    // std::vector<ICloserGenerator> closerGenerators1 = {
    //     blueFloorCloserGenerator(&device)
    // };
    // ActionNode* action1 = new ActionNode(
    //     "action1: 白黒の直線に沿って走行し、青色の床に到達したら終了",
    //     &device,
    //     goCurveActionFactory(
    //         350.0f,
    //         350.0f,
    //         false,
    //         10,
    //         assistGenerators1,
    //         closerGenerators1
    //     ),
    //     0
    // );
    // action0->setNext(action1);

    // std::vector<IAssistGenerator> assistGenerators2 = {
    //     laneTracingAssistGenerator(
    //         &device,
    //         true,
    //         50.0f,
    //         5.0f,
    //         5.0f,
    //         calcBlueWhiteBorderError
    //     ),
    //     slowlyAccelerateAssistGenerator(
    //         &device,
    //         10,
    //         1
    //     )
    // };
    // std::vector<ICloserGenerator> closerGenerators2 = {
    //     blackFloorCloserGenerator(&device)
    // };
    // ActionNode* action2 = new ActionNode(
    //     "action2: 白青の直線に沿って走行し、黒い床に到達したら終了",
    //     &device,
    //     goCurveActionFactory(
    //         150.0f,
    //         350.0f,
    //         false,
    //         10,
    //         assistGenerators2,
    //         closerGenerators2
    //     ),
    //     0
    // );
    // action1->setNext(action2);

    // std::vector<IAssistGenerator> assistGenerators3 = {
    //     laneTracingAssistGenerator(
    //         &device,
    //         true,
    //         25.0f,
    //         1.0f,
    //         1.0f,
    //         calcBlackWhiteBorderError
    //     ),
    //     slowlyAccelerateAssistGenerator(
    //         &device,
    //         100,
    //         1
    //     )
    // };
    // std::vector<ICloserGenerator> closerGenerators3 = {
    //     blueFloorCloserGenerator(&device)
    // };
    // ActionNode* action3 = new ActionNode(
    //     "action3: 白黒の曲線に沿って走行し、青色の床に到達したら終了",
    //     &device,
    //     goCurveActionFactory(
    //         100.0f,
    //         150.0f,
    //         true,
    //         10,
    //         assistGenerators3,
    //         closerGenerators3
    //     ),
    //     0
    // );
    // action2->setNext(action3);

    // ActionNode* action4 = new ActionNode(
    //     "action4: 停止",
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

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}