#include <BGE/System/Err.h>
#include <BGE/Graphics/Opengl.h>
#include <BGE/Graphics/CanvasImpl.h>
#include <BGE/Graphics/glfont/GLTextRenderer.h>
#include <BGE/GUI/WindowManager.h>
#include <cassert>

namespace bge
{

Canvas::~Canvas()
{
}

CanvasImpl::CanvasImpl()
{
    if(FT_Init_FreeType(&library_) != 0)
        err()<<"failed to initialize freetype library.\n";
}

CanvasImpl::~CanvasImpl()
{
	auto itr = fontMap_.begin();
	while(itr != fontMap_.end())
	{
		delete itr->second;
	    itr ++;
	}
    if(FT_Done_FreeType(library_) != 0)
        err()<<"failed to initialize freetype library.\n";
}

void CanvasImpl::drawLine(const Vector2f& start,const Vector2f& end)
{
    glBegin(GL_LINES);
        glVertex2f(start.x_,start.y_);
        glVertex2f(end.x_,end.y_);
    glEnd();
}

void CanvasImpl::drawRectangle(const FloatRect& rectangle,bool filled)
{
    if(filled)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(rectangle.left_,rectangle.top_);
        glVertex2f(rectangle.left_,rectangle.top_ + rectangle.height_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_ + rectangle.height_);
    }
    else
    {
        const float width = lineWidth();
        const float halfwidth = width * 0.5f;

        glBegin(GL_LINES);
        glVertex2f(rectangle.left_,rectangle.top_ - halfwidth);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_ - halfwidth);

        glVertex2f(rectangle.left_ + rectangle.width_ - halfwidth,rectangle.top_ - width);
        glVertex2f(rectangle.left_ + rectangle.width_ - halfwidth,rectangle.top_ + rectangle.height_ + width );

        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_ + rectangle.height_ + halfwidth );
        glVertex2f(rectangle.left_,rectangle.top_ + rectangle.height_ + halfwidth );

        glVertex2f(rectangle.left_ + halfwidth,rectangle.top_ + rectangle.height_ + width );
        glVertex2f(rectangle.left_ + halfwidth,rectangle.top_ - width   );
    }
    glEnd();
}

void CanvasImpl::drawGradientFilledRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2, bool horizontal)
{
    glBegin(GL_TRIANGLE_STRIP);
    if(horizontal)
    {
        glColor4f(color1.r_, color1.g_, color1.b_, color1.a_);
        glVertex2f(rectangle.left_,rectangle.top_);
        glVertex2f(rectangle.left_,rectangle.top_+rectangle.height_);
        glColor4f(color2.r_, color2.g_, color2.b_, color2.a_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_+rectangle.height_);
    }
    else
    {
        glColor4f(color1.r_,color1.g_,color1.b_,color1.a_);
        glVertex2f(rectangle.left_,rectangle.top_);
        glColor4f(color2.r_,color2.g_,color2.b_,color2.a_);
        glVertex2f(rectangle.left_,rectangle.top_+rectangle.height_);
        glColor4f(color1.r_, color1.g_,color1.b_,color1.a_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_);
        glColor4f(color2.r_,color2.g_,color2.b_,color2.a_);
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_+rectangle.height_);
    }
    glEnd();
}

void CanvasImpl::drawCircleSegment(const Vector2f& center, float radius,float t1, float t2, uint32_t segments, bool filled)
{
    const float a1 = t1 * Math::RADTODEG32 + 90.0f;
    const float a2 = t2 * Math::RADTODEG32 + 90.0f;
    float xtra = 0.0f;
    if(!filled)
    {
        float lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);
        xtra = lineWidth * 0.5f;
    }
    const float outerRadius = filled ? radius : radius + xtra;
    const float innerRadius = filled ? 0.0f : radius - xtra;

    const float cx = float(center.x_);
    const float cy = float(center.y_);

    glTranslatef(cx,cy,0.0f);

    static GLUquadricObj* quadric = gluNewQuadric();
    gluPartialDisk(quadric,innerRadius, outerRadius,segments, 1, a1, a2-a1);

    glTranslatef(-cx, -cy, 0.0f);
}

