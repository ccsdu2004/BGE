#include <BGE/System/Timer.h>
#include <BGE/System/TimeManager.h>
#include <BGE/System/Err.h>
#include <BGE/Graphics/OpenGL.h>
#include <BGE/Graphics/CanvasImpl.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/PropertyScheme.h>
#include <algorithm>
#include <functional>

namespace bge
{

WindowManager::~WindowManager()
{
}

struct TopLevelWindow
{
    Window* window;
    int     zVal;
    Dock    dock;

    TopLevelWindow(Window* widget,int z,const Dock* d=0) :
        window(widget),
        zVal(z)
    {
        if(d)
            dock=*d;
    }

    TopLevelWindow(const TopLevelWindow& tlw)
    {
        (*this) = tlw;
    }

    TopLevelWindow& operator = (const TopLevelWindow& tlw)
    {
        window = tlw.window;
        zVal = tlw.zVal;
        dock = tlw.dock;
        return *this;
    }

    bool operator == (const TopLevelWindow& tlw) const
    {
        return ((window == tlw.window) /*&& (zVal == tlw.zVal)*/);
    }

    bool operator != (const TopLevelWindow& tlw) const
    {
        return !((*this) == tlw);
    }

    bool operator > (const TopLevelWindow& tlw) const
    {
        return (zVal > tlw.zVal);
    }
};

class WindowManagerImpl : public WindowManager
{
    typedef std::list<TopLevelWindow>  Windows;
    typedef std::list<Timer*>          Timers;
public:
    WindowManagerImpl();
    virtual ~WindowManagerImpl();
public:
    Window* windowBelowCursor()const{return windowBelowCursor_;}
    Window* oldWindowBelowCursor()const{return oldWindowBelowCursor_;}
    Window* focusedWindow() const{return focusedWindow_;}
    void setFocusedWindow(Window* window);
public:
    WindowManager& initialize(const std::string& font,bool autoDelete = false);
    void terminate();
    void setAutoDelete(){autoDelete_ = true;}

    WindowManager& addWindow(Window* window);
    Window* popUpWindow()const;
    WindowManager& removeAllWindows();
    WindowManager& removeWindow(Window* window);
    bool isTopLevelWindow(Window* window)const;
    WindowManager& setMouseCapture(Window* window);
    WindowManager& releaseMouseCapture();
    Window* captureWindow() const { return windowCapture_; }
    WindowManager& setActiveWindow(Window* aWindow);
    WindowManager& setPopUpWindow(Window* aWindow)
    {
        popUpWindow_ = aWindow;
        return *this;
    }
    WindowManager& addTimer(Timer* timer)
    {
        timers_.push_back(timer);
        return *this;
    }

    WindowManager& removeTimer(Timer* timer);
    WindowManager& checkForWindowBelowCursor();
    const WindowManager& update()const;

    WindowManager& applicationResized(int x,int y);

    WindowManager& copyToClipboard(const String& str);
    String clipboardContents() const;
    Vector2f applicationSize() const { return applicationSize_; }

    std::string defaultFont() { return defaultFont_; }

    WindowManager& loadPropertyScheme(const std::string& font);

    Window* createWindow(const std::string& file);
	bool saveWindow(Window* window,const std::string& file);
public:
    bool onLeftButtonDown  (int x, int y);
    bool onLeftButtonUp    (int x, int y);
    bool onMiddleButtonDown(int x, int y);
    bool onMiddleButtonUp  (int x, int y);
    bool onRightButtonDown (int x, int y);
    bool onRightButtonUp   (int x, int y);
    bool onMouseMove(int x, int y);
    bool onChar(uint16_t ch);
    bool onKeyDown (uint16_t key,bool alt,bool ctrl,bool shift);
    bool onKeyUp   (uint16_t key,bool alt,bool ctrl,bool shift);

