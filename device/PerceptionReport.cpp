#include "PerceptionReport.h"
#include "CameraManager.h"

void writePerceptionReport(Device* device, PerceptionReport& report, uint8_t mask) {
    static int count = 0;

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

    // if (mask & PERCEPTION_REPORT_MASK_IMAGE) {
    //     if (count % 10 == 0) {
    //         CameraManager::getInstance().captureImageNow(report.image);
    //     }
    // }

    count++;
}