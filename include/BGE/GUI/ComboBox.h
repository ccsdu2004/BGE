#ifndef BGE_GUI_COMBOBOX_H
#define BGE_GUI_COMBOBOX_H
#include <BGE/GUI/Window.h>
#include <BGE/Graphics/RenderDesc.h>

namespace bge
{

class ListBox;
class ComboBoxMouseListener;
class ComboBoxDesc;
class TextDesc;

class BGE_EXPORT_API ComboBox : public Window
{
    DECLARE_OBJECT(ComboBox)
public:
    ComboBox(Window* parent = 0,const FloatRect& rect = FloatRect(0,0,72,48));
    virtual ~ComboBox();
public:
    ComboBox& addString(const String& string);
    String stringAt(uint32_t position)const;
    String selectedString()const;
    ComboBox& removeStringAt(uint32_t position);
    ComboBox& removeAllStrings();

    int selectedIndex() const;
    uint32_t numberOfStrings() const;
public:
    ComboBox& selectString(const String& string);
    ComboBox& selectStringAt(uint32_t position);

    virtual bool process(const Message& message);
    virtual Vector2f recommendedSize() const;

    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();

    bool isDropDownOpen() const { return isDropDownOpen_; }
public:
    Signal1<const String&> selectionChanged;
    Signal0 listOpened;
    Signal0 listClosed;
protected:
    virtual Window& onDraw();
    virtual void onLostFocus();
private:
    ComboBox& openDropDown();
    ComboBox& closeDropDown();
    friend class ComboBoxMouseListener;
    ListBox* listBox_;
    ComboBoxMouseListener* comboBoxMouseListener_;
    ComboBoxDesc* renderDesc_;
    bool isDropDownOpen_;
};

class BGE_EXPORT_API ComboBoxDesc : public RenderDesc
{
public:
    ComboBoxDesc();
    virtual ~ComboBoxDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    ComboBoxDesc& setComboBox(ComboBox* box)
    {
        comboBox_ = box;
        return *this;
    }

    TextDesc& textDesc() { return *textDesc_; }
public:
    bool loadProperty(TiXmlNode* node);
    bool saveProperty(TiXmlNode* node);
private:
    RenderDesc* background_;
    RenderDesc* border_;
    TextDesc*   textDesc_;
    ComboBox*   comboBox_;
};

}

#endif