    void setDock(Window* window,const Dock& d);
public:
    Canvas* canvas()const{return canvas_;}
    PropertyScheme* propertyScheme()const { return propertyScheme_; }
public:
    Signal1<const String&> clipboardContentsChanged;
private:
    const bool process(Message& message)const;
    bool processMouseMessage(MouseClickMessage& msg);
    bool isLastClickDoubleClick()const;
    void updateFocusIfNecessary();
    void triggerElapsedTimers()const;
    int zValueOf(Window* window)const;
    int largestZValue()const;
private:
    std::string defaultFont_;
    Windows windows_;
    Timers timers_;
    Vector2f applicationSize_;
    Vector2f cursorPos_;
    Vector2f lastClickPos_;
    float lastClickTime_;
    int lastClickButton_;
    Window* windowBelowCursor_;
    Window* oldWindowBelowCursor_;
    Window* focusedWindow_;
    Window* windowCapture_;
    Window* popUpWindow_;
    PropertyScheme* propertyScheme_;
    String clipboard_;
    Canvas* canvas_;
    bool autoDelete_;

    static const float doubleClickTimeThreshold_;
    static const float doubleClickMoveThreshold_;
};

const float WindowManagerImpl::doubleClickTimeThreshold_ = 0.3f;
const float WindowManagerImpl::doubleClickMoveThreshold_ = 5.0f;

WindowManager& WindowManager::instance()
{
    static WindowManagerImpl impl;
    return impl;
}

WindowManagerImpl::WindowManagerImpl():
    lastClickTime_(0),
    lastClickButton_(-1),
    windowBelowCursor_(0),
    focusedWindow_(0),
    windowCapture_(0),
    popUpWindow_(0),
    propertyScheme_(0),
    canvas_(0),
    autoDelete_(false)
{
}

WindowManagerImpl::~WindowManagerImpl()
{
}

WindowManager& WindowManagerImpl::initialize(const std::string& font,bool autoDelete)
{
    defaultFont_ = font;
    autoDelete_ = autoDelete;
    canvas_ = new CanvasImpl();
    propertyScheme_ = new PropertyScheme;
    return *this;
}

const WindowManager& WindowManagerImpl::update() const
{
    triggerElapsedTimers();

    canvas()->preRender(applicationSize_.x(),applicationSize_.y());

    for(auto wit = windows_.rbegin(); wit != windows_.rend(); ++wit)
    {
        const TopLevelWindow& tlw = *wit;
        tlw.window->render();
    }

    if(popUpWindow_ != 0)
        popUpWindow_->render();

    canvas()->postRender();
    return *this;
}

WindowManager& WindowManagerImpl::addWindow(Window* window)
{
    TopLevelWindow tlw(window,largestZValue() + 1);
    windows_.push_back(tlw);
    windows_.sort(std::greater<TopLevelWindow>());
    return *this;
}

WindowManager& WindowManagerImpl::removeAllWindows()
{
    windowBelowCursor_ = 0;
    focusedWindow_ = 0;
    windows_.clear();
    return*this;
}

WindowManager& WindowManagerImpl::removeWindow(Window* window)
{
    if(isTopLevelWindow(window))
    {
        TopLevelWindow tlw(window,zValueOf(window));
        windows_.remove(tlw);
        if(windowBelowCursor() != 0)
        {
            if(window->doesHierarchyContain(windowBelowCursor()))
                checkForWindowBelowCursor();
        }
        if(focusedWindow() != 0)
        {
            if(window->doesHierarchyContain(focusedWindow()))
                focusedWindow_ = 0;
        }
    }
    return *this;
}

WindowManager& WindowManagerImpl::removeTimer(Timer* timer)
{
    Timers::iterator it;
    bool isFound = false;
    for(it = timers_.begin(); it != timers_.end(); ++it)
    {
        Timer* holder = *it;
        if(holder == timer)
        {
            isFound = true;
            break;
        }
    }

    if(isFound)
        timers_.erase(it);

    return *this;
}

bool WindowManagerImpl::isTopLevelWindow(Window* window) const
{
    bool ret = false;
    for(Windows::const_iterator it = windows_.begin(); it != windows_.end(); ++it)
    {
        if((*it).window == window)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

const bool WindowManagerImpl::process(Message& message)const
{
    bool ret = false;
    if((popUpWindow() != 0) && (popUpWindow()->isVisible()) )
    {
        popUpWindow()->process(message);
    }
    else
    {
        for(Windows::const_iterator it = windows_.begin(); it != windows_.end(); ++it)
        {
            ret = ret || (*it).window->process(message);
        }
    }

    return ret;
}

WindowManager& WindowManagerImpl::setMouseCapture(Window* window)
{
    windowCapture_ = window;
    return *this;
}

WindowManager& WindowManagerImpl::releaseMouseCapture()
{
    windowCapture_ = 0;
    return *this;
}

bool WindowManagerImpl::processMouseMessage(MouseClickMessage& msg)
{
    bool isHandled = false;
    if(windowCapture_ != 0)
    {
        isHandled = windowCapture_->process(msg);
    }
    else
    {
        if(windowBelowCursor() != 0)
            isHandled = windowBelowCursor()->process(msg);
        else
            isHandled = process(msg); // XT : standard processing
    }
    return isHandled;
}

bool WindowManagerImpl::onLeftButtonDown(int x,int y)
{
    cursorPos_ = Vector2f(x,y);

    MouseClickMessage::Event evt = MouseClickMessage::pressed;
    if( lastClickButton_ == MouseClickMessage::left && isLastClickDoubleClick() )
    {
        evt = MouseClickMessage::doubleClicked;
    }
    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x, y), MouseClickMessage::left, evt);
    bool isHandled = processMouseMessage(rval);

    updateFocusIfNecessary();

    if(evt != MouseClickMessage::doubleClicked)
    {
        lastClickButton_ = MouseClickMessage::left;
        lastClickTime_ = TimeManager::systemTime();
        lastClickPos_ = cursorPos_;
    }
    else
    {
        lastClickButton_ = -1;
    }

    return isHandled;
}

bool WindowManagerImpl::onLeftButtonUp(int x,int y)
{
    cursorPos_ = Vector2f(x,y);
    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x,y), MouseClickMessage::left, MouseClickMessage::released);
    return processMouseMessage(rval);
}

