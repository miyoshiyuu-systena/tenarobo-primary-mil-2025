#include "SlowlyAccelerateAssist.h"
#include "IAssistGenerator.h"

IAssistGenerator slowlyAccelerateAssistGenerator(
    Device* device,
    int stepCount,
    int stepInterval
) {
    return [device, stepCount, stepInterval]() {
        return new SlowlyAccelerateAssist(device, stepCount, stepInterval);
    };
}

SlowlyAccelerateAssist::SlowlyAccelerateAssist(
    Device* device,
    int stepCount,
    int stepInterval
):
    IAssist(device)
    , mStepCount(stepCount)
    , mStepInterval(stepInterval)
{
}

SlowlyAccelerateAssist::~SlowlyAccelerateAssist()
{
}

void SlowlyAccelerateAssist::init()
{
    mStepIndex = 0;
}

void SlowlyAccelerateAssist::correct(float* speeds)
{
    int level = (int)(mStepIndex / mStepInterval) + 1;
    if (level > mStepCount) {
        level = mStepCount;
    }

    speeds[0] = speeds[0] * level / mStepCount;
    speeds[1] = speeds[1] * level / mStepCount;

    mStepIndex++;
}