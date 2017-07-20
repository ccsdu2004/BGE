#include <BGE/System/Err.h>
#include <BGE/GUI/Layout.h>
#include <BGE/GUI/Window.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(AbsoluteLayout,Layout)

AbsoluteLayout::AbsoluteLayout(Window* parent):
    Layout(parent)
{
}

AbsoluteLayout::~AbsoluteLayout()
{
}

Layout& AbsoluteLayout::addWindow(Window* window)
{
    if(parent())
    {
        windows_.push_back(window);
        parent()->insertChild(window);
    }
    return *this;
}

int AbsoluteLayout::count()const
{
    return windows_.size();
}

Layout& AbsoluteLayout::arrangeWithin()
{
    return *this;
}

Layout& AbsoluteLayout::removeWindow(Window* window)
{
    windows_.remove(window);
    if(parent())
    {
        parent()->removeChild(window);
    }
    return *this;
}

bool AbsoluteLayout::loadProperty(TiXmlNode* node)
{
    TiXmlElement* element = node->FirstChildElement("windows");
    while(element)
    {
		Window* window = parse(element);
		if(window)
			windows_.push_back(window);
		element = element->NextSiblingElement();
    }

    TiXmlNode* window = node->FirstChildElement("Layout");
    Layout::loadProperty(window);
    return true;
}

bool AbsoluteLayout::saveProperty(TiXmlNode* node)
{
    TiXmlElement windows("windows");
    for(auto itr = windows_.begin();itr != windows_.end();itr++)
    {
        TiXmlElement element("window");
        element.SetAttribute("class",(*itr)->getRtti()->name());
        (*itr)->saveProperty(&element);
        windows.InsertEndChild(element);
    }

    node->InsertEndChild(windows);

    TiXmlElement element("Layout");
    Layout::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
