#ifndef BGE_GUI_FRAMEWINDOW_H
#define BGE_GUI_FRAMEWINDOW_H
#include <BGE/System/Math.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Panel.h>

namespace bge
{

class FrameWindowMouseListener;
class FrameWindowDesc;
class TextDesc;
class Panel;

class BGE_EXPORT_API FrameWindow : public Window
{
    DECLARE_OBJECT(FrameWindow)
public:
    FrameWindow(const FloatRect& rect = FloatRect(0,0,200,200),Window* parent = 0, const String& text = L"");
    virtual ~FrameWindow();
public:
    Panel* clientPanel() const { return clientWindow_;}
    FrameWindow& setClientPanel(Panel* panel);

    bool isMovable()const;
    FrameWindow& enableMovement();
    FrameWindow& disableMovement();
public:
    FloatRect clientRectangle()const;

    virtual bool process(const Message& message);
    virtual Window* windowBelow(const Vector2f& p);
    virtual bool doesHierarchyContain(Window* window)const;

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);

    virtual void deleteLater();
protected:
    virtual Window& onDraw();
    virtual FrameWindow& clientWindowChanged();
    virtual void onResize();
    virtual void onMove();
private:
    FrameWindowMouseListener* mouseListener_;
    Panel* clientWindow_;
    int captureCount_;
    bool isMovable_;
    friend class FrameWindowMouseListener;
    FrameWindowDesc* renderDesc_;
};

class FrameWindowDesc : public RenderDesc
{
public:
    FrameWindowDesc();
    virtual ~FrameWindowDesc();
public:
    FrameWindowDesc& setFrameWindow(FrameWindow* frame)
    {
        frameWindow_ = frame;
        return *this;
    }

    virtual void draw(const FloatRect& area,Canvas* canvas)const;

    float topBorderWidth() const { return topBorderWidth_; }
    float leftBorderWidth() const { return leftBorderWidth_; }
    float rightBorderWidth() const { return rightBorderWidth_; }
    float bottomBorderWidth() const { return bottomBorderWidth_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    RenderDesc*  topLeftDesc_;
    RenderDesc*  titleDesc_;
    RenderDesc*  topDesc_;
    RenderDesc*  topRightDesc_;
    RenderDesc*  rightDesc_;
    RenderDesc*  bottomRightDesc_;
    RenderDesc*  bottomDesc_;
    RenderDesc*  bottomLeftDesc_;
    RenderDesc*  leftDesc_;
    TextDesc*    textDesc_;
    float topBorderWidth_;
    float leftBorderWidth_;
    float rightBorderWidth_;
    float bottomBorderWidth_;

    FrameWindow* frameWindow_;
};

}

#endif
