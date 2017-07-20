#ifndef BGE_SYSTEM_IO_H
#define BGE_SYSTEM_IO_H
#include <BGE/Config.h>
#include <BGE/System/NonCopyable.h>
#include <BGE/System/Deleter.h>
#include <string>

namespace bge
{

class BGE_EXPORT_API IOWriter : public Deleter
{
public:
    IOWriter();
    virtual ~IOWriter();
public:
    virtual uint32_t write(const void* buffer,uint32_t size,uint32_t number) = 0;
    virtual int32_t seek(int32_t position,int32_t mode) = 0;
    virtual uint32_t tell()const = 0;
    virtual std::string fileName()const = 0;
    virtual bool isValid()const = 0;
    virtual void close() = 0;
    virtual void flush() = 0;
};

class BGE_EXPORT_API IOReader : public Deleter
{
public:
    IOReader();
    virtual ~IOReader();
public:
    virtual uint32_t read(void* buffer, uint32_t size, uint32_t number) = 0;
    virtual uint32_t size()const = 0;
    virtual int32_t seek(int32_t offset,int32_t mode) = 0;
    virtual uint32_t tell()const = 0;
    virtual void close() = 0;
    virtual std::string fileName()const = 0;
    virtual bool isValid()const = 0;
};

class BGE_EXPORT_API FileSystem : NonCopyable, public Deleter
{
public:
    static FileSystem& instance();
public:
    virtual void initial(const char* arg) = 0;
    virtual bool setResourcePackage(const std::string& package) = 0;

	virtual std::string workDir()const = 0;
    virtual std::string userDir()const = 0;

    virtual IOReader* readFile(const std::string& filename) = 0;
    virtual IOWriter* writeFile(const std::string& filename) = 0;
};

}

#endif // BGE_IO_H
