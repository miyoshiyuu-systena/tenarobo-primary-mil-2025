#include "SlowlyAccelerateAssist.h"

IAssistGenerator slowlyAccelerateAssistGenerator(
    int stepCount,
    int stepInterval
) {
    return [stepCount, stepInterval]() {
        return new SlowlyAccelerateAssist(stepCount, stepInterval);
    };
}

SlowlyAccelerateAssist::SlowlyAccelerateAssist(
    int stepCount,
    int stepInterval
):
    IAssist()
    , mStepCount(stepCount)
    , mStepInterval(stepInterval)
{
    mask = 0b00000000;
}

SlowlyAccelerateAssist::~SlowlyAccelerateAssist()
{
}

void SlowlyAccelerateAssist::init()
{
    mStepIndex = 0;
}

void SlowlyAccelerateAssist::correct(float* speeds, PerceptionReport* report)
{
    int level = (int)(mStepIndex / mStepInterval) + 1;
    if (level > mStepCount) {
        level = mStepCount;
    }

    speeds[0] = speeds[0] * level / mStepCount;
    speeds[1] = speeds[1] * level / mStepCount;

    mStepIndex++;
}