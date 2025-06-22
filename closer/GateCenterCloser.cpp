#include "GateCenterCloser.h"
#include "Logger.h"
#include "CameraManager.h"
#include "PerceptionReporter.h"
#include <opencv2/opencv.hpp>

ICloserGenerator gateCenterCloserGenerator() {
    return []() {
        return new GateCenterCloser();
    };
}

GateCenterCloser::GateCenterCloser() : ICloser()
{
}

GateCenterCloser::~GateCenterCloser()
{
}

void GateCenterCloser::init()
{
}

bool GateCenterCloser::isClosed()
{
    PerceptionReport report = PerceptionReporter::getInstance().getLatest();

    if (!PerceptionReporter::getInstance().isImageUpdated()) {
        return false;
    }

    cv::Mat image_blur;
    cv::GaussianBlur(report.image, image_blur, cv::Size(5, 101), 0);
    CameraManager::getInstance().saveImage(image_blur);

    cv::Mat image_inrange;
    cv::inRange(image_blur, cv::Scalar(0, 0, 0), cv::Scalar(100, 100, 100), image_inrange);
    CameraManager::getInstance().saveImage(image_inrange);

    int left_post_index = -1;
    int right_post_index = -1;

    /**
     * 
     */
    bool is_left_post_start_found = false;
    bool is_left_post_end_found = false;
    bool is_right_post_start_found = false;

    uchar* row_ptr = image_inrange.ptr<uchar>(image_inrange.rows / 3);
    for (int i = 0; i < image_inrange.cols; i++) {
        if (row_ptr[i] == 255) {
            if (is_left_post_start_found == false) {
                is_left_post_start_found = true;
            } else if (is_left_post_end_found == false) {
                /** Do Nothing */
            } else if (is_right_post_start_found == false) {
                is_right_post_start_found = true;
                right_post_index = i;
            } else {
                /** Do Nothing */
            }
        }

        if (row_ptr[i] == 0) {
            if (is_left_post_start_found == false) {
                /** Do Nothing */
            } else if (is_left_post_end_found == false) {
                is_left_post_end_found = true;
                left_post_index = i;
            } else if (is_right_post_start_found == false) {
                /** Do Nothing */
            } else {
                /** Do Nothing */
            }
        }
    }

    if (is_left_post_start_found == false || is_left_post_end_found == false || is_right_post_start_found == false) {
        return false;
    }

    int gate_center_index = (left_post_index + right_post_index) / 2;
    Logger::getInstance().logInfo("gate_center_index: " + std::to_string(gate_center_index));

    return (100 < gate_center_index) && (gate_center_index < 220);
}