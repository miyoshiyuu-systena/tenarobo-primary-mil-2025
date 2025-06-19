#include "TimedCloser.h"
#include "ICloser.h"
#include "device/Device.h"

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