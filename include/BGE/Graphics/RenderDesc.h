#ifndef BGE_GRAPHICS_RENDERDESC_H
#define BGE_GRAPHICS_RENDERDESC_H
#include <BGE/Config.h>
#include <BGE/System/Math.h>
#include <BGE/System/String.h>
#include <BGE/System/Enum.h>
#include <BGE/System/Err.h>
#include <BGE/System/Object.h>
#include <BGE/System/NonCopyable.h>
#include <BGE/System/TinyXml.h>
#include <BGE/Graphics/Color.h>
#include <BGE/Graphics/Image.h>
#include <BGE/Graphics/Canvas.h>

namespace bge
{

class TextRenderer;

class RenderDesc : public Object
{
    DECLARE_OBJECT(RenderDesc)
public:
    RenderDesc(){}
    virtual ~RenderDesc(){}
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const = 0;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
};

class BGE_EXPORT_API SolidSurfaceDesc : public RenderDesc
{
    DECLARE_OBJECT(SolidSurfaceDesc)
public:
    SolidSurfaceDesc(const Color& aColor = Color(0.35f, 0.35f, 0.35f, 0.5f)) : color_(aColor) {}
    virtual ~SolidSurfaceDesc(){}
public:
    Color color() const { return color_; }
    SolidSurfaceDesc& setColor(const Color& color)
    {
        color_ = color;
        return *this;
    }

    virtual void draw(const FloatRect& area,Canvas* canvas) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    Color color_;
};

class BGE_EXPORT_API GradientSurfaceDesc : public RenderDesc
{
    DECLARE_OBJECT(GradientSurfaceDesc)
public:
    GradientSurfaceDesc():
        color1_(Color(0.35f, 0.35f, 0.35f, 0.5f)),
        color2_(Color(0.35f, 0.35f, 0.35f, 0.5f)),
        orientation_(Orientation_horizontal){}
    GradientSurfaceDesc(const Color& color1, const Color& color2, Orientation orientation) :
        color1_(color1),
        color2_(color2),
        orientation_(orientation) {}
    virtual ~GradientSurfaceDesc() {}
public:
    Color color1() const { return color1_; }
    GradientSurfaceDesc& setColor1(const Color& color)
    {
        color1_ = color;
        return *this;
    }

    Color color2() const { return color2_; }
    GradientSurfaceDesc& setColor2(const Color& color)
    {
        color2_ = color;
        return *this;
    }

    GradientSurfaceDesc& setColors(const Color& color1,const Color& color2)
    {
        color1_ = color1;
        color2_ = color2;
        return *this;
    }

    Orientation orientation() const { return orientation_; }
    GradientSurfaceDesc& setOrientation(Orientation orientation)
    {
        orientation_ = orientation;
        return *this;
    }

    virtual void draw(const FloatRect& area,Canvas* canvas) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    Color color1_;
    Color color2_;
    Orientation orientation_;
};

class BGE_EXPORT_API RectangleDesc : public RenderDesc
{
    DECLARE_OBJECT(RectangleDesc)
public:
    explicit RectangleDesc(RectangleStyle style = RectangleStyle_normal):
        style_(style),
        color1_(Color(1, 1, 1, 0.5f)),
        color2_(Color(0, 0, 0, 0.5f)),
        lineWidth_(1.0f){}
    RectangleDesc(RectangleStyle style,const Color& color1,const Color& color2,float width) :
        style_(style),
        color1_(color1),
        color2_(color2),
        lineWidth_(width){}
    virtual ~RectangleDesc(){}
public:
    RectangleStyle style() const { return style_; }
    RectangleDesc& setStyle(RectangleStyle style)
    {
        style_ = style;
        return *this;
    }

    Color color1() const { return color1_; }
    RectangleDesc& setColor1(const Color& color)
    {
        color1_ = color;
        return *this;
    }

    Color color2() const { return color2_; }
    RectangleDesc& setColor2(const Color& color)
    {
        color2_ = color;
        return *this;
    }

    RectangleDesc& setColors(const Color& color1,const Color& color2)
    {
        color1_ = color1;
        color2_ = color2;
        return *this;
    }

    float lineWidth() const { return lineWidth_; }
    RectangleDesc& setLineWidth(float width)
    {
        lineWidth_ = width;
        return *this;
    }
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    RectangleStyle style_;
    Color color1_;
    Color color2_;
    float lineWidth_;
};

class BGE_EXPORT_API TextDesc : public RenderDesc, NonCopyable
{
    DECLARE_OBJECT(TextDesc)
public:
    TextDesc();
    TextDesc(const std::string& file,int height,float xoffset,float yoffset,float margin,const Color& color);
    virtual ~TextDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const;
    std::string fileName() const { return fileName_; }
    TextDesc& setFileName(const std::string& fileName) { fileName_ = fileName; return *this; }
    int pointSize() const { return pixelHeight_; }
    TextDesc& setPointSize(int size) { pixelHeight_ = size; return *this; }
    float xOffset() const { return xOffset_; }
    TextDesc& setXOffset(float offset) { xOffset_ = offset; return *this; }
    float yOffset() const { return yOffset_; }
    TextDesc& setYOffset(float offset) { yOffset_ = offset; return *this; }

    TextDesc& setText(const String& text) { text_ = text; return *this; }

