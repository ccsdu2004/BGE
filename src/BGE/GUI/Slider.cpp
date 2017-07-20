#include <BGE/System/Timer.h>
#include <BGE/System/Serialization.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Slider.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

class SliderButton : public Button, public MouseListener, NonCopyable
{
	DECLARE_OBJECT(SliderButton)
public:
    SliderButton(Slider* slider);
    ~SliderButton(){}
protected:
    virtual bool moved(MouseMessage& message);

    Slider* slider_;
    Vector2f oldMousePos_;

    SliderButton& moveHorizontally(float dx);
    SliderButton& moveVertically(float dy);
};

IMPLEMENT_OBJECT(SliderButton,Button)

class SliderMouseListener : public MouseListener, NonCopyable
{
public:
    SliderMouseListener(Slider& slider) : slider_(slider) {}
    virtual ~SliderMouseListener() {}
public:
    virtual bool clicked(MouseClickMessage& message);
private:
    Slider& slider_;
};

IMPLEMENT_OBJECT_CREATE(Slider,Slideable)

Slider::Slider(Window* parent,Orientation orientation,const FloatRect& rectangle):
    Slideable(parent),
    button_(0),
    orientation_(orientation),
    tickPlacement_(TickPlacement_none),
    tickFrequency_(16),
    renderDesc_(0)
{
    setGeometry(rectangle);
    button_ = new SliderButton(this);
    button_->pressed.connect(this, &Slider::buttonPressed);
    button_->released.connect(this, &Slider::buttonReleased);
    mouseListener_ = new SliderMouseListener(*this);
    addMessageListener(mouseListener_);
}

Slider::~Slider()
{
    delete button_;
    removeMessageListener(mouseListener_);
    delete mouseListener_;
}

bool Slider::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
        ret = button_->process(message);

    if(!ret)
        ret = Window::process(message);
    return ret;
}

float Slider::trackLength() const
{
    const float windowLength = (orientation() == Orientation_horizontal) ? geometry().width_ : geometry().height_;
    return (windowLength * 0.9f);
}

Vector2f Slider::buttonSize() const
{
    const bool isHorz = (orientation() == Orientation_horizontal);
    const float l = isHorz ? geometry().width_  : geometry().height_;
    const float h = isHorz ? geometry().height_ : geometry().width_;

    float w = ceilf(l * 0.04f);
    if(w < 5.0f) w = 5.0f;
    return Vector2f(w, ceilf(h * 0.5f));
}

Slider& Slider::arrange()
{
    if(button_ != NULL)
        button_->setSize(buttonSize());
    return *this;
}

void Slider::onResize()
{
    arrange();
}

void Slider::onMove()
{
    arrange();
}

Vector2f Slider::recommendedSize() const
{
    return (orientation_ == Orientation_horizontal) ? Vector2f(150, 32) : Vector2f(32, 150);
}

Window& Slider::onDraw()
{
    Canvas* canvas = WindowManager::instance().canvas();
    SliderDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->sliderDesc_;

    const Vector2f topLeft = position();
    const Vector2f bottomRight = position() + size();

    renderDesc->draw(geometry(),canvas);

    canvas->setColor(renderDesc->tickColor());
    canvas->setLineWidth(renderDesc->tickWidth());

    // how many pixels correspond to a unit increment
    const float pixelsPerUnit = trackLength() / static_cast<float>(range());
    const int frequency = (tickFrequency() > 0) ? tickFrequency() : range();

    Vector2f p1;
    Vector2f p2;
    if(orientation() == Orientation_vertical)
    {
        p1 = topLeft + Vector2f(geometry().size().x_*0.5f, ((geometry().height_ - trackLength()) / 2));
        p2 = p1 + Vector2f(0, trackLength());

        if((tickPlacement() == TickPlacement_left) || (tickPlacement() == TickPlacement_both) )
        {
            const float x1 = topLeft.x();
            const float x2 = x1 + (geometry().width_ * 0.25f);
            float y = p1.y();
            for(int i = minimum(); i <= maximum(); i += frequency)
            {
                canvas->drawLine(Vector2f(x1, y), Vector2f(x2, y));
                y += pixelsPerUnit * static_cast<float>(frequency);
            }
        }
        if((tickPlacement() == TickPlacement_right) || (tickPlacement() == TickPlacement_both) )
        {
            const float x2 = bottomRight.x();
            const float x1 = x2 - (size().x_ * 0.25f);
            float y = p1.y();
            for(int i = minimum(); i <= maximum(); i += frequency)
            {
                canvas->drawLine(Vector2f(x1,y), Vector2f(x2, y));
                y += pixelsPerUnit * static_cast<float>(frequency);
            }
        }
    }
    else
    {
        p1 = topLeft + Vector2f(((size().x_ - trackLength()) * 0.5f), size().y_*0.5f);
        p2 = p1 + Vector2f(trackLength(), 0.0f);

        if((tickPlacement() == TickPlacement_below) || (tickPlacement() == TickPlacement_both))
        {
            const float y1 = bottomRight.y();
            const float y2 = y1 - (size().y_ * 0.25f);
            float x = p1.x();
            for(int i = minimum(); i <= maximum(); i += frequency)
            {
                canvas->drawLine(Vector2f(x, y1), Vector2f(x, y2));
                x += pixelsPerUnit * static_cast<float>(frequency);
            }
        }
        if((tickPlacement() == TickPlacement_above) || (tickPlacement() == TickPlacement_both) )
        {
            const float y2 = topLeft.y();
            const float y1 = y2 + (size().y_ * 0.25f);
            float x = p1.x();
            for(int i = minimum(); i <= maximum(); i += frequency)
            {
                canvas->drawLine(Vector2f(x, y1), Vector2f(x, y2));
                x += pixelsPerUnit * static_cast<float>(frequency);
            }
        }
    }
    canvas->drawLine(p1,p2);

    canvas->disableClipRectangle();
    canvas->setLineWidth(1.0f);
    canvas->setColor(Color(1, 1, 1, 1));

    const float fraction = float(value() - minimum()) / float(range());
    const float buttonOffset = ceilf((trackLength() * fraction) + 0.5f);
    const float halfButtonWidth  = ceilf(button_->size().x() * 0.5f);
    const float halfButtonHeight = ceilf(button_->size().y() * 0.5f);
    if(orientation() == Orientation_vertical)
    {
        button_->setPosition(p1 + Vector2f(-halfButtonWidth, buttonOffset - halfButtonHeight));
    }
    else
    {
        button_->setPosition(p1 + Vector2f(buttonOffset - halfButtonWidth, -halfButtonHeight));
    }

    button_->render();
    return *this;
}

