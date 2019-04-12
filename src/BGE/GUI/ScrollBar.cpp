#include <BGE/System/NonCopyable.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/ScrollBar.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(ScrollBar,Slideable)

class ScrollBarButtonListener;

class ScrollBarSlider : public Button, public MouseListener, NonCopyable
{
public:
    ScrollBarSlider(ScrollBar& scroll);
    virtual ~ScrollBarSlider(){}
public:
    float offsetToDiscreetPos() const { return offsetToDiscreetPos_; }
protected:
    virtual bool moved(MouseMessage& message);
    virtual Button& onReleased() { offsetToDiscreetPos_ = 0; return *this; }

    ScrollBarSlider& moveHorizontally(float dx);
    ScrollBarSlider& moveVertically(float dy);

    float screenUnitsPerScrollBarUnit()const;
private:
    float offsetToDiscreetPos_;
    ScrollBar& scrollBar_;
    Vector2f oldMousePos_;
};

class ScrollBarButton : public Button
{
public:
    enum Direction { up = 0, down, left, right };
public:
    ScrollBarButton(Window* parent,Direction direction);
    virtual ~ScrollBarButton();
public:
    Direction direction() const { return direction_; }
protected:
    virtual Window& draw();
private:
    Direction direction_;
};

class ScrollBarMouseListener : public MouseListener, NonCopyable
{
public:
    ScrollBarMouseListener(ScrollBar& scroll) : scrollBar_(scroll){}
    virtual ~ScrollBarMouseListener(){}
public:
    virtual bool clicked(MouseClickMessage& message);
private:
    ScrollBar& scrollBar_;
};

ScrollBar::ScrollBar(Window* parent,Orientation ajustification,
                     const Vector2f& topLeft, float extent) :
    Slideable(parent),
    mouseListener_(0),
    justification_(ajustification),
    visibleAmount_(1),
    renderDesc_(0)
{
    slider_ = new ScrollBarSlider(*this);
    slider_->pressed.connect(this, &ScrollBar::buttonPressed);
    slider_->released.connect(this, &ScrollBar::buttonReleased);

    Vector2f sz;

    if(justification() == Orientation_vertical)
    {
        incrementButton_ = new ScrollBarButton(this, ScrollBarButton::down);
        decrementButton_ = new ScrollBarButton(this, ScrollBarButton::up);
        sz = Vector2f(width(),extent);
    }
    else
    {
        incrementButton_ = new ScrollBarButton(this, ScrollBarButton::right);
        decrementButton_ = new ScrollBarButton(this, ScrollBarButton::left);
        sz = Vector2f(extent, width());
    }

    incrementButton_->pressed.connect (this, &ScrollBar::incrementButtonPressed);
    incrementButton_->released.connect(this, &ScrollBar::incrementButtonReleased);
    decrementButton_->pressed.connect (this, &ScrollBar::decrementButtonPressed);
    decrementButton_->released.connect(this, &ScrollBar::decrementButtonReleased);

    incrementTimer_.trigger.connect(this, &ScrollBar::incrementButtonPressed);
    decrementTimer_.trigger.connect(this, &ScrollBar::decrementButtonPressed);

    setPosition(topLeft);
    setSize(sz);

    mouseListener_ = new ScrollBarMouseListener(*this);
    addMessageListener(mouseListener_);
}

ScrollBar::~ScrollBar()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
    delete incrementButton_;
    delete decrementButton_;
    delete slider_;
}

void ScrollBar::deleteLater()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
    delete incrementButton_;
    delete decrementButton_;
    delete slider_;
	Slideable::deleteLater();
}

bool ScrollBar::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        if(incrementButton_->process(message)) ret = true;
        if(decrementButton_->process(message)) ret = true;
        if(slider_->process(message)) ret = true;
    }
    if(!ret) ret = Window::process(message);
    return ret ;
}

bool ScrollBar::containsFocus() const
{
    return Window::containsFocus() ||
        incrementButton_->containsFocus() ||
        decrementButton_->containsFocus() ||
        slider_->containsFocus();
}

bool ScrollBar::doesHierarchyContain(Window* window) const
{
    return Window::doesHierarchyContain(window) ||
        incrementButton_->doesHierarchyContain(window) ||
        decrementButton_->doesHierarchyContain(window) ||
        slider_->doesHierarchyContain(window);
}

float ScrollBar::trackLength() const
{
    const FloatRect rect(position(),size());

    float ret = 0;
    if(justification() == Orientation_horizontal)
    {
        ret = rect.size().x() -
            incrementButton_->size().x() - decrementButton_->size().x();
    }
    else
    {
        ret = rect.size().y() -
            incrementButton_->size().y() - decrementButton_->size().y();
    }
    return ret;
}

float ScrollBar::computeSliderLength() const
{
    const float length = trackLength();
    const int l = range();
    const float visibleFraction = float(visibleAmount()) / static_cast<float>(l);

    float ret = static_cast<float>(length) * visibleFraction;
    if(ret < decrementButton_->size().x())
        ret = decrementButton_->size().x();
    return ret;
}

