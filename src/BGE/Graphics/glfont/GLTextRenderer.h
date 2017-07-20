#ifndef BGE_GL_TEXT_RENDERER_H
#define BGE_GL_TEXT_RENDERER_H
#include <cstddef>
#include <BGE/Graphics/TextRenderer.h>

namespace bge
{
    namespace Font
    {
        class TypeFace;
    }
}

namespace bge
{

namespace Font
{

class GLTextRenderer : public TextRenderer
{
public:
    GLTextRenderer(const std::string& fontName,size_t pointSize);
    virtual ~GLTextRenderer();
public:
    virtual void render(const String& string,const FloatRect& rectangle,bool isMultiLine,
        Horizontal horizontalAlignment,Vertical verticalAlignment);
    virtual float width(const String& string) const;
    virtual float lineHeight() const;
    virtual size_t hitCharacterIndex(const String& string, float offset) const;
private:
    float baseLineY(const FloatRect& aRectangle,Vertical anAlignment) const;
    float penX(const String& aString, const FloatRect& aRectangle,Horizontal anAlignment) const;
    void renderMultiline(const String& aString, const FloatRect& aRectangle,Horizontal aHorizontalAlignment);
 
    TypeFace*  typeFace_;   
    std::string fontName_; 
};

}

}

#endif
