#ifndef BGE_GUI_WINDOW_H
#define BGE_GUI_WINDOW_H
#include <BGE/System/Object.h>
#include <BGE/System/String.h>
#include <BGE/System/TinyXml.h>
#include <BGE/System/Math.h>
#include <BGE/System/Sigslot.h>
#include <BGE/Graphics/Color.h>

namespace bge
{

class Message;
class MessageListener;

class BGE_EXPORT_API Window : public Object, public SlotHolder
{
    DECLARE_OBJECT(Window)
public:
    typedef std::list<MessageListener*> MessageListeners;
public:
    explicit Window(Window* parent = 0, const String& text = L"");
protected:
    virtual ~Window();
public:
    String text() const { return text_; }
    Window& setText(const String& text);

    Vector2f size() const;
    Window& setSize(const Vector2f& size);
    Vector2f position() const { return position_; }
    Window& setPosition(const Vector2f& aPosition);

    Window& setGeometry(const FloatRect& rect);
    FloatRect geometry()const{return FloatRect(position_,size_);}
public:
    bool isFocused() const;

    Window& enableFocused();
    Window& disableFocused();

    virtual bool containsFocus() const { return isFocused(); }
public:
    Window& render();

    Window& addMessageListener(MessageListener* listener);
    Window& removeMessageListener(MessageListener* listener);

    Window& show();
    Window& hide();

    Window& enable() { isEnabled_ = true; return*this; }
    Window& disable() { isEnabled_ = false; return*this; }
    bool isEnabled() const { return isEnabled_; }

    bool isVisible() const;

    virtual Vector2f recommendedSize() const = 0;

    virtual bool process(const Message& message);

    virtual Window* windowBelow(const Vector2f& p);

    Window& setAsActiveWindow();

    virtual bool loadAppearance(const std::string& file) = 0;
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);

    virtual bool doesHierarchyContain(Window* other)const{return this == other;}
    Window* topLevel()const;
public:
    Signal0 closed;
protected:
    virtual Window& onDraw() = 0;

    virtual void onResize(){}
    virtual void onMove(){}
    virtual void onTextChanged(){}
    virtual void onShow(){}
    virtual void onHide(){}
    virtual void onGainedFocus(){}
    virtual void onLostFocus(){}
protected:
    void setWidgetType(bool widget);
private:
    MessageListeners messageListeners_;
    String text_;
    Vector2f size_;
    Vector2f position_;
    bool isVisible_;
    bool isEnabled_;
    bool isSizeSetByUser_;
};

#define WINDOW_LOAD_APPAREARANCE(window)\
bool window::loadAppearance(const std::string& file)\
{\
    if(renderDesc_)\
        renderDesc_->deleteLater();\
    renderDesc_ = 0;\
    renderDesc_ = new window##Desc;\
    TiXmlDocument document(file);\
    if(!document.LoadFile())\
        return false;\
    TiXmlElement* root = document.RootElement();\
    renderDesc_->loadProperty(root);\
    return true;\
}

void serialize(const ObjectNamer<Window*>& object,TiXmlNode* node);

}

#endif
