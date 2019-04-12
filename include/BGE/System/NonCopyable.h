#ifndef BGE_SYSTEM_NONCOPYABLE_H
#define BGE_SYSTEM_NONCOPYABLE_H
#include <BGE/Config.h>

namespace bge
{

class NonCopyable
{
protected :
    NonCopyable(){}
private :
    NonCopyable(const NonCopyable&);
    NonCopyable& operator =(const NonCopyable&);
};

}

#endif
