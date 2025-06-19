#include "TimedCloser.h"
#include "ICloser.h"

TimedCloser::TimedCloser(Perception* perc, int max)
    : ICloser(perc)
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