#include <map>
#include <cstddef>
#include <BGE/Graphics/OpenGL.h>
#include <BGE/Graphics/Canvas.h>
#include <BGE/Graphics/glfont/GLTextRenderer.h>
#include <BGE/Graphics/glfont/TypeFace.h>

namespace bge
{

namespace Font
{

GLTextRenderer::GLTextRenderer(const std::string& fontName,size_t pointSize) :
    TextRenderer(fontName,pointSize),
    typeFace_(new TypeFace(fontName,pointSize)),
    fontName_(fontName)
{
}

GLTextRenderer::~GLTextRenderer()
{
    delete typeFace_;
}

void GLTextRenderer::render(const String& aString, const FloatRect& aRectangle, bool isMultiLine,
        Horizontal aHorizontalAlignment,Vertical aVerticalAlignment)
{
    if(typeFace_ != 0)
    {
        if(isMultiLine)
        {
            renderMultiline(aString, aRectangle, aHorizontalAlignment);
        }
        else
        {
            Vector2f position(
                penX(aString, aRectangle, aHorizontalAlignment),
                baseLineY(aRectangle, aVerticalAlignment)
                );
            typeFace_->render(aString, position);
        }
    }
}

float GLTextRenderer::width(const String& aString) const
{
    return typeFace_->width(aString);
}

float GLTextRenderer::lineHeight() const
{
    return typeFace_->lineHeight();
}

size_t GLTextRenderer::hitCharacterIndex(const String& aString, float anOffset) const
{
    return typeFace_->hitCharacterIndex(aString, anOffset);
}

float GLTextRenderer::baseLineY(const FloatRect& aRectangle,Vertical anAlignment) const
{
    float ret = 0.0f;
    if(typeFace_ != 0)
    {
        switch(anAlignment)
        {
            case Vertical_top:
            {
                ret = aRectangle.position().y() + typeFace_->ascent();
            }
            break;

            case Vertical_vCenter:
            {
                const float spaceAboveText = floorf((aRectangle.height_ - typeFace_->lineHeight()) * 0.5f);
                ret = aRectangle.position().y() + spaceAboveText + typeFace_->ascent();
            }
            break;

            case Vertical_bottom:
            {
                ret = aRectangle.bottom() - typeFace_->descent();
            }
            break;
        }
    }
    return ret;
}

float GLTextRenderer::penX(const String& aString, const FloatRect& aRectangle,
        Horizontal anAlignment) const
{
    float ret = 0.0f;
    if(typeFace_ != 0)
    {
        const float width = typeFace_->width(aString);
        switch(anAlignment)
        {
            case Horizontal_left:
            {
                ret = aRectangle.position().x();
            }
            break;

            case Horizontal_hCenter:
            {
                ret = floorf(aRectangle.position().x() - (width * 0.5f) + (aRectangle.width_ * 0.5f));
            }
            break;

            case Horizontal_right:
            {
                ret = aRectangle.size().x();// - width;
            }
            break;
        }
    }
    return ret;
}

void GLTextRenderer::renderMultiline(const String& aString, const FloatRect& aRectangle,
        Horizontal aHorizontalAlignment)
{
    std::string txt = aString;
    const float availableWidth = aRectangle.width_ - 4.0f;

    float y = baseLineY(aRectangle,Vertical_top);
    while(txt.length() > 0)
    {
        // find out where the text is cut off
        size_t pos = hitCharacterIndex(txt, availableWidth);

        if(pos == std::string::npos) pos = txt.size();

        if(pos > 0)
        {
            // make the line the string up to the cut off position
            std::string line = txt.substr(0, pos);
            int p = int(pos);
            // check to see if the cut off position is inside the string and not at the end
            if(pos < txt.length())
            {
                //  if we've cut the string at a space, then the cut is fine where
                // it is. If not, we'll have to find the last space in the line
                // and cut there
                if( (txt[0] != ' ') && (*line.rbegin() != ' ') )
                {
                    std::string::size_type cutPos = line.find_last_of(" ");
                    if(cutPos != std::string::npos)
                    {
                        line = line.substr(0, cutPos);
                        p = int(cutPos);
                    }
                }
            }
            // trim whitespace off the start and end of the line
            line.erase(0, line.find_first_not_of(" "));
            line.erase(line.find_last_not_of(" ")+1, line.length());

            typeFace_->render(line, Vector2f(penX(line, aRectangle, aHorizontalAlignment), y));

            // update txt to be whatever remains after taking the line off the front
            txt = txt.substr(p, txt.length());
        }
        else
        {
            txt = "";
        }

        y += typeFace_->lineHeight();
    }
}

}

}
