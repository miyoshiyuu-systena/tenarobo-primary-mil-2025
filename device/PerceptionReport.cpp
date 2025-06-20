#include "PerceptionReport.h"
#include "CameraManager.h"
#include "logger/Logger.h"

void writePerceptionReport(
    Device* device,
    PerceptionReport& report,
    int detectInterval,
    uint8_t mask
) {
    static int count = 0;
    static int prevIndex = -1;

    if (mask & PERCEPTION_REPORT_MASK_ULTRASONIC) {
        report.distance = device->ultrasonicSensor.getDistance();
    }

    if (mask & PERCEPTION_REPORT_MASK_FORCE) {
        report.force = device->forceSensor.getForce();
    }

    if (mask & PERCEPTION_REPORT_MASK_COLOR) {
        ColorSensor::HSV hsv;
        device->colorSensor.getHSV(hsv, true);
        report.h = hsv.h;
        report.s = hsv.s;
        report.v = hsv.v;
    }

    if (mask & PERCEPTION_REPORT_MASK_IMAGE) {
        /**
         * cameraInterval = 33 msに1回撮影するように間引く
         */
        int index = (int)((count * detectInterval) / cameraInterval);
        if (index != prevIndex) {
            CameraManager::getInstance().captureImageNow(report.image);
            prevIndex = index;
            report.isImageUpdated = true;
        } else {
            report.isImageUpdated = false;
        }
    }

    if (mask & PERCEPTION_REPORT_MASK_MOTOR_SPEED) {
        report.leftMotorSpeed = device->twinWheelDrive.getLeftSpeed();
        report.rightMotorSpeed = device->twinWheelDrive.getRightSpeed();
    }

    Logger::getInstance().logDebug("=Report=========================================");
    Logger::getInstance().logDebug("distance: " + std::to_string(report.distance));
    Logger::getInstance().logDebug("force: " + std::to_string(report.force));
    Logger::getInstance().logDebug("h: " + std::to_string(report.h));
    Logger::getInstance().logDebug("s: " + std::to_string(report.s));
    Logger::getInstance().logDebug("v: " + std::to_string(report.v));
    Logger::getInstance().logDebug("leftMotorSpeed: " + std::to_string(report.leftMotorSpeed));
    Logger::getInstance().logDebug("rightMotorSpeed: " + std::to_string(report.rightMotorSpeed));
    Logger::getInstance().logDebug("================================================");

    count++;
}