bool WindowManagerImpl::onMiddleButtonDown(int x,int y)
{
    cursorPos_ = Vector2f(x,y);

    MouseClickMessage::Event  evt = MouseClickMessage::pressed;
    if( lastClickButton_ == MouseClickMessage::middle && isLastClickDoubleClick() )
    {
        evt = MouseClickMessage::doubleClicked;
    }

    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x,y), MouseClickMessage::middle,evt);
    bool isHandled = processMouseMessage(rval);

    if( evt != MouseClickMessage::doubleClicked )
    {
        lastClickButton_ = MouseClickMessage::middle;
        lastClickTime_ = TimeManager::systemTime();
        lastClickPos_ = cursorPos_;
    }
    else
    {
        lastClickButton_ = -1;
    }

    return isHandled;
}

bool WindowManagerImpl::onMiddleButtonUp(int x,int y)
{
    cursorPos_ = Vector2f(x,y);
    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x,y),MouseClickMessage::middle, MouseClickMessage::released);
    return processMouseMessage(rval);
}

bool WindowManagerImpl::onRightButtonDown(int x,int y)
{
    cursorPos_ = Vector2f(x, y);

    bge::MouseClickMessage::Event evt = MouseClickMessage::pressed;
    if(lastClickButton_ == MouseClickMessage::right && isLastClickDoubleClick() )
    {
        evt = MouseClickMessage::doubleClicked;
    }

    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x, y), MouseClickMessage::right, evt);
    bool isHandled = processMouseMessage(rval);

    if(evt != MouseClickMessage::doubleClicked)
    {
        lastClickButton_ = MouseClickMessage::right;
        lastClickTime_ = TimeManager::systemTime();
        lastClickPos_ = cursorPos_;
    }
    else
    {
        lastClickButton_ = -1;
    }

    return isHandled;
}

bool WindowManagerImpl::onRightButtonUp(int x,int y)
{
    cursorPos_ = Vector2f(x, y);
    checkForWindowBelowCursor();
    MouseClickMessage rval(Message::mouseClick, Vector2f(x, y), MouseClickMessage::right, MouseClickMessage::released);
    return processMouseMessage(rval);
}

