#include <BGE/GUI/CheckBox.h>
#include <BGE/GUI/Message.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(CheckBox,BasicButton)

CheckBox::CheckBox(Window* parent,const String& text,ButtonGroup* group):
    BasicButton(parent,text,group),
    renderDesc_(0)
{
    toggleOn();
}

CheckBox::~CheckBox()
{
    delete renderDesc_;
}

Window& CheckBox::onDraw()
{
    CheckBoxDesc* checkBoxDesc = renderDesc_;
    if(checkBoxDesc == 0)
        checkBoxDesc = &WindowManager::instance().propertyScheme()->checkBoxDesc_;

    checkBoxDesc->setButton(*this);
    checkBoxDesc->setCheckBox(this);
    checkBoxDesc->draw(geometry(),WindowManager::instance().canvas());
    return *this;
}

CheckBox& CheckBox::enableChecked()
{
    setMouseOverStatus(true);
    return *this;
}

CheckBox& CheckBox::disableChecked()
{
    setMouseOverStatus(false);
    return *this;
}

CheckBox& CheckBox::toggle()
{
    if(isChecked())
        disableChecked();
    else
        enableChecked();
    return *this;
}

Vector2f CheckBox::recommendedSize() const
{
    const Vector2f textSize = buttonDesc().defaultTextSize();
    return textSize + Vector2f(textSize.y() + 8.0f, 8.0f);
}

WINDOW_LOAD_APPAREARANCE(CheckBox)

bool CheckBox::loadProperty(TiXmlNode* node)
{
    //DESERIALIZE_OBJECT(value,node,int)
    TiXmlNode* window = node->FirstChildElement("BasicButton");
    BasicButton::loadProperty(window);
    return true;
}

bool CheckBox::saveProperty(TiXmlNode* node)
{
    //SERIALIZE_OBJECT(value,node)
    TiXmlElement element("BasicButton");
    BasicButton::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

IMPLEMENT_OBJECT(CheckBoxDesc,RenderDesc)

CheckBoxDesc::CheckBoxDesc():
    checked_(new DefaultCheckDesc()),
    unchecked_(new RectangleDesc()),
    checkBox_(0)
{
}

CheckBoxDesc::~CheckBoxDesc()
{
}

bool CheckBoxDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("CheckBoxDesc");
	if(thisNode)
	{
        CREATE_BY_NODE(unchecked,node,RenderDesc)
        CREATE_BY_NODE(checked,node,RenderDesc)
		TiXmlNode* window = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(window);
	}
	return true;
}

bool CheckBoxDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("CheckDesc");
    unchecked_->saveProperty(&thisNode,"unchecked");
    checked_->saveProperty(&thisNode,"checked");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

void CheckBoxDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    BasicButtonDesc::draw(area,canvas);

    const float edgeLength = checkBox_->size().y() * 0.66f;
    const float verticalOffset = (area.size().y() - edgeLength) * 0.5f;
    const Vector2f squareTopLeft = checkBox_->position() + Vector2f(4.0f, verticalOffset);
    FloatRect rect(squareTopLeft,Vector2f(edgeLength, edgeLength));

    if(checkBox_->isChecked())
        checked_->draw(rect,canvas);
    else
        unchecked_->draw(rect,canvas);
}

}
