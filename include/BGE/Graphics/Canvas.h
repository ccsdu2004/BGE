#ifndef BGE_GUI_CANVAS_H
#define BGE_GUI_CANVAS_H
#include <BGE/Config.h>
#include <BGE/System/NonCopyable.h>
#include <BGE/System/Math.h>
#include <BGE/Graphics/Color.h>
#include <BGE/Graphics/Image.h>

namespace bge
{

class TextRenderer;

class BGE_EXPORT_API Canvas : private NonCopyable
{
public:
    virtual ~Canvas() = 0;
public:
    virtual void drawLine(const Vector2f& start,const Vector2f& end) = 0;
    virtual void drawRectangle(const FloatRect& rectangle,bool filled) = 0;
    virtual void drawGradientFilledRectangle(const FloatRect& rectangle,const Color& color1,const Color& color2,bool horizontal) = 0;
    virtual void drawCircleSegment(const Vector2f& center, float radius,float t1, float t2, uint32_t numSegments, bool filled) = 0;
    virtual void drawCircle(const Vector2f& center, float radius,uint32_t numSegments, bool filled) = 0;
    virtual void drawRoundedRectangle(const FloatRect& rectangle,float radius, bool filled) = 0;
    virtual void enableClipRectangle(const FloatRect& rectangle) = 0;
    virtual void disableClipRectangle() = 0;
    virtual void setColor(const Color& color) = 0;
    virtual void setLineWidth(float width) = 0;
    virtual float maximumLineWidth() const = 0;
    virtual float lineWidth() const = 0;
    virtual void drawTriangle(const Vector2f& p1,const Vector2f& p2,const Vector2f& p3,bool filled) = 0;
    virtual void drawImage(Image* image,const FloatRect& rect,bool tiled,float minx,float miny, float maxx, float maxy) = 0;
    virtual void enableTexturing(Image* image) = 0;
    virtual void disableTexturing() = 0;
    virtual void registerImage(Image* image) = 0;
    virtual void unregisterImage(int32_t handle) = 0;
	virtual TextRenderer* createText(const std::string& name,int point) = 0;

    virtual void drawDroppedRectangle(const FloatRect& rectangle,const Color& color1,const Color& color2) = 0;
    virtual void drawRaisedRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2) = 0;

    virtual void preRender(int width,int height) = 0;
    virtual void postRender() = 0;
};

}

#endif // BGE_GRAPHICS_CANVAS_H
