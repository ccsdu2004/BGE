#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/SpinBox.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/EditField.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/System/Serialization.h>

namespace bge
{

class SpinBoxButtonListener;
class SpinBoxFocusListener;

class SpinBoxButton : public Button
{
public:
    enum Direction { up = 0, down };
public:
    SpinBoxButton(Window* parent,Direction direction);
    virtual ~SpinBoxButton();
public:
    Direction direction()const { return direction_; }
protected:
    virtual Window& draw();
private:
    Direction direction_;
};

IMPLEMENT_OBJECT_CREATE(SpinBox,Slideable)

SpinBox::SpinBox(Window* parent,int value):
    Slideable(parent),
    mouseListener_(0)
{
    incrementButton_ = new SpinBoxButton(this,SpinBoxButton::up);
    decrementButton_ = new SpinBoxButton(this,SpinBoxButton::down);
    editField_ = new EditField(this,toString(value));

    incrementButton_->pressed.connect(this, &SpinBox::incrementButtonPressed);
    incrementButton_->released.connect(this, &SpinBox::incrementButtonReleased);
    decrementButton_->pressed.connect(this, &SpinBox::decrementButtonPressed);
    decrementButton_->released.connect(this, &SpinBox::decrementButtonReleased);

    incrementTimer_.trigger.connect(this, &SpinBox::incrementButtonPressed);
    decrementTimer_.trigger.connect(this, &SpinBox::decrementButtonPressed);

    editField_->returnPressed.connect(this, &SpinBox::textEdited);
    setValue(value);
}

SpinBox::~SpinBox()
{
    delete incrementButton_;
    delete decrementButton_;
    delete editField_;
}

void SpinBox::deleteLater()
{
    delete incrementButton_;
    delete decrementButton_;
    delete editField_;
	Slideable::deleteLater();
}

bool SpinBox::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        if(incrementButton_->process(message))
            ret = true;
        if(decrementButton_->process(message))
            ret = true;
        if(editField_->process(message))
            ret = true;
    }

    if(!ret)
        ret = Window::process(message);
    return ret;
}

Window* SpinBox::windowBelow(const Vector2f& p)
{
    Window* ret = Window::windowBelow(p);
    if(ret != 0)
    {
        Window* cw = incrementButton_->windowBelow(p);
        if(cw != 0)
            ret = cw;
        else
        {
            cw = decrementButton_->windowBelow(p);
            if(cw != 0)
                ret = cw;
        }
    }
    return ret;
}

SpinBox& SpinBox::arrange()
{
    Vector2f editFieldSize(size() - Vector2f(size().y()/2, 0));
    Vector2f buttonSize(size().y()/2, size().y() / 2);

    editField_->setPosition(position());
    editField_->setSize(editFieldSize);

    (*incrementButton_)
        .setSize(buttonSize)
        .setPosition(position() + Vector2f(editFieldSize.x(), 0));

    (*decrementButton_)
        .setSize(buttonSize)
        .setPosition(position() + Vector2f(editFieldSize.x(), size().y() / 2));

    return *this;
}

void SpinBox::incrementButtonPressed()
{
    textEdited();
    int newValue = value() + unitIncrement();
    setValue(newValue);

    if(!incrementTimer_.isStarted())
    {
        incrementTimer_.setInterval(0.25f);
        incrementTimer_.start();
    }
    else
        incrementTimer_.setInterval(0.05f);

    updateDisplay();
}

void SpinBox::decrementButtonPressed()
{
    textEdited();
    int newValue = value() - unitIncrement();
    setValue(newValue);

    if(!decrementTimer_.isStarted())
    {
        decrementTimer_.setInterval(0.25f);
        decrementTimer_.start();
    }
    else
        decrementTimer_.setInterval(0.05f);

    updateDisplay();
}

void SpinBox::incrementButtonReleased()
{
    unFocusEditField();
    incrementTimer_.stop();
}

void SpinBox::decrementButtonReleased()
{
    unFocusEditField();
    decrementTimer_.stop();
}

void SpinBox::textEdited()
{
    if(isTextNumeric())
    {
        int newValue = fromString<int>(editField_->text().toAnsiString());//string_cast<int>(editField_->text());
        setValue(newValue);
    }
    updateDisplay();
}

void SpinBox::updateDisplay()
{
    editField_->setText(toString(value()));
}

void SpinBox::focusEditField()
{
    WindowManager::instance().setFocusedWindow(editField_);
}

void SpinBox::unFocusEditField()
{
    textEdited();
    editField_->disableFocused();
}

Window& SpinBox::onDraw()
{
    incrementButton_->render();
    decrementButton_->render();
    editField_->render();
    return *this;
}

bool SpinBox::isTextNumeric()
{
    const std::string txt = editField_->text();
    bool ret = true;
    for (int i = 0; i < int(txt.length()); ++i)
    {
        if(!isdigit(txt[i]))
        {
            ret = false;
            break;
        }
    }

    return true;
}

bool SpinBox::loadAppearance(const std::string& file)
{
    return editField_->loadAppearance(file);
}

bool SpinBox::loadProperty(TiXmlNode* node)
{
    TiXmlNode* window = node->FirstChildElement("Slideable");
    Slideable::loadProperty(window);
    return true;
}

bool SpinBox::saveProperty(TiXmlNode* node)
{
    TiXmlElement element("Slideable");
    Slideable::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

Vector2f SpinBox::recommendedSize() const
{
    return Vector2f(20, 20);
}

void SpinBox::onGainedFocus()
{
    focusEditField();
}

void SpinBox::onLostFocus()
{
    unFocusEditField();
}

void SpinBox::onResize()
{
    arrange();
}

void SpinBox::onMove()
{
    arrange();
}

SpinBoxButton::SpinBoxButton(Window* parent,Direction direction):
    Button(parent,L""),
    direction_(direction)
{
}

SpinBoxButton::~SpinBoxButton()
{
}

Window& SpinBoxButton::draw()
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
    default:
        break;
    }

    Button::onDraw();
    return *this;
}

}
