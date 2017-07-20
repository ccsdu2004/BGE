#include <BGE/System/Math.h>
#include <BGE/Graphics/Color.h>
#include <algorithm>

namespace bge
{

const Color Color::Black(0,0,0);
const Color Color::White(1.0f,1.0f,1.0f);
const Color Color::Red(1.0f,0,0);
const Color Color::Green(0,1.0f,0);
const Color Color::Blue(0,0,1.0f);
const Color Color::Yellow(1.0f,1.0f,0);
const Color Color::Magenta(1.0f,0,1.0f);
const Color Color::Cyan(0,1.0f,1.0f);
const Color Color::Transparent(0.0f,0.0f,0.0f,0.0f);

//IMPLEMENT_OBJECT(Color,Object)

Color::Color() :
r_(0),
g_(0),
b_(0),
a_(1.0f)
{
}

Color::Color(float red,float green,float blue,float alpha):
    r_(red),
    g_(green),
    b_(blue),
    a_(alpha)
{
}

////////////////////////////////////////////////////////////
bool operator ==(const Color& left, const Color& right)
{
    return (left.r_ == right.r_) &&
           (left.g_ == right.g_) &&
           (left.b_ == right.b_) &&
           (left.a_ == right.a_);
}

////////////////////////////////////////////////////////////
bool operator !=(const Color& left, const Color& right)
{
    return !(left == right);
}

void serialize(const ObjectNamer<Color>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    SERIALIZE_OBJECT_3("r",object.value().r_,&element)
    SERIALIZE_OBJECT_3("g",object.value().g_,&element)
    SERIALIZE_OBJECT_3("b",object.value().b_,&element)
    SERIALIZE_OBJECT_3("a",object.value().a_,&element)
    node->InsertEndChild(element);
}

}
