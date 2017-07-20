#include <BGE/System/Object.h>
#include <BGE/System/Err.h>
#include <BGE/System/Serialization.h>
#include <algorithm>

#define IMPLEMENT_BASE_OBJECT(object)\
    const bge::Rtti  object::rtti##object(#object,NULL,sizeof(object),NULL);\
    inline const bge::Rtti* object::getRtti()const\
    {\
	    return RUNTIME_RTTI(object);\
	}\
    inline std::string object::objectName()const\
    {\
	    return getRtti()->name();\
	}

namespace bge
{

IMPLEMENT_BASE_OBJECT(Object)

Object::Object(Object* parent)
{
	parent_ = NULL;
	children_ = new std::list<Object*>();
    isWidget_ = false;
    if(parent)
	    parent->insertChild(this);
	else
        parent_ = NULL;
}

Object::~Object()
{
}

bool Object::isExactly(const Rtti& type)const
{
    return getRtti()->isExactly(type);
}

bool Object::isDerived(const Rtti& base)const
{
    return getRtti()->isDerived(base);
}

bool Object::isExactlyTypeOf(const Object* obj)const
{
    return obj && getRtti()->isExactly(*obj->getRtti());
}

bool Object::isDerivedTypeOf(const Object* obj)const
{
    return obj && getRtti()->isDerived(*obj->getRtti());
}

void Object::setName(const std::string &name)
{
    if(!name_.empty())
    {
        err() << " object's name cannot to be changed.\n";
        return;
    }

    name_ = name;
}

std::string Object::name()const
{
    return name_;
}

void Object::setParent(Object* ptr)
{
    remove();
	parent_ = ptr;

	if(parent_)
		parent_->insertChild(this);
}

Object* Object::parent()const
{
    return parent_;
}

void Object::removeChild(Object* child)
{
    if(hasChild(child))
    {
		children_->remove(child);
	}
}

bool Object::hasChild(Object* child)
{
	if(children_->empty())
		return false;
	return std::find(children_->begin(),children_->end(),child) != children_->end();
}

void Object::insertChild(Object* child)
{
	if(child && (child != this))
	{
		child->remove();
		children_->push_back(child);
		child->parent_ = this;
	}
}

void Object::remove()
{
	if(parent_)
		parent_->removeChild(this);
	parent_ = NULL;
}

void Object::removeAllChild()
{
    auto itr = children_->begin();
	for(;itr != children_->end(); ++itr)
	{
		(*itr)->parent_ = NULL;
		(*itr)->deleteLater();
	}

	children_->clear();
}

void Object::deleteLater()
{
    remove();
	removeAllChild();
    delete this;
}

void Object::setWidgetType(bool widget)
{
    isWidget_ = widget;
}

Object* Object::findChild(const std::string& name)
{
    auto itr = children_->begin();
    while(itr != children_->end())
    {
        Object* child = *itr;
        if(child->name() == name)
            return child;
        itr ++;
    }

    return NULL;
}

bool Object::loadProperty(TiXmlNode* node)
{
    std::string objectName_;
    DESERIALIZE_OBJECT(objectName,node,std::string)
    name_ = objectName_;
    return true;
}

bool Object::saveProperty(TiXmlNode* node)
{
    std::string objectName_ = name();
    SERIALIZE_OBJECT(objectName,node)
    std::string className_ = getRtti()->name();
    SERIALIZE_OBJECT(className,node)
    return true;
}

}










