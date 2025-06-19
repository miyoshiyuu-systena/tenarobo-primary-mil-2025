#include "TimedCloser.h"
#include "ICloser.h"
#include "device/Device.h"
#include "ICloserGenerator.h"

ICloserGenerator timedCloserGenerator(Device* device, int max) {
    return [device, max]() {
        return new TimedCloser(device, max);
    };
}

TimedCloser::TimedCloser(Device* device, int max)
    : ICloser(device)
    , mMax(max + 1)
{    
}

TimedCloser::~TimedCloser()
{
}

void TimedCloser::init()
{
    mCount = 0;
}

bool TimedCloser::isClosed()
{
    mCount++;
    return mCount > mMax;
}