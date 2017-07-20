//! 2010.06.01
#ifndef BGE_SYSTEM_TIMER_H
#define BGE_SYSTEM_TIMER_H
#include <BGE/Config.h>
#include <BGE/System/SigSlot.h>

namespace bge
{

class BGE_EXPORT_API Timer
{
public:
    explicit Timer(uint32_t interval = 1000);
    virtual ~Timer();
public:
    int32_t  interval()const{return interval_;}
    Timer& setInterval(uint32_t seconds)
    {
        interval_ = seconds;
        return *this;
    }

    bool   isStarted()const{return isStarted_;}
    bool   isElapsed();
public:
    Timer& start();
    Timer& stop();
public:
    Signal0 trigger;
private:
    uint32_t interval_;
    bool     isStarted_;
    uint32_t lastTriggerTime_;
};

}

#endif