bool WindowManagerImpl::onMouseMove(int x,int y)
{
    cursorPos_ = Vector2f(x,y);
    checkForWindowBelowCursor();
    MouseMessage rval(Message::mouseMoved,Vector2f(x, y));
    bool isHandled = false;
    if(windowCapture_)
    {
        isHandled = windowCapture_->process(rval);
    }
    else
    {
        if(windowBelowCursor() != 0)
            isHandled = /*windowBelowCursor()->*/process(rval);
        else
            isHandled = process(rval); // XT : standard processing
    }

    return isHandled;
}

bool WindowManagerImpl::onChar(uint16_t ch)
{
    KeyMessage rval(Message::characterInput,ch,0,false,false,false);
    return process(rval);
}

bool WindowManagerImpl::onKeyDown(uint16_t key,bool alt,bool ctrl,bool shift)
{
    KeyMessage rval(Message::keyPressed,0,key,alt,ctrl,shift);
    return process(rval);
}

bool WindowManagerImpl::onKeyUp(uint16_t key,bool alt,bool ctrl,bool shift)
{
    KeyMessage rval(Message::keyReleased,0,key,alt,ctrl,shift);
    return process(rval);
}

WindowManager& WindowManagerImpl::checkForWindowBelowCursor()
{
    Windows::const_iterator it;
    bool done = false;
    if(popUpWindow_ != 0)
    {
        if(popUpWindow_->doesHierarchyContain(popUpWindow_->windowBelow(cursorPos_)))
        {
            windowBelowCursor_ = popUpWindow_;
            done = true;
        }
    }

    if(!done)
    {
        Window* wbc = 0;
        for(it = windows_.begin(); it != windows_.end(); ++it)
        {
            Window* windowToTest = (*it).window;

            if(windowToTest->isVisible())
            {
                Window* w = windowToTest->windowBelow(cursorPos_);
                if(w != 0)
                {
                    wbc = w;
                    break;
                }
            }
        }
        windowBelowCursor_ = wbc;
    }

    return *this;
}

void WindowManagerImpl::terminate()
{
    if(autoDelete_)
    {
        auto itr = windows_.begin();
        while(itr != windows_.end())
        {
            TopLevelWindow window = *itr;
            window.window->deleteLater();
            window.window = 0;
            itr ++;
        }
        windows_.clear();
    }

    delete propertyScheme_;
    propertyScheme_ = 0;
    delete canvas_;
    canvas_ = 0;
}

int WindowManagerImpl::zValueOf(Window* window)const
{
    if(!isTopLevelWindow(window))
        return -1;

    int ret = 0;
    for(Windows::const_iterator it = windows_.begin(); it != windows_.end(); ++it)
    {
        const TopLevelWindow& tlw = *it;
        if(tlw.window == window)
        {
            ret = tlw.zVal;
            break;
        }
    }

    return ret;
}

int WindowManagerImpl::largestZValue()const
{
    int ret = 0;
    if(windows_.size() > 0)
    {
        Windows::const_iterator it = windows_.begin();
        ret = (*it).zVal;
        while(it != windows_.end())
        {
            const TopLevelWindow& tlw = *it;
            if(tlw.zVal > ret) ret = tlw.zVal;
            ++it;
        }
    }
    return ret;
}

WindowManager& WindowManagerImpl::setActiveWindow(Window* window)
{
    Window* topLevelWindow = window->topLevel();
    if(isTopLevelWindow(topLevelWindow))
    {
        int zVal = zValueOf(topLevelWindow);
        auto it = std::find(windows_.begin(),windows_.end(),TopLevelWindow(window,zVal));
        if ( it != windows_.end() )
        {
            TopLevelWindow& tlw = *it;
            tlw.zVal = largestZValue() + 1;
        }
    }

    windows_.sort(std::greater<TopLevelWindow>());

    int z = 0;
    for(auto it = windows_.rbegin(); it != windows_.rend(); ++it)
    {
        TopLevelWindow& tlw = *it;
        tlw.zVal = z;
        z++;
    }

    checkForWindowBelowCursor();
    return *this;
}

