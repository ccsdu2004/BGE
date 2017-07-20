#include <BGE/System/Err.h>
#include <BGE/System/Serialization.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

IMPLEMENT_OBJECT(Window,Object)

Window::Window(Window* parent,const String& text):
    Object(parent),
    text_(text),
    size_(Vector2f(0, 0)),
    position_(Vector2f(0, 0)),
    isVisible_(true),
    isEnabled_(true),
    isSizeSetByUser_(false)
{
    setWidgetType(true);
}

Window::~Window()
{
}

Window& Window::setGeometry(const FloatRect& rect)
{
    setSize(rect.size());
    setPosition(rect.position());
    return *this;
}

Vector2f Window::size() const
{
    return (isSizeSetByUser_) ? size_ : recommendedSize();
}

Window& Window::setText(const String& text)
{
    text_ = text;
    onTextChanged();
    return *this;
}

Window& Window::addMessageListener(MessageListener* listener)
{
    messageListeners_.push_back(listener);
    return *this;
}

Window& Window::removeMessageListener(MessageListener* listener)
{
    messageListeners_.remove(listener);
    /*MessageListeners::iterator it;
    for(it = messageListeners_.begin(); it != messageListeners_.end(); ++it)
    {
        MessageListener* h = *it;
        if(h == listener)
        {
            messageListeners_.erase(it);
            break;
        }
    }*/
    return *this;
}

bool Window::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        MessageListeners::iterator it;
        for(it = messageListeners_.begin(); it != messageListeners_.end(); ++it)
        {
            MessageListener* listener = *it;
            ret = ret || listener->dispatchMessage(message,this);
        }
    }
    return ret;
}

Window* Window::windowBelow(const Vector2f& p)
{
    Window* ret = 0;
    if(isVisible())
    {
        if(geometry().contains(p))
            ret = this;
    }
    return ret;
}

Window& Window::show()
{
    isVisible_ = true;
    WindowManager::instance().checkForWindowBelowCursor();
    onShow();
    return *this;
}

Window& Window::hide()
{
    isVisible_ = false;
    WindowManager::instance().checkForWindowBelowCursor();
    onHide();
    return *this;
}

bool Window::isVisible() const
{
    return isVisible_;
}

Window& Window::render()
{
    if(isVisible())
    {
        onDraw();
        if(!isEnabled())
        {
            Canvas* canvas = WindowManager::instance().canvas();
            canvas->setColor(Color(0.5f,0.6f,0.5f,0.55f));
            canvas->drawRectangle(geometry(), true);
        }
    }
    return *this;
}

Window& Window::setSize(const Vector2f& aSize)
{
    size_ = aSize;
    onResize();
    isSizeSetByUser_ = true;
    return *this;
}

Window& Window::setPosition(const Vector2f& position)
{
    position_ = position;
    onMove();
    return *this;
}

Window& Window::enableFocused()
{
    onGainedFocus();
    return *this;
}

Window& Window::disableFocused()
{
    onLostFocus();
    return *this;
}

bool Window::isFocused() const
{
    return (WindowManager::instance().focusedWindow() == this);
}

Window& Window::setAsActiveWindow()
{
    WindowManager::instance().setActiveWindow(this);
    return *this;
}

Window* Window::topLevel()const
{
    Window* ret = (Window*)this;
    Window* widget = ret;
    while(widget != 0)
    {
        ret = widget;
        widget = Object::castTo<Window>(ret->parent());
    }
    return ret;
}

void Window::setWidgetType(bool widget)
{
    if(!widget)
    {
        err() << "window must to be widget type\n";
        return;
    }
    Object::setWidgetType(true);
}

bool Window::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(text,node,String)
    DESERIALIZE_OBJECT(position,node,Vector2<float>)
    DESERIALIZE_OBJECT(size,node,Vector2<float>)
    DESERIALIZE_OBJECT(isVisible,node,bool)
    DESERIALIZE_OBJECT(isEnabled,node,bool)
    TiXmlNode* window = node->FirstChildElement("Object");
    Object::loadProperty(window);
    return true;
}

bool Window::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(text,node)
    SERIALIZE_OBJECT(size,node)
    SERIALIZE_OBJECT(position,node)
    SERIALIZE_OBJECT(isVisible,node)
    SERIALIZE_OBJECT(isEnabled,node)
    TiXmlElement element("Object");
    Object::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void serialize(const ObjectNamer<Window*>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    object.value()->saveProperty(&element);
    node->InsertEndChild(element);
}

}