void CanvasImpl::drawCircle(const Vector2f& center,float radius,uint32_t segments,bool filled)
{
    drawCircleSegment(center,radius,0,2.0f*Math::PI32,segments,filled);
}

void CanvasImpl::drawDroppedRectangle(const FloatRect& rectangle,const Color& color1,const Color& color2)
{
    const float width = lineWidth();
    const float halfwidth = width * 0.5f;

    glBegin(GL_LINES);
        glColor4f(color1.r_,color1.g_,color1.b_,color1.a_);
        glVertex2f(rectangle.left_,rectangle.top_ + rectangle.height_ - halfwidth );
        glVertex2f(rectangle.left_ + rectangle.width_,rectangle.top_ + rectangle.height_ - halfwidth );

        glVertex2f(rectangle.left_ + rectangle.width_- halfwidth,rectangle.top_ + rectangle.height_ - width   );
        glVertex2f(rectangle.left_ + rectangle.width_ - halfwidth, rectangle.top_ + width       );

        glColor4f(color2.r_, color2.g_, color2.b_, color2.a_);
        glVertex2f(rectangle.left_ + rectangle.width_, rectangle.top_ + halfwidth );
        glVertex2f(rectangle.left_, rectangle.top_+ halfwidth );

        glVertex2f(rectangle.left_ + halfwidth, rectangle.top_ + width);
        glVertex2f(rectangle.left_ + halfwidth,rectangle.top_ + rectangle.height_ - width   );
    glEnd();
}

void CanvasImpl::drawRaisedRectangle(const FloatRect& rectangle,const Color& color1, const Color& color2)
{
    drawDroppedRectangle(rectangle,color2,color1);
}

void CanvasImpl::enableClipRectangle(const FloatRect& rectangle)
{
    GLfloat vp[4];
    glGetFloatv(GL_VIEWPORT,vp);

    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<int>(rectangle.left_),
              static_cast<int>(vp[3] - vp[1] - rectangle.top_ - rectangle.height_),
              static_cast<int>(rectangle.size().x_)+1,
              static_cast<int>(rectangle.size().y_)+1);

}

void CanvasImpl::disableClipRectangle()
{
    glDisable(GL_SCISSOR_TEST);
}

void CanvasImpl::setColor(const Color& color)
{
    glColor4f(color.r_,color.g_,color.b_,color.a_);
}

void CanvasImpl::setLineWidth(float width)
{
    glLineWidth(width);
}

float CanvasImpl::maximumLineWidth()const
{
    GLfloat ret[2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,ret);
    return float(ret[1]);
}

float CanvasImpl::lineWidth()const
{
    GLfloat ret;
    glGetFloatv(GL_LINE_WIDTH, &ret);
    return ret;
}

void CanvasImpl::drawTriangle(const Vector2f& p1,const Vector2f& p2,const Vector2f& p3,bool filled)
{
    if(filled)
        glBegin(GL_TRIANGLES);
    else
        glBegin(GL_LINE_LOOP);

    glVertex2f(p1.x_,p1.y_);
    glVertex2f(p2.x_,p2.y_);
    glVertex2f(p3.x_,p3.y_);
    glEnd();
}

