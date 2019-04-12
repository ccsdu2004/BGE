#include <BGE/System/Math.h>
#include <BGE/System/Serialization.h>
#include <BGE/Graphics/OpenGL.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/Graphics/TextRenderer.h>
#include <BGE/GUI/WindowManager.h>
#include <vector>

namespace bge
{

IMPLEMENT_OBJECT(RenderDesc,Object)

bool RenderDesc::loadProperty(TiXmlNode* node)
{
    TiXmlNode* super = node->FirstChildElement("Object");
    Object::loadProperty(super);
    return true;
}

bool RenderDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(!name)
    {
        TiXmlElement element("Object");
        Object::saveProperty(&element);
        node->InsertEndChild(element);
    }
    else
    {
        TiXmlElement this_(name);
        {
            TiXmlElement element("Object");
            Object::saveProperty(&element);
            this_.InsertEndChild(element);
        }
        node->InsertEndChild(this_);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(SolidSurfaceDesc,RenderDesc)

void SolidSurfaceDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    canvas->setColor(color_);
    canvas->drawRectangle(area,true);
    canvas->setColor(Color(1,1,1,1));
}

bool SolidSurfaceDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(color,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    return true;
}

bool SolidSurfaceDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(color,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(GradientSurfaceDesc,RenderDesc)
void GradientSurfaceDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    canvas->drawGradientFilledRectangle(area,color1_,color2_,orientation_ == Orientation_horizontal);
    canvas->setColor(Color(1,1,1,1));
}

bool GradientSurfaceDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(color1,node)
    DESERIALIZE_OBJECT(color2,node)
    DESERIALIZE_OBJECT(orientation,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    return true;
}

bool GradientSurfaceDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(color1,node)
        SERIALIZE_OBJECT(color2,node)
        SERIALIZE_OBJECT(orientation,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(RectangleDesc,RenderDesc)
void RectangleDesc::draw(const FloatRect& area,Canvas* canvas)const
{
    canvas->setLineWidth(lineWidth_);
    if(style_ == RectangleStyle_dropped)
        canvas->drawDroppedRectangle(area,color1_,color2_);
    else if(style_ == RectangleStyle_raised)
        canvas->drawRaisedRectangle(area,color1_, color2_);
    else
    {
        canvas->setColor(color1_);
        canvas->drawRectangle(area,false);
    }

    canvas->setLineWidth(1.0f);
    canvas->setColor(Color(1,1,1,1));
}

bool RectangleDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(color1,node)
    DESERIALIZE_OBJECT(color2,node)
    DESERIALIZE_OBJECT(style,node)
    DESERIALIZE_OBJECT(lineWidth,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    return true;
}

bool RectangleDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(color1,node)
        SERIALIZE_OBJECT(color2,node)
        SERIALIZE_OBJECT(style,node)
        SERIALIZE_OBJECT(lineWidth,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(TextDesc,RenderDesc)
TextDesc::TextDesc():
    fileName_(WindowManager::instance().defaultFont()), pixelHeight_(14), xOffset_(0.0f),
    yOffset_(0.0f), margin_(3.0f), color_(Color(1, 1, 1, 0.75f)),
    isMultiLine_(false),
    horizontalAlignment_(Horizontal_hCenter),
    verticalAlignment_(Vertical_vCenter)
{
	textRenderer_ = WindowManager::instance().canvas()->createText(fileName_,pixelHeight_);
}

TextDesc::TextDesc(const std::string& name,int pixelHeight,float xOffset,float yOffset,float margin,const Color& color):
    fileName_(name),pixelHeight_(pixelHeight),xOffset_(xOffset),
    yOffset_(yOffset), margin_(margin), color_(color),
    isMultiLine_(false),
    horizontalAlignment_(Horizontal_hCenter),
    verticalAlignment_(Vertical_vCenter)
{
    textRenderer_ = WindowManager::instance().canvas()->createText(fileName_,pixelHeight_);
}

TextDesc::~TextDesc()
{
}

void TextDesc::reloadTextRenderer()
{
    textRenderer_ = WindowManager::instance().canvas()->createText(fileName_,pixelHeight_);
}

void TextDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    if(!text_.empty())
    {
        canvas->setColor(color_);

        FloatRect rectangle = area;
        rectangle.expandBy(-margin_);
        rectangle.translate(Vector2f(xOffset_, yOffset_));

        const bool isMultiline = isMultiLine_ && textSize().x() >= rectangle.width_;
        textRenderer_->render(text_, rectangle, isMultiline, horizontalAlignment_, verticalAlignment_);
    }
}

Vector2f TextDesc::textSize() const
{
    Vector2f Result(textRenderer_->width(text_), textRenderer_->lineHeight());
    return Result;
}

float TextDesc::lineHeight() const
{
    const float height = textRenderer_->lineHeight();
    return height;
}

size_t TextDesc::hitCharacterIndex(const String& string,float offset) const
{
    const size_t index = textRenderer_->hitCharacterIndex(string,offset);
    return index;
}

bool TextDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(fileName,node)
    DESERIALIZE_OBJECT(pixelHeight,node)
    DESERIALIZE_OBJECT(xOffset,node)
    DESERIALIZE_OBJECT(yOffset,node)
    DESERIALIZE_OBJECT(margin,node)
    DESERIALIZE_OBJECT(color,node)
    DESERIALIZE_OBJECT(isMultiLine,node)
    DESERIALIZE_OBJECT(horizontalAlignment,node)
    DESERIALIZE_OBJECT(verticalAlignment,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    reloadTextRenderer();
    return true;
}

bool TextDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(fileName,node)
        SERIALIZE_OBJECT(pixelHeight,node)
        SERIALIZE_OBJECT(xOffset,node)
        SERIALIZE_OBJECT(yOffset,node)
        SERIALIZE_OBJECT(margin,node)
        SERIALIZE_OBJECT(color,node)
        SERIALIZE_OBJECT(isMultiLine,node)
        SERIALIZE_OBJECT(horizontalAlignment,node)
        SERIALIZE_OBJECT(verticalAlignment,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(DefaultCheckDesc,RenderDesc)
void DefaultCheckDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    canvas->setColor(color_);
    const float edgeLength = area.size().y();
    const Vector2f squareTopLeft = area.position();
    canvas->drawRectangle(FloatRect(squareTopLeft,Vector2f(edgeLength,edgeLength)), false);

    GLfloat lineWidth = 1.0f;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    GLboolean isLineSmoothEnabled = false;
    glGetBooleanv(GL_LINE_SMOOTH, &isLineSmoothEnabled);

    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    canvas->drawLine(squareTopLeft + Vector2f(3.0f, 3.0f),  squareTopLeft + Vector2f(edgeLength-3.0f, edgeLength-3.0f));
    canvas->drawLine(squareTopLeft + Vector2f(edgeLength-3.0f, 3.0f), squareTopLeft + Vector2f(3.0f, edgeLength-3.0f));

    if(lineWidth != 2.0f) glLineWidth(lineWidth);
    if(!isLineSmoothEnabled) glDisable(GL_LINE_SMOOTH);
}

bool DefaultCheckDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(color,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    return true;
}

bool DefaultCheckDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(color,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(DefaultFrameDesc,RenderDesc)
void DefaultFrameDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    canvas->setLineWidth(float(borderLineWidth_));
    float w = area.size().x();
    float r = w * 0.5f;

    const int numSegmentsInArc = 7;

    switch(section_)
    {
    case FrameSection_right: // Right
        canvas->setColor(backgroundColor_);
        canvas->drawRectangle(area, true);
        canvas->setColor(foregroundColor_);
        canvas->drawLine(area.bottomLeft() + Vector2f(r, 0.0f),  area.topLeft() + Vector2f(r, 0.0f));
        break;
    case FrameSection_topRight: // TopRight
        canvas->setColor(backgroundColor_);
        canvas->drawCircleSegment(area.bottomLeft(), w, 0.0f, Math::PI32*0.5f, numSegmentsInArc, true);
        canvas->setColor(foregroundColor_);
        canvas->drawCircleSegment(area.bottomLeft(), r, 0.0f, Math::PI32*0.5f, numSegmentsInArc, false);
        break;
    case FrameSection_top: // Top
        w = area.size().y();
        r = w * 0.5f;
        canvas->setColor(backgroundColor_);
        canvas->drawRectangle(area, true);
        canvas->setColor(foregroundColor_);
        canvas->drawLine(area.topLeft() + Vector2f(0.0f, r),  area.topRight() + Vector2f(0.0f,r));
        break;
    case FrameSection_topLeft: // TopLeft
        canvas->setColor(backgroundColor_);
        canvas->drawCircleSegment(area.bottomRight(), w, Math::PI32*0.5f, Math::PI32, numSegmentsInArc, true);
        canvas->setColor(foregroundColor_);
        canvas->drawCircleSegment(area.bottomRight(), r, Math::PI32*0.5f, Math::PI32, numSegmentsInArc, false);
        break;
    case FrameSection_left: // Left
        canvas->setColor(backgroundColor_);
        canvas->drawRectangle(area, true);
        canvas->setColor(foregroundColor_);
        canvas->drawLine(area.bottomLeft() + Vector2f(r, 0.0f),  area.topLeft() + Vector2f(r, 0.0f));
        break;
    case FrameSection_bottomLeft: // BottomLeft
        canvas->setColor(backgroundColor_);
        canvas->drawCircleSegment(area.topRight(), w, Math::PI32, Math::PI32*1.5f, numSegmentsInArc, true);
        canvas->setColor(foregroundColor_);
        canvas->drawCircleSegment(area.topRight(), r, Math::PI32, Math::PI32*1.5f, numSegmentsInArc, false);
        break;
    case FrameSection_bottom: // Bottom
        w = area.size().y();
        r = w * 0.5f;
        canvas->setColor(backgroundColor_);
        canvas->drawRectangle(area, true);
        canvas->setColor(foregroundColor_);
        canvas->drawLine(area.topLeft() + Vector2f(0.0f, r),  area.topRight() + Vector2f(0.0f,r));
        break;
    case FrameSection_bottomRight: // BottomRight
        canvas->setColor(backgroundColor_);
        canvas->drawCircleSegment(area.topLeft(), w, Math::PI32*1.5f, Math::PI32*2.0f, numSegmentsInArc, true);
        canvas->setColor(foregroundColor_);
        canvas->drawCircleSegment(area.topLeft(), r, Math::PI32*1.5f, Math::PI32*2.0f, numSegmentsInArc, false);
        break;
    case FrameSection_title: // Title
        canvas->setColor(backgroundColor_);
        canvas->drawRectangle(area, true);
        canvas->setColor(foregroundColor_);
        canvas->drawRoundedRectangle(FloatRect(area.left(),area.top()+2.0f,area.width_,area.height_-4.0f), 5, true);
        break;
    default:
		break;
    }
    canvas->setLineWidth(1);
}

bool DefaultFrameDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(section,node)
    DESERIALIZE_OBJECT(foregroundColor,node)
    DESERIALIZE_OBJECT(backgroundColor,node)
    DESERIALIZE_OBJECT(borderLineWidth,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);
    return true;
}

bool DefaultFrameDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(section,node)
        SERIALIZE_OBJECT(foregroundColor,node)
        SERIALIZE_OBJECT(backgroundColor,node)
        SERIALIZE_OBJECT(borderLineWidth,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(BitmapDesc,RenderDesc)
BitmapDesc::BitmapDesc(const std::string& aFileName, Mapping aMapping,int aTopOffset, int aBottomOffset, int aLeftOffset, int aRightOffset):
    fileName_(aFileName),
    mapping_(aMapping),
    image_(0),
    top_(aTopOffset),
    bottom_(aBottomOffset),
    left_(aLeftOffset),
    right_(aRightOffset)
{
    image_ = Image::create(fileName_);
    WindowManager::instance().canvas()->registerImage(image_);
}

void BitmapDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    const float width  = static_cast<float>(image_->size().width());
    const float height = static_cast<float>(image_->size().height());

    if( (width > 0.0f) && (height > 0.0f) )
    {
        const float fRight = (right_ < 0) ? width : static_cast<float>(right_);
        const float fBottom = (bottom_ < 0) ? height : static_cast<float>(bottom_);
        const float minX = (static_cast<float>(left_) / width) + 0.001f;
        const float maxY = 1.0f - ((static_cast<float>(top_) / height) + 0.001f);
        const float maxX = (fRight / width) - 0.001f;
        const float minY = 1.0f - ((fBottom / height) - 0.001f);
        canvas->drawImage(image_,area,(mapping_ == Mapping_tiled), minX, minY, maxX, maxY);
    }
}

bool BitmapDesc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(fileName,node)
    DESERIALIZE_OBJECT(mapping,node)
    DESERIALIZE_OBJECT(top,node)
    DESERIALIZE_OBJECT(left,node)
    DESERIALIZE_OBJECT(bottom,node)
    DESERIALIZE_OBJECT(right,node)
    TiXmlNode* super = node->FirstChildElement("RenderDesc");
    RenderDesc::loadProperty(super);