float ScrollBar::discreetSliderOffset() const
{
    const float slideLength = trackLength() - computeSliderLength();
    const float valueFraction = static_cast<float>(value()) / static_cast<float>(range());
    return width() + (slideLength * valueFraction);
}

ScrollBar& ScrollBar::arrange()
{
    const int l = range();
    if(visibleAmount() > l)
        setVisibleAmount(l);
    const Vector2f buttonPoint(width(), width());
    if(justification() == Orientation_vertical)
    {
        (*incrementButton_)
            .setSize(buttonPoint)
            .setPosition(position()+ Vector2f(0, size().y() - width()))
            ;
        (*decrementButton_)
            .setSize(buttonPoint)
            .setPosition(position())
            ;
    }
    else
    {
        (*incrementButton_)
            .setSize(buttonPoint)
            .setPosition(position() + Vector2f(size().x() - width(), 0));
        (*decrementButton_)
            .setSize(buttonPoint)
            .setPosition(position())
            ;
    }
    return *this;
}

void ScrollBar::incrementButtonPressed()
{
    int newValue = value() + unitIncrement();
    setValue(newValue);

    if(!incrementTimer_.isStarted())
    {
        incrementTimer_.setInterval(0.25f);
        incrementTimer_.start();
    }
    else
        incrementTimer_.setInterval(0.05f);
}

void ScrollBar::decrementButtonPressed()
{
    int newValue = value() - unitIncrement();
    setValue(newValue);

    if(!decrementTimer_.isStarted())
    {
        decrementTimer_.setInterval(0.25f);
        decrementTimer_.start();
    }
    else
        decrementTimer_.setInterval(0.05f);
}

void ScrollBar::incrementButtonReleased()
{
    incrementTimer_.stop();
}

void ScrollBar::decrementButtonReleased()
{
    decrementTimer_.stop();
}

void ScrollBar::onResize()
{
    arrange();
}

void ScrollBar::onMove()
{
    arrange();
}

Vector2f ScrollBar::recommendedSize() const
{
    return (justification_ == Orientation_horizontal) ? Vector2f(150.0f, width()) : Vector2f(width(), 150.0f);
}

float ScrollBar::width() const
{
    ScrollBarDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->scrollBarDesc_;

    return renderDesc->width();
}

Window& ScrollBar::onDraw()
{
    ScrollBarDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->scrollBarDesc_;

    const Vector2f topLeft = position();

    Canvas* canvas = WindowManager::instance().canvas();
    renderDesc->draw(geometry(),canvas);

    canvas->disableClipRectangle();
    canvas->setColor(Color(1, 1, 1, 1));

    if(justification() == Orientation_vertical)
    {
        const float width = decrementButton_->size().x();
        (*slider_)
            .setSize(Vector2f(width, computeSliderLength()))
            .setPosition(topLeft + Vector2f(0.0f, discreetSliderOffset() + slider_->offsetToDiscreetPos()));
    }
    else
    {
        const float height = decrementButton_->size().y();
        (*slider_)
            .setSize(Vector2f(computeSliderLength(), height))
            .setPosition(topLeft + Vector2f(discreetSliderOffset() + slider_->offsetToDiscreetPos(), 0.0f));
    }

    slider_->render();

    incrementButton_->render();
    decrementButton_->render();
    return *this;
}

WINDOW_LOAD_APPAREARANCE(ScrollBar)

bool ScrollBar::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(justification,node)
    DESERIALIZE_OBJECT(visibleAmount,node)
    DESERIALIZE_OBJECT(trackColor,node)
    TiXmlNode* window = node->FirstChildElement("Slideable");
    Slideable::loadProperty(window);
    return true;
}

bool ScrollBar::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(justification,node)
    SERIALIZE_OBJECT(visibleAmount,node)
    SERIALIZE_OBJECT(trackColor,node)
    TiXmlElement element("Slideable");
    Slideable::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}



ScrollBarButton::ScrollBarButton(Window* parent,Direction direction):
    Button(parent,L""),
    direction_(direction)
{
}

ScrollBarButton::~ScrollBarButton()
{
}

Window& ScrollBarButton::draw()
{
    BasicButtonDesc* buttonDesc = renderDesc_;
    if(buttonDesc == 0)
        buttonDesc = &WindowManager::instance().propertyScheme()->buttonDesc_;

    switch(direction())
    {
    case up:
        buttonDesc->setArrow(BasicButtonDesc::up);
        break;
    case down:
        buttonDesc->setArrow(BasicButtonDesc::down);
        break;
    case left:
        buttonDesc->setArrow(BasicButtonDesc::left);
        break;
    case right:
        buttonDesc->setArrow(BasicButtonDesc::right);
        break;
    default:
        break;
    }

    Button::onDraw();
    return *this;
}

ScrollBarSlider::ScrollBarSlider(ScrollBar& scroll) :
    Button(&scroll,L""),
    scrollBar_(scroll),
    oldMousePos_(-1.0f, -1.0f),
    offsetToDiscreetPos_(0.0f)
{
    keepPressedOnExit();

    const float sz = scrollBar_.width();
    switch(scrollBar_.justification())
    {
    case Orientation_horizontal:
        setPosition(scrollBar_.position() + Vector2f(sz, 0.0f));
        setSize(Vector2f(sz, sz));
        break;
    case Orientation_vertical:
        setPosition(scrollBar_.position() + Vector2f(0.0f, sz));
        setSize(Vector2f(sz, sz));
        break;
    }
    addMessageListener(this);
}

