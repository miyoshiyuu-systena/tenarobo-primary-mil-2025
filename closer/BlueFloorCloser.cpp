#include "BlueFloorCloser.h"
#include "config.h"

ICloserGenerator blueFloorCloserGenerator() {
    return []() {
        return new BlueFloorCloser();
    };
}

/**
 * 設定ファイルから青床検出のしきい値を取得する関数群
 */
static int getBlueFloorHUpperThreshold() {
    return config.getIntValue("blueFloorHUpperThreshold", 220);
}

static int getBlueFloorHLowerThreshold() {
    return config.getIntValue("blueFloorHLowerThreshold", 180);
}

static int getBlueFloorSUpperThreshold() {
    return config.getIntValue("blueFloorSUpperThreshold", 100);
}

static int getBlueFloorSLowerThreshold() {
    return config.getIntValue("blueFloorSLowerThreshold", 50);
}

static int getBlueFloorVUpperThreshold() {
    return config.getIntValue("blueFloorVUpperThreshold", 100);
}

static int getBlueFloorVLowerThreshold() {
    return config.getIntValue("blueFloorVLowerThreshold", 50);
}

BlueFloorCloser::BlueFloorCloser() : ICloser()
{
    mask = PERCEPTION_REPORT_MASK_COLOR;
}

BlueFloorCloser::~BlueFloorCloser()
{
}

void BlueFloorCloser::init()
{
}

bool BlueFloorCloser::isClosed(PerceptionReport* report)
{
    static int H_UPPER_THRESHOLD = getBlueFloorHUpperThreshold();
    static int H_LOWER_THRESHOLD = getBlueFloorHLowerThreshold();
    static int S_UPPER_THRESHOLD = getBlueFloorSUpperThreshold();
    static int S_LOWER_THRESHOLD = getBlueFloorSLowerThreshold();
    static int V_UPPER_THRESHOLD = getBlueFloorVUpperThreshold();
    static int V_LOWER_THRESHOLD = getBlueFloorVLowerThreshold();
    
    return (
        (H_LOWER_THRESHOLD <= report->h && report->h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= report->s && report->s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= report->v && report->v <= V_UPPER_THRESHOLD)
    );
}