WindowManager& WindowManagerImpl::loadPropertyScheme(const std::string& file)
{
    delete propertyScheme_;
    propertyScheme_ = new PropertyScheme;
    propertyScheme_->load(file);
    return *this;
}

WindowManager& WindowManagerImpl::copyToClipboard(const String& str)
{
    clipboard_ = str;
    clipboardContentsChanged(str);
    return *this;
}

String  WindowManagerImpl::clipboardContents() const
{
    return clipboard_;
}

Window* WindowManagerImpl::popUpWindow()const
{
    return popUpWindow_;
}

WindowManager& WindowManagerImpl::applicationResized(int x,int y)
{
    applicationSize_ = Vector2f(x,y);

    for(Windows::const_iterator it = windows_.begin(); it != windows_.end(); ++it)
    {
        const Dock& dock = (*it).dock;
        if(dock.docked())
        {
            Window* window = (*it).window;
            Vector2f pos = window->position();
            dock.computePosition(pos,window->size(),Vector2f(x,y));
            window->setPosition(pos);
        }
        else
        {
            Window* window = (*it).window;
            Vector2f pos = window->position();
            window->setPosition(pos);
        }
    }
    return *this;
}

void WindowManagerImpl::setDock(Window* window,const Dock& d)
{
    if(!isTopLevelWindow(window))
        return;

    Windows::iterator it = std::find(windows_.begin(), windows_.end(), TopLevelWindow(window,0));
    if(it == windows_.end())
        return;
    (*it).dock = d;
}

bool WindowManagerImpl::isLastClickDoubleClick() const
{
    return ((TimeManager::systemTime() - lastClickTime_ < doubleClickTimeThreshold_) &&
        (abs(lastClickPos_.x()-cursorPos_.x()) < doubleClickMoveThreshold_) &&
        (abs(lastClickPos_.y()-cursorPos_.y()) < doubleClickMoveThreshold_) );
}

void WindowManagerImpl::updateFocusIfNecessary()
{
    if((popUpWindow() == 0) || (!popUpWindow()->geometry().contains(cursorPos_)))
    {
        if(windowBelowCursor() != focusedWindow())
        {
            setFocusedWindow(windowBelowCursor());
        }
    }
}

void WindowManagerImpl::setFocusedWindow(Window* window)
{
    if(window != focusedWindow())
    {
        if(focusedWindow() != 0)
            focusedWindow()->disableFocused();
        focusedWindow_ = window;
        if(window != 0)
            window->enableFocused();
    }
}

void WindowManagerImpl::triggerElapsedTimers()const
{
    Timers::const_iterator it;
    for(it = timers_.begin(); it != timers_.end(); ++it)
    {
        Timer* timer = *it;
        if(timer->isElapsed())
        {
            timer->trigger();
        }
    }
}

bool WindowManagerImpl::saveWindow(Window* window,const std::string& file)
{
    if(!window)    
		return false;

    TiXmlDocument document(file);
    TiXmlElement root("window");
	root.SetAttribute("class",window->objectName());
    window->saveProperty(&root);
    document.InsertEndChild(root);
    document.SaveFile();
	return true;
}

Window* WindowManagerImpl::createWindow(const std::string& file)
{
    TiXmlDocument document(file);
    if(!document.LoadFile())
	{
		err() << "load file "<<file<<" failed\n";
        return NULL;
	}
	TiXmlElement* root = document.RootElement();
    if(!root)
	{
		err() << "get "<<file<<" root element of xml failed\n";
        return NULL;
	}
	std::string classname = root->Attribute("class");
    Rtti* rtti = Rtti::rttiByObjectName(classname);
    if(!rtti)
	{
		err() << "get "<<classname << "'s rtti failed\n";
        return NULL;
	}
	Object* object = rtti->createObject();
	if(!object)
	{
	    err() <<"create object from class:"<<classname <<" failed\n";
		return NULL;
	}
    Window* window = Object::castTo<Window>(object);
    if(window)
        window->loadProperty(root);
    else
        err()<<"create class:"<<classname<<" failed\n";
    return window;
}

}