bool ScrollBarSlider::moved(MouseMessage& message)
{
    if(oldMousePos_.x() >= 0.0f)
    {
        if(state() == Button::down)
        {
            int value = scrollBar_.value();
            switch(scrollBar_.justification())
            {
            case Orientation_horizontal:
                {
                    const float delta = message.x() - oldMousePos_.x();
                    if(delta != 0) moveHorizontally(delta);
                }
                break;
            case Orientation_vertical:
                {
                    const float delta = message.y() - oldMousePos_.y();
                    if(delta != 0) moveVertically(delta);
                }
                break;
            }
            if(value != scrollBar_.value()) scrollBar_.sliderMoved(scrollBar_.value());
        }
    }
    oldMousePos_.setX(message.x());
    oldMousePos_.setY(message.y());
    return true;
}

ScrollBarSlider& ScrollBarSlider::moveHorizontally(float dx)
{
    setPosition(position() + Vector2f(dx, 0.0f));
    const float positionOnTrack = position().x() - scrollBar_.position().x() - scrollBar_.width();
    scrollBar_.setValue(static_cast<int>(positionOnTrack / screenUnitsPerScrollBarUnit()));
    const float properPosition = scrollBar_.position().x() + scrollBar_.discreetSliderOffset();
    const float actualPosition = position().x();

    offsetToDiscreetPos_ = actualPosition - properPosition;

    if((actualPosition < scrollBar_.position().x() + scrollBar_.width()) && (offsetToDiscreetPos() < 0) )
    {
        offsetToDiscreetPos_ = 0.0f;
    }
    const float endOfSlider = actualPosition + scrollBar_.computeSliderLength();
    const float endOfTrack  = scrollBar_.position().x() + scrollBar_.width() + scrollBar_.trackLength();
    if( (endOfSlider > endOfTrack) && (offsetToDiscreetPos() > 0.0f) )
        offsetToDiscreetPos_ = 0.0f;
    return *this;
}

ScrollBarSlider& ScrollBarSlider::moveVertically(float dy)
{
    setPosition(position() +Vector2f(0.0f,dy));
    const float positionOnTrack = position().y() - scrollBar_.position().y() - scrollBar_.width();

    scrollBar_.setValue(static_cast<int>(positionOnTrack / screenUnitsPerScrollBarUnit()));
    const float properPosition = scrollBar_.position().y() + scrollBar_.discreetSliderOffset();
    const float actualPosition = position().y();
    offsetToDiscreetPos_ = actualPosition - properPosition;

    if((actualPosition < scrollBar_.position().y() + scrollBar_.width()) && (offsetToDiscreetPos() < 0.0f) )
    {
        offsetToDiscreetPos_ = 0.0f;
    }
    const float endOfSlider = actualPosition + scrollBar_.computeSliderLength();
    const float endOfTrack  = scrollBar_.position().y() + scrollBar_.width() + scrollBar_.trackLength();
    if( (endOfSlider > endOfTrack) && (offsetToDiscreetPos() > 0.0f) )
        offsetToDiscreetPos_ = 0.0f;

    return *this;
}

float ScrollBarSlider::screenUnitsPerScrollBarUnit() const
{
    const float trackLength = scrollBar_.trackLength() - scrollBar_.computeSliderLength();
    const float unitFraction = 1.0f / float(scrollBar_.range());
    return trackLength * unitFraction;
}

bool ScrollBarMouseListener::clicked(MouseClickMessage& message)
{
    bool isHandled = false;
    if(message.event() == MouseClickMessage::released)
    {
        const float positionOnTrack = (scrollBar_.justification() == Orientation_horizontal) ?
            static_cast<float>(message.x()) - scrollBar_.position().x() - scrollBar_.width() :
            static_cast<float>(message.y()) - scrollBar_.position().y() - scrollBar_.width();

        if((positionOnTrack > 0.0f) && (positionOnTrack < scrollBar_.trackLength()))
        {
            const float sliderOffset = scrollBar_.discreetSliderOffset() - scrollBar_.width();
            if(positionOnTrack < sliderOffset)
                scrollBar_.setValue(scrollBar_.value() - scrollBar_.blockIncrement());
            else if(positionOnTrack > (sliderOffset + scrollBar_.computeSliderLength()))
                scrollBar_.setValue(scrollBar_.value() + scrollBar_.blockIncrement());
        }
        isHandled = true;
    }

    return isHandled;
}

ScrollBarDesc::ScrollBarDesc():
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    width_(16.0f)
{
}

ScrollBarDesc::~ScrollBarDesc()
{
    delete background_;
    delete border_;
}

bool ScrollBarDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("ScrollBarDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		TiXmlNode* window = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(window);
	}
	return true;
}

bool ScrollBarDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ScrollBarDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    SERIALIZE_OBJECT(width,&thisNode)
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}
}
