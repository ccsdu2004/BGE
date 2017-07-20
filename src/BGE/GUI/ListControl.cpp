#include <BGE/System/Err.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/ListControl.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/ScrollBar.h>
#include <algorithm>
#include <numeric>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(ListControl,ListWindow)

ListControl::ListControl(Window* parent,const FloatRect& rect,uint32_t numColumns):
    ListWindow(parent, rect),
    usingDefaultWidths_(true),
    renderDesc_(0)
{
    columnWidths_.resize(numColumns);
    for(uint32_t i = 0; i < numColumns; ++i)
    {
        columnWidths_[i] = 1;
    }

    setPosition(rect.position());
    setSize(rect.size());
}

ListControl::~ListControl()
{
}

ListControl& ListControl::addRow(const std::vector<String>& strings)
{
    //ASSERT ( strings.size() <= numberOfColumns() );

    Row newRow(*this);
    for(unsigned i = 0; i < strings.size(); ++i)
    {
        newRow.setEntry(i, strings[i]);
    }

    rows_.push_back(newRow);
    onContentsChanged();
    return *this;
}

ListControl& ListControl::removeRowAt(uint32_t pos)
{
    Rows::iterator it;
    uint32_t counter = 0;
    for(it = rows_.begin(); it != rows_.end(); ++it)
    {
        if(counter == pos) break;
        counter++;
    }

    if(counter < numberOfRows())
        rows_.erase(it);
    onContentsChanged();
    return *this;
}

ListControl& ListControl::setColumnWidths(const std::vector<float>& widths)
{
    //ASSERT ( widths.size() == numberOfColumns() );

    std::vector<float>::const_iterator it;
    int index = 0;
    for(it = widths.begin(); it != widths.end(); ++it)
    {
        columnWidths_[index++] = *it;
    }
    usingDefaultWidths_ = false;
    return *this;
}

uint32_t ListControl::columnIndexAt(const Vector2f& p)const
{
    uint32_t ret = 0;
    for(unsigned i = 0; i < numberOfColumns()-1; ++i)
    {
        if((p.x() >= long(columnWidths_[i])) && (p.x() < long(columnWidths_[i+1])))
            break;
        ret++;
    }
    if(ret >= numberOfColumns())
        ret = numberOfColumns()-1;
    return ret;
}

Window& ListControl::resizeColumns()
{
    const float clientWidth = (size().x() - 2.0f);
    if(usingDefaultWidths_)
    {
        const float defaultWidth = floorf(clientWidth / numberOfColumns());
        for(uint32_t i = 0; i < numberOfColumns()-1; ++i)
        {
            columnWidths_[i] = defaultWidth;
        }
        columnWidths_[numberOfColumns()-1] = 0.0f;
        columnWidths_[numberOfColumns()-1] = std::accumulate(columnWidths_.begin(), columnWidths_.end(), 0.0f);
    }
    else
    {
        const float totalWidth = std::accumulate(columnWidths_.begin(), columnWidths_.end(), 0.0f);
        columnWidths_[numberOfColumns()-1] = clientWidth - totalWidth;
    }
    return *this;
}

Window& ListControl::onDraw()
{
    ListControlDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->listControlDesc_;

    renderDesc->setListWindow(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());

    verticalScrollBar_->render();
    WindowManager::instance().canvas()->setColor(Color(1, 1, 1, 1));
    return *this;
}

float ListControl::entryHeight() const
{
    ListControlDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->listControlDesc_;
    return renderDesc->textDesc().lineHeight();
}

const ListControl::Row& ListControl::rowAt(uint32_t pos)const
{
    if(pos >= numberOfRows())
        err() << "tried to access the row of a ListControl with an invalid index\n";
    Rows::const_iterator it = rows_.begin();
    std::advance(it,pos);
    return *it;
}

void ListControl::onResize()
{
    ListWindow::onResize();
    resizeColumns();
}

WINDOW_LOAD_APPAREARANCE(ListControl)

