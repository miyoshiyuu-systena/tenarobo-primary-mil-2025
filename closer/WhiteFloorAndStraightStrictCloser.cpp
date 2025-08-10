#include "WhiteFloorAndStraightStrictCloser.h"
#include "PerceptionReporter.h"
#include "PerceptionReport.h"

ICloserGenerator whiteFloorAndStraightStrictGenerator() {
    return []() -> ICloser* {
        return new WhiteFloorAndStraightStrict();
    };
}

WhiteFloorAndStraightStrict::WhiteFloorAndStraightStrict() : ICloser()
{
}

WhiteFloorAndStraightStrict::~WhiteFloorAndStraightStrict()
{
}

void WhiteFloorAndStraightStrict::init()
{
}

bool WhiteFloorAndStraightStrict::isClosed()
{
    PerceptionReport report = PerceptionReporter::getInstance().getLatest();

    if (!PerceptionReporter::getInstance().isImageUpdated()) {
        return false;
    }

    static int H_UPPER_THRESHOLD = 360;
    static int H_LOWER_THRESHOLD = 0;
    static int S_UPPER_THRESHOLD = 100;
    static int S_LOWER_THRESHOLD = 0;
    static int V_UPPER_THRESHOLD = 100;
    static int V_LOWER_THRESHOLD = 70;

    // XXX　とりあえず色だけで終了判定
    
    return (
        (H_LOWER_THRESHOLD <= report.h && report.h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= report.s && report.s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= report.v && report.v <= V_UPPER_THRESHOLD)
    );
}