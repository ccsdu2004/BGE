#include <BGE/System/Err.h>
#include <BGE/GUI/Panel.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(Panel,Window)

Panel::Panel(const FloatRect& rect,Window* parent,const String& text):
    Window(parent,text),
    layouter_(0),
	buttonGroup_(0),
    renderDesc_(0)
{
    setGeometry(rect);
}

Panel::~Panel()
{
}

Panel& Panel::arrangeChildren()
{
    if(layouter_)
        layouter_->arrangeWithin();
    return *this;
}

bool Panel::process(const Message& message)
{
    bool ret = false;
    if(layouter_)
    {
        std::list<Window*> list = layouter_->windows();
        if(isVisible() && isEnabled())
        {
            for(auto it = list.begin();it != list.end(); ++it)
            {
                Window* window = *it;
                ret = window->process(message);
                if(ret)
                    break;
            }
        }
    }
    if(!ret)
        Window::process(message);

    return ret;
}

bool Panel::doesHierarchyContain(Window* target) const
{
    bool ret = false;
    if(layouter_)
    {
        std::list<Window*> list = layouter_->windows();
        for(auto it = list.begin();it != list.end(); ++it)
        {
            Window* window = *it;
            if(window->doesHierarchyContain(target))
            {
                ret = true;
                break;
            }
        }
    }
    return (ret || Window::doesHierarchyContain(target));
}

Vector2f Panel::recommendedSize() const
{
    return Vector2f(150, 150);
}

Window& Panel::onDraw()
{
    PanelDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->panelDesc_;

    renderDesc->draw(geometry(),WindowManager::instance().canvas());

    if(layouter_)
    {
        std::list<Window*> list = layouter_->windows();
        for(auto it = list.begin(); it != list.end(); ++it)
        {
            (*it)->render();
        }
    }
    return *this;
}

Window* Panel::windowBelow(const Vector2f& p)
{
    Window* ret = Window::windowBelow(p);

    if(ret != 0 && layouter_)
    {
        std::list<Window*> list = layouter_->windows();
        for(auto it = list.begin(); it != list.end(); ++it)
        {
            Window* window = *it;
            Window* cw = window->windowBelow(p);
            if(cw != 0)
            {
                ret = cw;
                break;
            }
        }
    }
    return ret;
}

void Panel::onResize()
{
    arrangeChildren();
}

void Panel::onMove()
{
    arrangeChildren();
}

Panel& Panel::setLayout(Layout* layouter)
{
    //if(layouter_ != NULL)
    //    layouter_->deleteLater();
    if(layouter)
    {
        layouter_ = layouter;
        layouter_->setParent(this);
    }
    return *this;
}

WINDOW_LOAD_APPAREARANCE(Panel)

bool Panel::loadProperty(TiXmlNode* node)
{
	if(buttonGroup_)
		buttonGroup_->deleteLater();
	buttonGroup_ = NULL;

	if(layouter_)
		layouter_->deleteLater();
	layouter_ = NULL;

	TiXmlElement* groupNode = node->FirstChildElement("buttongroup");
	if(groupNode)
	{
		std::string classname = groupNode->Attribute("class");
		Rtti* rtti = Rtti::rttiByObjectName(classname);
		if(rtti)
		{
			Object* object = rtti->createObject();
			buttonGroup_ = Object::castTo<ButtonGroup>(object);
			if(buttonGroup_)
			{
				buttonGroup_->setName(groupNode->Attribute("name"));
				buttonGroup_->setParent(this);
			}
			else
				err()<<"create class:"<<classname<<" failed\n";
		}
	}

    TiXmlElement* layout = node->FirstChildElement("Layout");
    if(layout)
    {
        const char* tag = layout->Attribute("class");
		std::string classname;
		if(tag)
			classname = tag;
		Rtti* rtti = Rtti::rttiByObjectName(classname);
		if(!rtti)
			return false;
        Object* object = rtti->createObject();
        if(!object)
            return false;
        
		layouter_ = Object::castTo<Layout>(object);
        if(layouter_)
		{
			setLayout(layouter_);
			layouter_->loadProperty(layout);
			layouter_->arrangeWithin();
		}
	}

    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool Panel::saveProperty(TiXmlNode* node)
{
	if(buttonGroup_)
	{
	    std::string buttongroup_ = buttonGroup_->name();
		TiXmlElement group("buttongroup");
		group.SetAttribute("class",buttonGroup_->objectName());
		group.SetAttribute("name",buttongroup_);
		node->InsertEndChild(group);
		if(buttongroup_.empty())
			err() << "BasicButton need a name.\n";
	}

    if(layouter_)
    {
        TiXmlElement layout("Layout");
		layout.SetAttribute("class",layouter_->objectName());
        layouter_->saveProperty(&layout);
        node->InsertEndChild(layout);
    }
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void PanelDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    if(background_ != 0)
        background_->draw(area,canvas);
    if(border_ != 0)
        border_->draw(area,canvas);
}

bool PanelDesc::loadProperty(TiXmlNode* node)
{
	TiXmlNode* thisNode = node->FirstChildElement("PanelDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		TiXmlNode* desc = thisNode->FirstChildElement("RenderDesc");
		if(desc)
			RenderDesc::loadProperty(desc);
	}
	return true;
}

bool PanelDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("PanelDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

}

