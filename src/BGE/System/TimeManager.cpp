#include <BGE/System/TimeManager.h>
#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <ctime>
#endif

namespace bge
{

float TimeManager::systemTime()
{
#ifdef WIN32
    return static_cast<float>(timeGetTime()) * 0.001f;
#else
    clock_t clockTime = clock();
    return static_cast<float>(clockTime) / static_cast<float>(CLOCKS_PER_SEC);
#endif
}

}
