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
     * 
     * @note
     *  カメラを立ち上げたアプリケーションではきちんとクロージングまでしないと、
     *  RTOSがlock_flagエラーを出して止まってしまう。
     *  ↑　制御プログラムを止めても、カメラの使用権を解放していないため、次のプログラムの実行と競合して実行時エラーを出す。
     * 
     *  その上、カメラの使用権を後から解放する方法が確立できていない。次のプログラムが動かず、かなり面倒なことになる・・・。
     *  一応、以下の方法を試したときにカメラの機能が回復することがあった。
     *  おそらくRasPiがカメラとの接続を諦めて、カメラの使用権を解放してくれるのだと思う。
     *  - カメラのUSBを引き抜いて、1分ほど放置する
     *  - カメラのUSBを引き抜いて、別のUSBポートに接続する
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
     * @return 取得成功時true
     */
    bool captureImageNow();

    /**
     * カメラが初期化されているかチェック
     * @return 初期化済みの場合true
     */
    bool isInitialized() const { return m_initialized.load(); }

    /**
     * 一番最近の画像を取得
     * @return 画像
     */
    cv::Mat getLatestImage() const { return m_latestImage; }

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
    cv::Mat m_latestImage;                           ///< 一番最近の画像
};

#endif // CAMERA_MANAGER_H 