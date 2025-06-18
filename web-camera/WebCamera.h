#ifndef WEB_CAMERA_H
#define WEB_CAMERA_H

#include <opencv2/opencv.hpp>
#include <string>

/**
 * Webカメラ
 */
class WebCamera {
    public:
        /**
         * コンストラクタ
         * 初期化そのもの
         */
        WebCamera(
            /**
             * 画像を保存するディレクトリ
             */
            const std::string& imageDirectory,

            /**
             * 画像のサフィックス
             */
            const std::string& imageSuffix
        );

        /**
         * デストラクタ
         * シャットダウンを含む
         */
        ~WebCamera();

        /**
         * 画像をキャプチャする
         * @param image キャプチャした画像(引数として参照を渡し、処理が終了したときには画像を格納している)
         * @return キャプチャに成功した場合true
         */
        bool captureImage(cv::Mat& image);

        /**
         * カメラが初期化されているかチェック
         * @return 初期化済みの場合true
         */
        bool isInitialized() const;

    private:
        /**
         * 画像を保存するディレクトリ
         */
        std::string m_imageDirectory;

        /**
         * 画像のサフィックス
         */
        std::string m_imageSuffix;

        /**
         * カメラのソフトウェアアクセスの確立に成功したかどうか
         */
        bool m_isInitialized;

        /**
         * カメラのソフトウェアアクセス
         */
        cv::VideoCapture m_camera;

        /**
         * 保存した画像の枚数
         */
        int m_savedImageCount;
};

#endif // WEB_CAMERA_H