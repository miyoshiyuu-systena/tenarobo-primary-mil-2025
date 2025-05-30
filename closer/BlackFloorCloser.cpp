#include "BlackFloorCloser.h"
#include "config.h"

ICloserGenerator blackFloorCloserGenerator() {
    return [](Device*& device) {
        return new BlackFloorCloser(device);
    };
}

/**
 * 設定ファイルから黒床検出のしきい値を取得する関数群
 */
static int getBlackFloorHUpperThreshold() {
    return config.getIntValue("blackFloorHUpperThreshold", 360);
}

static int getBlackFloorHLowerThreshold() {
    return config.getIntValue("blackFloorHLowerThreshold", 0);
}

static int getBlackFloorSUpperThreshold() {
    return config.getIntValue("blackFloorSUpperThreshold", 100);
}

static int getBlackFloorSLowerThreshold() {
    return config.getIntValue("blackFloorSLowerThreshold", 0);
}

static int getBlackFloorVUpperThreshold() {
    return config.getIntValue("blackFloorVUpperThreshold", 30);
}

static int getBlackFloorVLowerThreshold() {
    return config.getIntValue("blackFloorVLowerThreshold", 0);
}

BlackFloorCloser::BlackFloorCloser(Device*& device)
: ICloser()
, mDevice(device)
{
}

BlackFloorCloser::~BlackFloorCloser()
{
}

void BlackFloorCloser::init()
{
}

bool BlackFloorCloser::isClosed()
{
    static int H_UPPER_THRESHOLD = getBlackFloorHUpperThreshold();
    static int H_LOWER_THRESHOLD = getBlackFloorHLowerThreshold();
    static int S_UPPER_THRESHOLD = getBlackFloorSUpperThreshold();
    static int S_LOWER_THRESHOLD = getBlackFloorSLowerThreshold();
    static int V_UPPER_THRESHOLD = getBlackFloorVUpperThreshold();
    static int V_LOWER_THRESHOLD = getBlackFloorVLowerThreshold();

    return (
        (H_LOWER_THRESHOLD <= mReport.h && mReport.h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= mReport.s && mReport.s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= mReport.v && mReport.v <= V_UPPER_THRESHOLD)
    );
}