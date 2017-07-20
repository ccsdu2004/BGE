#ifndef BGE_GRAPHICS_IMAGE_H
#define BGE_GRAPHICS_IMAGE_H
#include <BGE/Config.h>
#include <BGE/System/Deleter.h>
#include <BGE/System/Math.h>
#include <string>

namespace bge
{

typedef void (UnregisterImageFunction)(size_t handle);

class BGE_EXPORT_API Image : public Deleter
{
protected:
    Image(const std::string& fileName) : fileName_(fileName), handle_(0) {}
    virtual ~Image();
public:
    std::string fileName() const { return fileName_; }
    virtual Vector2i size() const = 0;
    virtual size_t numberOfBitsPerPixel() const = 0;
    virtual const unsigned char* data() const = 0;
    virtual unsigned char* data() = 0;
    size_t handle() const { return handle_; }
    const Image& setHandle(size_t handle) const { handle_ = handle; return *this; }
public:
    static Image* create(const std::string& fileName);
private:
    std::string fileName_;
    mutable size_t handle_;

    //static UnregisterImageFunction* unregisterImage_;
};

}

#endif
