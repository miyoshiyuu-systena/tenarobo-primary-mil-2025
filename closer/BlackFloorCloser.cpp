#include "BlackFloorCloser.h"

ICloserGenerator blackFloorCloserGenerator() {
    return []() {
        return new BlackFloorCloser();
    };
}

static const int H_UPPER_THRESHOLD = 255;
static const int H_LOWER_THRESHOLD = 0;
static const int S_UPPER_THRESHOLD = 255;
static const int S_LOWER_THRESHOLD = 0;
static const int V_UPPER_THRESHOLD = 30;
static const int V_LOWER_THRESHOLD = 0;

BlackFloorCloser::BlackFloorCloser() : ICloser()
{
}

BlackFloorCloser::~BlackFloorCloser()
{
}

void BlackFloorCloser::init()
{
}

bool BlackFloorCloser::isClosed(PerceptionReport* report)
{
    return (
        (H_LOWER_THRESHOLD <= report->h && report->h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= report->s && report->s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= report->v && report->v <= V_UPPER_THRESHOLD)
    );
}