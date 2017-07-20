#ifndef BGE_GUI_MESSAGE_H
#define BGE_GUI_MESSAGE_H
#include <BGE/System/Math.h>
//#include <BGE/Graphics/Export.h>
#include <string>
#include <BGE/System/String.h>
#include <BGE/System/NonCopyable.h>

namespace bge
{

class Window;

class BGE_EXPORT_API Message
{
public:
    enum Id
    {
        null = -1,
        mouseMoved,     //!< \sa MouseMessage
        mouseClick,     //!< \sa MouseMessage
        mouseEntered,   //!< \sa MouseMessage
        mouseExited,    //!< \sa MouseMessage
        characterInput, //!< \sa KeyMessage
        keyPressed,     //!< \sa KeyMessage
        keyReleased,    //!< \sa KeyMessage
        lostFocus,      //!< \sa FocusMessage
        gainedFocus,    //!< \sa FocusMessage
        sliderMoved,    //!< \sa SlideableMessage
        edit,           //!< \sa EditMessage
        selected,       //!< \sa SelectionMessage
        deselected      //!< \sa SelectionMessage
    };
public:
    Message(Id id = null);
    virtual ~Message();
public:
    Id id() const { return id_; }
private:
    Id id_;
};

class BGE_EXPORT_API KeyMessage : public Message, NonCopyable
{
public:
    KeyMessage(Id id,uint16_t character,uint16_t key,bool alt,bool ctrl,bool shift);
    virtual ~KeyMessage(){}
public:
    uint16_t character() const { return character_; }
    uint16_t key() const { return key_; }
    bool alt()const{return alt_;}
    bool ctrl()const{return ctrl_;}
    bool shift()const{return shift_;}
private:
    uint16_t character_;
    uint16_t key_;
    bool alt_;
    bool ctrl_;
    bool shift_;
public:
    static const uint16_t  Key_F1;
    static const uint16_t  Key_F2;
    static const uint16_t  Key_F3;
    static const uint16_t  Key_F4;
    static const uint16_t  Key_F5;
    static const uint16_t  Key_F6;
    static const uint16_t  Key_F7;
    static const uint16_t  Key_F8;
    static const uint16_t  Key_F9;
    static const uint16_t  Key_F10;
    static const uint16_t  Key_F11;
    static const uint16_t  Key_F12;
    static const uint16_t  Key_LEFT;
    static const uint16_t  Key_UP;
    static const uint16_t  Key_RIGHT;
    static const uint16_t  Key_DOWN;
    static const uint16_t  Key_PAGE_UP;
    static const uint16_t  Key_PAGE_DOWN;
    static const uint16_t  Key_HOME;
    static const uint16_t  Key_END;
    static const uint16_t  Key_INSERT;
    static const uint16_t  Key_DELETE;
    static const uint16_t  Key_BACKSPACE;
};

class BGE_EXPORT_API MouseMessage : public Message
{
public:
    MouseMessage(Id id,const Vector2f& position);
    virtual ~MouseMessage() {}
public:
    Vector2f position() const { return position_; }
    float x() const { return position().x_; }
    float y() const { return position().y_; }
private:
    Vector2f position_;
};

class BGE_EXPORT_API MouseClickMessage : public MouseMessage
{
public:
    enum Button { left, middle, right };
    enum Event { pressed, released, doubleClicked };
public:
    MouseClickMessage(Id id, const Vector2f& position, Button button, Event event);
    virtual ~MouseClickMessage() {}
public:
    Button button() const { return button_; }
    Event event() const { return event_; }
private:
    Button button_;
    Event event_;
};

class BGE_EXPORT_API EditMessage : public Message
{
public:
    EditMessage(const String& text) : Message(Message::edit), text_(text) {}
    virtual ~EditMessage() {}
public:
    String text() const { return text_; }
private:
    String text_;
};

class BGE_EXPORT_API SlideableMessage : public Message
{
public:
    SlideableMessage(int value) : Message(sliderMoved), value_(value) {}
    virtual ~SlideableMessage() {}
public:
    int value() const { return value_; }
private:
    int value_;
};

class BGE_EXPORT_API SelectionMessage : public Message
{
public:
    SelectionMessage(Id id, uint32_t index) :
        Message(id), index_(index) {}
    virtual ~SelectionMessage() {}
public:
    uint32_t itemIndex() const { return index_; }
private:
    uint32_t index_;
};

class BGE_EXPORT_API MessageListener
{
public:
    virtual ~MessageListener(){}
public:
    virtual bool dispatchMessage(const Message& message,Window* receiver)
    {
        return false;
    }
};

class BGE_EXPORT_API KeyListener : public MessageListener
{
public:
    virtual ~KeyListener(){}
public:
    virtual bool onCharacterInput(uint16_t) { return false; }
    virtual bool onKeyPressed(uint16_t,bool,bool,bool) { return false; }
    virtual bool onKeyReleased(uint16_t,bool,bool,bool) { return false; }
    virtual bool dispatchMessage(const Message& message,Window* receiver);
};

class BGE_EXPORT_API MouseListener : public MessageListener
{
public:
    MouseListener();
    virtual ~MouseListener() {}
public:
    virtual bool clicked(MouseClickMessage& ) { return false; }
    virtual bool entered(MouseMessage& ) { return false; }
    virtual bool exited(MouseMessage& ) { return false; }
    virtual bool moved(MouseMessage& ) { return false; }
    virtual bool dispatchMessage(const Message& message, Window* receiver);
protected:
    Vector2f oldPosition_;
};

}

#endif // BGE_GUI_MESSAGE_H
