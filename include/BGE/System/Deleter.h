#ifndef BGE_SYSTEM_DELETER_H
#define BGE_SYSTEM_DELETER_H
#include <BGE/Config.h>

namespace bge
{

class BGE_EXPORT_API Deleter
{
public:
    virtual ~Deleter();
    virtual void deleteLater() = 0;
};

}

#endif // BGE_SYSTEM_DELETER_H
