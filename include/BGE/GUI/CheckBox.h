#ifndef BGE_GUI_CHECKBOX_H
#define BGE_GUI_CHECKBOX_H
#include <BGE/Config.h>
#include <BGE/GUI/BasicButton.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

class CheckBoxDesc;

class BGE_EXPORT_API CheckBox : public BasicButton
{
    DECLARE_OBJECT(CheckBox)
public:
    CheckBox(Window* parent = 0,const String& text = L"",ButtonGroup* group = 0);
    virtual ~CheckBox();
public:
    bool isChecked() const { return isMouseOver(); }
    CheckBox& enableChecked();
    CheckBox& disableChecked();
    CheckBox& toggle();

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
private:
    CheckBoxDesc* renderDesc_;
};
/*
class CheckDesc : public RenderDesc
{
public:
    CheckDesc();
    virtual ~CheckDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    CheckDesc& setCheckBox(CheckBox* checkBox)
    {
        checkBox_ = checkBox;
        return *this;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node,const char* name = 0);
private:
    CheckBox*   checkBox_;
    RenderDesc* checked_;
    RenderDesc* unchecked_;
};*/

class BGE_EXPORT_API CheckBoxDesc : public BasicButtonDesc
{
    DECLARE_OBJECT(CheckBoxDesc)
public:
    CheckBoxDesc();
    virtual ~CheckBoxDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    CheckBoxDesc& setCheckBox(CheckBox* checkBox)
    {
        checkBox_ = checkBox;
        return *this;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    RenderDesc* checked_;
    RenderDesc* unchecked_;
    CheckBox*   checkBox_;
};
/*
class BGE_EXPORT_API CheckBoxGroup : public ButtonGroup
{
public:
    CheckBoxGroup() {}
    virtual ~CheckBoxGroup() {}
public:
    CheckBox* selectedCheckBox() const;
    CheckBoxGroup& select(CheckBox* checkbox);
    uint32_t indexOf(CheckBox* checkbox)const;
};*/

}

#endif
