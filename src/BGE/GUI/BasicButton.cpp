#include <BGE/System/NonCopyable.h>
#include <BGE/System/Serialization.h>
#include <BGE/System/Err.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/Message.h>

namespace bge
{

IMPLEMENT_OBJECT(ButtonGroup,Object)

bool ButtonGroup::loadProperty(TiXmlNode* node)
{
	if(!parent())
	{
		err() << "ButtonGroup need parent\n";
		return false;
	}

	TiXmlElement* item = node->FirstChildElement("buttons");
	if(item)
		item = item->FirstChildElement("button");
    while(item)
    {
		BasicButton* button = Object::castTo<BasicButton>(parent()->findChild(item->GetText()));
		if(button)
			buttons_.push_back(button);
		else
			err() << "ButtonGroup parse button failed.\n";
		item = item->NextSiblingElement();
    }
    TiXmlNode* object = node->FirstChildElement("Object");
    Object::loadProperty(object);
    return true;    
}

bool ButtonGroup::saveProperty(TiXmlNode* node)
{
    TiXmlElement buttons("buttons");
	
	auto itr = buttons_.begin();
	while(itr != buttons_.end())
	{
		std::string button_ = (*itr)->name(); 
		if(button_.empty())
			err()<<" BasicButton must have name for savePropry\n";
		SERIALIZE_OBJECT(button,&buttons) 
		itr ++;
	}
    
	node->InsertEndChild(buttons);

    TiXmlElement element("Object");
    Object::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

IMPLEMENT_OBJECT(BasicButton,Window)

class ButtonMouseListener : public MouseListener
{
public:
    ButtonMouseListener(BasicButton& button) : button_(button) {}
public:
    virtual bool clicked(MouseClickMessage& message)
    {
        bool isHandled = false;
        if(message.button() == MouseClickMessage::left)
        {
            isHandled = true;
            if( (message.event() == MouseClickMessage::pressed) ||
                (message.event() == MouseClickMessage::doubleClicked) )
            {
                if(WindowManager::instance().captureWindow() != &button_)
                    WindowManager::instance().setMouseCapture(&button_);
                button_.captureCount_++;
                button_.state_ = BasicButton::down;
                button_.pressed();

            }
            else if(message.event() == MouseClickMessage::released)
            {
                if(&button_ == button_.windowBelow(message.position()))
                {
                    button_.state_ = BasicButton::rollOver;
                    button_.released();
                    button_.clicked();
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

    virtual bool exited(MouseMessage& message)
    {
        if(button_.state_ != BasicButton::down)
            button_.state_ = BasicButton::up;
        else
        {
            if(!button_.keepPressedOnExit_)
            {
                button_.released();
                button_.state_ = BasicButton::up;
            }
        }
        return true;
    }

    virtual bool entered(MouseMessage& message)
    {
        if(button_.state_ != BasicButton::down)
            button_.state_ = BasicButton::rollOver;
        if(button_.captureCount_ > 0)
        {
            button_.state_ = BasicButton::down;
            button_.pressed();
        }
        return true;
    }

    virtual bool moved(MouseMessage& message)
    {
        return button_.state_ == BasicButton::rollOver;
    }
private:
    BasicButton& button_;
};

BasicButton::BasicButton(Window* parent,const String& text,ButtonGroup* group):
    Window(parent,text),
    state_(up),
    captureCount_(0),
    keepPressedOnExit_(false),
    isToggleOn_(false),
    isMouseOver_(false),
    group_(group),
    renderDesc_(0)
{
    buttonMouseListener_ = new ButtonMouseListener(*this);
    addMessageListener(buttonMouseListener_);

    if(group_ != 0)
        group_->buttons_.push_back(this);

    pressed.connect(this,&BasicButton::onPressedLeft);
}

BasicButton::~BasicButton()
{
    removeMessageListener(buttonMouseListener_);
    delete buttonMouseListener_;

    if(group_ != 0)
        group_->buttons_.remove(this);

	if(renderDesc_)
        delete renderDesc_;
}

BasicButton& BasicButton::setMouseOverStatus(bool isMouseOverButton)
{
    if(isToggleOn_)
    {
        if (isMouseOverButton && !isMouseOver_)
        {
            isMouseOver_ = true;

            if(group_ != 0)
            {
                process(SelectionMessage(Message::selected, group_->indexOf(this)));
                ButtonGroup::BasicButtons::iterator it;
                for(it = group_->buttons_.begin(); it != group_->buttons_.end(); ++it)
                {
                    BasicButton* btn = *it;
                    if(btn != this)
                        btn->setMouseOverStatus(false);
                }
            }
            else
            {
                process(SelectionMessage(Message::selected, 0));
                toggled(isMouseOver());
            }
        }
        else if (!isMouseOverButton && isMouseOver_)
        {
            bool canDeselect = true;
            if(group_ != 0)
            {
                canDeselect = false;
                ButtonGroup::BasicButtons::iterator it;
                for(it = group_->buttons_.begin(); it != group_->buttons_.end(); ++it)
                {
                    BasicButton* btn = *it;
                    if(btn != this)
                    {
                        if(btn->isMouseOver()) canDeselect = true;
                    }
                }
            }
            if(canDeselect)
            {
                isMouseOver_ = false;
                if(group_ != 0) {
                    process(SelectionMessage(Message::deselected,group_->indexOf(this)));
                    toggled(isMouseOver());
                }
                else {
                    process(SelectionMessage(Message::deselected, 0));
                    toggled(isMouseOver());
                }
            }
        }
    }
    return *this;
}

BasicButton::State BasicButton::drawState() const
{
    State s = state();

    if(!isToggleOn_)
        return s;

    return s == rollOver ? (isMouseOver()?rollOverOn:rollOver) : (isMouseOver()?down:up);
}

BasicButton& BasicButton::toggle()
{
    if (isToggleOn_)
        setMouseOverStatus(!isMouseOver());
    return *this;
}

BasicButton& BasicButton::onPressedLeft()
{
    if(isToggleOn_)
        setMouseOverStatus(!isMouseOver_);
    return *this;
}

BasicButton& BasicButton::keepPressedOnExit()
{
    keepPressedOnExit_ = true;
    return *this;
}

BasicButton& BasicButton::drawStandardButton()
{
    buttonDesc().draw(geometry(),WindowManager::instance().canvas());
    return *this;
}

Vector2f BasicButton::recommendedSize() const
{
    return buttonDesc().defaultTextSize() + Vector2f(8, 8);
}

BasicButtonDesc& BasicButton::buttonDesc() const
{
    BasicButtonDesc* result = renderDesc_;
    if(result == 0)
        result = &WindowManager::instance().propertyScheme()->buttonDesc_;

    result->setButton(*this);
    return *result;
}

WINDOW_LOAD_APPAREARANCE(BasicButton)

bool BasicButton::loadProperty(TiXmlNode* node)
{
	std::string buttongroup_;
	DESERIALIZE_OBJECT(buttongroup,node,std::string)
	DESERIALIZE_OBJECT(isToggleOn,node,bool)
	if(!buttongroup_.empty() && parent())
	{
	    group_ = Object::castTo<ButtonGroup>(parent()->findChild(buttongroup_));
	}
        
	TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool BasicButton::saveProperty(TiXmlNode* node)
{   
	if(group_)
	{
	    std::string buttongroup_ = group_->name();
		if(buttongroup_.empty())
			err() << "ButtonGroup need name for saveProperty.\n";
        SERIALIZE_OBJECT(buttongroup,node)
	}
	SERIALIZE_OBJECT(isToggleOn,node)
	TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void BasicButton::deleteLater()
{
    if(renderDesc_)
	    renderDesc_->deleteLater();
    renderDesc_ = NULL;
	Window::deleteLater();
}

ButtonStateDesc::ButtonStateDesc():
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    textDesc_(new TextDesc()),
    button_(0)
{
}

ButtonStateDesc::ButtonStateDesc(RenderDesc* background, RenderDesc* border):
    background_(background),
    border_(border),
    textDesc_(new TextDesc()),
    button_(0)
{
}

ButtonStateDesc::~ButtonStateDesc()
{
    delete background_;
    delete border_;
    delete textDesc_;
}

Color ButtonStateDesc::textColor() const
{
    return (textDesc_ != 0) ? textDesc_->color() : Color();
}

void ButtonStateDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    if(background_ != 0)
        background_->draw(area,canvas);
    if(border_ != 0)
        border_->draw(area,canvas);

    if(!button_->text().empty())
    {
        FloatRect rect = area;

        textDesc_->setText(button_->text());

        const bool isCheckBox = dynamic_cast<const CheckBox*>(button_) != 0;

        if(isCheckBox)
        {
            rect.translate(Vector2f(area.size().y(), 0));
            rect.setSize(Vector2f(area.size().x() - area.size().y(), area.size().y()));
        }

        if(button_->state() == BasicButton::down)
        {
            rect.top_ += 1.0f;
            textDesc_->draw(rect,canvas);
        }
        else
            textDesc_->draw(area,canvas);
    }
}

bool ButtonStateDesc::loadProperty(TiXmlNode* node,const char* name)
{
    TiXmlElement* this_ = node->FirstChildElement(name);
    if(!this_)
        return false;

    CREATE_BY_NODE(background,this_,RenderDesc)
    CREATE_BY_NODE(border,this_,RenderDesc)
    CREATE_BY_NODE(textDesc,this_,TextDesc)
    return true;
}

bool ButtonStateDesc::saveProperty(TiXmlNode* node,const char* name)
{
    TiXmlElement thisNode(name);
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    textDesc_->saveProperty(&thisNode,"textDesc");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

IMPLEMENT_OBJECT(BasicButtonDesc,RenderDesc)

BasicButtonDesc::BasicButtonDesc():
    pressedDesc_(new SolidSurfaceDesc(Color(0.2f,0.2f,0.4f)),new RectangleDesc(RectangleStyle_dropped)),
    releasedDesc_(new SolidSurfaceDesc(),new RectangleDesc(RectangleStyle_raised)),
    rollOverDesc_(new SolidSurfaceDesc(Color(0.35f,0.35f,0.5f,1)),new RectangleDesc(RectangleStyle_raised)),
    rollOverOnDesc_(new SolidSurfaceDesc(Color(0.25f,0.25f,0.45f)),new RectangleDesc(RectangleStyle_dropped)),
    arrow_(none),
    button_(0)
{
}

BasicButtonDesc::~BasicButtonDesc()
{
}

Vector2f BasicButtonDesc::defaultTextSize() const
{
    TextDesc& textDesc = releasedDesc_.textDesc();
    textDesc.setText(button_->text());
    return textDesc.textSize() + Vector2f(8.0f, 8.0f);
}

bool BasicButtonDesc::loadProperty(TiXmlNode* node)
{
    TiXmlElement* thisNode = node->FirstChildElement("ButtonDesc");
    if(!thisNode)
        return false;
    
	pressedDesc_.loadProperty(thisNode,"pressedDesc");
    releasedDesc_.loadProperty(thisNode,"releasedDesc");
    rollOverDesc_.loadProperty(thisNode,"rollOverDesc");
    rollOverOnDesc_.loadProperty(thisNode,"rollOverOnDesc");
  
    TiXmlNode* desc = thisNode->FirstChildElement("RenderDesc");
	if(desc)
        RenderDesc::loadProperty(desc);
    return true;
}

bool BasicButtonDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ButtonDesc");
    pressedDesc_.saveProperty(&thisNode,"pressedDesc");
    releasedDesc_.saveProperty(&thisNode,"releasedDesc");
    rollOverDesc_.saveProperty(&thisNode,"rollOverDesc");
    rollOverOnDesc_.saveProperty(&thisNode,"rollOverOnDesc");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

void BasicButtonDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    FloatRect rect(area);
    canvas->enableClipRectangle(rect);

    Color textColor;
    switch(button_->drawState())
    {
    case BasicButton::rollOverOn:
        rollOverOnDesc_.setButton(*button_);
        rollOverOnDesc_.draw(rect,canvas);
        textColor = rollOverOnDesc_.textColor();
        break;
    case BasicButton::down:
        pressedDesc_.setButton(*button_);
        pressedDesc_.draw(rect,canvas);
        textColor = pressedDesc_.textColor();
        break;
    case BasicButton::rollOver:
        rollOverDesc_.setButton(*button_);
        rollOverDesc_.draw(rect,canvas);
        textColor = rollOverDesc_.textColor();
        break;
    case BasicButton::up:
        releasedDesc_.setButton(*button_);
        releasedDesc_.draw(rect,canvas);
        textColor = releasedDesc_.textColor();
        break;
    }

    if(arrow_ != none)
    {
        Vector2f points[3];
        Vector2f center = rect.center();
        if(button_->state() == up)
            center.y_ -= 1.0f;
        else if(button_->state() == down)
            center.y_ += 1.0f;
        else if(button_->state() == left)
            center.x_ -= 1.0f;
        else if(button_->state() == right)
            center.x_ += 1.0f;
        const float horzOffset = rect.size().x() * 0.25f;
        const float vertOffset = rect.size().y() * 0.25f;
        switch(arrow_)
        {
        case BasicButtonDesc::up:
            points[0] = center + Vector2f(-horzOffset,  vertOffset);
            points[1] = center + Vector2f( 0.0f,       -vertOffset);
            points[2] = center + Vector2f( horzOffset,  vertOffset);
            break;
        case BasicButtonDesc::down:
            points[0] = center + Vector2f(-horzOffset, -vertOffset);
            points[1] = center + Vector2f( 0.0f,        vertOffset);
            points[2] = center + Vector2f( horzOffset, -vertOffset);
            break;
        case BasicButtonDesc::left:
            points[0] = center + Vector2f( horzOffset, -vertOffset);
            points[1] = center + Vector2f(-horzOffset,  0.0f      );
            points[2] = center + Vector2f( horzOffset,  vertOffset);
            break;
        case BasicButtonDesc::right:
            points[0] = center + Vector2f(-horzOffset, -vertOffset);
            points[1] = center + Vector2f( horzOffset,  0.0f      );
            points[2] = center + Vector2f(-horzOffset,  vertOffset);
            break;
        case BasicButtonDesc::none:
            err() << "BasicButton invalid arrow direction\n";
        }
        arrow_ = none;
        canvas->setColor(textColor);
        canvas->drawTriangle(points[0], points[1], points[2], true);
    }
    canvas->setColor(Color(1, 1, 1, 1));
    canvas->disableClipRectangle();
}

}
