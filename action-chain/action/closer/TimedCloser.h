#ifndef _TIMED_CLOSER_H_
#define _TIMED_CLOSER_H_

#include    "ICloser.h"

/**
 * 判定周期を何回か経過したら終了判定を出すクラス
 * 時限式
 */
 // TODO: コメント
class TimedCloser : public ICloser
{
    public:
        TimedCloser(Perception* perc, int max);
        ~TimedCloser();

        void init() override;
        bool isClosed() override;
    
    private:
        int mCount;
        int mMax;
};

#endif // _TIMED_CLOSER_H_