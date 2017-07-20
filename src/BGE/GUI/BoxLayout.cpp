#include <BGE/System/Err.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/Layout.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(BoxLayout,Layout)

BoxLayout::BoxLayout(Orientation justification,Window* window):
    Layout(window),
    justification_(justification),
    isChildSizeRespected_(true)
{
}

BoxLayout::BoxLayout(Orientation justification,float spacing,bool respectChildSizes):
    justification_(justification),
    isChildSizeRespected_(respectChildSizes)
{
    setSpacing(spacing);
}

Layout& BoxLayout::addWindow(Window* window)
{
    if(parent())
    {
        windows_.push_back(window);
        parent()->insertChild(window);
    }
    else
        err() <<"parent is NULL.\n";
    return *this;
}

Layout& BoxLayout::removeWindow(Window* window)
{
    windows_.remove(window);
    if(parent())
    {
        parent()->removeChild(window);
    }
    return *this;
}

Layout& BoxLayout::arrangeWithin()
{
    Window* window = (Window*)parent();
    const float width = window->size().x();
    const float height = window->size().y();
    const float space = spacing();

    if(!isChildSizeRespected())
    {
        float windowWidth  = 0.0f;
        float windowHeight = 0.0f;
        float numWindows = static_cast<float>(windows_.size());
        if(justification() == Orientation_horizontal)
        {
            windowWidth  = (width - (spacing() * (numWindows + 1.0f))) / numWindows;
            windowHeight = (height - (spacing() * 2.0f));
        }
        else
        {
            windowWidth  = (width - (spacing() * 2.0f));
            windowHeight = (height - (spacing() * (numWindows + 1.0f))) / numWindows;
        }

        for(std::list<Window*>::iterator it = windows_.begin(); it != windows_.end(); ++it)
        {
            (*it)->setSize(Vector2f(windowWidth,windowHeight));
        }
    }

    if(justification() == Orientation_horizontal)
    {
        const float parentCenter = window->position().y() + (height * 0.5f);
        float x = window->position().x() + space;
        for(std::list<Window*>::iterator it = windows_.begin(); it != windows_.end(); ++it)
        {
            Window* window = *it;
            const float y = parentCenter - (window->size().y() * 0.5f);
            window->setPosition(Vector2f(x, y));
            x += window->size().x() + space;
        }
    }
    else
    {
        const float parentCenter = window->position().x() + (width * 0.5f);
        float y = window->position().y() + space;
        for(std::list<Window*>::iterator it = windows_.begin(); it != windows_.end(); ++it)
        {
            Window* window = *it;
            const float x = parentCenter - (window->size().x() * 0.5f);
            window->setPosition(Vector2f(x, y));
            y += window->size().y() + space;
        }
    }

    return *this;
}

bool BoxLayout::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(justification,node,Orientation)
    DESERIALIZE_OBJECT(isChildSizeRespected,node,bool)
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
	arrangeWithin();
    return true;
}

bool BoxLayout::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(justification,node)
    SERIALIZE_OBJECT(isChildSizeRespected,node)

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
