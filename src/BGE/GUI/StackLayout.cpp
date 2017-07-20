#include <BGE/GUI/Layout.h>
#include <BGE/GUI/Window.h>
#include <algorithm>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(StackLayout,Layout)

StackLayout::StackLayout(Window* parent):
    Layout(parent),
    current_(-1)
{
}

StackLayout::~StackLayout()
{
}

Layout& StackLayout::addWindow(Window* window)
{
    if(parent())
    {
        windows_.push_back(window);
        parent()->insertChild(window);
    }
    return *this;
}

Layout& StackLayout::insertWindow(int index,Window* window)
{
    if(parent())
    {
        if(index < 0) index = 0;
        if(index > windows_.size())
            index = windows_.size();

        auto itr = windows_.begin();
        std::advance(itr,index);

        windows_.insert(itr,window);
    }
    return *this;
}

Window* StackLayout::currentWindow()const
{
    if(current_ == -1)
        return NULL;
    auto itr = windows_.begin();
    std::advance(itr,current_);
    return *itr;
}

int StackLayout::currentIndex()const
{
    return current_;
}

Window* StackLayout::window(int index)const
{
    if(current_ == -1 || index > windows_.size() || index < 0)
        return NULL;

    auto itr = windows_.begin();
    std::advance(itr,index);
    return *itr;
}

int StackLayout::count()const
{
    return windows_.size();
}

Layout& StackLayout::setCurrentIndex(int index)
{
    if(index != current_)
    {
        currentChanged(index);
    }
    return *this;
}

Layout& StackLayout::setCurrentWidget(Window* window)
{
    auto itr = std::find(windows_.begin(),windows_.end(),window);
    int index = std::distance(windows_.begin(),itr);
    setCurrentIndex(index);
    return *this;
}

Layout& StackLayout::removeWindow(Window* window)
{
    windows_.remove(window);
    if(parent())
    {
        parent()->removeChild(window);
    }
    if(current_ > windows_.size())
        current_ --;
    return *this;
}

Layout& StackLayout::arrangeWithin()
{
    if(parent())
    {
        Window* current = window(current_);
        auto itr = windows_.begin();
        while(itr != windows_.end())
        {
            if(current != *itr)
                (*itr)->hide();
            itr ++;
        }

        if(current)
        {
            current->show();
            Window* ptr = Object::castTo<Window>(parent());
            current->setPosition(ptr->geometry().center()-ptr->geometry().size()*0.5f);
        }
    }
    return *this;
}

bool StackLayout::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(current,node,int)
    TiXmlNode* window = node->FirstChildElement("Layout");
    Layout::loadProperty(window);
    return true;
}

bool StackLayout::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(current,node)

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
