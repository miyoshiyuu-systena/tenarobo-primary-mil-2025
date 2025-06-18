#include "CameraManager.h"
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
CameraManager::CameraManager() 
    : m_initialized(false)
    , m_cameraTaskRunning(false)
    , m_imageDirectory("/home/mil/work/RasPike-ART/sdk/workspace/img/") {
}

/**
 * カメラの初期化
 */
bool CameraManager::initializeCamera() {
    if (m_initialized.load()) {
        return true; // 既に初期化済み
    }

    std::cout << "カメラの初期化を開始..." << std::endl;

    // 複数のカメラデバイスを試行
    for (int i = 0; i < 3; i++) {
        std::cout << "カメラデバイス " << i << " を試行中..." << std::endl;
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
                std::cout << "カメラデバイス " << i << " が正常に初期化されました" << std::endl;
                std::cout << "画像サイズ: " << testFrame.cols << "x" << testFrame.rows << std::endl;
                
                // 最新画像として保存
                {
                    std::lock_guard<std::mutex> lock(m_imageMutex);
                    testFrame.copyTo(m_latestImage);
                }
                
                m_initialized.store(true);
                return true;
            } else {
                std::cout << "カメラデバイス " << i << " からフレームを取得できませんでした" << std::endl;
                m_cap.release();
            }
        } else {
            std::cout << "カメラデバイス " << i << " を開けませんでした" << std::endl;
        }
    }
    
    std::cout << "カメラの初期化に失敗しました" << std::endl;
    return false;
}

/**
 * カメラの終了処理
 */
void CameraManager::shutdownCamera() {
    if (m_initialized.load()) {
        m_cap.release();
        m_initialized.store(false);
        std::cout << "カメラを終了しました" << std::endl;
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
        std::cout << "画像を保存しました: " << filename << std::endl;
        return filename;
    } else {
        std::cerr << "画像の保存に失敗しました: " << filename << std::endl;
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
        std::cout << "カメラタスクを開始しました" << std::endl;
    }
}

/**
 * カメラタスクを停止
 */
void CameraManager::stopCameraTask() {
    m_cameraTaskRunning.store(false);
    std::cout << "カメラタスクを停止しました" << std::endl;
}

/**
 * カメラタスクの実行関数
 */
void CameraManager::runCameraTask() {
    if (!m_initialized.load()) {
        std::cout << "カメラが初期化されていません" << std::endl;
        return;
    }

    std::cout << "カメラタスク開始" << std::endl;
    
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
        usleep(10000); // 10ms待機
    }
    
    std::cout << "カメラタスク終了" << std::endl;
}

/**
 * 画像保存用のディレクトリを作成
 */
void CameraManager::createImageDirectory(const std::string& directory) {
    struct stat st = {0};
    
    if (stat(directory.c_str(), &st) == -1) {
        if (mkdir(directory.c_str(), 0755) == 0) {
            std::cout << "画像ディレクトリを作成しました: " << directory << std::endl;
        } else {
            std::cerr << "画像ディレクトリの作成に失敗しました: " << directory << std::endl;
        }
    }
} 