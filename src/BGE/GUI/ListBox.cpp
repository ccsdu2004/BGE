#include <BGE/GUI/ListBox.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/ScrollBar.h>
#include <BGE/Graphics/RenderDesc.h>
#include <algorithm>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(ListBox,ListWindow)

ListBox::ListBox(Window* parent,const FloatRect& rect):
    ListWindow(parent,rect),
    renderDesc_(0)
{
    setSize(rect.size());
    setPosition(rect.position());
}

ListBox::~ListBox()
{
}

ListBox& ListBox::addString(const String& string)
{
    strings_.push_back(string);
    onContentsChanged();
    return *this;
}

ListBox& ListBox::removeStringAt(uint32_t pos)
{
    if(selectedIndex() == pos)
        select(-1);
    if(pos < numberOfStrings())
    {
        Strings::iterator it = strings_.begin();
        std::advance(it, pos);
        strings_.erase(it);
    }

    onContentsChanged();
    return *this;
}

ListBox& ListBox::removeAllStrings()
{
    select(-1);
    strings_.clear();
    onContentsChanged();
    return *this;
}

String ListBox::stringAt(uint32_t position)const
{
    String ret;
    if(position < numberOfStrings())
    {
        Strings::const_iterator it = strings_.begin();
        std::advance(it, position);
        ret = *it;
    }
    return ret;
}

bool ListBox::contains(const String& string) const
{
    return ( std::find(strings_.begin(), strings_.end(),string) != strings_.end() );
}

int ListBox::indexOf(const String& string) const
{
    int ret = -1;
    if(numberOfStrings() > 0)
    {
        ret = 0;
        for(Strings::const_iterator it = strings_.begin(); it != strings_.end(); ++it)
        {
            if((*it) == string)
                break;
            ++ret;
        }
    }
    if(ret >= numberOfStrings())
        ret = -1;

    return ret;
}

Window& ListBox::onDraw()
{
    ListBoxDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->listBoxDesc_;

    renderDesc->setListWindow(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());
    verticalScrollBar_->render();

    return *this;
}

float ListBox::entryHeight() const
{
    ListBoxDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->listBoxDesc_;
    return renderDesc->textDesc().lineHeight();
}

WINDOW_LOAD_APPAREARANCE(ListBox)

bool ListBox::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT_CONTAINER(strings,node,std::list<String>,String)
    TiXmlNode* window = node->FirstChildElement("ListWindow");
    ListWindow::loadProperty(window);
    return true;
}

bool ListBox::saveProperty(TiXmlNode* node)
{
    SERIALIZE_CONTAINER(strings,node)
    TiXmlElement element("ListWindow");
    ListWindow::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void ListBoxDesc::drawRow(size_t row, const FloatRect& rectangle)const
{
    Canvas* canvas = WindowManager::instance().canvas();
    ListBox* listBox = dynamic_cast<ListBox*>(listWindow_);

    textDesc_->setText(listBox->stringAt(row));

    FloatRect textRectangle = rectangle;
    textRectangle.translate(Vector2f(2.0f, 0.0f));

    if(static_cast<int>(row) == listBox->selectedIndex())
    {
        canvas->setColor(selectionColor_);
        canvas->drawRectangle(rectangle, true);

        const Color unselectedColor = textDesc_->color();
        (*textDesc_)
            .setColor(selectedTextColor_)
            .draw(textRectangle,canvas)
            ;
        textDesc_->setColor(unselectedColor);
    }
    else if(static_cast<int>(row) == listBox->highlightedIndex())
    {
        canvas->setColor(highlightColor_);
        canvas->drawRectangle(rectangle, true);

        const Color unselectedColor = textDesc_->color();
        (*textDesc_)
            .setColor(highlightedTextColor_)
            .draw(textRectangle,canvas)
            ;
        textDesc_->setColor(unselectedColor);
    }
    else
    {
        textDesc_->draw(textRectangle,canvas);
    }
}

}

