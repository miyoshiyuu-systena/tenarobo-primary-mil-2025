#include "WhiteFloorAndStraightStrictCloser.h"

ICloserGenerator whiteFloorAndStraightStrictGenerator() {
    return [](Device*& device) -> ICloser* {
        return new WhiteFloorAndStraightStrict(device);
    };
}

WhiteFloorAndStraightStrict::WhiteFloorAndStraightStrict(Device*& device)
: ICloser()
, mDevice(device)
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
    static int H_UPPER_THRESHOLD = 360;
    static int H_LOWER_THRESHOLD = 0;
    static int S_UPPER_THRESHOLD = 100;
    static int S_LOWER_THRESHOLD = 0;
    static int V_UPPER_THRESHOLD = 100;
    static int V_LOWER_THRESHOLD = 70;

    // XXX　とりあえず色だけで終了判定
    
    return (
        (H_LOWER_THRESHOLD <= mReport.h && mReport.h <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= mReport.s && mReport.s <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= mReport.v && mReport.v <= V_UPPER_THRESHOLD)
    );
}