bool ListControl::loadProperty(TiXmlNode* node)
{
    //DESERIALIZE_OBJECT_CONTAINER(strings,node,std::list<String>,String)
    DESERIALIZE_OBJECT(usingDefaultWidths,node,float)
    DESERIALIZE_OBJECT_CONTAINER(columnWidths,node,std::vector<float>,float)
    TiXmlNode* window = node->FirstChildElement("ListWindow");
    ListWindow::loadProperty(window);
    return true;
}

bool ListControl::saveProperty(TiXmlNode* node)
{
    // rows?
    SERIALIZE_OBJECT(usingDefaultWidths,node)
    //SERIALIZE_CONTAINER(columnWidths,node)
    TiXmlElement element("ListWindow");
    ListWindow::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

ListControlDesc::ListControlDesc():
    separatorWidth_(1.0f),
    separatorColor_(Color(1, 1, 1, 0.75f))
{
}

ListControlDesc::~ListControlDesc()
{
}

bool ListControlDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("ListControlDesc");
	if(thisNode)
	{
		DESERIALIZE_OBJECT(separatorWidth,thisNode,float)
		DESERIALIZE_OBJECT(separatorColor,thisNode,Color)
		TiXmlNode* desc = thisNode->FirstChildElement("ListWindowDesc");
		ListWindowDesc::loadProperty(desc);
	}
	return true;
}

bool ListControlDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ListControlDesc");
    SERIALIZE_OBJECT(separatorWidth,&thisNode)
    SERIALIZE_OBJECT(separatorColor,&thisNode)
    TiXmlElement element("ListWindowDesc");
    ListWindowDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

void ListControlDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    ListWindowDesc::draw(area,canvas);

    const Vector2f topLeft     = area.position();
    const Vector2f bottomLeft  = area.bottomRight();

    ListControl* listControl = dynamic_cast<ListControl*>(listWindow_);
    canvas->setColor(separatorColor_);
    canvas->setLineWidth(separatorWidth_);
    float curX = listControl->columnWidth(0);
    for(int sep = 1; sep < listControl->numberOfColumns(); ++sep)
    {
        Vector2f offs(curX,0);
        canvas->drawLine(topLeft + offs, topLeft + offs + Vector2f(0,area.height_));
        curX += listControl->columnWidth(sep);
    }
}

void ListControlDesc::drawRow(size_t aRowIndex, const FloatRect& aRowRectangle) const
{
    ListControl* listControl = dynamic_cast<ListControl*>(listWindow_);

    const ListControl::Row& row = listControl->rowAt(aRowIndex);

    FloatRect textRectangle = aRowRectangle;
    textRectangle.translate(Vector2f(2.0f, 0.0f));

    bool isNormalText = true;
    Color backgroundColor;
    Color textColor;
    if(static_cast<int>(aRowIndex) == listControl->selectedIndex())
    {
        isNormalText = false;
        backgroundColor = selectionColor_;
        textColor = selectedTextColor_;
    }
    else if(static_cast<int>(aRowIndex) == listControl->highlightedIndex())
    {
        isNormalText = false;
        backgroundColor = highlightColor_;
        textColor = highlightedTextColor_;
    }

    if(!isNormalText)
    {
        Canvas* canvas = WindowManager::instance().canvas();
        canvas->setColor(backgroundColor);
        canvas->drawRectangle(aRowRectangle, true);
        const Color unselectedTextColor = textDesc_->color();
        textDesc_->setColor(textColor);
        for(unsigned j = 0; j < listControl->numberOfColumns(); ++j)
        {
            (*textDesc_)
                .setText(row.stringAt(j))
                .draw(textRectangle,canvas)
                ;
            textRectangle.translate(Vector2f(listControl->columnWidth(j), 0.0f));
        }
        textDesc_->setColor(unselectedTextColor);
    }
    else
    {
        for(unsigned j = 0; j < listControl->numberOfColumns(); ++j)
        {
            (*textDesc_)
                .setText(row.stringAt(j))
                .draw(textRectangle,WindowManager::instance().canvas())
                ;
            textRectangle.translate(Vector2f(listControl->columnWidth(j), 0.0f));
        }
    }
}

}


