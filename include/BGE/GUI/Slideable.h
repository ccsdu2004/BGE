#ifndef BGE_GUI_SLIDEABLE_H
#define BGE_GUI_SLIDEABLE_H
#include <BGE/GUI/Window.h>

namespace bge
{

class BGE_EXPORT_API Slideable : public Window
{
    DECLARE_OBJECT(Slideable)
public:
    Slideable(Window* parent = 0);
    virtual ~Slideable();
public:
    int value() const { return value_; }
    Slideable& setValue(int value);

    int maximum() const { return maximum_; }
    Slideable& setMaximum(int value);

    int minimum() const { return minimum_; }
    Slideable& setMinimum(int aValue);

    int unitIncrement() const { return unitIncrement_; }
    Slideable& setUnitIncrement(int value)
    {
        unitIncrement_ = value;
        return *this;
    }

    int blockIncrement() const { return blockIncrement_; }
    Slideable& setBlockIncrement(int value)
    {
        blockIncrement_ = value;
        return *this;
    }

    uint32_t range() const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
public:
    Signal1<int> valueChanged;
    Signal0 sliderPressed;
    Signal0 sliderReleased;
    Signal1<int> sliderMoved;
private:
    int value_;
    int maximum_;
    int minimum_;
    int unitIncrement_;
    int blockIncrement_;
};

}

#endif
