#include "LaneChangeAssist.h"

IAssistGenerator laneChangeAssistGenerator(
    bool isRightSide
) {
    return [isRightSide](Device*& device) {
        return new LaneChangeAssist(isRightSide);
    };
}

LaneChangeAssist::LaneChangeAssist(
    bool isRightSide
):
    IAssist()
    , mIsRightSide(isRightSide)
{
    mDetectCount = 0;
}

LaneChangeAssist::~LaneChangeAssist()
{
}

void LaneChangeAssist::init()
{
}

void LaneChangeAssist::correct(float* speeds)
{
    float weight = 1.5f;
    if (mIsRightSide) {
        if (mDetectCount < 40) {
            speeds[0] = speeds[0] * weight;
            speeds[1] = speeds[1] / weight;
        }
        else if (mDetectCount < 80) {
            speeds[0] = speeds[0] / weight;
            speeds[1] = speeds[1] * weight;
        }
        else if (mDetectCount < 120) {
            speeds[0] = speeds[0] * weight;
            speeds[1] = speeds[1] / weight;
        }
        else if (mDetectCount < 160) {
            speeds[0] = speeds[0] * weight;
            speeds[1] = speeds[1] / weight;
        }
        else {
            /** No Operation */
        }
    } else {
        if (mDetectCount < 40) {
            speeds[0] = speeds[0] / weight;
            speeds[1] = speeds[1] * weight;
        } 
        else if (mDetectCount < 80) {
            speeds[0] = speeds[0] * weight;
            speeds[1] = speeds[1] / weight;
        }
        else if (mDetectCount < 120) {
            speeds[0] = speeds[0] / weight;
            speeds[1] = speeds[1] * weight;
        }
        else if (mDetectCount < 160) {
            speeds[0] = speeds[0] * weight;
            speeds[1] = speeds[1] / weight;
        }
        else {
            /** No Operation */
        }
    }
    mDetectCount++;
}