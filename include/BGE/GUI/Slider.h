#ifndef BGE_GUI_SLIDER_H
#define BGE_GUI_SLIDER_H
#include <BGE/System/Enum.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Slideable.h>

namespace bge
{

class SliderButton;
class SliderMouseListener;
class SliderDesc;

class BGE_EXPORT_API Slider : public Slideable
{
    DECLARE_OBJECT(Slider)
public:
    Slider(Window* parent = 0,Orientation orientation = Orientation_horizontal,const FloatRect& rectangle = FloatRect(0,0,164,36));
    virtual ~Slider();
public:
    Orientation orientation() const { return orientation_; }
    TickPlacement tickPlacement() const { return tickPlacement_; }

    Slider& setTickPlacement(TickPlacement tickPlacement)
    { tickPlacement_ = tickPlacement; return *this; }

    Slider& setTickFrequency(int frequency) { tickFrequency_ = frequency; return *this; }

    int tickFrequency() const { return tickFrequency_; }
public:
    virtual bool process(const Message& message);
    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
protected:
    virtual void onResize();
    virtual void onMove();
    virtual Window& onDraw();

    float trackLength() const;
    Vector2f buttonSize() const;
private:
    Slider& arrange();

    SliderButton* button_;
    Orientation   orientation_;
    TickPlacement tickPlacement_;
    int tickFrequency_;
    SliderMouseListener* mouseListener_;
    SliderDesc* renderDesc_;

    friend class SliderMouseListener;
    friend class SliderButton;

    void buttonPressed()  { sliderPressed(); }
    void buttonReleased() { sliderReleased(); }
};

class SliderDesc : public RenderDesc
{
public:
    SliderDesc();
    virtual ~SliderDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    Color tickColor() const { return tickColor_; }
    float tickWidth() const { return tickWidth_; }
public:
    bool loadProperty(TiXmlNode* node);
    bool saveProperty(TiXmlNode* node);
private:
    RenderDesc* background_;
    RenderDesc* border_;
    Color       tickColor_;
    float       tickWidth_;
};

}

#endif
