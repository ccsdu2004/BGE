#ifndef BGE_DEVICE_H
#define BGE_DEVICE_H
//#include <BGE/Graphics/Export.h>
#include <BGE/System/String.h>
#include <BGE/System/NonCopyable.h>
#include <BGE/System/Deleter.h>
#include <cstdint>

namespace bge
{

class BGE_EXPORT_API Device : public NonCopyable, public Deleter
{
public:
    static Device* create();
public:
    Device();
    virtual ~Device() = 0;
public:
    virtual bool initial() = 0;
    virtual void terminate() = 0;

    virtual bool createWindow(int32_t width,int32_t height,const String& title) = 0;
    virtual void closeWindow() = 0;
    virtual bool isRunning() = 0;

    virtual void setWindowSize(int32_t width,int32_t height) = 0;
    virtual void getWindowSize(int32_t& width,int32_t& height) = 0;

    virtual void preRender() = 0;
    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;
};

}

#endif // BGI_DEVICE_H
