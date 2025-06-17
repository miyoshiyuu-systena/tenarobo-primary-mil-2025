#include    "app.h"
#include    <stdio.h>
#include    <cstdlib>
#include    <ctime>
#include    <cmath>
#include    <string>
#include    <vector>
#include    <unistd.h>
#include    "spikeapi.h"
#include    "syssvc/syslog.h"
#include    "logging/Logger.h"
#include    "PerceptionTask.h"
#include    "organ/TwinWheelDrive.h"
#include    "action/ActionNode.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"
#include    "action/AroundBottleEdgeAction.h"
#include    "action/StartOnPressureSensorAction.h"
#include    "action/GenerateInfinityWanderAroundAction.h"
#include    <opencv2/opencv.hpp>
#include    <sys/stat.h>
#include    <iomanip>
#include    <sstream>
#include    "CameraManager.h"

using namespace spikeapi;

/**
 * カメラタスクの実装
 * RTOSから周期起動で呼び出される関数
 */
extern "C" void camera_task(intptr_t exinf) {
    CameraManager& cameraManager = CameraManager::getInstance();
    
    // カメラが初期化されている場合のみ処理
    if (cameraManager.isInitialized()) {
        // 1フレーム取得して保存
        cv::Mat frame;
        if (cameraManager.getLatestImage(frame)) {
            // 定期的に画像を保存（10回に1回）
            static int frameCount = 0;
            if (++frameCount % 10 == 0) {
                cameraManager.saveImage(frame, "camera_cycle");
            }
        }
    }
    
    // タスク終了（周期起動なので自動的に再実行される）
    ext_tsk();
}



/**
 * ActionChainを手繰り寄せながら、順繰りに実行する
 */
void execute_and_haul_action_chain(ActionNode* firstAction) {
    Logger& logger = Logger::getInstance();
    ActionNode* currentAction = firstAction;
    ActionNode* previousAction = nullptr;
    
    while (currentAction != nullptr) {
        logger.logInfo("アクション実行中...");
        currentAction->execute();

        if (!currentAction->isEnd()) {
            logger.logError("アクションが終了していないにもかかわらず、次のアクションに移動しました");
            currentAction->deleteNext();
            delete currentAction;
            currentAction = nullptr;
            break;
        }
        
        // 次のアクションに移動
        previousAction = currentAction;
        currentAction = currentAction->getNext();
        delete previousAction;
    }
}

/**
 * ActionChainの処理
 * 生成から実行まで
 * @param   exinf     拡張情報
 */
void    main_task_action_chain(intptr_t exinf)   {  
    /**
     * アクションチェーンの形成
     */
    ActionNode* action0 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        start_on_pressure_sensor_action(3.0f),
        "ボタンが押されるまでハチ公モード！！"
    );

    ActionNode* action1 = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        around_bottle_edge_action(
            250,    // 円弧の半径[mm]
            120,    // 円弧の角度[°]
            3000,   // 円弧を描く時間[ms]
            1000    // 休憩時間[ms]
        ),
        "ペットボトルの周りを半円回るよ"
    );
    action0->setNext(action1);

    // ActionChainの実行
    execute_and_haul_action_chain(action0);
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // カメラマネージャーの初期化
    CameraManager& cameraManager = CameraManager::getInstance();
    bool camera_initialized = false;
    
    // カメラの初期化
    camera_initialized = cameraManager.initializeCamera();
    
    if (camera_initialized) {
        logger.logInfo("カメラが正常に初期化されました");
        
        // カメラタスクを開始
        cameraManager.startCameraTask();
        
        // カメラ周期起動を開始
        sta_cyc(CAMERA_CYC);
        
        // カメラタスクの起動を待機
        usleep(500000); // 500ms待機
        
        // 初期画像を保存
        cv::Mat initialImage;
        if (cameraManager.getLatestImage(initialImage)) {
            std::string savedPath = cameraManager.saveImage(initialImage, "camera_init");
            if (!savedPath.empty()) {
                logger.logInfo("初期画像を保存しました: " + savedPath);
            }
        }
    }
    
    if (!camera_initialized) {
        logger.logWarning("カメラの初期化に失敗しました。カメラなしで動作を続行します。");
        // カメラなしでも動作を続行
    }


    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    // アクション実行前の画像を保存
    if (camera_initialized) {
        cv::Mat preActionFrame;
        if (cameraManager.getLatestImage(preActionFrame)) {
            std::string savedPath = cameraManager.saveImage(preActionFrame, "pre_action");
            if (!savedPath.empty()) {
                logger.logInfo("アクション実行前の画像を保存しました: " + savedPath);
            }
        }
    }

    // アクションチェーンの処理
    main_task_action_chain(0);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // プログラム終了前の画像を保存
    if (camera_initialized) {
        cv::Mat finalFrame;
        if (cameraManager.getLatestImage(finalFrame)) {
            std::string savedPath = cameraManager.saveImage(finalFrame, "program_end");
            if (!savedPath.empty()) {
                logger.logInfo("プログラム終了時の画像を保存しました: " + savedPath);
            }
        }
    }

    // カメラタスクの停止とリソース解放
    if (camera_initialized) {
        // カメラ周期起動を停止
        stp_cyc(CAMERA_CYC);
        
        cameraManager.stopCameraTask();
        cameraManager.shutdownCamera();
        logger.logInfo("カメラリソースを解放しました");
    }

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}