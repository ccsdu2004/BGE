#ifndef BGE_GUI_SCROLLBAR_H
#define BGE_GUI_SCROLLBAR_H
#include <BGE/System/Timer.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Slideable.h>

namespace bge
{

class ScrollBarSlider;
class ScrollBarButton;
class ScrollBarListener;
class ScrollBarMouseListener;
class ScrollBarDesc;

class BGE_EXPORT_API ScrollBar : public Slideable
{
    DECLARE_OBJECT(ScrollBar)
public:
    ScrollBar(Window* parent = 0,Orientation justification = Orientation_horizontal,const Vector2f& topLeft = Vector2f(0,0),float extent = 48);
    virtual ~ScrollBar();
public:
    Orientation justification() const { return justification_; }
    int visibleAmount() const { return visibleAmount_; }
    ScrollBar& setVisibleAmount(int amount)
    {
        visibleAmount_ = amount;
        return *this;
    }

    Color trackColor() const { return trackColor_; }
    ScrollBar& setTrackColor(const Color& color)
    {
        trackColor_ = color;
        return *this;
    }

    float width() const;
public:
    virtual bool process(const Message& message);
    virtual bool containsFocus() const;
    virtual bool doesHierarchyContain(Window* window) const;

    float trackLength() const;
    float computeSliderLength() const;
    float discreetSliderOffset() const;

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
protected:
    virtual Window& onDraw();

    virtual void onResize();
    virtual void onMove();
    ScrollBar& arrange();

    void incrementButtonPressed();
    void decrementButtonPressed();
    void incrementButtonReleased();
    void decrementButtonReleased();

    void buttonPressed()  { sliderPressed(); }
    void buttonReleased() { sliderReleased(); }
private:
    friend class ScrollBarMouseListener;
    ScrollBarMouseListener* mouseListener_;

    Orientation justification_;
    int visibleAmount_;
    Color trackColor_;

    ScrollBarButton* incrementButton_;
    ScrollBarButton* decrementButton_;

    Timer incrementTimer_;
    Timer decrementTimer_;

    ScrollBarSlider* slider_;
    ScrollBarDesc* renderDesc_;
};

class ScrollBarDesc : public RenderDesc
{
public:
    ScrollBarDesc();
    virtual ~ScrollBarDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const
    {
        if(background_ != 0)
            background_->draw(area,canvas);
        if(border_ != 0)
            border_->draw(area,canvas);
    }

    float width() const { return width_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    RenderDesc* background_;
    RenderDesc* border_;
    float width_;
};

}

#endif
