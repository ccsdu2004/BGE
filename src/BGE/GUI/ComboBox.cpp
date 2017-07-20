#include <BGE/System/NonCopyable.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/ComboBox.h>
#include <BGE/GUI/Message.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/ListBox.h>
#include <BGE/GUI/PropertyScheme.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(ComboBox,Window)

class ComboBoxMouseListener : public MouseListener, public SlotHolder,NonCopyable
{
public:
    ComboBoxMouseListener(ComboBox& box): comboBox_(box) {}
public:
    virtual bool clicked(MouseClickMessage& message)
    {
        bool isHandled = false;
        if(message.button() == MouseClickMessage::left)
        {
            isHandled = true;
            if(message.event() == MouseClickMessage::pressed)
            {
                if(comboBox_.listBox_ != 0)
                {
                    if(!comboBox_.isDropDownOpen_)
                    {
                        comboBox_.openDropDown();
                    }
                    else
                    {
                        comboBox_.closeDropDown();
                    }
                }
            }
        }
        return isHandled;
    }

    ComboBoxMouseListener& listBoxSelectionChanged(int val)
    {
        comboBox_.selectStringAt(val);
        return *this;
    }

    ComboBoxMouseListener& listBoxClicked()
    {
        comboBox_.closeDropDown();
        return *this;
    }
private:
    ComboBox& comboBox_;
};

ComboBox::ComboBox(Window* parent,const FloatRect& rect):
    Window(parent),
    listBox_(0),
    renderDesc_(0),
    isDropDownOpen_(false)
{
    setPosition(rect.position());
    setSize(rect.size());

    comboBoxMouseListener_ = new ComboBoxMouseListener(*this);
    addMessageListener(comboBoxMouseListener_);

    listBox_ = new ListBox(this,FloatRect(rect.left(),rect.bottom(),rect.right(), rect.bottom()+30));
    listBox_->selectionChanged.connect(comboBoxMouseListener_,
                    &ComboBoxMouseListener::listBoxSelectionChanged);
    listBox_->leftClicked.connect(comboBoxMouseListener_,
                    &ComboBoxMouseListener::listBoxClicked);
    listBox_->hide();
}

ComboBox::~ComboBox()
{
    closeDropDown();
    WindowManager::instance().removeWindow(listBox_);
    delete listBox_;
    removeMessageListener(comboBoxMouseListener_);
    delete comboBoxMouseListener_;
}

ComboBox& ComboBox::addString(const String& string)
{
    listBox_->addString(string);
    return *this;
}

String ComboBox::stringAt(uint32_t position) const
{
    return listBox_->stringAt(position);
}

String ComboBox::selectedString() const
{
    return text();
}

int ComboBox::selectedIndex() const
{
    return listBox_->selectedIndex();
}

ComboBox& ComboBox::removeStringAt(uint32_t position)
{
    listBox_->removeStringAt(position);
    return *this;
}

ComboBox& ComboBox::removeAllStrings()
{
    listBox_->removeAllStrings();
    return *this;
}

uint32_t ComboBox::numberOfStrings() const
{
    return listBox_->numberOfStrings();
}

ComboBox& ComboBox::selectString(const String& string)
{
    if(!listBox_->contains(string))
    {
        const unsigned int index = listBox_->numberOfStrings();
        (*listBox_)
            .addString(string)
            .select(index);
    }
    else
    {
        const unsigned int index = listBox_->indexOf(string);
        listBox_->select(index);
    }

    setText(string);
    selectionChanged(string);
    return *this;
}

ComboBox& ComboBox::selectStringAt(uint32_t position)
{
    if((position < numberOfStrings()) && (numberOfStrings() > 0) )
        selectString(listBox_->stringAt(position));
    return *this;
}

bool ComboBox::process(const Message& message)
{
    return Window::process(message);
}

Vector2f ComboBox::recommendedSize() const
{
    ComboBoxDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->comboBoxDesc_;

    renderDesc->textDesc().setText(text());
    return renderDesc->textDesc().textSize() + Vector2f(30, 8);
}

Window& ComboBox::onDraw()
{
    ComboBoxDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->comboBoxDesc_;

    renderDesc->setComboBox(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());

    if(isDropDownOpen())
    {
        listBox_->setPosition(Vector2f(0, size().y()) + position());
        listBox_->setSize(Vector2f(size().x(), 110));
    }

    return *this;
}

