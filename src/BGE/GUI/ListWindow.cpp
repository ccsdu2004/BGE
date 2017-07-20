#include <BGE/System/NonCopyable.h>
#include <BGE/System/Serialization.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Message.h>
#include <BGE/GUI/ListWindow.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/ScrollBar.h>

namespace bge
{

class ListWindowMouseListener : public MouseListener, NonCopyable
{
public:
    ListWindowMouseListener(ListWindow& list):
        listWindow_(list)
    {
    }
public:
    virtual bool clicked(MouseClickMessage& message);
    virtual bool exited(MouseMessage& message);
    virtual bool moved(MouseMessage& message);
private:
    ListWindow& listWindow_;
};

IMPLEMENT_OBJECT(ListWindow,Window)

ListWindow::ListWindow(Window* parent,const FloatRect& rect):
    Window(parent),
    selectedIndex_(-1),
    highlightedIndex_(-1),
    lineHeightFactor_(1.325f)
{
    verticalScrollBar_ = new ScrollBar(this,Orientation_vertical,
        rect.topLeft() - Vector2f(0, 0),size().y());

    (*verticalScrollBar_)
        .setMinimum(0)
        .setMaximum(1)
        .setValue(0)
        ;

    listWindowMouseListener_ = new ListWindowMouseListener(*this);
    addMessageListener(listWindowMouseListener_);
}

ListWindow::~ListWindow()
{
    removeMessageListener(listWindowMouseListener_);
    delete listWindowMouseListener_;
    delete verticalScrollBar_;
}

void ListWindow::deleteLater()
{
    removeMessageListener(listWindowMouseListener_);
    delete listWindowMouseListener_;
    delete verticalScrollBar_;
	Window::deleteLater();
}

bool ListWindow::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
        ret = verticalScrollBar_->process(message);

    if(ret)
        highlight(-1);
    else
        Window::process(message);

    return ret;
}

ListWindow& ListWindow::positionScrollBar()
{
    setScrollBarParameters();
    const float barSize = verticalScrollBar_->width();
    verticalScrollBar_->setPosition(position() + Vector2f(size().x() - barSize, 0.0f));
    verticalScrollBar_->setSize(Vector2f(barSize, size().y()));
    return *this;
}

uint32_t ListWindow::firstVisibleEntry() const
{
    float ret = 0.5f + (float(verticalScrollBar_->value()) - (float(lineHeight()) * 0.5f)) / float(lineHeight());
    if(ret < 0.0)
        ret = 0.0;
    return ret;
}

uint32_t ListWindow::entryIndexAt(const Vector2f& p) const
{
    //ASSERT( p.y() >= 0.0f );
    //ASSERT( lineHeight() > 0.0f );
    uint32_t ret = firstVisibleEntry() + static_cast<unsigned int>(p.y() / lineHeight());
    if(ret >= numberOfEntries()) ret = numberOfEntries()-1;
    return ret;
}

Vector2f ListWindow::recommendedSize() const
{
    return Vector2f(150.0f, 150.0f);
}

float ListWindow::lineHeight() const
{
    return ceilf(entryHeight() * lineHeightFactor());
}

ListWindow& ListWindow::select(int index)
{
    if((index >= 0) && (index < int(numberOfEntries())))
    {
        if(selectedIndex_ != index)
        {
            process(SelectionMessage(Message::deselected, selectedIndex_));
            selectedIndex_ = index;
            process(SelectionMessage(Message::selected, selectedIndex_));
        }
    }
    else
    {
        process(SelectionMessage(Message::deselected, selectedIndex_));
        selectedIndex_ = -1;
    }
    return *this;
}

void ListWindow::onResize()
{
    positionScrollBar();
}

void ListWindow::onMove()
{
    positionScrollBar();
}

float ListWindow::heightOfAllContents() const
{
    const float numStrings = (numberOfEntries() > 0) ? static_cast<float>(numberOfEntries()-1) : 0.0f;
    return (lineHeight() * 0.5f) + (numStrings * lineHeight());
}

float ListWindow::heightOfVisibleContents() const
{
    return (size().y() > heightOfAllContents()) ? heightOfAllContents() : size().y();
}

void ListWindow::setScrollBarParameters()
{
    const int visibleAmount = static_cast<int>(heightOfVisibleContents());
    (*verticalScrollBar_)
        .setVisibleAmount(visibleAmount)
        .setUnitIncrement(static_cast<int>(lineHeight()))
        .setBlockIncrement(visibleAmount - static_cast<int>(lineHeight()));
}

void  ListWindow::onContentsChanged()
{
    verticalScrollBar_->setMaximum(static_cast<int>(heightOfAllContents()));
    setScrollBarParameters();
}

void ListWindow::highlight(int index)
{
    highlightedIndex_ = index;
}

bool ListWindow::containsFocus() const
{
    return Window::containsFocus() || verticalScrollBar_->containsFocus();
}

bool ListWindow::doesHierarchyContain(Window* window) const
{
    return Window::doesHierarchyContain(window) || verticalScrollBar_->doesHierarchyContain(window);
}

