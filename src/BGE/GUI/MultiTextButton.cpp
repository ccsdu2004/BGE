#include <BGE/GUI/MultiTextButton.h>
#include <BGE/GUI/Message.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

class MTButtonMouseListener : public MouseListener
{
public:
    MTButtonMouseListener(MultiTextButton& button) : button_(button) {}
public:
    virtual bool clicked(MouseClickMessage& message)
    {
        bool isHandled = false;
        if(message.button() == MouseClickMessage::right)
        {
            isHandled = true;
            if(message.event() == MouseClickMessage::pressed)
            {
                if(WindowManager::instance().captureWindow() != &button_)
                    WindowManager::instance().setMouseCapture(&button_);
                button_.captureCount_++;
                button_.state_ = BasicButton::down;
            }
            else if(message.event() == MouseClickMessage::released)
            {
                if(&button_ == button_.windowBelow(message.position()))
                {
                    button_.state_ = BasicButton::rollOver;
                    button_.onPressedRight();
                }
                else
                {
                    button_.state_ = BasicButton::up;
                }
                if(button_.captureCount_ == 1)
                    WindowManager::instance().releaseMouseCapture();
                if(button_.captureCount_ > 0)
                    button_.captureCount_--;
            }
        }
        return isHandled;
    }
protected:
    MultiTextButton& button_;
    MTButtonMouseListener(const MTButtonMouseListener& anMTButtonMouseListener);
    MTButtonMouseListener& operator = (const MTButtonMouseListener& anMTButtonMouseListener);
};

IMPLEMENT_OBJECT_CREATE(MultiTextButton,BasicButton)

MultiTextButton::MultiTextButton(Window* parent,const String& text):
    BasicButton(parent,text)
{
    addString(text);
    stringIndex_ = 0;

    mtButtonMouseListener_ = new MTButtonMouseListener(*this);
    addMessageListener(mtButtonMouseListener_);
    released.connect(this, &MultiTextButton::onPressedLeft);
}

MultiTextButton::~MultiTextButton()
{
    removeMessageListener(mtButtonMouseListener_);
    delete mtButtonMouseListener_;
}

void MultiTextButton::deleteLater()
{
    removeMessageListener(mtButtonMouseListener_);
    delete mtButtonMouseListener_;
	BasicButton::deleteLater();
}

MultiTextButton& MultiTextButton::onPressedLeft()
{
    if(numberOfStrings() == 0)
        setText(L"");
    else
    {
        stringIndex_++;
        if(stringIndex_ >= numberOfStrings())
            stringIndex_ = 0;

        setText(stringAt(stringIndex_));
        selectionChanged(text());
    }

    return *this;
}

MultiTextButton& MultiTextButton::onPressedRight()
{
    if(numberOfStrings() == 0)
        setText(L"");
    else
    {
        if(stringIndex_ == 0)
            stringIndex_ = numberOfStrings();
        stringIndex_--;

        setText(stringAt(stringIndex_));
        selectionChanged(text());
    }
    return *this;
}

Window& MultiTextButton::onDraw()
{
    drawStandardButton();
    return *this;
}

bool MultiTextButton::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT_CONTAINER(strings,node,std::list<String>,String)
    TiXmlNode* window = node->FirstChildElement("BasicButton");
    BasicButton::loadProperty(window);
    return true;
}

bool MultiTextButton::saveProperty(TiXmlNode* node)
{
    SERIALIZE_CONTAINER(strings,node)
    TiXmlElement element("BasicButton");
    BasicButton::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
