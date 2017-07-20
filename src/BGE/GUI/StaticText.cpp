#include <BGE/System/Serialization.h>
#include <BGE/Graphics/Color.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/Graphics/TextRenderer.h>
#include <BGE/GUI/StaticText.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(StaticText,Window)

StaticText::StaticText(Window* parent,const String& text):
    Window(parent,text),
    renderDesc_(0)
{
}

StaticText::~StaticText()
{
}

Vector2f StaticText::recommendedSize() const
{
    StaticTextDesc* renderDesc = renderDesc_;
    if(renderDesc == NULL)
        renderDesc = &WindowManager::instance().propertyScheme()->staticTextDesc_;

    renderDesc->textDesc().setText(text());
    return renderDesc->textDesc().textSize() + Vector2f(8, 8);
}

Window& StaticText::onDraw()
{
    StaticTextDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->staticTextDesc_;

    renderDesc->setStaticText(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());
    return *this;
}

WINDOW_LOAD_APPAREARANCE(StaticText)

bool StaticText::loadProperty(TiXmlNode* node)
{
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool StaticText::saveProperty(TiXmlNode* node)
{
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

StaticTextDesc::StaticTextDesc():
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    textDesc_(new TextDesc)
{
}

StaticTextDesc::~StaticTextDesc()
{
    delete background_;
    delete border_;
    delete textDesc_;
}

void StaticTextDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    if(background_ != 0)
        background_->draw(area,canvas);
    if(border_ != 0)
        border_->draw(area,canvas);

    canvas->enableClipRectangle(area);

    FloatRect rect(area);
    rect.expandBy(-1.0f);

    (*textDesc_)
        .setText(staticText_->text())
        .drawAsMultiLine()
        .draw(rect,canvas);

    canvas->disableClipRectangle();
    canvas->setColor(Color(1, 1, 1, 1));
}

bool StaticTextDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("StaticTextDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		CREATE_BY_NODE(textDesc,thisNode,TextDesc)
		thisNode = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(thisNode);
		return true;
	}
	return false;
}

bool StaticTextDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("StaticTextDesc");
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

