#include <BGE/System/Serialization.h>
#include <BGE/System/String.h>
#include <BGE/System/Err.h>
#include <BGE/System/Math.h>
#include <list>

namespace bge
{

void serialize(const ObjectNamer<Vector2f>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    SERIALIZE_OBJECT_3("x",object.value().x_,&element)
    SERIALIZE_OBJECT_3("y",object.value().y_,&element)
    node->InsertEndChild(element);
}

void serialize(const ObjectNamer<Vector3f>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    SERIALIZE_OBJECT_3("x",object.value().x_,&element)
    SERIALIZE_OBJECT_3("y",object.value().y_,&element)
    SERIALIZE_OBJECT_3("z",object.value().z_,&element)
    node->InsertEndChild(element);
}

void serialize(const ObjectNamer<String>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
	std::string buffer;
	if(!object.value().empty())
	{
		char ch[5] = {0};
		auto itr = object.value().begin();
		while(itr != object.value().end())
		{
		    buffer += toString<uint32_t>(*itr);
			buffer += ',';
			itr ++;
		}
		buffer = buffer.substr(0,buffer.size()-1);
	}
    TiXmlText text(buffer);
    element.InsertEndChild(text);
    node->InsertEndChild(element);
}

void serialize(const ObjectNamer<std::string>& object,TiXmlNode* node)
{
    TiXmlElement element(object.name());
    TiXmlText text(object.value());
    element.InsertEndChild(text);
    node->InsertEndChild(element);
}

}
