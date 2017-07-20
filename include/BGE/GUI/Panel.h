#ifndef BGE_GUI_PANEL_H
#define BGE_GUI_PANEL_H
#include <BGE/GUI/Window.h>
#include <BGE/GUI/Layout.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

class PanelDesc;
class ButtonGroup;

class BGE_EXPORT_API Panel : public Window
{
    DECLARE_OBJECT(Panel)
public:
    Panel(const FloatRect& rectangle = FloatRect(0,0,148,148),Window* parent = 0,const String& text = L"");
    virtual ~Panel();
public:
    Panel& arrangeChildren();
    Panel& setLayout(Layout* layouter);

    virtual bool process(const Message& message);

    virtual Window* windowBelow(const Vector2f& p);
    virtual bool doesHierarchyContain(Window* window)const;

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
    virtual void onResize();
    virtual void onMove();
private:
    Layout* layouter_;
	ButtonGroup* buttonGroup_;
    PanelDesc* renderDesc_;
};

class BGE_EXPORT_API PanelDesc : public RenderDesc
{
public:
    PanelDesc():
        background_(new SolidSurfaceDesc(Color(0.2f,0.2f,0.4f,0.75f))),
        border_(new RectangleDesc()){}

    virtual ~PanelDesc()
    {
        delete background_;
        delete border_;
    }
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    RenderDesc* background_;
    RenderDesc* border_;
};

}

#endif