void ComboBox::onLostFocus()
{
    if(isDropDownOpen_)
    {
        bool hideListBox = true;
        if(WindowManager::instance().windowBelowCursor() != 0)
        {
            if(listBox_->doesHierarchyContain(WindowManager::instance().windowBelowCursor()))
            {
                hideListBox = false;
            }
        }
        if(hideListBox)
        {
            listBox_->hide();
            isDropDownOpen_ = false;
            listClosed();
        }
    }
    Window::onLostFocus();
}

ComboBox& ComboBox::openDropDown()
{
    listBox_->show();
    isDropDownOpen_ = true;
    listOpened();
    WindowManager::instance().setPopUpWindow(listBox_);
    return *this;
}

ComboBox& ComboBox::closeDropDown()
{
    listBox_->hide();
    isDropDownOpen_ = false;
    listClosed();
    WindowManager::instance().setPopUpWindow(0);
    return *this;
}

void ComboBox::deleteLater()
{
    closeDropDown();
    WindowManager::instance().removeWindow(listBox_);
    delete listBox_;
    removeMessageListener(comboBoxMouseListener_);
    delete comboBoxMouseListener_;
	Window::deleteLater();
}

WINDOW_LOAD_APPAREARANCE(ComboBox)

bool ComboBox::loadProperty(TiXmlNode* node)
{
	std::vector<String> item_;
	TiXmlElement* items = node->FirstChildElement("strings");
	if(items)
	{
		DESERIALIZE_OBJECT_CONTAINER(item,items,std::vector<String>,String)
		auto itr = item_.begin();
		while(itr != item_.end())
		{
			listBox_->addString(*itr);
			itr ++;
		}
	}

	if(numberOfStrings() != 0)
		selectStringAt(0);

    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool ComboBox::saveProperty(TiXmlNode* node)
{
	std::vector<String> strings_;
	for(int32_t i = 0;i < listBox_->numberOfStrings();i++)
		strings_.push_back(listBox_->stringAt(i));

    SERIALIZE_CONTAINER(strings,node)
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

ComboBoxDesc::ComboBoxDesc() :
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    textDesc_(new TextDesc)
{
}

ComboBoxDesc::~ComboBoxDesc()
{
    delete background_;
    delete border_;
    delete textDesc_;
}

void ComboBoxDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    textDesc_->setHorizontalAlignment(Horizontal_left);

    if(background_ != 0) background_->draw(area,WindowManager::instance().canvas());
    if(border_ != 0)     border_->draw(area,WindowManager::instance().canvas());

    const Vector2f topLeft = comboBox_->position();
    const Vector2f bottomRight = comboBox_->position() + comboBox_->size();
    FloatRect rect(topLeft,comboBox_->size());// bottomRight);

    // draw the text
    textDesc_->setText(comboBox_->text());
    Vector2f textSize = textDesc_->textSize();

    Vector2f halfSize = (bottomRight - topLeft) * 0.5f;
    Vector2f halfTextSize = textSize * 0.5f;

    Vector2f offset(6.0f, halfSize.y() - halfTextSize.y());

    FloatRect textRect = area;
    textRect.setSize(Vector2f(area.size().x() - 20.0f, area.size().y()));
    canvas->enableClipRectangle(textRect);
    textDesc_->draw(textRect,WindowManager::instance().canvas());
    canvas->disableClipRectangle();

    canvas->setColor(textDesc_->color());

    // get the points that make up the arrow
    Vector2f points[3];
    const Vector2f center = bottomRight + Vector2f(-10.0f, -halfSize.y());
    float horzOffset = halfSize.y() / 3.0f;
    if(horzOffset < 4.0f) horzOffset = 4.0f;
    if(horzOffset > 8.0f) horzOffset = 8.0f;
    float vertOffset = halfSize.y() / 3.0f;
    if(vertOffset < 4.0f) vertOffset = 4.0f;
    if(vertOffset > 8.0f) vertOffset = 8.0f;
    points[0] = center + Vector2f(-horzOffset, -vertOffset);
    points[1] = center + Vector2f( 0.0f,        vertOffset);
    points[2] = center + Vector2f( horzOffset, -vertOffset);

    // draw the arrow
    canvas->drawTriangle(points[0], points[1], points[2], true);
    canvas->setColor(Color(1, 1, 1, 1));
}

bool ComboBoxDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("ComboBoxDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,node,RenderDesc)
		CREATE_BY_NODE(border,node,RenderDesc)
		CREATE_BY_NODE(textDesc,node,TextDesc)
		TiXmlNode* desc = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(desc);
	}
    return true;
}

bool ComboBoxDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ComboBoxDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    textDesc_->saveProperty(&thisNode,"textDesc");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

}
