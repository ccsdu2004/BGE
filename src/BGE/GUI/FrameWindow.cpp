#include <BGE/System/NonCopyable.h>
#include <BGE/System/Err.h>
#include <BGE/GUI/FrameWindow.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(FrameWindow,Window)

class FrameWindowMouseListener : public MouseListener, NonCopyable
{
public:
    FrameWindowMouseListener(FrameWindow& frame):frameWindow_(frame){}
    virtual ~FrameWindowMouseListener(){}
protected:
    virtual bool moved(MouseMessage& message);
    virtual bool clicked(MouseClickMessage& message);
private:
    FrameWindow& frameWindow_;
    Vector2f oldMousePos_;
};

FrameWindow::FrameWindow(const FloatRect& rect,Window* parent,const String& text):
    Window(parent,text),
    clientWindow_(0),
    captureCount_(0),
    isMovable_(false),
    renderDesc_(0)
{
    setGeometry(rect);
    mouseListener_ = new FrameWindowMouseListener(*this);
    addMessageListener(mouseListener_);
}

FrameWindow::~FrameWindow()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
}

void FrameWindow::deleteLater()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
	mouseListener_ = NULL;
    Window::deleteLater();
}

FloatRect FrameWindow::clientRectangle() const
{
    FrameWindowDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->frameWindowDesc_;

    const Vector2f topLeft = position() +
        Vector2f(renderDesc->leftBorderWidth(), renderDesc->topBorderWidth());
    const Vector2f bottomRight = position() + size() -
        Vector2f(renderDesc->rightBorderWidth(), renderDesc->bottomBorderWidth());
    return FloatRect(topLeft,bottomRight-topLeft);
}

FrameWindow& FrameWindow::setClientPanel(Panel* panel)
{
    clientWindow_ = panel;
    clientWindow_->setParent(this);
    clientWindowChanged();
    return *this;
}

FrameWindow& FrameWindow::clientWindowChanged()
{
    if(clientPanel() != 0)
    {
        clientPanel()->setGeometry(clientRectangle());
        clientPanel()->setParent(this);
    }
    return *this;
}

bool FrameWindow::process(const Message& message)
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        if(clientPanel() != 0)
            ret = clientPanel()->process(message);
        if(!ret)
            ret = Window::process(message);
    }

    return ret;
}

bool FrameWindow::doesHierarchyContain(Window* window)const
{
    bool ret = false;
    if(clientPanel() != 0)
        ret = clientPanel()->doesHierarchyContain(window);
    return (ret || Window::doesHierarchyContain(window));
}

Vector2f FrameWindow::recommendedSize() const
{
    return Vector2f(200,200);
}

Window& FrameWindow::onDraw()
{
    Canvas* canvas = WindowManager::instance().canvas();
    FrameWindowDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->frameWindowDesc_;

    renderDesc->setFrameWindow(this);
    renderDesc->draw(geometry(),WindowManager::instance().canvas());

    if(clientPanel() != 0)
    {
        canvas->enableClipRectangle(clientRectangle());
        clientPanel()->render();
        canvas->disableClipRectangle();
    }
    return *this;
}

Window* FrameWindow::windowBelow(const Vector2f& p)
{
    Window* ret = Window::windowBelow(p);
    if(ret == (Window*)(this) && clientPanel())
    {
        Window* cw = clientPanel()->windowBelow(p);
        if(cw != 0)
            ret = cw;
    }
    return ret;
}

bool FrameWindow::isMovable() const
{
    return isMovable_;
}

FrameWindow& FrameWindow::enableMovement()
{
    isMovable_ = true;
    return *this;
}

FrameWindow& FrameWindow::disableMovement()
{
    isMovable_ = false;
    return *this;
}

void FrameWindow::onResize()
{
    if(clientPanel() != 0)
        clientPanel()->setGeometry(clientRectangle());
}

void FrameWindow::onMove()
{
    if(clientPanel() != 0)
        clientPanel()->setGeometry(clientRectangle());
}

WINDOW_LOAD_APPAREARANCE(FrameWindow)

bool FrameWindow::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(isMovable,node,bool)
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    if(clientWindow_)
        clientWindow_->deleteLater();
	clientWindow_ = NULL;

    TiXmlElement* client = node->FirstChildElement("Client");
    if(client)
    {
        std::string classname = client->Attribute("class");
		if(classname != "Panel")
			err()<<"FrameWindow 's clientWindow must to be Panel\n";
		else
		{
			Object* object = Rtti::rttiByObjectName(classname)->createObject();
			if(object)
			{
				Panel* panel = Object::castTo<Panel>(object);
				if(panel)
				{
					panel->loadProperty(client);
					panel->arrangeChildren();
					setClientPanel(panel);
				}
			}
		}
    }
    return true;
}

