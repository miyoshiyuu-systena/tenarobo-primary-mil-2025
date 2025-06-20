#include    "web-camera/CameraManager.h"

ICloserGenerator curveCloserGenerator() {
    return []() {
        return new CurveCloser();
    };
}

CurveCloser::CurveCloser() : ICloser()
{
}

CurveCloser::~CurveCloser()
{
}

void CurveCloser::init()
{
}

bool CurveCloser::isClosed(PerceptionReport* report)
{
    cv::Mat image;

    /**
     * グレースケールに変換
     */
     cv::cvtColor(report->image, image, cv::COLOR_BGR2GRAY);
     CameraManager::getInstance().saveImage(image);

    /**
     * ガウシアンぼかし
     * @param image グレースケールに変換した画像
     * @param image ガウシアンブラーでノイズを軽減した画像
     * @param Size(5, 5) ガウシアンぼかしのカーネルサイズ
     * @note
     *  ぼかしのメカニズムは、ピクセル（1画素）の色に、その周辺のピクセルの色を取り込んでごちゃ混ぜにすること
     *  ごちゃ混ぜにするときに取り込むピクセルの範囲を指定する（=カーネルサイズ）
     *  なので奇数 * 奇数で指定するべし
     * @note
     *  cv::Size（幅、高さ）
     * 
     * @param 0 ガウシアンぼかしの標準偏差
     * @note
     *  色をごちゃ混ぜにすると言っても、メチャクチャにするのではなく、そのピクセルに近い色ほど、新しい色の決定に大きく寄与する。
     *  標準偏差はその程度を示す。
     *  大きな標準偏差にすると、広く色をごちゃ混ぜにする。（ぼかしの程度が著しくなる）
     * 
     */
    /**
     * XXX：
     * ノイズを軽減するために入れているが、あまり重要ではないかもしれない
     * 処理全体が遅くなってしまうかも。
     */
    cv::GaussianBlur(image, image, cv::Size(5, 5), 0);
    CameraManager::getInstance().saveImage(image);

    /**
     * エッジ検出
     */
    cv::Canny(image, image, 100, 200);
    CameraManager::getInstance().saveImage(image);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(image, lines, 1, CV_PI / 180, 50, 50, 10);
    CameraManager::getInstance().saveImage(image);

    for (const auto& l : lines) {
        cv::line(image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        // line(画像, 始点, 終点, 色(BGR), 太さ, アンチエイリアス)
    }

    CameraManager::getInstance().saveImage(image);

    return false;
}
