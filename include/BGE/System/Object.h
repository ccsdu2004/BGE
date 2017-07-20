//! 2009.11.06
#ifndef BGE_SYSTEM_OBJECT_H
#define BGE_SYSTEM_OBJECT_H
#include <BGE/Config.h>
#include <BGE/System/Deleter.h>
#include <BGE/System/TinyXml.h>
#include <list>
#include <string>
#include <map>

#define RUNTIME_RTTI(object)\
    (bge::Rtti*)(&object::rtti##object)

#define DECLARE_OBJECT(object)\
public: \
	static  const bge::Rtti rtti##object;\
	virtual const bge::Rtti* getRtti()const;\
	virtual std::string objectName()const;

#define IMPLEMENT_OBJECT(object,parent)\
    const bge::Rtti   object::rtti##object(#object,RUNTIME_RTTI(parent),sizeof(object),/*CREATE_OBJECT_MACROS(object)*/0);\
    const bge::Rtti*  object::getRtti()const\
    {\
	    return  RUNTIME_RTTI(object);\
	}\
    std::string object::objectName()const\
    {\
	    return  getRtti()->name();\
	}

#define IMPLEMENT_OBJECT_CREATE(object,parent)\
    const bge::Rtti   object::rtti##object(#object,RUNTIME_RTTI(parent),sizeof(object),CREATE_OBJECT_MACROS(object));\
    const bge::Rtti*  object::getRtti()const\
    {\
	    return  RUNTIME_RTTI(object);\
	}\
    std::string object::objectName()const\
    {\
	    return  getRtti()->name();\
	}

namespace bge
{

class Rtti;
class ObjectImpl;

class BGE_EXPORT_API Object
{
public:
	Object(Object* parent = NULL);
//protected:
    virtual ~Object();
public:
    bool  isExactly(const Rtti& type)const;
    bool  isDerived(const Rtti& base)const;
    bool  isExactlyTypeOf(const Object* obj)const;
    bool  isDerivedTypeOf(const Object* obj)const;
public:
    std::string name()const;
    void setName(const std::string &name);

    bool isWidgetType()const;

    Object* parent()const;
    void setParent(Object* parent);

    Object* findChild(const std::string& name);
    bool hasChild(Object* obj);

	void remove();
	void removeAllChild();

    // no call.
    void removeChild(Object* child);
    void insertChild(Object* child);
public:
	template<class T>
    static T* castTo(Object* o)
	{
	    return dynamic_cast<T*>(o);
	}

    virtual void deleteLater();
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual void setWidgetType(bool widget);
private:
    Object* parent_;
    std::string name_;
	std::list<Object*>* children_;
    bool isWidget_;
private:
	DECLARE_OBJECT(Object)
};

typedef Object* (* ObjectCreatePtr)();

class BGE_EXPORT_API Rtti
{
public:
    Rtti(const std::string &name,const Rtti* parent,uint32_t size,ObjectCreatePtr ptr);
	~Rtti();
public:
    std::string name()const;
    uint32_t diskUsed()const;

    bool isExactly(const Rtti& type)const;
    bool isDerived(const Rtti& type)const;

    Object* createObject();

	static const Rtti* first(){return first_;}
	const Rtti* next()const{return next_;}
    const Rtti* parent()const{return parent_;}
	static Rtti* rttiByObjectName(const std::string & objectname);
private:
    const std::string   name_;
    const Rtti*         parent_;
    uint32_t            size_;
	ObjectCreatePtr     creator_;
    static Rtti*        first_;
	Rtti*               next_;
	static std::map<std::string,Rtti*> rttitable;
};

struct ObjectFactory
{
    template<class Base,class Product>
    static Base* Create()
    {
	    return new Product;
	}
};

}

#define CREATE_OBJECT_MACROS(product)\
    ((ObjectCreatePtr)(&ObjectFactory::Create<Object,product>))

#endif