bool FrameWindow::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(isMovable,node)
    {
        if(clientWindow_)
        {
            TiXmlElement client("Client");
            client.SetAttribute("class",clientWindow_->getRtti()->name());
            clientWindow_->saveProperty(&client);
            node->InsertEndChild(client);
        }
    }
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

bool FrameWindowMouseListener::moved(MouseMessage& message)
{
    if(frameWindow_.isMovable())
    {
        if(WindowManager::instance().captureWindow() == &frameWindow_)
        {
            Vector2f delta(message.x() - oldMousePos_.x(), message.y() - oldMousePos_.y());
            frameWindow_.setPosition(frameWindow_.position() + delta);
        }
        oldMousePos_.setX(message.x());
        oldMousePos_.setY(message.y());
        return true;
    }
    return false;
}

bool FrameWindowMouseListener::clicked(MouseClickMessage& message)
{
    bool isHandled = false;
    if(message.button() == MouseClickMessage::left)
    {
        if(message.event() == MouseClickMessage::pressed)
        {
            if(frameWindow_.isMovable())
            {
                if(WindowManager::instance().captureWindow() != &frameWindow_)
                    WindowManager::instance().setMouseCapture(&frameWindow_);
                frameWindow_.captureCount_++;
            }
            frameWindow_.setAsActiveWindow();
            isHandled = true;
        }
        else if(message.event() == MouseClickMessage::released)
        {
            if(frameWindow_.isMovable())
            {
                if(frameWindow_.captureCount_ == 1)
                    WindowManager::instance().releaseMouseCapture();
                if(frameWindow_.captureCount_ > 0)
                    frameWindow_.captureCount_--;
            }
            isHandled = true;
        }
    }
    return isHandled;
}

FrameWindowDesc::FrameWindowDesc():
    topLeftDesc_(new DefaultFrameDesc(FrameSection_topLeft)),
    titleDesc_(new DefaultFrameDesc(FrameSection_title)),
    topDesc_(new DefaultFrameDesc(FrameSection_top)),
    topRightDesc_(new DefaultFrameDesc(FrameSection_topRight)),
    rightDesc_(new DefaultFrameDesc(FrameSection_right)),
    bottomRightDesc_(new DefaultFrameDesc(FrameSection_bottomRight)),
    bottomDesc_(new DefaultFrameDesc(FrameSection_bottom)),
    bottomLeftDesc_(new DefaultFrameDesc(FrameSection_bottomLeft)),
    leftDesc_(new DefaultFrameDesc(FrameSection_left)),
    textDesc_(new TextDesc(WindowManager::instance().defaultFont(),18,0,0,0,Color(0.0f,0.0f,0.3f,1.0f))),
    topBorderWidth_(30.0f),
    leftBorderWidth_(30.0f),
    rightBorderWidth_(30.0f),
    bottomBorderWidth_(30.0f),
    frameWindow_(0)
{
}

FrameWindowDesc::~FrameWindowDesc()
{
    delete topLeftDesc_;
    delete titleDesc_;
    delete topDesc_;
    delete topRightDesc_;
    delete rightDesc_;
    delete bottomRightDesc_;
    delete bottomDesc_;
    delete bottomLeftDesc_;
    delete leftDesc_;
    delete textDesc_;
}

void FrameWindowDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    canvas->setColor(Color(1,1,1,1));

    const Vector2f topLeft = frameWindow_->position();
    const Vector2f bottomRight = frameWindow_->position() + frameWindow_->size();
    const Vector2f topRight(bottomRight.x(), topLeft.y());
    const Vector2f bottomLeft(topLeft.x(), bottomRight.y());

    const Vector2f topLeftCenter     = topLeft + Vector2f(leftBorderWidth_, topBorderWidth_);
    const Vector2f bottomRightCenter = bottomRight + Vector2f(-rightBorderWidth_, -bottomBorderWidth_);
    const Vector2f bottomLeftCenter  = Vector2f(topLeft.x() + leftBorderWidth_, bottomRight.y() - bottomBorderWidth_);
    const Vector2f topRightCenter    = Vector2f(bottomRight.x() - rightBorderWidth_, topLeft.y() + topBorderWidth_);

    topLeftDesc_->draw(FloatRect(topLeft,topLeftCenter-topLeft),WindowManager::instance().canvas());
    topRightDesc_->draw(FloatRect(topRight-Vector2f(rightBorderWidth_,0),Vector2f(rightBorderWidth_,rightBorderWidth_)),WindowManager::instance().canvas());
    rightDesc_->draw(FloatRect(topRightCenter, Vector2f(bottomRight.x(), bottomRightCenter.y())-topRightCenter),WindowManager::instance().canvas());
    bottomRightDesc_->draw(FloatRect(bottomRightCenter, bottomRight-bottomRightCenter),WindowManager::instance().canvas());
    bottomDesc_->draw(FloatRect(bottomLeftCenter, Vector2f(bottomRightCenter.x(), bottomRight.y())-bottomLeftCenter),WindowManager::instance().canvas());
    bottomLeftDesc_->draw(FloatRect(bottomLeftCenter-Vector2f(rightBorderWidth_,0),Vector2f(rightBorderWidth_,rightBorderWidth_)),WindowManager::instance().canvas());
    leftDesc_->draw(FloatRect(Vector2f(topLeft.x(), topLeftCenter.y()), bottomLeftCenter-Vector2f(topLeft.x(),topLeftCenter.y())),WindowManager::instance().canvas());

    textDesc_->setText(frameWindow_->text());
    Vector2f textSize = textDesc_->textSize();
    Vector2f barSize(textSize.x()+10.0f,topBorderWidth_);

    const float clientWidth = bottomRightCenter.x() - topLeftCenter.x();
    if(barSize.x() > clientWidth)
        barSize.setX(clientWidth);

    Vector2f barMini = topLeft + Vector2f(leftBorderWidth_,0.0f);

    titleDesc_->draw(FloatRect(Vector2f(topLeftCenter.x(), topLeft.y()),
        Vector2f(barSize.x(), topLeftCenter.y()-topLeft.y())),WindowManager::instance().canvas());

    topDesc_->draw(FloatRect(Vector2f(topLeftCenter.x() + barSize.x(), topLeft.y()),
        topRightCenter-Vector2f(topLeftCenter.x() + barSize.x(), topLeft.y())),WindowManager::instance().canvas());

    textDesc_->draw(FloatRect(Vector2f(topLeftCenter.x(), topLeft.y()),
                    Vector2f(barSize.x(), topLeftCenter.y()-topLeft.y())),WindowManager::instance().canvas());

    canvas->setColor(Color(1, 1, 1, 1));
}

bool FrameWindowDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("FrameWindowDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(topLeftDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(titleDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(topDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(topRightDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(rightDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(bottomRightDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(bottomDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(bottomLeftDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(leftDesc,thisNode,RenderDesc)
		CREATE_BY_NODE(textDesc,thisNode,TextDesc)
		DESERIALIZE_OBJECT(topBorderWidth,thisNode,float)
		DESERIALIZE_OBJECT(leftBorderWidth,thisNode,float)
		DESERIALIZE_OBJECT(rightBorderWidth,thisNode,float)
		DESERIALIZE_OBJECT(bottomBorderWidth,thisNode,float)
		TiXmlNode* window = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(window);
	}
    return true;
}

bool FrameWindowDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("FrameWindowDesc");
    topLeftDesc_->saveProperty(&thisNode,"topLeftDesc");
    titleDesc_->saveProperty(&thisNode,"titleDesc");
    topDesc_->saveProperty(&thisNode,"topDesc");
    topRightDesc_->saveProperty(&thisNode,"topRightDesc");
    rightDesc_->saveProperty(&thisNode,"rightDesc");
    bottomRightDesc_->saveProperty(&thisNode,"bottomRightDesc");
    bottomDesc_->saveProperty(&thisNode,"bottomDesc_");
    bottomLeftDesc_->saveProperty(&thisNode,"bottomLeftDesc");
    leftDesc_->saveProperty(&thisNode,"leftDesc");
    textDesc_->saveProperty(&thisNode,"textDesc");
    SERIALIZE_OBJECT(topBorderWidth,&thisNode)
    SERIALIZE_OBJECT(leftBorderWidth,&thisNode)
    SERIALIZE_OBJECT(rightBorderWidth,&thisNode)
    SERIALIZE_OBJECT(bottomBorderWidth,&thisNode)
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

}