void CanvasImpl::drawRoundedRectangle(const FloatRect& rect,float radius, bool filled)
{
    const float r = radius;
    const Vector2f bottomLeft(rect.left_,rect.top_+rect.height_);
    const Vector2f topRight(rect.left_+rect.width_,rect.top_);
    const Vector2f bottomLeftCenter  = bottomLeft + Vector2f(r, -r);
    const Vector2f topRightCenter    = topRight + Vector2f(-r, r);
    const Vector2f topLeftCenter     = Vector2f(bottomLeft.x_ + r, topRight.y_ + r);
    const Vector2f bottomRightCenter = Vector2f(topRight.x_ - r, bottomLeft.y_ - r);

    // now draw the four rounded corners
    drawCircleSegment( topLeftCenter,     radius, Math::PI32*0.5f, Math::PI32,      9, filled );
    drawCircleSegment( bottomRightCenter, radius, Math::PI32*1.5f, Math::PI32*2.0f, 9, filled );
    drawCircleSegment( bottomLeftCenter,  radius, Math::PI32,      Math::PI32*1.5f, 9, filled );
    drawCircleSegment( topRightCenter,    radius, 0,       Math::PI32*0.5f, 9, filled );

    if(filled)
    {
        // fill the rest in with rectangles
        drawRectangle(FloatRect(topLeftCenter + Vector2f(-r, 0),bottomRightCenter + Vector2f(r, 0)-topLeftCenter-Vector2f(-r,0)) , filled);
        drawRectangle(FloatRect(topLeftCenter + Vector2f(0, -r),topRightCenter-topLeftCenter - Vector2f(0, -r)) ,filled);
        drawRectangle(FloatRect(bottomLeftCenter,bottomRightCenter + Vector2f(0, r)-bottomLeftCenter), filled);
    }
    else
    {
        // fill the gaps in with lines
        drawLine(bottomLeftCenter  + Vector2f(-r,  0), topLeftCenter     + Vector2f(-r,  0));
        drawLine(bottomLeftCenter  + Vector2f( 0, -r), bottomRightCenter + Vector2f( 0, -r));
        drawLine(bottomRightCenter + Vector2f( r,  0), topRightCenter    + Vector2f( r,  0));
        drawLine(topLeftCenter     + Vector2f( 0,  r), topRightCenter    + Vector2f( 0,  r));
    }
}

void CanvasImpl::drawImage(Image* image,const FloatRect& rect,bool tiled,float minx,float miny, float maxx, float maxy)
{
    enableTexturing(image);

    glColor3f(1.0f,1.0f,1.0f);

    glBegin(GL_QUADS);
        glTexCoord2f(minx,miny);
        glVertex2f(rect.left_,rect.top_+rect.height_);
        glTexCoord2f(minx,maxy);
        glVertex2f(rect.left_,rect.top_);
        glTexCoord2f(maxx,maxy);
        glVertex2f(rect.left_+rect.width_,rect.top_);
        glTexCoord2f(maxx,miny);
        glVertex2f(rect.left_+rect.width_,rect.top_ + rect.height_);
    glEnd();

    disableTexturing();
}

void CanvasImpl::enableTexturing(Image* image)
{
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    if(image->handle() == 0)
        registerImage(image);

    assert(image->handle() != 0);

    glBindTexture(GL_TEXTURE_2D,image->handle());
}

void CanvasImpl::disableTexturing()
{
    glPopAttrib();
}

TextRenderer* CanvasImpl::createText(const std::string& name,int point)
{
    FontDetails fontDetails(name,point);
    std::map<FontDetails,Font::GLTextRenderer*>::iterator fontIterator = fontMap_.find(fontDetails);
    if(fontIterator == fontMap_.end())
    {
        fontMap_[fontDetails] = new Font::GLTextRenderer(name,point);
    }

    fontIterator = fontMap_.find(fontDetails);
    //ASSERT(fontIterator != fontMap.end());
    return fontIterator->second;
}

void CanvasImpl::registerImage(Image* image)
{
    GLuint name = 0;
    glGenTextures(1,&name);

    if(name != 0)
    {
        glBindTexture(GL_TEXTURE_2D,name);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        assert((image->numberOfBitsPerPixel() == 24) || (image->numberOfBitsPerPixel() == 32) );
        const GLuint format = (image->numberOfBitsPerPixel() == 24) ? GL_RGB : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D,0,format,image->size().x_,image->size().y_,0,format,GL_UNSIGNED_BYTE,image->data());

        image->setHandle(name);
    }
}

void CanvasImpl::unregisterImage(int32_t handle)
{
    const GLuint name = static_cast<GLuint>(handle);
    glDeleteTextures(1,&name);
}

void CanvasImpl::preRender(int width,int height)
{
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
    glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE );
    glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
    glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0.0,width,height,0.0,-10,10);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void CanvasImpl::postRender()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
    glPopClientAttrib();
}

}
