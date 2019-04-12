//! 2010.06.01
#ifndef BGE_SYSTEM_Serialization_H
#define BGE_SYSTEM_Serialization_H
#include <BGE/Config.h>
#include <BGE/System/TinyXml.h>
#include <BGE/System/Math.h>
#include <BGE/System/String.h>
#include <BGE/System/Enum.h>
#include <utility>
#include <string>

namespace bge
{

template<typename T>
class ObjectNamer
{
public:
    ObjectNamer(const char* name,T& object):node_(name,object)
    {
    }
public:
    const std::string name()const{return node_.first;}
    T& value()const{return node_.second;}
    void setValue(const T& value)
    {
        node_.second = value;
    }
private:
    std::pair<std::string,T&> node_;
};

template<typename T>
void serialize(const ObjectNamer<T>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    TiXmlText text(toString(object.value()));
    element.InsertEndChild(text);
    node->InsertEndChild(element);
}

void BGE_EXPORT_API serialize(const ObjectNamer<std::string>& object,TiXmlNode* node);
void BGE_EXPORT_API serialize(const ObjectNamer<String>& object,TiXmlNode* node);
void BGE_EXPORT_API serialize(const ObjectNamer<Vector2<float> >& object,TiXmlNode* node);
void BGE_EXPORT_API serialize(const ObjectNamer<Vector2i>& object, TiXmlNode* node);
void BGE_EXPORT_API serialize(const ObjectNamer<Vector3<float> >& object,TiXmlNode* node);

template<typename T>
ObjectNamer<T> NameObject(T& object,const char* name)
{
    return ObjectNamer<T>(name,object);
}

#define TAGGED_OBJECT(x) NameObject(x##_,#x)
#define SERIALIZE_OBJECT(x,node) \
    {\
        serialize(TAGGED_OBJECT(x),node);\
    }

#define SERIALIZE_OBJECT_3(name,value,node)\
    {\
        serialize(bge::NameObject(value,name),node);\
    }

template<class Container>
void serializeContainer(const ObjectNamer<Container>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    Container container(object.value());
    auto itr = container.begin();
    while(itr != container.end())
    {
        auto item_ = *itr;
        SERIALIZE_OBJECT(item,&element);
        itr ++;
    }
    node->InsertEndChild(element);
}

#define SERIALIZE_CONTAINER(x,node) \
    {\
        serializeContainer(TAGGED_OBJECT(x),node);\
    }

template<class T>
inline T deserialize(const char* name,TiXmlNode* node)
{
    TiXmlElement* element = node->FirstChildElement(name);
	if(!element)
		return T();
    std::string value = element->GetText();
    return fromString<T>(value);
}

template<class Container,class T>
inline Container deserializeContainer(const char* name,TiXmlNode* node)
{
    Container container;
    TiXmlElement* element = node->FirstChildElement(name);
    while(element)
    {
        std::string value = element->GetText();
        container.push_back(fromString<T>(value));
        element = element->NextSiblingElement();
    }
    return container;
}

template<>
inline Vector2<float> deserialize<>(const char* name,TiXmlNode* node)
{
    TiXmlNode* element = node->FirstChildElement(name);
    Vector2<float> vec2;
    vec2.x_ = deserialize<float>("x",element);
    vec2.y_ = deserialize<float>("y",element);
    return vec2;
}

template<>
inline Vector3<float> deserialize<>(const char* name,TiXmlNode* node)
{
    TiXmlNode* element = node->FirstChildElement(name);
    Vector3<float> vec3;
    vec3.x_ = deserialize<float>("x",element);
    vec3.y_ = deserialize<float>("y",element);
    vec3.z_ = deserialize<float>("z",element);
    return vec3;
}

template<>
inline String deserialize(const char* name,TiXmlNode* node)
{
    TiXmlElement* element = node->FirstChildElement(name);
    std::string text = element->GetText();
	String value;
	splitAndConvert(text,',',value);
	return value;
}

}

#define DESERIALIZE_OBJECT(x,node)\
    x##_ = deserialize<decltype(x##_)>(#x,node);

#define DESERIALIZE_OBJECT_CONTAINER(x,node,Container,T)\
    x##_ = deserializeContainer<Container,T>(#x,node);

#endif
