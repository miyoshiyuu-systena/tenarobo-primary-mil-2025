#include "BlueFloorCloser.h"
#include "ICloser.h"

static const int H_UPPER_THRESHOLD = 220;
static const int H_LOWER_THRESHOLD = 180;
static const int S_UPPER_THRESHOLD = 200;
static const int S_LOWER_THRESHOLD = 50;
static const int V_UPPER_THRESHOLD = 100;
static const int V_LOWER_THRESHOLD = 50;

BlueFloorCloser::BlueFloorCloser(Perception* perc) : ICloser(perc)
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
    return (
        (H_LOWER_THRESHOLD <= mPerc->getColorH() && mPerc->getColorH() <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= mPerc->getColorS() && mPerc->getColorS() <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= mPerc->getColorV() && mPerc->getColorV() <= V_UPPER_THRESHOLD)
    );
}