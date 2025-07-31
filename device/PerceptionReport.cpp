#include "PerceptionReport.h"
#include "CameraManager.h"

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
        }
    }

    count++;
}