    if(image_ != 0)
        image_->deleteLater();

    image_ = Image::create(fileName_);
    WindowManager::instance().canvas()->registerImage(image_);
    return true;
}

bool BitmapDesc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(fileName,node)
        SERIALIZE_OBJECT(mapping,node)
        SERIALIZE_OBJECT(top,node)
        SERIALIZE_OBJECT(left,node)
        SERIALIZE_OBJECT(bottom,node)
        SERIALIZE_OBJECT(right,node)
        TiXmlElement element("RenderDesc");
        RenderDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

IMPLEMENT_OBJECT_CREATE(Bitmap1x3Desc,BitmapDesc)
void Bitmap1x3Desc::draw(const FloatRect& area,Canvas* canvas) const
{
    const float width  = static_cast<float>(image_->size().width());
    const float height = static_cast<float>(image_->size().height());

    if( (width > 0.0f) && (height > 0.0f) )
    {
        const int iRight = (right() < 0) ? image_->size().width() : right();
        const int iTop = (top() < 0) ? image_->size().height() : top();

        std::vector<float> xOffsets(4);
        xOffsets.push_back((static_cast<float>(left()) / width) + 0.001f);
        xOffsets.push_back(static_cast<float>(left() + leftOffset_) / width);
        xOffsets.push_back(static_cast<float>(iRight - rightOffset_) / width);
        xOffsets.push_back((static_cast<float>(right()) / width) - 0.001f);

        std::vector<float> rectOffsets(4);
        rectOffsets.push_back(area.left());
        rectOffsets.push_back(area.left() + leftOffset_);
        rectOffsets.push_back(area.right() - rightOffset_);
        rectOffsets.push_back(area.right());
        const float minY = (static_cast<float>(bottom()) / height) + 0.001f;
        const float maxY = (static_cast<float>(iTop) / height) - 0.001f;
        for(int x = 0; x < 3; ++x)
        {
            const float minX = xOffsets[x];
            const float maxX = xOffsets[x+1];
            const FloatRect rect(rectOffsets[x], area.top(), rectOffsets[x+1], area.bottom());
            canvas->drawImage(image_,rect,(mapping_ == Mapping_tiled), minX, minY, maxX, maxY);
        }
    }
}

bool Bitmap1x3Desc::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(leftOffset,node)
    DESERIALIZE_OBJECT(rightOffset,node)
    TiXmlNode* super = node->FirstChildElement("BitmapDesc");
    return BitmapDesc::loadProperty(super);
}

bool Bitmap1x3Desc::saveProperty(TiXmlNode* node,const char* name)
{
    if(name)
    {
        TiXmlElement this_(name);
        this_.SetAttribute("class",getRtti()->name());
        saveProperty(&this_);
        node->InsertEndChild(this_);
    }
    else
    {
        SERIALIZE_OBJECT(leftOffset,node)
        SERIALIZE_OBJECT(rightOffset,node)
        TiXmlElement element("BitmapDesc");
        BitmapDesc::saveProperty(&element);
        node->InsertEndChild(element);
    }
    return true;
}

}

