#include <BGE/Graphics/Device.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/System/Err.h>
#include <glfw/glfw3.h>
#include <iostream>

namespace bge
{

void GLFWerrorfun_(int flag,const char* ch)
{

}

void GLFWwindowposfun_(GLFWwindow* window,int x,int y)
{

}

void GLFWwindowsizefun_(GLFWwindow* window,int width,int height)
{
    WindowManager::instance().applicationResized(width,height);
}

void GLFWwindowclosefun_(GLFWwindow* window)
{

}

void GLFWwindowrefreshfun_(GLFWwindow* window)
{

}

void GLFWwindowfocusfun_(GLFWwindow* window,int flag)
{

}

void GLFWwindowiconifyfun_(GLFWwindow* window,int flag)
{

}

void GLFWframebuffersizefun_(GLFWwindow* window,int width,int height)
{
    WindowManager::instance().applicationResized(width,height);
}

void GLFWmousebuttonfun_(GLFWwindow* window,int button,int state,int flag)
{
    double x,y;
    glfwGetCursorPos(window,&x,&y);
    if(button == GLFW_MOUSE_BUTTON_1 && state == GLFW_PRESS)
        WindowManager::instance().onLeftButtonDown(x,y);
    else if(button == GLFW_MOUSE_BUTTON_2 && state == GLFW_PRESS)
        WindowManager::instance().onRightButtonDown(x,y);
    else if(button == GLFW_MOUSE_BUTTON_1 && state == GLFW_RELEASE)
        WindowManager::instance().onLeftButtonUp(x,y);
    else if(button == GLFW_MOUSE_BUTTON_2 && state == GLFW_RELEASE)
        WindowManager::instance().onRightButtonUp(x,y);
}

void GLFWcursorposfun_(GLFWwindow* window,double x,double y)
{
    WindowManager::instance().onMouseMove(x,y);
}

void GLFWcursorenterfun_(GLFWwindow* window,int flag)
{

}

void GLFWscrollfun_(GLFWwindow* window,double x,double y)
{

}

void GLFWkeyfun_(GLFWwindow* window,int key,int flag1,int flag2,int flag3)
{
    if(flag2 == GLFW_PRESS)
        WindowManager::instance().onKeyDown(key,false,false,false);
    else if(flag2 == GLFW_RELEASE)
        WindowManager::instance().onKeyUp(key,false,false,false);
}

void GLFWcharfun_(GLFWwindow* window,unsigned int ch)
{
    WindowManager::instance().onChar(ch);
}

void GLFWcharmodsfun_(GLFWwindow* window,unsigned int flag,int data)
{

}

void GLFWdropfun_(GLFWwindow* window,int flag,const char** files)
{

}

void GLFWmonitorfun_(GLFWmonitor* monitor,int flag)
{

}

class DeviceImpl : public Device
{
public:
    DeviceImpl()
    {
        mWindow = 0;
    }

    ~DeviceImpl()
    {
    }

    bool initial()
    {
        int32_t init = glfwInit();
        return init;
    }

    void terminate()
    {
        glfwTerminate();
    }

    bool createWindow(int32_t width,int32_t height,const String& title)
    {
        if(mWindow)
            return false;

        mWindow = glfwCreateWindow(width,height,title.toAnsiString().c_str(),NULL,NULL);

        if(!mWindow)
            return false;

        glfwSetCharCallback(mWindow,GLFWcharfun_);
        glfwSetCursorEnterCallback(mWindow,GLFWcursorenterfun_);
        glfwSetKeyCallback(mWindow,GLFWkeyfun_);
        //glfwSetWindowCloseCallback(mWindow,GLFWwindowclosefun_);
        glfwSetWindowFocusCallback(mWindow,GLFWwindowfocusfun_);
        glfwSetWindowIconifyCallback(mWindow,GLFWwindowiconifyfun_);
        glfwSetWindowPosCallback(mWindow,GLFWwindowposfun_);
        glfwSetWindowSizeCallback(mWindow,GLFWwindowsizefun_);
        glfwSetMouseButtonCallback(mWindow,GLFWmousebuttonfun_);
        glfwSetCursorPosCallback(mWindow,GLFWcursorposfun_);
        glfwSetFramebufferSizeCallback(mWindow,GLFWframebuffersizefun_);
        glfwSetWindowPosCallback(mWindow,GLFWwindowposfun_);

        /*
        setMouseButtonCallback(mMouseButtonCB);
        setCursorPositionCallback(mCursorPositionCB);*/

        glfwMakeContextCurrent(mWindow);
        WindowManager::instance().applicationResized(width,height);
        glfwSwapInterval(1);
        return true;
    }

    void closeWindow()
    {
        if(mWindow)
            glfwDestroyWindow(mWindow);
        mWindow = 0;
    }

    void setWindowSize(int32_t width,int32_t height)
    {
        if(mWindow)
            glfwSetWindowSize(mWindow,width,height);
    }

    void getWindowSize(int32_t& width,int32_t& height)
    {
        if(mWindow)
            glfwGetWindowSize(mWindow,&width,&height);
    }

    bool isRunning()
    {
        if(mWindow == NULL)
            return false;
        return !glfwWindowShouldClose(mWindow);
    }

    void preRender()
    {
        int width,height;
        getWindowSize(width,height);
        glViewport(0,0,width,height);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void swapBuffers()
    {
        if(mWindow)
            glfwSwapBuffers(mWindow);
        else
            err() << "window handle is null!\n";
    }

    void pollEvents()
    {
        glfwPollEvents();
    }

    void deleteLater()
    {
        delete this;
    }
private:
    GLFWwindow* mWindow;
};


Device::Device()
{
}

Device::~Device()
{
}

Device* Device::create()
{
    return new DeviceImpl();
}

}
