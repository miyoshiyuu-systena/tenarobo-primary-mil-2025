#include "CameraManager.h"
#include "logging/Logger.h"
#include <iostream>
#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <unistd.h>

/**
 * シングルトンインスタンス取得
 */
CameraManager& CameraManager::getInstance() {
    static CameraManager instance;
    return instance;
}

/**
 * プライベートコンストラクタ
 */
/**
 * XXX: 保存する画像のサフィックスを出力できないか
 */
CameraManager::CameraManager() 
    : m_initialized(false)
    , m_cameraTaskRunning(false)
    /**
     * XXX: コンストラクタで注入できないか
     */
    , m_imageDirectory("/home/mil/work/RasPike-ART/sdk/workspace/img/") {}

/**
 * カメラの初期化
 */
bool CameraManager::initializeCamera() {
    if (m_initialized.load()) {
        return true; // 既に初期化済み
    }

    Logger::getInstance().logInfo("カメラの初期化を開始...");

    // 複数のカメラデバイスを試行
    for (int i = 0; i < 3; i++) {
        Logger::getInstance().logInfo("カメラデバイス " + std::to_string(i) + " を試行中...");
        m_cap.open(i);
        
        if (m_cap.isOpened()) {
            // カメラ設定を最小限に
            m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
            m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
            m_cap.set(cv::CAP_PROP_FPS, 5);  // 低フレームレート
            
            // 設定を適用するために少し待機
            usleep(100000); // 100ms待機
            
            // テストフレームを取得
            cv::Mat testFrame;
            if (m_cap.read(testFrame) && !testFrame.empty()) {
                Logger::getInstance().logInfo("カメラデバイス " + std::to_string(i) + " が正常に初期化されました");
                Logger::getInstance().logInfo("画像サイズ: " + std::to_string(testFrame.cols) + "x" + std::to_string(testFrame.rows));
                
                // 最新画像として保存
                {
                    std::lock_guard<std::mutex> lock(m_imageMutex);
                    testFrame.copyTo(m_latestImage);
                }
                
                m_initialized.store(true);
                return true;
            } else {
                Logger::getInstance().logInfo("カメラデバイス " + std::to_string(i) + " からフレームを取得できませんでした");
                m_cap.release();
            }
        } else {
            Logger::getInstance().logInfo("カメラデバイス " + std::to_string(i) + " を開けませんでした");
        }
    }
    
    Logger::getInstance().logInfo("カメラの初期化に失敗しました");
    return false;
}

/**
 * カメラの終了処理
 */
void CameraManager::shutdownCamera() {
    if (m_initialized.load()) {
        m_cap.release();
        m_initialized.store(false);
        Logger::getInstance().logInfo("カメラを終了しました");
    }
}

/**
 * 画像を保存
 */
std::string CameraManager::saveImage(const cv::Mat& image, const std::string& prefix) {
    createImageDirectory(m_imageDirectory);
    
    // ファイル名を生成（日時 + プレフィックス）
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y%m%d-%H%M%S");
    
    if (!prefix.empty()) {
        oss << "-" << prefix;
    }
    
    oss << ".jpg";
    
    std::string filename = m_imageDirectory + oss.str();
    
    // 画像を保存
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95); // JPEG品質95%
    
    bool success = cv::imwrite(filename, image, compression_params);
    
    if (success) {
        Logger::getInstance().logInfo("画像を保存しました: " + filename);
        return filename;
    } else {
        Logger::getInstance().logError("画像の保存に失敗しました: " + filename);
        return "";
    }
}

/**
 * 最新の画像を取得
 */
bool CameraManager::getLatestImage(cv::Mat& image) {
    if (!m_initialized.load()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_imageMutex);
    if (!m_latestImage.empty()) {
        m_latestImage.copyTo(image);
        return true;
    }
    
    return false;
}

/**
 * その瞬間の画像を取得
 */
bool CameraManager::captureImageNow(cv::Mat& image) {
    if (!m_initialized.load()) {
        return false;
    }
    // スレッドセーフにVideoCaptureを使う
    std::lock_guard<std::mutex> lock(m_imageMutex);
    if (m_cap.isOpened()) {
        if (m_cap.read(image) && !image.empty()) {
            return true;
        }
    }
    return false;
}

/**
 * カメラタスクを開始
 */
void CameraManager::startCameraTask() {
    if (!m_cameraTaskRunning.load()) {
        m_cameraTaskRunning.store(true);
        Logger::getInstance().logInfo("カメラタスクを開始しました");
    }
}

/**
 * カメラタスクを停止
 */
void CameraManager::stopCameraTask() {
    m_cameraTaskRunning.store(false);
    Logger::getInstance().logInfo("カメラタスクを停止しました");
}

/**
 * カメラタスクの実行関数
 */
void CameraManager::runCameraTask() {
    if (!m_initialized.load()) {
        Logger::getInstance().logError("カメラが初期化されていません");
        return;
    }

    Logger::getInstance().logInfo("カメラタスク開始");
    
    while (m_cameraTaskRunning.load()) {
        // 非ブロッキングでフレームを取得
        if (m_cap.grab()) {
            cv::Mat frame;
            if (m_cap.retrieve(frame) && !frame.empty()) {
                // 最新画像を更新
                {
                    std::lock_guard<std::mutex> lock(m_imageMutex);
                    frame.copyTo(m_latestImage);
                }
                
                // 定期的に画像を保存（10フレームごと）
                static int frameCount = 0;
                if (++frameCount % 10 == 0) {
                    saveImage(frame, "camera_task");
                }
            }
        }
        
        // RTOSの他のタスクにCPU時間を譲る
        /**
         * XXX: RTOSのインターフェースを使用できないかdly_tsk
         */
        usleep(10000); // 10ms待機
    }
    
    Logger::getInstance().logInfo("カメラタスク終了");
}

/**
 * 画像保存用のディレクトリを作成
 */
void CameraManager::createImageDirectory(const std::string& directory) {
    struct stat st = {0};
    
    if (stat(directory.c_str(), &st) == -1) {
        if (mkdir(directory.c_str(), 0755) == 0) {
            Logger::getInstance().logInfo("画像ディレクトリを作成しました: " + directory);
        } else {
            Logger::getInstance().logError("画像ディレクトリの作成に失敗しました: " + directory);
        }
    }
} 