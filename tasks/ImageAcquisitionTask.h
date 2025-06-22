#ifndef IMAGE_ACQUISITION_TASK_H
#define IMAGE_ACQUISITION_TASK_H

#include <opencv2/opencv.hpp>
#include "spikeapi.h"

/**
 * 画像取得タスク
 * カメラの初期化、サイクリックな画像取得、リソース解放を行う
 */
class ImageAcquisitionTask {
public:
    /**
     * タスクの実行関数（TOPPERS/ASP3から呼び出される）
     * @param exinf 拡張情報
     */
    static void run(intptr_t exinf);

    /**
     * タスクの停止要求
     */
    static void requestStop();

    /**
     * タスクが実行中かチェック
     * @return 実行中の場合true
     */
    static bool isRunning();

private:


    /**
     * カメラの設定
     * @param cap VideoCaptureオブジェクト
     * @return 設定成功時true
     */
    static bool configureCamera(cv::VideoCapture& cap);

    /**
     * リソースの解放
     * @param cap VideoCaptureオブジェクト
     */
    static void cleanup(cv::VideoCapture& cap);

    /**
     * 画像取得処理
     * @param cap VideoCaptureオブジェクト
     * @param image 取得した画像を格納
     * @return 取得成功時true
     */
    static bool captureImage(cv::VideoCapture& cap, cv::Mat& image);

    // タスクの制御フラグ
    static volatile bool s_shouldStop;
    static volatile bool s_isRunning;
    
    // フレームレート制御
    static const int FRAME_INTERVAL_MS = 33;  // 約30fps
    
    // カメラ設定
    static const int CAMERA_WIDTH = 320;
    static const int CAMERA_HEIGHT = 240;
    static const int CAMERA_FPS = 30;
};

#endif // IMAGE_ACQUISITION_TASK_H
