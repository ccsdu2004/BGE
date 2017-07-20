#ifndef BGE_PNG_IMAGE_H
#define BGE_PNG_IMAGE_H
#include <BGE/System/Math.h>
#include <BGE/Graphics/Image.h>

namespace bge
{

class PNGImage : public Image
{
protected:
    PNGImage(const std::string& fileName);
    virtual ~PNGImage();
public:
    virtual Vector2i size() const { return size_; }
    virtual size_t numberOfBitsPerPixel() const { return numberOfBitsPerPixel_; }
    virtual const unsigned char* data() const { return data_; }
    virtual unsigned char* data() { return data_; }

    void deleteLater();
private:
    Vector2i size_;           
    size_t numberOfBitsPerPixel_;
    unsigned char* data_;
    friend class Image;
};

} // namespace bge

#endif
