#include <BGE/System/Object.h>

namespace bge
{

Rtti* Rtti::first_ = NULL;
std::map<std::string,Rtti*> Rtti::rttitable;

std::string Rtti::name()const
{
    return name_;
}

uint32_t Rtti::diskUsed()const
{
    return size_;
}

bool Rtti::isExactly (const Rtti& type)const
{
    return &type == this;
}

Rtti::Rtti(const std::string &name,const Rtti* parent,uint32_t size,ObjectCreatePtr ptr):
name_(name),
parent_(parent),
size_(size),
creator_(ptr),
next_(first_)
{
    first_ = this;
}

Rtti::~Rtti()
{
	if(this == first_)
	{
		first_ = next_;
	}
	else
	{
		Rtti * info = first_;
		while(info)
		{
			if(info->next_ == this)
			{
				info->next_ = next_;
				break;
			}
			info = info->next_;
		}
	}
}

bool Rtti::isDerived(const Rtti& type)const
{
    const Rtti* search = this;
    while (search)
    {
        if (search == &type)
        {
            return true;
        }
        search = search->parent_;
    }
    return false;
}

Object* Rtti::createObject()
{
    if(creator_)
	    return creator_();
	return NULL;
}

Rtti* Rtti::rttiByObjectName(const std::string & objectname)
{
    if(rttitable[objectname])
		return rttitable[objectname];
	else
        for(Rtti* info = first_; info ; info = info->next_)
        {
            if(info->name() == objectname)
            {
                rttitable[objectname] = info;
                return info;
            }
        }
    return NULL;
}

}

