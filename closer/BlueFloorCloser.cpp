#include "BlueFloorCloser.h"

ICloserGenerator blueFloorCloserGenerator() {
    return []() {
        return new BlueFloorCloser();
    };
}

static const int H_UPPER_THRESHOLD = 220;
static const int H_LOWER_THRESHOLD = 180;
static const int S_UPPER_THRESHOLD = 200;
static const int S_LOWER_THRESHOLD = 50;
static const int V_UPPER_THRESHOLD = 100;
static const int V_LOWER_THRESHOLD = 50;

BlueFloorCloser::BlueFloorCloser() : ICloser()
{
}

BlueFloorCloser::~BlueFloorCloser()
{
}

void BlueFloorCloser::init()
{
}

bool BlueFloorCloser::isClosed(PerceptionReport* report)
{
    return (
        (H_LOWER_THRESHOLD <= report->h && report->h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= report->s && report->s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= report->v && report->v <= V_UPPER_THRESHOLD)
    );
}