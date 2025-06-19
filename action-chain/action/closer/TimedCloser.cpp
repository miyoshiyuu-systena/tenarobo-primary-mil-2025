#include "TimedCloser.h"
#include "ICloser.h"
#include "device/Device.h"
#include "ICloserGenerator.h"

ICloserGenerator timedCloserGenerator(int max) {
    return [max]() {
        return new TimedCloser(max);
    };
}

TimedCloser::TimedCloser(int max)
    : ICloser()
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

bool TimedCloser::isClosed(PerceptionReport* report)
{
    mCount++;
    return mCount > mMax;
}