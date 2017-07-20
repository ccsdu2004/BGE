#include <BGE/System/Serialization.h>
#include <BGE/Graphics/Color.h>
#include <BGE/GUI/ProgressBar.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(ProgressBar,Window)

ProgressBar::ProgressBar(Window* parent,const FloatRect& rectangle,const Orientation orientation):
    Window(parent,L""),
    orientation_(orientation),
    percentage_(0),
    renderDesc_(0)
{
    setGeometry(rectangle);
}

ProgressBar::~ProgressBar()
{
}

Vector2f ProgressBar::recommendedSize() const
{
    return (orientation_ == Orientation_horizontal) ? Vector2f(150, 32) : Vector2f(32, 150);
}

Window& ProgressBar::onDraw()
{
    if(percentage() < 0)   setPercentage(0);
    if(percentage() > 100) setPercentage(100);

    ProgressBarDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->progressBarDesc_;

    renderDesc->setProgressBar(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());
    return *this;
}

WINDOW_LOAD_APPAREARANCE(ProgressBar)

bool ProgressBar::loadProperty(TiXmlNode* node)
{
    //DESERIALIZE_OBJECT(orientation,node,Orientation)
    DESERIALIZE_OBJECT(percentage,node,float)
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool ProgressBar::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(orientation,node)
    SERIALIZE_OBJECT(percentage,node)
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

ProgressBarDesc::ProgressBarDesc() :
    horzEmptyDesc_(new SolidSurfaceDesc(Color(1, 0, 0, 0.5f))),
    horzFullDesc_ (new SolidSurfaceDesc(Color(0, 1, 0, 0.5f))),
    vertEmptyDesc_(new SolidSurfaceDesc(Color(1, 0, 0, 0.5f))),
    vertFullDesc_ (new SolidSurfaceDesc(Color(0, 1, 0, 0.5f))),
    border_(new RectangleDesc())
{
}

ProgressBarDesc::~ProgressBarDesc()
{
    delete horzEmptyDesc_;
    delete horzFullDesc_;
    delete vertEmptyDesc_;
    delete vertFullDesc_;
    delete border_;
}

void ProgressBarDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    const Vector2f topLeft = area.position();
    const Vector2f bottomRight = area.bottomRight();

    const Vector2f barTopLeft = topLeft + Vector2f(1,1);
    const Vector2f barBottomRight = bottomRight + Vector2f(-1,-1);

    if(progressBar_->orientation() == Orientation_horizontal)
    {
        const float entireWidth = float(progressBar_->size().x()) - 2.0f;
        const float f = float(progressBar_->percentage()) * 0.01f;
        const unsigned int fullWidth  = (unsigned int)(entireWidth * f);

        if(fullWidth > 0)
        {
            int offs = (fullWidth > 0) ? 1 : 0;
            canvas->enableClipRectangle(FloatRect(barTopLeft - Vector2f(0, 1),
                Vector2f(barTopLeft.x() + fullWidth + offs, barBottomRight.y())-barTopLeft + Vector2f(0, 1)));
            if(horzFullDesc_ != 0)
                horzFullDesc_->draw(area,canvas);
        }

        if(entireWidth - fullWidth > 0)
        {
            canvas->enableClipRectangle(FloatRect(
                Vector2f(barTopLeft.x() + fullWidth, barTopLeft.y() - 1),
                barBottomRight + Vector2f(1, 0)-Vector2f(barTopLeft.x() + fullWidth,barTopLeft.y()-2)));
            if(horzEmptyDesc_ != 0)
                horzEmptyDesc_->draw(area,canvas);
        }
    }
    else
    {
        const float entireHeight = float(progressBar_->size().y()) - 2.0f;
        const float frevent = progressBar_->percentage() * 0.01f;
        const unsigned int fullHeight  = (unsigned int)(entireHeight * frevent);

        if(fullHeight > 0)
        {
            int offs = (entireHeight - fullHeight == 0) ? 1 : 0;
            canvas->enableClipRectangle(FloatRect(
                Vector2f(barTopLeft.x(), barBottomRight.y() - fullHeight - offs),
                barBottomRight + Vector2f(1, 0)-Vector2f(barTopLeft.x(), barBottomRight.y() - fullHeight - offs)));
            if(vertFullDesc_ != 0) vertFullDesc_->draw(area,WindowManager::instance().canvas());
        }

        if(entireHeight - fullHeight > 0)
        {
            canvas->enableClipRectangle(FloatRect(
                barTopLeft + Vector2f(0, -1), Vector2f(barBottomRight.x() + 1-barTopLeft.x(),
                barBottomRight.y() - fullHeight+2-barTopLeft.y())));
            if(vertEmptyDesc_ != 0) vertEmptyDesc_->draw(area,canvas);
        }
    }

    canvas->disableClipRectangle();
    canvas->setColor(Color(1, 1, 1, 1));

    if(border_ != 0)
        border_->draw(area,WindowManager::instance().canvas());
}

bool ProgressBarDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("ProgressBarDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(horzEmptyDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(horzFullDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(vertEmptyDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		TiXmlNode* window = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(window);
	}
	return true;
}

bool ProgressBarDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("ProgressBarDesc");
    horzEmptyDesc_->saveProperty(&thisNode,"horzEmptyDesc");
    horzFullDesc_->saveProperty(&thisNode,"horzFullDesc");
    vertEmptyDesc_->saveProperty(&thisNode,"vertEmptyDesc");
    border_->saveProperty(&thisNode,"border");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

}


