#ifndef BGE_GUI_STATICTEXT_H
#define BGE_GUI_STATICTEXT_H
#include <BGE/System/Object.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Window.h>

namespace bge
{

class StaticTextDesc;
class TextDesc;

class StaticText : public Window
{
    DECLARE_OBJECT(StaticText)
public:
    StaticText(Window* parent = 0,const String& text = L"");
    virtual ~StaticText();
public:
    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
private:
    StaticTextDesc* renderDesc_;
};

class BGE_EXPORT_API StaticTextDesc : public RenderDesc
{
public:
    StaticTextDesc();
    virtual ~StaticTextDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const;

    StaticTextDesc& setStaticText(StaticText* text)
    {
        staticText_ = text;
        return *this;
    }

    TextDesc& textDesc(){ return *textDesc_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    StaticText* staticText_;
    RenderDesc* background_;
    RenderDesc* border_;
    TextDesc*   textDesc_;
};

}

#endif
