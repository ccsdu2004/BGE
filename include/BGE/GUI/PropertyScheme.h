#ifndef BGE_GUI_PROPERTYSCHEME_H
#define BGE_GUI_PROPERTYSCHEME_H
#include <BGE/Graphics/Color.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Panel.h>
#include <BGE/GUI/StaticText.h>
#include <BGE/GUI/Slider.h>
#include <BGE/GUI/ProgressBar.h>
#include <BGE/GUI/ListControl.h>
#include <BGE/GUI/FrameWindow.h>
#include <BGE/GUI/ComboBox.h>
#include <BGE/GUI/CheckBox.h>
#include <BGE/GUI/ScrollBar.h>
#include <BGE/GUI/ListBox.h>
#include <BGE/GUI/EditField.h>

namespace bge
{

class BGE_EXPORT_API PropertyScheme
{
public:
    PropertyScheme();
    virtual ~PropertyScheme();
public:
    bool load(const std::string& file);
    bool save(const std::string& file);
public:
    BasicButtonDesc buttonDesc_;
    PanelDesc       panelDesc_;
    EditFieldDesc   editFieldDesc_;
    ScrollBarDesc   scrollBarDesc_;
    ListBoxDesc     listBoxDesc_;
    ListControlDesc listControlDesc_;
    ProgressBarDesc progressBarDesc_;
    CheckBoxDesc    checkBoxDesc_;
    SliderDesc      sliderDesc_;
    FrameWindowDesc frameWindowDesc_;
    StaticTextDesc  staticTextDesc_;
    ComboBoxDesc    comboBoxDesc_;
};

}

#endif