WINDOW_LOAD_APPAREARANCE(Slider)

bool Slider::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(orientation,node,Orientation)
    DESERIALIZE_OBJECT(tickPlacement,node,TickPlacement)
    TiXmlNode* window = node->FirstChildElement("Slideable");
    Slideable::loadProperty(window);
    return true;
}

bool Slider::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(orientation,node)
    SERIALIZE_OBJECT(tickPlacement,node)
    TiXmlElement element("Slideable");
    Slideable::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void Slider::deleteLater()
{
    delete button_;
    removeMessageListener(mouseListener_);
    delete mouseListener_;
    Slideable::deleteLater();
}

SliderButton::SliderButton(Slider* slider):
    Button(slider,L""),
    slider_(slider),
    oldMousePos_(-1.0f,-1.0f)
{
    keepPressedOnExit();
    setGeometry(FloatRect(slider_->position(),slider_->buttonSize()-slider_->position()));
    addMessageListener(this);
}

bool SliderButton::moved(MouseMessage& message)
{
    if(state() == Button::down)
    {
        int value = slider_->value();
        switch(slider_->orientation())
        {
        case Orientation_horizontal:
            moveHorizontally(message.x());
            break;
        case Orientation_vertical:
            moveVertically(message.y());
            break;
        }
        if(value != slider_->value()) 
			slider_->sliderMoved(slider_->value());
    }
    return true;
}

SliderButton& SliderButton::moveHorizontally(float xPos)
{
    const float trackLength = slider_->trackLength();
    const float pixelsPerUnit = trackLength / static_cast<float>(slider_->range());
    setPosition(Vector2f(xPos - (size().x() * 0.5f), position().y()));
    const float trackOffset = (slider_->size().x() - trackLength) * 0.5f;
    const float positionOnTrack = geometry().center().x() - slider_->position().x() - trackOffset;
    slider_->setValue(slider_->minimum() + static_cast<int>((positionOnTrack / pixelsPerUnit) + 0.5f));
    return *this;
}

SliderButton& SliderButton::moveVertically(float yPos)
{
    const float trackLength = slider_->trackLength();
    const float pixelsPerUnit = trackLength / static_cast<float>(slider_->range());

    setPosition(Vector2f(position().x(), yPos));
    const float trackOffset = (slider_->size().y() - trackLength) * 0.5f;
    const float positionOnTrack =
        geometry().center().y() - slider_->position().y() - trackOffset;

    slider_->setValue(slider_->minimum() + static_cast<int>((positionOnTrack / pixelsPerUnit) + 0.5f));
    return *this;
}

bool SliderMouseListener::clicked(MouseClickMessage& message)
{
    bool isHandled = false;
    if(message.event() == MouseClickMessage::released)
    {
        bool isHorz = (slider_.orientation() == Orientation_horizontal);
        const float positionOnTrack = isHorz ?
            message.x() - slider_.position().x() :
            message.y() - slider_.position().y();
        const float length = isHorz ? slider_.size().x() : slider_.size().y();

        if((positionOnTrack > 0) && (positionOnTrack < length))
        {
            const float sliderOffset = isHorz ?
                slider_.button_->position().x() - slider_.position().x() :
                slider_.button_->position().y() - slider_.position().y();
            const float sliderSize = isHorz ? slider_.button_->size().x() :
                slider_.button_->size().y();
            if(positionOnTrack < sliderOffset)
                slider_.setValue(slider_.value() - slider_.blockIncrement());
            else if(positionOnTrack > (sliderOffset + sliderSize))
                slider_.setValue(slider_.value() + slider_.blockIncrement());
        }
        isHandled = true;
    }
    return isHandled;
}

SliderDesc::SliderDesc() :
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    tickColor_(Color(1, 1, 1, 0.75f)),
    tickWidth_(1.0f)
{
}

SliderDesc::~SliderDesc()
{
    delete background_;
    delete border_;
}

void SliderDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    if(background_ != 0) background_->draw(area,canvas);
    if(border_ != 0)     border_->draw(area,canvas);
}

bool SliderDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("SliderDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		DESERIALIZE_OBJECT(tickColor,thisNode,Color)
		DESERIALIZE_OBJECT(tickWidth,thisNode,float)
		TiXmlNode* desc = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(desc);
	}
	return true;
}

bool SliderDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("SliderDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    SERIALIZE_OBJECT(tickColor,&thisNode)
    SERIALIZE_OBJECT(tickWidth,&thisNode)
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

}


