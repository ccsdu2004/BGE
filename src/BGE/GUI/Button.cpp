#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/WindowManager.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(Button,BasicButton)

Button::Button(Window* parent,const String& text,ButtonGroup* group):
    BasicButton(parent,text,group)
{
}

Window& Button::onDraw()
{
    drawStandardButton();
    return *this;
}

bool Button::loadProperty(TiXmlNode* node)
{
    TiXmlNode* window = node->FirstChildElement("BasicButton");
    BasicButton::loadProperty(window);
    return true;
}

bool Button::saveProperty(TiXmlNode* node)
{
    TiXmlElement element("BasicButton");
    BasicButton::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
