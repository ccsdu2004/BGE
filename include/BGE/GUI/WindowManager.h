#ifndef BGE_GUI_WINDOWMANAGER_H
#define BGE_GUI_WINDOWMANAGER_H
#include <BGE/System/String.h>
#include <BGE/System/Math.h>
#include <BGE/System/NonCopyable.h>
#include <BGE/System/Sigslot.h>
#include <BGE/Graphics/Color.h>
#include <BGE/Graphics/Canvas.h>
#include <BGE/GUI/Dock.h>

namespace bge
{

class Window;
class Timer;
class Message;
class PropertyScheme;
class MouseClickMessage;

class BGE_EXPORT_API WindowManager : NonCopyable
{
public:
    virtual ~WindowManager();
public:
    virtual Window* windowBelowCursor() const = 0;
    virtual Window* oldWindowBelowCursor() const = 0;
    virtual Window* focusedWindow() const = 0;
    virtual void setFocusedWindow(Window* window) = 0;
public:
    static WindowManager& instance();

    virtual WindowManager& initialize(const std::string& font,bool autoDelete = false) = 0;
    virtual void terminate() = 0;

    virtual Window* createWindow(const std::string& file) = 0;
	virtual bool saveWindow(Window* window,const std::string& file) = 0;

    virtual WindowManager& addWindow(Window* window) = 0;
    virtual Window* popUpWindow() const = 0;
    virtual WindowManager& removeAllWindows() = 0;
    virtual WindowManager& removeWindow(Window* window) = 0;
    virtual bool isTopLevelWindow(Window* window) const = 0;
    virtual WindowManager& addTimer(Timer* timer) = 0;
    virtual WindowManager& removeTimer(Timer* timer) = 0;
    virtual WindowManager& checkForWindowBelowCursor() = 0;
    virtual const WindowManager& update() const = 0;
    virtual WindowManager& setMouseCapture(Window* window) = 0;
    virtual WindowManager& releaseMouseCapture() = 0;
    virtual Window* captureWindow() const = 0;
    virtual WindowManager& setActiveWindow(Window* window) = 0;
    virtual WindowManager& applicationResized(int x,int y) = 0;

    virtual WindowManager& copyToClipboard(const String& str) = 0;
    virtual String clipboardContents() const = 0;
    virtual Vector2f applicationSize() const = 0;
    virtual std::string defaultFont() = 0;
    virtual WindowManager& setPopUpWindow(Window* window) = 0;
    virtual WindowManager& loadPropertyScheme(const std::string& file) = 0;
public:
    virtual bool onLeftButtonDown  (int x, int y) = 0;
    virtual bool onLeftButtonUp    (int x, int y) = 0;
    virtual bool onMiddleButtonDown(int x, int y)= 0;
    virtual bool onMiddleButtonUp  (int x, int y) = 0;
    virtual bool onRightButtonDown (int x, int y) = 0;
    virtual bool onRightButtonUp   (int x, int y) = 0;
    virtual bool onMouseMove       (int x, int y) = 0;
    virtual bool onChar(uint16_t charCode) = 0;
    virtual bool onKeyDown(uint16_t keyCode,bool alt,bool ctrl,bool shift) = 0;
    virtual bool onKeyUp(uint16_t  keyCode,bool alt,bool ctrl,bool shift) = 0;
    virtual void setDock(Window* window,const Dock& d) = 0;
public:
    virtual Canvas* canvas()const = 0;
    virtual PropertyScheme* propertyScheme()const = 0;
public:
    Signal1<const String&> clipboardContentsChanged;
};

}

#endif
