#include "StraightCloser.h"
#include "config.h"
#include <cmath>

ICloserGenerator straightCloserGenerator() {
    return [](Device*& device) {
        return new StraightCloser(device);
    };
}

StraightCloser::StraightCloser(Device*& device)
: ICloser()
, mDevice(device)
{
    mSeqCountIsStraight = 0;
}

StraightCloser::~StraightCloser()
{
}

void StraightCloser::init()
{
}

int StraightCloser::getSeqCountIsStraightMax()
{
    return config.getIntValue("seqCountIsStraightMax", 10);
}

bool StraightCloser::isClosed()
{
    return false;
}
// {
//     cv::Mat image;

//     /**
//      * グレースケールに変換
//      */
//      cv::cvtColor(report.image, image, cv::COLOR_BGR2GRAY);

//     /**
//      * ガウシアンぼかし
//      * @param image グレースケールに変換した画像
//      * @param image ガウシアンブラーでノイズを軽減した画像
//      * @param Size(5, 5) ガウシアンぼかしのカーネルサイズ
//      * @note
//      *  ぼかしのメカニズムは、ピクセル（1画素）の色に、その周辺のピクセルの色を取り込んでごちゃ混ぜにすること
//      *  ごちゃ混ぜにするときに取り込むピクセルの範囲を指定する（=カーネルサイズ）
//      *  なので奇数 * 奇数で指定するべし
//      * @note
//      *  cv::Size（幅、高さ）
//      * 
//      * @param 0 ガウシアンぼかしの標準偏差
//      * @note
//      *  色をごちゃ混ぜにすると言っても、メチャクチャにするのではなく、そのピクセルに近い色ほど、新しい色の決定に大きく寄与する。
//      *  標準偏差はその程度を示す。
//      *  大きな標準偏差にすると、広く色をごちゃ混ぜにする。（ぼかしの程度が著しくなる）
//      *  0にすると
//      * 
//      */
//     /**
//      * XXX：
//      * ノイズを軽減するために入れているが、あまり重要ではないかもしれない
//      * 処理全体が遅くなってしまうかも。
//      */
//     static int gaussianKernelSize = config.getIntValue("straightGaussianKernelSize", 5);
//     // cv::GaussianBlur(image, image, cv::Size(gaussianKernelSize, gaussianKernelSize), 0);

//     /**
//      * エッジ検出
//      * @param image ガウシアンぼかしした画像
//      * @param image エッジ検出した画像
//      * @param 100 エッジ検出の下限
//      * @param 200 エッジ検出の上限
//      * @note
//      *  エッジ検出は、画像中のエッジを検出するための手法
//      *  エッジは、画像中の明るさの変化が激しい部分を指す
//      */
//     static int cannyLowerThreshold = config.getIntValue("straightCannyLowerThreshold", 100);
//     static int cannyUpperThreshold = config.getIntValue("straightCannyUpperThreshold", 200);
//     // cv::Canny(image, image, cannyLowerThreshold, cannyUpperThreshold);

//     /**
//      * ハフ変換
//      * @param image エッジ検出した画像
//      * @param lines ハフ変換で検出した線分
//      * @param 1 ハフ変換の精度
//      * @param CV_PI / 180 ハフ変換の角度の精度
//      * @param 50 ハフ変換の閾値
//      * @param 50 ハフ変換の最小線分長
//      * @param 10 ハフ変換の最大線分数
//      * @note
//      *  ハフ変換は、画像中の線分を検出するための手法
//      *  線分を検出するために、画像中の各ピクセルに対して、そのピクセルを通る直線のパラメータを計算する
//      *  そのパラメータを使って、線分を検出する
//      *  
//      */
//     static int houghThreshold = config.getIntValue("straightHoughThreshold", 50);
//     static int houghMinLineLength = config.getIntValue("straightHoughMinLineLength", 10);
//     static int houghMaxLineGap = config.getIntValue("straightHoughMaxLineGap", 10);
//     std::vector<cv::Vec4i> lines;
//     // cv::HoughLinesP(image, lines, 1, CV_PI / 180, houghThreshold, houghMinLineLength, houghMaxLineGap);

//     bool isStraight = false;
//     for (const auto& l : lines) {
//         /**
//         * 線分の始点と終点に着目し、ロボットの足元からスタートしているかどうかを判定する
//         * l[0] 始点のx座標
//         * l[1] 始点のy座標
//         * l[2] 終点のx座標
//         * l[3] 終点のy座標
//         */
//         static int lineXMin = config.getIntValue("straightLineXMin", 100);
//         static int lineXMax = config.getIntValue("straightLineXMax", 220);
//         static int lineYMin = config.getIntValue("straightLineYMin", 200);
//         if (
//             ((lineXMin < l[0]) && (l[0] < lineXMax) && (l[1] > lineYMin)) ||
//             ((lineXMin < l[2]) && (l[2] < lineXMax) && (l[3] > lineYMin))
//         ) {
//             isStraight = true;
//         }
//     }

//     if (isStraight) {
//         mSeqCountIsStraight++;
//     } else {
//         mSeqCountIsStraight = 0;
//     }
    
//     return mSeqCountIsStraight > getSeqCountIsStraightMax();
// }