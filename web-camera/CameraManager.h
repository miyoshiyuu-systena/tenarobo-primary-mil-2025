#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <atomic>
#include <mutex>
#include "../config.h"

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
     * @return 保存されたファイルのパス
     */
    std::string saveImage(const cv::Mat& image);

    /**
     * その瞬間の画像を取得
     * @param image 取得した画像を格納
     * @return 取得成功時true
     */
    bool captureImageNow(cv::Mat& image);

    /**
     * カメラが初期化されているかチェック
     * @return 初期化済みの場合true
     */
    bool isInitialized() const { return m_initialized.load(); }

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
    cv::VideoCapture m_cap;                    ///< カメラキャプチャ
    std::mutex m_imageMutex;                   ///< 画像アクセス用ミューテックス
    std::string m_imageDirectory;              ///< 画像保存ディレクトリ
    std::string m_imageFileNameSuffix;         ///< 画像ファイル名サフィックス
    int m_imageCount;                          ///< 画像カウント
};

#endif // CAMERA_MANAGER_H 