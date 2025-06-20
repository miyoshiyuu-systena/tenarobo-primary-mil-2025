#include    "web-camera/CameraManager.h"
#include    "CurveCloser.h"
#include    "logger/Logger.h"
#include    <cmath>

ICloserGenerator curveCloserGenerator() {
    return []() {
        return new CurveCloser();
    };
}

CurveCloser::CurveCloser() : ICloser()
{
    mSeqCountIsCurve = 0;
}

CurveCloser::~CurveCloser()
{
}

void CurveCloser::init()
{
}

bool CurveCloser::isClosed(PerceptionReport* report)
{
    if (!report->isImageUpdated) {
        return false;
    }

    cv::Mat image;

    /**
     * グレースケールに変換
     */
     cv::cvtColor(report->image, image, cv::COLOR_BGR2GRAY);

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
     *  0にすると
     * 
     */
    /**
     * XXX：
     * ノイズを軽減するために入れているが、あまり重要ではないかもしれない
     * 処理全体が遅くなってしまうかも。
     */
    cv::GaussianBlur(image, image, cv::Size(5, 5), 0);

    /**
     * エッジ検出
     * @param image ガウシアンぼかしした画像
     * @param image エッジ検出した画像
     * @param 100 エッジ検出の下限
     * @param 200 エッジ検出の上限
     * @note
     *  エッジ検出は、画像中のエッジを検出するための手法
     *  エッジは、画像中の明るさの変化が激しい部分を指す
     */
    cv::Canny(image, image, 100, 200);

    /**
     * ハフ変換
     * @param image エッジ検出した画像
     * @param lines ハフ変換で検出した線分
     * @param 1 ハフ変換の精度
     * @param CV_PI / 180 ハフ変換の角度の精度
     * @param 50 ハフ変換の閾値
     * @param 50 ハフ変換の最小線分長
     * @param 10 ハフ変換の最大線分数
     * @note
     *  ハフ変換は、画像中の線分を検出するための手法
     *  線分を検出するために、画像中の各ピクセルに対して、そのピクセルを通る直線のパラメータを計算する
     *  そのパラメータを使って、線分を検出する
     *  
     */
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(image, lines, 1, 2 * CV_PI / 180, 50, 15, 10);

    bool isCurve = true;
    for (const auto& l : lines) {
        /**
        * 線分の始点と終点に着目し、ロボットの足元からスタートしているかどうかを判定する
        * l[0] 始点のx座標
        * l[1] 始点のy座標
        * l[2] 終点のx座標
        * l[3] 終点のy座標
        */
        if (
            ((100 < l[0]) && (l[0] < 220) && (l[1] > 200)) ||
            ((100 < l[2]) && (l[2] < 220) && (l[3] > 200))
        ) {
            isCurve = false;
            cv::line(report->image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            // line(画像, 始点, 終点, 色(BGR), 太さ, アンチエイリアス)
        }
    }
    CameraManager::getInstance().saveImage(report->image);

    if (isCurve) {
        mSeqCountIsCurve++;
    } else {
        mSeqCountIsCurve = 0;
    }
    
    return mSeqCountIsCurve > SEQ_COUNT_IS_CURVE_MAX;
}
