#include "HachikouColorAction.h"
#include "spikeapi.h"
#include "Logger.h"

ActionCall hachikouColorActionFactory(
    float forceDetectThreshold,
    int detectInterval
)
{
    return [forceDetectThreshold, detectInterval](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        while (device->forceSensor.getForce() <= forceDetectThreshold) {
            dly_tsk(detectInterval * 1000);
        }

        // 床の色を取得
        ColorSensor::HSV hsv;
        device->colorSensor.getHSV(hsv, true);
        Logger::getInstance().logInfo(
            "H: " + std::to_string(hsv.h) +
            ", S: " + std::to_string(hsv.s) +
            ", V: " + std::to_string(hsv.v)
        );
    };
}