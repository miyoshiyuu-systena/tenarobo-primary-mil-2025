#include "TimedCloser.h"

ICloserGenerator timedCloserGenerator(int max) {
    return [max]() {
        return new TimedCloser(max);
    };
}

TimedCloser::TimedCloser(int max)
    : ICloser()
    , mMax(max + 1)
{    
    mask = 0b00000000;
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