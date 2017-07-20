#ifndef BGE_GUI_PROGRESSBAR_H
#define BGE_GUI_PROGRESSBAR_H
#include <BGE/System/Math.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Window.h>

namespace bge
{

class ProgressBarDesc;

class BGE_EXPORT_API ProgressBar : public Window
{
    DECLARE_OBJECT(ProgressBar)
public:
    ProgressBar(Window* parent = 0,const FloatRect& rectangle = FloatRect(0,0,164,36),const Orientation orientation = Orientation_horizontal);
    virtual ~ProgressBar();
public:
    Orientation orientation() const { return orientation_; }
    ProgressBar& setOrientation(Orientation o)
    {
        orientation_ = o;
        return *this;
    }

    float percentage() const { return percentage_; }
    ProgressBar& setPercentage(float p)
    {
        percentage_ = p;
        return *this;
    }
public:
    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
private:
    Orientation orientation_;
    float percentage_;
    ProgressBarDesc* renderDesc_;
};

class BGE_EXPORT_API ProgressBarDesc : public RenderDesc
{
public:
    ProgressBarDesc();
    virtual ~ProgressBarDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    ProgressBarDesc& setProgressBar(ProgressBar* aProgressBar)
    {
        progressBar_ = aProgressBar;
        return *this;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    RenderDesc*  horzEmptyDesc_;
    RenderDesc*  horzFullDesc_;
    RenderDesc*  vertEmptyDesc_;
    RenderDesc*  vertFullDesc_;
    RenderDesc*  border_;
    ProgressBar* progressBar_;
};

}

#endif
