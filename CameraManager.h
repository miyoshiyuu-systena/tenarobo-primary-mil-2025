#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

/**
 * カメラ管理クラス
 * メインタスクとカメラタスク間でデータを共有する
 */
class CameraManager {
public:
    /**
     * シングルトンインスタンス取得
     */
    static CameraManager& getInstance();

    /**
     * カメラの初期化
     * @return 初期化成功時true
     */
    bool initializeCamera();

    /**
     * カメラの終了処理
     */
    void shutdownCamera();

    /**
     * 画像を保存
     * @param image 保存する画像
     * @param prefix ファイル名のプレフィックス
     * @return 保存されたファイルのパス
     */
    std::string saveImage(const cv::Mat& image, const std::string& prefix = "");

    /**
     * 最新の画像を取得
     * @param image 取得した画像を格納
     * @return 取得成功時true
     */
    bool getLatestImage(cv::Mat& image);

    /**
     * カメラが初期化されているかチェック
     * @return 初期化済みの場合true
     */
    bool isInitialized() const { return m_initialized.load(); }

    /**
     * カメラタスクを開始
     */
    void startCameraTask();

    /**
     * カメラタスクを停止
     */
    void stopCameraTask();

    /**
     * カメラタスクの実行関数（RTOSタスクから呼び出される）
     */
    void runCameraTask();

private:
    /**
     * プライベートコンストラクタ（シングルトンパターン）
     */
    CameraManager();

    /**
     * コピーコンストラクタを削除
     */
    CameraManager(const CameraManager&) = delete;

    /**
     * 代入演算子を削除
     */
    CameraManager& operator=(const CameraManager&) = delete;

    /**
     * 画像保存用のディレクトリを作成
     */
    void createImageDirectory(const std::string& directory);

    std::atomic<bool> m_initialized;           ///< カメラ初期化フラグ
    std::atomic<bool> m_cameraTaskRunning;     ///< カメラタスク実行フラグ
    cv::VideoCapture m_cap;                    ///< カメラキャプチャ
    cv::Mat m_latestImage;                     ///< 最新の画像
    std::mutex m_imageMutex;                   ///< 画像アクセス用ミューテックス
    std::string m_imageDirectory;              ///< 画像保存ディレクトリ
};

#endif // CAMERA_MANAGER_H 