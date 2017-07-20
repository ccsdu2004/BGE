//! 2010.06.01
#include <BGE/System/Timer.h>
#include <BGE/System/TimeManager.h>

namespace bge
{

Timer::Timer(uint32_t inv):
interval_(inv),
isStarted_(false)
{
}

Timer::~Timer()
{
    stop();
}

Timer& Timer::start()
{
    //!ASSERT(GetInterval() > 0);
    if(!isStarted())
    {
        isStarted_ = true;
        lastTriggerTime_ = TimeManager::systemTime();
    }
    return *this;
}

Timer& Timer::stop()
{
    if(isStarted())
    {
        isStarted_ = false;
    }
    return *this;
}

bool Timer::isElapsed()
{
    bool ret = false;
    if(isStarted())
    {
        uint32_t now = TimeManager::systemTime();
        if(now > (lastTriggerTime_ + interval()))
        {
            ret = true;
            lastTriggerTime_ = now;
        }
    }
    return ret;
}

}

