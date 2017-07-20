#include <BGE/GUI/Message.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/WindowManager.h>
#include <glfw/glfw3.h>
#include <iostream>

namespace bge
{

const uint16_t  KeyMessage::Key_F1           = GLFW_KEY_F1;
const uint16_t  KeyMessage::Key_F2           = GLFW_KEY_F2;
const uint16_t  KeyMessage::Key_F3           = GLFW_KEY_F3;
const uint16_t  KeyMessage::Key_F4           = GLFW_KEY_F4;
const uint16_t  KeyMessage::Key_F5           = GLFW_KEY_F5;
const uint16_t  KeyMessage::Key_F6           = GLFW_KEY_F6;
const uint16_t  KeyMessage::Key_F7           = GLFW_KEY_F7;
const uint16_t  KeyMessage::Key_F8           = GLFW_KEY_F8;
const uint16_t  KeyMessage::Key_F9           = GLFW_KEY_F9;
const uint16_t  KeyMessage::Key_F10          = GLFW_KEY_F10;
const uint16_t  KeyMessage::Key_F11          = GLFW_KEY_F11;
const uint16_t  KeyMessage::Key_F12          = GLFW_KEY_F12;
const uint16_t  KeyMessage::Key_LEFT         = GLFW_KEY_LEFT;
const uint16_t  KeyMessage::Key_UP           = GLFW_KEY_UP;
const uint16_t  KeyMessage::Key_RIGHT        = GLFW_KEY_RIGHT;
const uint16_t  KeyMessage::Key_DOWN         = GLFW_KEY_DOWN;
const uint16_t  KeyMessage::Key_PAGE_UP      = GLFW_KEY_PAGE_UP;
const uint16_t  KeyMessage::Key_PAGE_DOWN    = GLFW_KEY_PAGE_DOWN;
const uint16_t  KeyMessage::Key_HOME         = GLFW_KEY_HOME;
const uint16_t  KeyMessage::Key_END          = GLFW_KEY_END;
const uint16_t  KeyMessage::Key_INSERT       = GLFW_KEY_INSERT;
const uint16_t  KeyMessage::Key_DELETE       = GLFW_KEY_DELETE;
const uint16_t  KeyMessage::Key_BACKSPACE    = GLFW_KEY_BACKSPACE;

Message::Message(Id id):id_(id)
{
}

Message::~Message()
{
}

KeyMessage::KeyMessage(Id id,uint16_t character,uint16_t key,bool alt,bool ctrl,bool shift):
    Message(id),
    character_(character),
    key_(key),
    alt_(alt),
    ctrl_(ctrl),
    shift_(shift)
{
}

bool KeyListener::dispatchMessage(const Message& message,Window* receiver)
{
    bool ret = false;
    if(receiver->isFocused())
    {
        switch(message.id())
        {
        case Message::characterInput:
        {
            KeyMessage* keymsg = (KeyMessage*)&message;
            onCharacterInput((dynamic_cast<const KeyMessage&>(message)).character());
            break;
        }
        case Message::keyPressed:
        {
            const KeyMessage& msg = (dynamic_cast<const KeyMessage&>(message));
            onKeyPressed(msg.key(),msg.alt(),msg.ctrl(),msg.shift());
            break;
        }
        case Message::keyReleased:
        {
            const KeyMessage& msg = (dynamic_cast<const KeyMessage&>(message));
            onKeyReleased(msg.key(),msg.alt(),msg.ctrl(),msg.shift());
            break;
        }
        default:
            break; // Default case is don't do anything
        }
    }
    return ret;
}

MouseMessage::MouseMessage(Id id,const Vector2f& position) :
    Message(id),
    position_(position)
{
}

MouseClickMessage::MouseClickMessage(Id id, const Vector2f& position,
    Button button, Event event) :
    MouseMessage(id, position),
    button_(button),
    event_(event)
{
}

MouseListener::MouseListener():
    oldPosition_(-1,-1)
{
}

bool MouseListener::dispatchMessage(const Message& message, Window* receiver)
{
    bool ret = false;

    Message::Id id = message.id();

    if( (id == Message::mouseClick)   ||
        (id == Message::mouseEntered) ||
        (id == Message::mouseExited)  ||
        (id == Message::mouseMoved) )
    {
        const MouseMessage& e = dynamic_cast<const MouseMessage&>(message);
        const Vector2f& newPosition = e.position();

        const bool newPosInWindow = receiver->geometry().contains(newPosition);
        const bool oldPosInWindow = receiver->geometry().contains(oldPosition_);

        const Window * capture = WindowManager::instance().captureWindow();

        if(id == Message::mouseMoved)
        {
            if((!capture && newPosInWindow) || capture == receiver)
                ret = moved((MouseMessage&)(message));

            if( oldPosInWindow && !newPosInWindow )
            {
                MouseMessage rval(Message::mouseExited, newPosition);
                receiver->process(rval);
            }
            else if( !oldPosInWindow && newPosInWindow )
            {
                MouseMessage rval(Message::mouseEntered, newPosition);
                receiver->process(rval);
            }
            oldPosition_ = newPosition;
        }

        switch(id)
        {
        case Message::mouseClick:
            if((!capture && newPosInWindow) || capture == receiver)
                ret = ret || clicked((MouseClickMessage&)(message));
            break;

        case Message::mouseEntered:
            if((!capture && newPosInWindow) || capture == receiver)
                ret = ret || entered((MouseMessage&)(message));
            break;

        case Message::mouseExited:
            if(!capture || capture == receiver)
                ret = ret || exited((MouseMessage&)(message));
            break;

        default:
            break;
        }
    }
    return ret;
}

}
