#include <BGE/System/Serialization.h>
#include <BGE/System/Err.h>
#include <BGE/GUI/Slideable.h>
#include <BGE/GUI/Message.h>

namespace bge
{

IMPLEMENT_OBJECT(Slideable,Window)

Slideable::Slideable(Window* parent):
    Window(parent,L""),
    maximum_(255),
    minimum_(0)
{
    setValue(0);
    setUnitIncrement(1);
    setBlockIncrement(64);
}

Slideable::~Slideable()
{
}

Slideable& Slideable::setValue(int newValue)
{
    value_ = newValue;

    if(value() < minimum())
        setValue(minimum());
    else if(value() > maximum())
        setValue(maximum());

    process(SlideableMessage(value()));
    valueChanged(value());
    return *this;
}

uint32_t Slideable::range() const
{
    const int mini = minimum();
    const int maxi = maximum();
    //if(maxi < mini);
    return (maxi - mini);
}

Slideable& Slideable::setMaximum(int newValue)
{
    //ASSERT(aValue > minimum());
    maximum_ = newValue;
    if(maximum() < value())
        setValue(maximum());
    return *this;
}

Slideable& Slideable::setMinimum(int newValue)
{
    //ASSERT(aValue < maximum());
    minimum_ = newValue;
    if(minimum() > value())
        setValue(minimum());
    return *this;
}

bool Slideable::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(value,node,int)
    DESERIALIZE_OBJECT(minimum,node,int)
    DESERIALIZE_OBJECT(maximum,node,int)
    DESERIALIZE_OBJECT(blockIncrement,node,int)
    DESERIALIZE_OBJECT(unitIncrement,node,int)
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool Slideable::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(value,node)
    SERIALIZE_OBJECT(minimum,node)
    SERIALIZE_OBJECT(maximum,node)
    SERIALIZE_OBJECT(blockIncrement,node)
    SERIALIZE_OBJECT(unitIncrement,node)

    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
