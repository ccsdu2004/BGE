#ifndef BGE_TEXT_RENDERER_H
#define BGE_TEXT_RENDERER_H
#include <BGE/Config.h>
#include <BGE/System/Enum.h>
#include <BGE/System/Object.h>
#include <BGE/System/Math.h>
#include <BGE/System/String.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

class BGE_EXPORT_API TextRenderer
{
protected:
    TextRenderer(const std::string& font,size_t point):
            fontName_(font),pointSize_(point){}
    virtual ~TextRenderer(){}
public:
    std::string fontName() const { return fontName_; }
    size_t pointSize() const { return pointSize_; }

    virtual void render(const String& string,const FloatRect& rectangle,bool multiline,
        Horizontal halignment,Vertical valignment) = 0;

    virtual float lineHeight() const = 0;
    virtual float width(const String& string) const = 0;

    virtual size_t hitCharacterIndex(const String& string,float offset) const = 0;
private:
    std::string fontName_;
    size_t      pointSize_;
};

}

#endif
