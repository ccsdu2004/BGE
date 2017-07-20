#include <BGE/System/Err.h>
#include <BGE/GUI/Layout.h>
#include <BGE/GUI/Window.h>
#include <algorithm>

namespace bge
{

IMPLEMENT_OBJECT(LayoutInfo,Object)
IMPLEMENT_OBJECT(Layout,Object)

Layout::Layout(Window* parent):
    Object(parent),
    spacing_(4.0f)
{
}

Layout::~Layout()
{
}

Window* Layout::parse(TiXmlElement* element)
{
	std::string classname = element->Attribute("class");
	Rtti* rtti = Rtti::rttiByObjectName(classname);
	if(rtti)
	{
		Object* object = rtti->createObject();
		Window* window = Object::castTo<Window>(object);
		if(window)
		{
			window->loadProperty(element);
			std::cout<<window->size().x_<<" "<<window->size().y_<<std::endl;
			return window;
		}
		else
			err()<<"create class:"<<classname<<" failed\n";
	}
	return NULL;
}

bool Layout::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(spacing,node)
    Object::saveProperty(node);
    return true;
}

bool Layout::loadProperty(TiXmlNode* node)
{
	DESERIALIZE_OBJECT(spacing,node,float)
	Object::loadProperty(node);
    return true;
}

}

