#include "BlackFloorCloser.h"

static const int H_UPPER_THRESHOLD = 255;
static const int H_LOWER_THRESHOLD = 0;
static const int S_UPPER_THRESHOLD = 255;
static const int S_LOWER_THRESHOLD = 0;
static const int V_UPPER_THRESHOLD = 30;
static const int V_LOWER_THRESHOLD = 0;

BlackFloorCloser::BlackFloorCloser(Perception* perc) : ICloser(perc)
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
    return (
        (H_LOWER_THRESHOLD <= mPerc.getColorH() && mPerc.getColorH() <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= mPerc.getColorS() && mPerc.getColorS() <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= mPerc.getColorV() && mPerc.getColorV() <= V_UPPER_THRESHOLD)
    );
}