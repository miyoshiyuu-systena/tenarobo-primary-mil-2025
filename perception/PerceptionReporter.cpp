#include "PerceptionReporter.h"
#include "CameraManager.h"
#include "Logger.h"
#include "Device.h"
#include "config.h"
#include "PerceptionMask.h"

/**
 * シングルトンインスタンス取得
 */
PerceptionReporter& PerceptionReporter::getInstance()
{
    static PerceptionReporter instance;
    return instance;
}

/**
 * コンストラクタ
 */
PerceptionReporter::PerceptionReporter()
{
    init();
}

/**
 * 初期化
 */
void PerceptionReporter::init()
{
    mLatestReport = PerceptionReport();
    mCount = 0;
    mPrevIndex = 0;
    mIsImageUpdated = false;
    mIsLogOutput = config.getValue("perceptionLogOutput") == "1";
}

PerceptionReport PerceptionReporter::getLatest() const
{
    return mLatestReport;
}

bool PerceptionReporter::isImageUpdated() const
{
    return mIsImageUpdated;
}

void PerceptionReporter::update(int detectInterval, uint8_t mask)
{
    if (mask & PERCEPTION_REPORT_MASK_ULTRASONIC) {
        mLatestReport.distance = device.ultrasonicSensor.getDistance();
    }

    if (mask & PERCEPTION_REPORT_MASK_FORCE) {
        mLatestReport.force = device.forceSensor.getForce();
    }

    if (mask & PERCEPTION_REPORT_MASK_COLOR) {
        ColorSensor::HSV hsv;
        device.colorSensor.getHSV(hsv, true);
        mLatestReport.h = hsv.h;
        mLatestReport.s = hsv.s;
        mLatestReport.v = hsv.v;
    }

    if (mask & PERCEPTION_REPORT_MASK_IMAGE) {
        int index = (int)((mCount * detectInterval) / CAMERA_INTERVAL);
        if (index != mPrevIndex) {
            CameraManager::getInstance().captureImageNow(mLatestReport.image);
            mIsImageUpdated = true;
        } else {
            mIsImageUpdated = false;
        }
        mPrevIndex = index;
    }

    if (mask & PERCEPTION_REPORT_MASK_MOTOR_ENCODE) {
        mLatestReport.leftMotorEncode = device.twinWheelDrive.getLeftEncode();
        mLatestReport.rightMotorEncode = device.twinWheelDrive.getRightEncode();
    }

    
    if (mIsLogOutput) {
        Logger::getInstance().logDebug("=Report=========================================");
        Logger::getInstance().logDebug("distance: " + std::to_string(mLatestReport.distance) + " [mm]");
        Logger::getInstance().logDebug("force: " + std::to_string(mLatestReport.force) + " [N]");
        Logger::getInstance().logDebug("h: " + std::to_string(mLatestReport.h) + " [0-360]");
        Logger::getInstance().logDebug("s: " + std::to_string(mLatestReport.s) + " [0-100]");
        Logger::getInstance().logDebug("v: " + std::to_string(mLatestReport.v) + " [0-100]");
        Logger::getInstance().logDebug("leftMotorEncode: " + std::to_string(mLatestReport.leftMotorEncode) + " [deg]");
        Logger::getInstance().logDebug("rightMotorEncode: " + std::to_string(mLatestReport.rightMotorEncode) + " [deg]");
        Logger::getInstance().logDebug("================================================");
    }

    mCount++;
}
