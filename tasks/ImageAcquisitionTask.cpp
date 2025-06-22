#include "ImageAcquisitionTask.h"
#include "Logger.h"
#include <unistd.h>

// 静的メンバ変数の定義
volatile bool ImageAcquisitionTask::s_shouldStop = false;
volatile bool ImageAcquisitionTask::s_isRunning = false;

/**
 * タスクの実行関数
 */
void ImageAcquisitionTask::run(intptr_t exinf) {
    Logger::getInstance().logInfo("ImageAcquisitionTask: タスク開始");
    
    s_isRunning = true;
    s_shouldStop = false;
    
    cv::VideoCapture cap;
    cv::Mat capturedImage;
    
    // 1. 一番最初に起動した時：カメラの設定
    
    // VideoCaptureをオープン
    bool cameraOpened = false;
    for (int deviceId = 0; deviceId < 3; deviceId++) {
        Logger::getInstance().logInfo("ImageAcquisitionTask: カメラデバイス " + std::to_string(deviceId) + " を試行中");
        
        cap.open(deviceId);
        if (cap.isOpened()) {
            if (configureCamera(cap)) {
                // テストフレームを取得して確認
                cv::Mat testFrame;
                if (cap.read(testFrame) && !testFrame.empty()) {
                    Logger::getInstance().logInfo("ImageAcquisitionTask: カメラデバイス " + std::to_string(deviceId) + " で初期化成功");
                    cameraOpened = true;
                    break;
                } else {
                    Logger::getInstance().logWarning("ImageAcquisitionTask: デバイス " + std::to_string(deviceId) + " でテストフレーム取得失敗");
                    cap.release();
                }
            } else {
                Logger::getInstance().logWarning("ImageAcquisitionTask: デバイス " + std::to_string(deviceId) + " の設定失敗");
                cap.release();
            }
        } else {
            Logger::getInstance().logWarning("ImageAcquisitionTask: デバイス " + std::to_string(deviceId) + " のオープン失敗");
        }
    }
    
    if (!cameraOpened) {
        Logger::getInstance().logError("ImageAcquisitionTask: 利用可能なカメラが見つかりません");
        s_isRunning = false;
        return;
    }
    
    // 2. サイクリックに画像を取得
    Logger::getInstance().logInfo("ImageAcquisitionTask: サイクリック画像取得開始");
    int frameCount = 0;
    
    while (!s_shouldStop) {
        try {
            if (captureImage(cap, capturedImage)) {
                frameCount++;
                
                // TODO: ここで共有バッファに画像を書き込む
                // 現在は取得のみ確認
                if (frameCount % 300 == 0) { // 10秒毎にログ出力（30fps想定）
                    Logger::getInstance().logInfo("ImageAcquisitionTask: " + std::to_string(frameCount) + " フレーム取得完了");
                }
            } else {
                Logger::getInstance().logWarning("ImageAcquisitionTask: フレーム取得失敗");
                
                // エラーが続く場合はカメラを再初期化
                static int consecutiveErrors = 0;
                consecutiveErrors++;
                if (consecutiveErrors >= 10) {
                    Logger::getInstance().logWarning("ImageAcquisitionTask: カメラ再初期化を試行");
                    cap.release();
                    usleep(1000 * 1000); // 1秒待機
                    
                    cap.open(0); // デバイス0で再試行
                    if (cap.isOpened() && configureCamera(cap)) {
                        consecutiveErrors = 0;
                        Logger::getInstance().logInfo("ImageAcquisitionTask: カメラ再初期化成功");
                    } else {
                        Logger::getInstance().logError("ImageAcquisitionTask: カメラ再初期化失敗");
                        break;
                    }
                }
            }
        } catch (const cv::Exception& e) {
            Logger::getInstance().logError("ImageAcquisitionTask: OpenCV例外 - " + std::string(e.what()));
        } catch (const std::exception& e) {
            Logger::getInstance().logError("ImageAcquisitionTask: 例外 - " + std::string(e.what()));
        }
        
        // フレームレート制御
        dly_tsk(FRAME_INTERVAL_MS * 1000); // μs単位
    }
    
    // 3. タスク終了時：リソースの解放
    cleanup(cap);
    
    s_isRunning = false;
    Logger::getInstance().logInfo("ImageAcquisitionTask: タスク終了（総フレーム数: " + std::to_string(frameCount) + "）");
}



/**
 * カメラの設定
 */
bool ImageAcquisitionTask::configureCamera(cv::VideoCapture& cap) {
    Logger::getInstance().logInfo("ImageAcquisitionTask: カメラ設定開始");
    
    try {
        // フレームサイズ設定
        cap.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
        
        // フレームレート設定
        cap.set(cv::CAP_PROP_FPS, CAMERA_FPS);
        
        // バッファサイズ設定（低遅延のため）
        cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
        
        // 設定を適用するために少し待機
        usleep(100 * 1000); // 100ms
        
        // 設定値を確認
        int actualWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        int actualHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        double actualFps = cap.get(cv::CAP_PROP_FPS);
        
        Logger::getInstance().logInfo("ImageAcquisitionTask: 設定完了 - " + 
            std::to_string(actualWidth) + "x" + std::to_string(actualHeight) + 
            " @ " + std::to_string(actualFps) + "fps");
        
        return true;
    } catch (const cv::Exception& e) {
        Logger::getInstance().logError("ImageAcquisitionTask: カメラ設定でOpenCV例外 - " + std::string(e.what()));
        return false;
    }
}

/**
 * リソースの解放
 */
void ImageAcquisitionTask::cleanup(cv::VideoCapture& cap) {
    Logger::getInstance().logInfo("ImageAcquisitionTask: リソース解放開始");
    
    try {
        if (cap.isOpened()) {
            cap.release();
            Logger::getInstance().logInfo("ImageAcquisitionTask: VideoCaptureを解放");
        }
        
        // その他のリソース解放処理があれば追加
        
    } catch (const cv::Exception& e) {
        Logger::getInstance().logError("ImageAcquisitionTask: リソース解放でOpenCV例外 - " + std::string(e.what()));
    }
    
    Logger::getInstance().logInfo("ImageAcquisitionTask: リソース解放完了");
}

/**
 * 画像取得処理
 */
bool ImageAcquisitionTask::captureImage(cv::VideoCapture& cap, cv::Mat& image) {
    if (!cap.isOpened()) {
        return false;
    }
    
    try {
        bool success = cap.read(image);
        return success && !image.empty();
    } catch (const cv::Exception& e) {
        Logger::getInstance().logError("ImageAcquisitionTask: 画像取得でOpenCV例外 - " + std::string(e.what()));
        return false;
    }
}

/**
 * タスクの停止要求
 */
void ImageAcquisitionTask::requestStop() {
    s_shouldStop = true;
    Logger::getInstance().logInfo("ImageAcquisitionTask: 停止要求");
}

/**
 * タスクが実行中かチェック
 */
bool ImageAcquisitionTask::isRunning() {
    return s_isRunning;
}
