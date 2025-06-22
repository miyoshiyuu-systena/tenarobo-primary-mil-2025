#include "CameraManager.h"
#include "Logger.h"
#include "spikeapi.h"
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
    , m_imageCount(0) {
    // config.hの設定を使用
    m_imageDirectory = config.getImgFilePath();
    m_imageFileNameSuffix = config.getImgFileNameSuffix();
    
    std::cout << "画像ディレクトリ: " << m_imageDirectory << std::endl;
    std::cout << "画像ファイルサフィックス: " << m_imageFileNameSuffix << std::endl;
}

/**
 * カメラの初期化
 */
bool CameraManager::initializeCamera() {
    loc_cpu();
    if (m_initialized.load()) {
        unl_cpu();
        return true; // 既に初期化済み
    }

    // 複数のカメラデバイスを試行
    for (int i = 0; i < 3; i++) {
        m_cap.open(i);
        
        if (m_cap.isOpened()) {
            // カメラ設定を最小限に
            m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
            m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
            m_cap.set(cv::CAP_PROP_FPS, 30); // フレームレート
            
            // 設定を適用するために少し待機
            usleep(100 * 1000); // 100ms待機
            
            // テストフレームを取得
            cv::Mat testFrame;
            if (m_cap.read(testFrame) && !testFrame.empty()) {
                m_initialized.store(true);
                Logger::getInstance().logInfo("カメラの初期化に成功しました");
                unl_cpu();
                return true;
            } else {
                m_cap.release();
            }
        } else {
        }
    }
    
    Logger::getInstance().logInfo("カメラの初期化に失敗しました");
    unl_cpu();
    return false;
}

/**
 * カメラの終了処理
 */
void CameraManager::shutdownCamera() {
    loc_cpu();
    if (m_initialized.load()) {
        m_cap.release();
        m_initialized.store(false);
        Logger::getInstance().logInfo("カメラを終了しました");
    }
    unl_cpu();
}

/**
 * 画像を保存
 */
std::string CameraManager::saveImage(const cv::Mat& image) {
    createImageDirectory(m_imageDirectory);
    
    std::ostringstream oss;
    oss << m_imageCount << "_" << m_imageFileNameSuffix;
    
    oss << ".jpg";
    
    std::string filename = m_imageDirectory + oss.str();
    
    // 画像を保存
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95); // JPEG品質95%
    
    bool success = cv::imwrite(filename, image, compression_params);
    
    if (success) {
        m_imageCount++;
        return filename;
    } else {
        return "";
    }
}

/**
 * その瞬間の画像を取得
 */
bool CameraManager::captureImageNow(cv::Mat& image) {
    loc_cpu();
    if (!m_initialized.load()) {
        unl_cpu();
        return false;
    }
    // スレッドセーフにVideoCaptureを使う
    if (m_cap.isOpened()) {
        if (m_cap.read(image) && !image.empty()) {
            unl_cpu();
            return true;
        }
    }
    unl_cpu();
    return false;
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

/**
 * 設定を再読み込みして、カメラ設定を更新
 */
void CameraManager::reloadConfig() {
    // 新しい設定値を読み込み
    m_imageDirectory = config.getImgFilePath();
    m_imageFileNameSuffix = config.getImgFileNameSuffix();
    
    // 新しいディレクトリの作成
    createImageDirectory(m_imageDirectory);
    
    Logger::getInstance().logInfo("カメラ設定を再読み込みしました");
    std::cout << "新しい画像ディレクトリ: " << m_imageDirectory << std::endl;
    std::cout << "新しい画像ファイルサフィックス: " << m_imageFileNameSuffix << std::endl;
} 