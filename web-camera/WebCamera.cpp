#include "web-camera/WebCamera.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include <unistd.h>  // usleep()のために追加

WebCamera::WebCamera(): 
    m_isInitialized(false),
    m_savedImageCount(0) 
{}

WebCamera::~WebCamera()
{
    if (m_isInitialized) {
        m_camera.release();
        m_isInitialized = false;
    }
}

void WebCamera::init() {
    std::lock_guard<std::mutex> lock(m_cameraMutex);
    
    /**
     * カメラのソフトウェアアクセスの確立
     * Raspberry Piは、USB経由でビデオの提供を受けているが、USBの数だけ同時に撮影ができるように複数のポートを設けている
     * ここでは複数ポートを操作して、カメラを接続しているものをアクセス先として開く
     */
     for (int i = 0; i < 3; i++) {
        Logger::getInstance().logDebug("WebCamera::init() - try to open camera: " + std::to_string(i));
        m_camera.open(i);
        if (m_camera.isOpened()) {
            m_camera.set(cv::CAP_PROP_FRAME_WIDTH, 320);        // 画像の幅
            m_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);       // 画像の高さ
            m_camera.set(cv::CAP_PROP_FPS, 5);                  // フレームレート
                                                                // 1秒間に取得する画像の枚数（撮影のペース）
            
            usleep(100 * 1000);     // 100ms待機 (POSIXの標準関数を使用)
                                    // 上記設定を適用するためのディレイ
            
            cv::Mat testFrame;
            /**
             * 1度撮影に成功したら、カメラのソフトウェアアクセスの確立に成功したとみなす
             */
            const bool isSuccess = m_camera.read(testFrame) && !testFrame.empty();
            if (isSuccess) {
                m_isInitialized = true;
                Logger::getInstance().logDebug("WebCamera::init() - camera initialized");
                return;
            }

            /**
             * カメラのソフトウェアアクセスの確立に失敗した場合
             * カメラのソフトウェアアクセスを閉じる
             */
            m_camera.release();
        }
    }
    m_isInitialized = false;
}

bool WebCamera::isInitialized() const {
    return m_isInitialized;
}

bool WebCamera::captureImage(cv::Mat& image) {
    Logger::getInstance().logDebug("WebCamera::captureImage() - try to capture image");
    
    std::lock_guard<std::mutex> lock(m_cameraMutex);
    
    if (!m_isInitialized) {
        return false;
    }

    /**
     * カメラから画像を取得する（非ブロッキング処理）
     * grab()でフレームをキャプチャし、retrieve()でデータを取得する
     * これによりRTOSタスクスケジューリングとの競合を軽減
     */
    if (m_camera.grab()) {
        const bool isSuccess = m_camera.retrieve(image) && !image.empty();
        return isSuccess;
    }
    return false;
}
