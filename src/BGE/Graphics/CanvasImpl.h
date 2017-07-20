#ifndef BGE_GRAPHICS_CANVASIMPL_H
#define BGE_GRAPHICS_CANVASIMPL_H
#include <BGE/Graphics/Canvas.h>
#include <BGE/Graphics/glfont/GLTextRenderer.h>
#include <BGE/System/Err.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace bge
{

class CanvasImpl : public Canvas
{
public:
    CanvasImpl();
    ~CanvasImpl();
public:
    void drawLine(const Vector2f& start,const Vector2f& end);
    void drawRectangle(const FloatRect& rectangle,bool filled);
    void drawGradientFilledRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2, bool horizontal);
    void drawCircleSegment(const Vector2f& center, float radius,float t1, float t2, uint32_t numSegments, bool filled);
    void drawCircle(const Vector2f& center, float radius,uint32_t numSegments, bool filled);
    void drawRoundedRectangle(const FloatRect& rectangle,float radius, bool filled);
    void enableClipRectangle(const FloatRect& rectangle);
    void disableClipRectangle();
    void setColor(const Color& color);
    void setLineWidth(float width);
    float maximumLineWidth()const;
    float lineWidth()const;
    void drawTriangle(const Vector2f& p1,const Vector2f& p2,const Vector2f& p3,bool filled);
    void drawImage(Image* image,const FloatRect& rect,bool tiled,float minx,float miny, float maxx, float maxy);
    void enableTexturing(Image* image);
	TextRenderer* createText(const std::string& name,int point);
    void disableTexturing();
    void registerImage(Image* image);
    void unregisterImage(int32_t handle);
public:
    void drawDroppedRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2);
    void drawRaisedRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2);

    void preRender(int width,int height);
    void postRender();
public:
    FT_Library& ftLibrary(){return library_;}
private:
	class FontDetails
	{
		std::string name_;
		int pointSize_;
	public:
		FontDetails(const std::string& name,int pixelHeight) :
			name_(name),pointSize_(pixelHeight) {}
		FontDetails(const FontDetails& fd) { (*this) = fd; }
		FontDetails& operator = (const FontDetails& fd)
		{   name_ = fd.name_; pointSize_ = fd.pointSize_; return *this; }
		bool operator == (const FontDetails& fd) const
		{ return (name_ == fd.name_) && (pointSize_ == fd.pointSize_); }
		bool operator != (const FontDetails& fd) const
		{ return !( (*this) == fd); }
		bool operator < (const FontDetails& fd) const
		{
			bool ret = false;
			if(pointSize_ < fd.pointSize_)
				ret = true;
			else
			{
				if(pointSize_ == fd.pointSize_) 
					ret = (name_ < fd.name_);
			}
			return ret;
		}
	};
    std::map<FontDetails,Font::GLTextRenderer*> fontMap_;
	FT_Library library_;
};

}


#endif
