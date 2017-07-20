#include <BGE/System/TimeLine.h>

namespace bge
{

IMPLEMENT_OBJECT(TimeLine,Object)

struct TimeLineImpl
{
    TimeLineImpl():
        startTime(0),
        duration(1000),
        startFrame(0),
        endFrame(0),
        updateInterval(1000/25),
        totalLoopCount(1),
        currentLoopCount(0),
        currentTime(0),
        timerId(0),
        direction(TimeLine::Forward),
        state(TimeLine::NotRunning)
    {
    }

    TimeLine* this_;
    int32_t startTime;
    int32_t duration;
    int32_t startFrame;
    int32_t endFrame;
    int32_t updateInterval;
    int32_t totalLoopCount;
    int32_t currentLoopCount;

    int32_t currentTime;
    int32_t timerId;
    //QElapsedTimer timer;

    TimeLine::Direction direction;
    TimeLine::State state;

    void setState(TimeLine::State newState)
    {
        if(newState != state)
            this_->stateChanged(state = newState);
    }

    void setCurrentTime(int msecs);
};

TimeLine::~TimeLine()
{
}


}
