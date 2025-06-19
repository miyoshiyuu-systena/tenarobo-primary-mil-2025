#include "BlueFloorCloser.h"
#include "ICloser.h"
#include "ColorSensor.h"

static const int H_UPPER_THRESHOLD = 220;
static const int H_LOWER_THRESHOLD = 180;
static const int S_UPPER_THRESHOLD = 200;
static const int S_LOWER_THRESHOLD = 50;
static const int V_UPPER_THRESHOLD = 100;
static const int V_LOWER_THRESHOLD = 50;

BlueFloorCloser::BlueFloorCloser(Device* device) : ICloser(device)
{
}

BlueFloorCloser::~BlueFloorCloser()
{
}

void BlueFloorCloser::init()
{
}

bool BlueFloorCloser::isClosed()
{
    ColorSensor::HSV hsv;
    mDevice->colorSensor.getHSV(hsv, true);
    return (
        (H_LOWER_THRESHOLD <= hsv.h && hsv.h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= hsv.s && hsv.s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= hsv.v && hsv.v <= V_UPPER_THRESHOLD)
    );
}