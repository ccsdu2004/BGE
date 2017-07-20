#include <BGE/System/Err.h>
#include <BGE/GUI/Layout.h>
#include <BGE/GUI/Window.h>

namespace bge
{

IMPLEMENT_OBJECT(FlowLayout,Layout)

FlowLayout::FlowLayout(Window* window,Horizontal alignment):
    Layout(window),
    alignment_(alignment)
{
}

FlowLayout::FlowLayout(Horizontal alignment,float spacing):
    Layout(NULL),
    alignment_(alignment)
{
    setSpacing(spacing);
}

Layout& FlowLayout::addWindow(Window* window)
{
    if(parent())
    {
        windows_.push_back(window);
        parent()->insertChild(window);
    }
    else
        err() << "parent is NULL.\n";
    return *this;
}

Layout& FlowLayout::removeWindow(Window* window)
{
    windows_.remove(window);
    if(parent())
        parent()->removeChild(window);
    return *this;
}

Layout& FlowLayout::arrangeWithin()
{
    Window* window_ = (Window*)parent();
    if(!window_)
        return *this;

    const float width = window_->size().x();
    const float height = window_->size().y();

    std::list<Window*>::iterator first = windows_.begin();
    std::list<Window*>::iterator last = first;

    bool done = false;

    if(width > spacing())
    {
        float y = spacing();
        while(!done)
        {
            float x = spacing();
            bool isRowDone = false;
            while(!isRowDone)
            {
                if(last == windows_.end())
                {
                    done = true;
                    isRowDone = true;
                }
                else
                {
                    Window* window = (*last);
                    float windowWidth = window->size().x();
                    if(windowWidth + 2.0f*spacing() > width)
                    {
                        windowWidth = width - 2.0f*spacing();
                        window->setSize(Vector2f(windowWidth,window->size().y()));
                    }
                    float windowHeight = window->size().y();
                    if(windowHeight + 2.0f*spacing() > height)
                    {
                        windowHeight = width - 2.0f*spacing();
                        window->setSize(Vector2f(window->size().x(),windowHeight));
                    }

                    if(x + windowWidth + spacing() <= width)
                    {
                        x += windowWidth + spacing();
                        ++last;
                    }
                    else
                    {
                        isRowDone = true;
                    }
                }
            }


            const float totalWidth = x;
            switch(alignment_)
            {
            case Horizontal_left:
                x = spacing();
                break;
            case Horizontal_right:
                x = width - totalWidth + spacing();
                break;
            case Horizontal_hCenter:
                x = (width * 0.5f) - (totalWidth * 0.5f) + spacing();
                break;
            }

            float maxY = 0.0f;
            for(std::list<Window*>::iterator it = first; it != last; ++it)
            {
                Window* window = (*it);
                window->setPosition(Vector2f(x, y) + window_->position());
                float windowWidth = window->size().x();
                x += windowWidth + spacing();
                if(window->size().y() > maxY)
                    maxY = window->size().y();
            }
            y += maxY + spacing();
            first = last;
        }
    }
    return *this;
}

bool FlowLayout::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(alignment,node,Horizontal)
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

bool FlowLayout::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(alignment,node)

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
