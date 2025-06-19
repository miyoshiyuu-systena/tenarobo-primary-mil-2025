#include "BlackFloorCloser.h"
#include "ICloser.h"
#include "ColorSensor.h"

static const int H_UPPER_THRESHOLD = 255;
static const int H_LOWER_THRESHOLD = 0;
static const int S_UPPER_THRESHOLD = 255;
static const int S_LOWER_THRESHOLD = 0;
static const int V_UPPER_THRESHOLD = 30;
static const int V_LOWER_THRESHOLD = 0;

BlackFloorCloser::BlackFloorCloser(Device* device) : ICloser(device)
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
    ColorSensor::HSV hsv;
    mDevice->colorSensor.getHSV(hsv, true);
    return (
        (H_LOWER_THRESHOLD <= hsv.h && hsv.h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= hsv.s && hsv.s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= hsv.v && hsv.v <= V_UPPER_THRESHOLD)
    );
}