    Vector2f textSize() const;
    float lineHeight() const;

    size_t hitCharacterIndex(const String& string,float offset) const;
    Color color() const { return color_; }
    TextDesc& setColor(const Color& color) { color_ = color; return *this; }

    float margin() const { return margin_; }
    TextDesc& setMargin(float margin) { margin_ = margin; return *this; }

    TextDesc& setHorizontalAlignment(Horizontal alignment)
    {
        horizontalAlignment_ = alignment;
        return *this;
    }

    TextDesc& setVerticalAlignment(Vertical alignment)
    {
        verticalAlignment_ = alignment;
        return *this;
    }

    TextDesc& drawAsMultiLine() { isMultiLine_ = true; return *this; }
    TextDesc& drawAsSingleLine() { isMultiLine_ = false; return *this; }

    void reloadTextRenderer();
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    std::string fileName_;
    int pixelHeight_;
    float xOffset_;
    float yOffset_;
    float margin_;
    Color color_;
    String text_;
    bool isMultiLine_;
    Horizontal horizontalAlignment_;
    Vertical   verticalAlignment_;
    TextRenderer* textRenderer_;
};

class BGE_EXPORT_API DefaultCheckDesc : public RenderDesc
{
    DECLARE_OBJECT(DefaultCheckDesc)
public:
    explicit DefaultCheckDesc(const Color& color = Color(1,1,1,0.75f)) : color_(color) {}
    virtual ~DefaultCheckDesc() {}
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;

    Color color() const { return color_; }
    DefaultCheckDesc& setColor(const Color& color)
    {
        color_ = color;
        return *this;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    Color color_;
};

class BGE_EXPORT_API DefaultFrameDesc : public RenderDesc
{
    DECLARE_OBJECT(DefaultFrameDesc)
public:
    explicit DefaultFrameDesc(FrameSection section = FrameSection_top):section_(section),
        backgroundColor_(Color(0.35f, 0.35f, 0.35f, 0.5f)),
        foregroundColor_(Color(1, 1, 1, 0.75f)),
        borderLineWidth_(5) {}

    DefaultFrameDesc(FrameSection section,const Color& background,
                  const Color& foreground,int borderLineWidth):
        section_(section),
        backgroundColor_(background),
        foregroundColor_(foreground),
        borderLineWidth_(borderLineWidth) {}
    virtual ~DefaultFrameDesc() {}
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    FrameSection frameSection() const { return section_; }

    DefaultFrameDesc& setFrameSection(FrameSection aFrameSection)
    {
        section_ = aFrameSection;
        return *this;
    }

    Color backgroundColor() const { return backgroundColor_; }
    DefaultFrameDesc& setBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
        return *this;
    }

    Color foregroundColor() const { return foregroundColor_; }
    DefaultFrameDesc& setForegroundColor(const Color& color)
    {
        foregroundColor_ = color;
        return *this;
    }

    int borderLineWidth() const { return borderLineWidth_; }
    DefaultFrameDesc& setBorderLineWidth(int lineWidth)
    {
        borderLineWidth_ = lineWidth;
        return *this;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    FrameSection section_;
    Color backgroundColor_;
    Color foregroundColor_;
    int   borderLineWidth_;
};

class BGE_EXPORT_API BitmapDesc : public RenderDesc
{
    DECLARE_OBJECT(BitmapDesc)
public:
    BitmapDesc() : fileName_(""), mapping_(Mapping_stretched), image_(0), top_(0), bottom_(-1), left_(0), right_(-1) {}
    BitmapDesc(const std::string& file, Mapping mapping,
               int topOffset, int bottomOffset, int leftOffset, int rightOffset);
    virtual ~BitmapDesc()
    {
        if(image_ != 0) image_->deleteLater();
    }
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;

    std::string fileName() const { return fileName_; }
    void setFileName(const std::string& fileName) { fileName_ = fileName; }

    Mapping mapping() const { return mapping_; }
    void setMapping(Mapping mapping) { mapping_ = mapping; }

    void setTop(int top) { top_ = top; }
    void setBottom(int bottom) { bottom_ = bottom; }

    void setLeft(int left) { left_ = left; }
    void setRight(int right) { right_ = right; }

    int top() const { return top_; }
    int bottom() const { return bottom_; }
    int left() const { return left_; }
    int right() const { return right_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
protected:
    std::string fileName_;
    Mapping mapping_;
    Image* image_;
private:
    int top_;
    int bottom_;
    int left_;
    int right_;
};

class BGE_EXPORT_API Bitmap1x3Desc : public BitmapDesc
{
    DECLARE_OBJECT(Bitmap1x3Desc)
public:
    Bitmap1x3Desc() : leftOffset_(0), rightOffset_(0) {}
    virtual ~Bitmap1x3Desc(){}
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    int leftOffset_;
    int rightOffset_;
};

#define CREATE_BY_NODE(x,node,target)\
    TiXmlElement* x = node->FirstChildElement(#x);\
    if(x)\
    {\
        std::string classname = x->Attribute("class");\
        if(!classname.empty())\
        {\
            if(x##_)\
                x##_->deleteLater();\
            x##_ = Object::castTo<target>(Rtti::rttiByObjectName(classname)->createObject());\
            if(x##_)\
                x##_->loadProperty(x);\
        }\
    }

}

#endif
