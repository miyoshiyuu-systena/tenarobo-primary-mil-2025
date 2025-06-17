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

using namespace spikeapi;

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

    // カメラの初期化
    cv::VideoCapture cap;
    
    // カメラデバイスの権限確認
    logger.logInfo("カメラデバイスの権限を確認中...");
    system("ls -la /dev/video*");
    system("groups $USER");
    
    // 複数のカメラデバイスを試行（V4L2バックエンドを明示的に指定）
    int camera_index = 0;
    bool camera_opened = false;
    
    // まず、デバイスパスを直接指定して試行
    std::vector<std::string> device_paths = {
        "/dev/video0",
        "/dev/video1", 
        "/dev/video2",
        "/dev/video3"
    };
    
    for (const auto& device_path : device_paths) {
        logger.logInfo("カメラデバイス " + device_path + " を試行中...");
        cap.open(device_path, cv::CAP_V4L2);
        if (cap.isOpened()) {
            camera_opened = true;
            logger.logInfo("カメラデバイス " + device_path + " が正常に開かれました");
            break;
        } else {
            logger.logError("カメラデバイス " + device_path + " が開けませんでした");
        }
    }
    
    // デバイスパスで失敗した場合、インデックスで試行
    if (!camera_opened) {
        logger.logInfo("デバイスパスでの試行が失敗しました。インデックスで試行します。");
        while (camera_index < 4 && !camera_opened) {
            logger.logInfo("カメラデバイス " + std::to_string(camera_index) + " を試行中...");
            // V4L2バックエンドを明示的に指定
            cap.open(camera_index, cv::CAP_V4L2);
            if (cap.isOpened()) {
                camera_opened = true;
                logger.logInfo("カメラデバイス " + std::to_string(camera_index) + " が正常に開かれました");
            } else {
                logger.logError("カメラデバイス " + std::to_string(camera_index) + " が開けませんでした");
                camera_index++;
            }
        }
    }
    
    if (!camera_opened) {
        logger.logError("利用可能なカメラが見つかりませんでした");
        // カメラが使えない場合でも他の処理は続行
    } else {
        // カメラ設定
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
        cap.set(cv::CAP_PROP_FPS, 10);
        
        // カメラ設定の確認
        double width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        double height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        double fps = cap.get(cv::CAP_PROP_FPS);
        logger.logInfo("カメラ設定 - 幅: " + std::to_string((int)width) + 
                      ", 高さ: " + std::to_string((int)height) + 
                      ", FPS: " + std::to_string((int)fps));
        
        // カメラの詳細情報を確認
        double brightness = cap.get(cv::CAP_PROP_BRIGHTNESS);
        double contrast = cap.get(cv::CAP_PROP_CONTRAST);
        logger.logInfo("カメラ詳細 - 明度: " + std::to_string(brightness) + 
                      ", コントラスト: " + std::to_string(contrast));
        
        // カメラ初期化後の待機時間
        logger.logInfo("カメラ初期化後の待機中...");
        dly_tsk(1 * 1000 * 1000); // 1秒待機
        
        logger.logInfo("カメラ処理を開始します");
        
        cv::Mat frame;
        int frame_count = 0;
        const int max_frames = 100; // 最大フレーム数（無限ループを防ぐ）
        
        // 複数のバックエンドで試行
        std::vector<int> backends = {cv::CAP_V4L2, cv::CAP_V4L};
        bool frame_read_success = false;
        
        for (int backend : backends) {
            if (frame_read_success) break;
            
            logger.logInfo("バックエンド " + std::to_string(backend) + " でフレーム読み取りを試行中...");
            
            // カメラを再初期化
            cap.release();
            cap.open("/dev/video0", backend);
            
            if (!cap.isOpened()) {
                logger.logError("バックエンド " + std::to_string(backend) + " でカメラを開けませんでした");
                continue;
            }
            
            // 設定を再適用
            cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
            cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
            cap.set(cv::CAP_PROP_FPS, 10);
            
            // 短い待機
            dly_tsk(500 * 1000); // 500ms待機
            
            // フレーム読み取りを試行
            bool read_result = cap.read(frame);
            logger.logInfo("バックエンド " + std::to_string(backend) + " での読み取り結果: " + 
                          std::string(read_result ? "成功" : "失敗"));
            
            if (read_result && !frame.empty()) {
                frame_read_success = true;
                logger.logInfo("フレームサイズ: " + std::to_string(frame.cols) + "x" + std::to_string(frame.rows));
                break;
            }
        }
        
        if (!frame_read_success) {
            logger.logError("すべてのバックエンドでフレーム読み取りに失敗しました");
            cap.release();
        } else {
            // 成功した場合は通常のループを実行
            while (frame_count < max_frames && cap.read(frame)) {
                if (frame.empty()) {
                    logger.logError("フレームが空です");
                    break;
                }
                
                logger.logInfo("フレーム " + std::to_string(frame_count) + " を読み取りました");
                
                // 画像処理の例（グレースケール変換）
                cv::Mat gray_frame;
                cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
                
                // 画像を保存（デバッグ用）
                if (frame_count % 30 == 0) { // 30フレームごとに保存
                    std::string filename = "frame_" + std::to_string(frame_count) + ".png";
                    cv::imwrite(filename, frame);
                    logger.logInfo("フレーム " + std::to_string(frame_count) + " を保存しました: " + filename);
                }
                
                frame_count++;
                
                // 短い待機時間（100ms）
                dly_tsk(100 * 1000);
            }
        }
        
        logger.logInfo("カメラ処理を終了しました。処理したフレーム数: " + std::to_string(frame_count));
        cap.release();
    }

    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    // アクションチェーンの処理
    main_task_action_chain(0);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}