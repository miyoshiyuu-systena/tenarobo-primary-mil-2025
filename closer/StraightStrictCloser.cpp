#include "StraightStrictCloser.h"
#include "PerceptionReporter.h"
#include "PerceptionReport.h"
#include "config.h"
#include <cmath>

ICloserGenerator straightStrictCloserGenerator() {
    return []() {
        return new StraightStrictCloser();
    };
}

StraightStrictCloser::StraightStrictCloser() : ICloser()
{
}

StraightStrictCloser::~StraightStrictCloser()
{
}

void StraightStrictCloser::init()
{
}

int StraightStrictCloser::getSeqCountIsStraightMax()
{
    return config.getIntValue("straightStrictSeqCountIsStraightMax", 5);
}

bool StraightStrictCloser::isClosed()
{
    PerceptionReport report = PerceptionReporter::getInstance().getLatest();

    if (!PerceptionReporter::getInstance().isImageUpdated()) {
        return false;
    }
    
    cv::Mat image;

    /**
     * グレースケールに変換
     */
    cv::cvtColor(report.image, image, cv::COLOR_BGR2GRAY);

    /**
     * ガウシアンぼかし
     */
    static int gaussianKernelSizeWidth = config.getIntValue("straightStrictGaussianKernelSizeWidth", 5);
    static int gaussianKernelSizeHeight = config.getIntValue("straightStrictGaussianKernelSizeHeight", 51);
    cv::GaussianBlur(image, image, cv::Size(gaussianKernelSizeWidth, gaussianKernelSizeHeight), 0);

    /**
     * エッジ検出
     */
    static int cannyLowerThreshold = config.getIntValue("straightStrictCannyLowerThreshold", 100);  // 100
    static int cannyUpperThreshold = config.getIntValue("straightStrictCannyUpperThreshold", 200);  // 200
    cv::Canny(image, image, cannyLowerThreshold, cannyUpperThreshold);

    /**
     * ハフ変換
     */
    static int houghThreshold = config.getIntValue("straightStrictHoughThreshold", 100);
    static int houghMinLineLength = config.getIntValue("straightStrictHoughMinLineLength", 100);
    static int houghMaxLineGap = config.getIntValue("straightStrictHoughMaxLineGap", 10);
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(image, lines, 1, CV_PI / 180, houghThreshold, houghMinLineLength, houghMaxLineGap);

    bool isStraight = false;
    for (const auto& l : lines) {
        /**
         * 線分の始点と終点に着目し、ロボットの足元からスタートしているかどうかを判定する
         */
        static int lineXMin = config.getIntValue("straightStrictLineXMin", 100);
        static int lineXMax = config.getIntValue("straightStrictLineXMax", 220);
        static int lineYMin = config.getIntValue("straightStrictLineYMin", 200);
        if (
            ((lineXMin < l[0]) && (l[0] < lineXMax) && (l[1] > lineYMin)) ||
            ((lineXMin < l[2]) && (l[2] < lineXMax) && (l[3] > lineYMin))
        ) {
            isStraight = true;
        }
    }

    if (isStraight) {
        mSeqCountIsStraight++;
    } else {
        mSeqCountIsStraight = 0;
    }
    
    return mSeqCountIsStraight > getSeqCountIsStraightMax();
}