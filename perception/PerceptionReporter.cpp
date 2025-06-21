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
    mPrevIndex = -1;
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

void PerceptionReporter::update(int detectInterval)
{

    /**
     * 超音波センサの距離を更新する
     */
    mLatestReport.distance = device.ultrasonicSensor.getDistance();

    /**
     * 力センサの力を更新する
     */
    mLatestReport.force = device.forceSensor.getForce();

    /**
     * カラーセンサの色を更新する
     */
    ColorSensor::HSV hsv;
    device.colorSensor.getHSV(hsv, true);
    mLatestReport.h = hsv.h;
    mLatestReport.s = hsv.s;
    mLatestReport.v = hsv.v;

    /**
     * カメラの画像を更新する
     * カメラは最大30fpsのものを使用しているため、
     * Spike-artの他のセンサと同じペースで取得し続けても意味がない
     * 更新のペースに合わせて取得する
     */
    int index = (int)((mCount * detectInterval) / CAMERA_INTERVAL);
    if (index != mPrevIndex) {
        CameraManager::getInstance().captureImageNow(mLatestReport.image);
        mIsImageUpdated = true;
    } else {
        mIsImageUpdated = false;
    }
    mPrevIndex = index;

    /**
     * モータのエンコーダの値を更新する
     */
    mLatestReport.leftMotorEncode = device.twinWheelDrive.getLeftEncode();
    mLatestReport.rightMotorEncode = device.twinWheelDrive.getRightEncode();

    /**
     * ログ出力
     */
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