Window* ListWindow::windowBelow(const Vector2f& p)
{
    Window* ret = Window::windowBelow(p);
    if(ret != 0)
    {
        Window* cw = verticalScrollBar_->windowBelow(p);
        if (cw)
            ret=cw;
    }

    return ret;
}

bool ListWindowMouseListener::clicked(MouseClickMessage& message)
{
    bool isHandled = false;
    if(message.button() == MouseClickMessage::left)
    {
        isHandled = true;
        if(message.event() == MouseClickMessage::pressed)
        {
            if(message.position().x() < listWindow_.verticalScrollBar_->position().x())
            {
                const Vector2f p = message.position() - listWindow_.position();
                const int newIndex = listWindow_.entryIndexAt(p);
                const int oldIndex = listWindow_.selectedIndex();
                if(newIndex != oldIndex)

                {
                    listWindow_.select(newIndex);
                    listWindow_.selectionChanged(newIndex);
                }
            }
        }
        else if(message.event() == MouseClickMessage::released)
        {
            listWindow_.leftClicked();
        }
    }
    else if(message.button() == MouseClickMessage::right)
    {
        isHandled = true;
        if(message.event() == MouseClickMessage::released)
        {
            listWindow_.rightClicked();
        }
    }

    return isHandled;
}

bool ListWindowMouseListener::exited(MouseMessage& aMessage)
{
    bool isHandled = false;
    if(aMessage.position().x() < listWindow_.verticalScrollBar_->position().x())
    {
        listWindow_.highlight(-1);
        isHandled = true;
    }

    return isHandled;
}

bool ListWindowMouseListener::moved(MouseMessage& message)
{
    bool isHandled = false;
    if(message.position().x() < listWindow_.verticalScrollBar_->position().x())
    {
        const Vector2f p = message.position() - listWindow_.position();
        const int index = listWindow_.entryIndexAt(p);
        listWindow_.highlight(index);
        isHandled = true;
    }
    return isHandled;
}

ListWindowDesc::ListWindowDesc() :
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()), textDesc_(new TextDesc()),
    selectionColor_(0.65f, 0.65f, 0.85f, 1.0f),
    selectedTextColor_(0.35f, 0.35f, 0.35f, 1.0f),
    highlightColor_(0.5f, 0.5f, 0.65f, 0.4f),
    highlightedTextColor_(0.65f, 0.65f, 0.85f, 1.0f)
{
}

ListWindowDesc::~ListWindowDesc()
{
    delete background_;
    delete border_;
    delete textDesc_;
}

bool ListWindowDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("ListWindowDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		CREATE_BY_NODE(textDesc,thisNode,TextDesc)
		DESERIALIZE_OBJECT(selectionColor,thisNode,Color)
		DESERIALIZE_OBJECT(selectedTextColor,thisNode,Color)
		DESERIALIZE_OBJECT(highlightColor,thisNode,Color)
		DESERIALIZE_OBJECT(highlightedTextColor,thisNode,Color)
		TiXmlNode* desc = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(desc);
	}
	return true;
}

bool ListWindowDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ListWindowDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    textDesc_->saveProperty(&thisNode,"textDesc");
    SERIALIZE_OBJECT(selectionColor,&thisNode)
    SERIALIZE_OBJECT(selectedTextColor,&thisNode)
    SERIALIZE_OBJECT(highlightColor,&thisNode)
    SERIALIZE_OBJECT(highlightedTextColor,&thisNode)
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

void ListWindowDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    textDesc_->setHorizontalAlignment(Horizontal_left);

    if(background_ != 0)
        background_->draw(area,canvas);

    canvas->enableClipRectangle(area);

    const Vector2f topLeft     = area.position();
    const Vector2f bottomRight = area.bottomRight();

    const size_t topRow = listWindow_->firstVisibleEntry();
    const float lineHeight = listWindow_->lineHeight();

    const size_t numVisibleRows = static_cast<size_t>(area.size().y() / lineHeight);

    const float yOffs = floorf(lineHeight * 0.25f);
    float y = listWindow_->position().y() + yOffs;
    const size_t numRows = listWindow_->numberOfEntries();
    const size_t maxRow = (numRows < topRow+numVisibleRows) ? numRows : topRow+numVisibleRows;
    for(size_t curRow = topRow; curRow < maxRow; ++curRow)
    {
        FloatRect rowRectangle(Vector2f(topLeft.x(), y-yOffs),
                Vector2f(bottomRight.x() - listWindow_->verticalScrollBar()->size().x(),
                y+lineHeight-yOffs)-Vector2f(topLeft.x(),y-yOffs));

        drawRow(curRow, rowRectangle);
        y += lineHeight;
    }

    canvas->disableClipRectangle();

    if(border_ != 0)
        border_->draw(area,canvas);

    canvas->setColor(Color(1, 1, 1, 1));
}

bool ListWindow::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(selectedIndex,node,int)
    DESERIALIZE_OBJECT(highlightedIndex,node,int)
    DESERIALIZE_OBJECT(lineHeightFactor,node,float)
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool ListWindow::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(selectedIndex,node)
    SERIALIZE_OBJECT(highlightedIndex,node)
    SERIALIZE_OBJECT(lineHeightFactor,node)
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

}
