#ifndef BGE_GUI_BASICBUTTON_H
#define BGE_GUI_BASICBUTTON_H
#include <BGE/Config.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Window.h>

namespace bge
{

class ButtonMouseListener;
class BasicButtonDesc;
class TextDesc;
class BasicButton;

class BGE_EXPORT_API ButtonGroup : public Object
{
	DECLARE_OBJECT(ButtonGroup)
public:
    typedef std::list<BasicButton*> BasicButtons;
public:
    ButtonGroup(){}
    virtual ~ButtonGroup(){}
public:
    BasicButton* selected()const;
    ButtonGroup& select(BasicButton* button);
    int indexOf(BasicButton* button)const;

	void insertButton(BasicButton* button)
	{
		buttons_.push_back(button);
	}
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    friend class BasicButton;
    BasicButtons buttons_;
};

class BGE_EXPORT_API BasicButton : public Window
{
    DECLARE_OBJECT(BasicButton)
public:
    enum State { up = 0,down,rollOver, rollOverOn };
public:
    explicit BasicButton(Window* parent = 0, const String& text = L"", ButtonGroup* group = 0);
    virtual ~BasicButton();
public:
    State state() const { return state_; }
    BasicButton& toggleOn() { isToggleOn_ = true; return *this; }
    BasicButton& toggleOff() { isToggleOn_ = false; return *this; }
    bool isToggleOn() const { return isToggleOn_; }

    BasicButton& setMouseOverStatus(bool isMouseOverButton);
    bool isMouseOver() const    { return isMouseOver_; }

    State drawState() const;
    BasicButton& toggle();

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	void deleteLater();

    BasicButtonDesc& buttonDesc() const;
public:
    Signal0 pressed;
    Signal0 released;
    Signal0 clicked;
    Signal1<bool> toggled;
protected:
    BasicButton& keepPressedOnExit();
    BasicButton& drawStandardButton();
    BasicButton& onPressedLeft();
    ButtonMouseListener* buttonMouseListener_;

    State state_;
    int  captureCount_;
    bool keepPressedOnExit_;
    bool isToggleOn_;
    bool isMouseOver_;
    ButtonGroup* group_;
    BasicButtonDesc* renderDesc_;
private:
    friend class ButtonMouseListener;
};

class BGE_EXPORT_API ButtonStateDesc : public RenderDesc
{
public:
    ButtonStateDesc();
    ButtonStateDesc(RenderDesc* background,RenderDesc* border);
    virtual ~ButtonStateDesc();
public:
    const ButtonStateDesc& setButton(const BasicButton& button) const
    {
        button_ = &button;
        return *this;
    }

    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    TextDesc& textDesc() const { return *textDesc_; }
    Color textColor() const;
public:
    virtual bool loadProperty(TiXmlNode* node,const char* = 0);
    virtual bool saveProperty(TiXmlNode* node,const char* = 0);
private:
    RenderDesc* background_;
    RenderDesc* border_;
    TextDesc*   textDesc_;
    mutable const BasicButton* button_;
};

class BGE_EXPORT_API BasicButtonDesc : public RenderDesc
{
    DECLARE_OBJECT(BasicButtonDesc)
public:
    enum Arrow { none = 0, up, down, left, right };
public:
    BasicButtonDesc();
    virtual ~BasicButtonDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
    BasicButtonDesc& setArrow(Arrow arrow)
    {
        arrow_ = arrow;
        return *this;
    }

    BasicButtonDesc& setButton(const BasicButton& button)
    {
        button_ = &button;
        return *this;
    }
    Vector2f defaultTextSize() const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    ButtonStateDesc pressedDesc_;
    ButtonStateDesc releasedDesc_;
    ButtonStateDesc rollOverDesc_;
    ButtonStateDesc rollOverOnDesc_;
    mutable Arrow arrow_;
    const BasicButton* button_;
};

class BGE_EXPORT_API Button : public BasicButton
{
    DECLARE_OBJECT(Button)
public:
    Button(Window* parent = 0,const String& text = L"",ButtonGroup* group = 0);
    virtual ~Button(){}

    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
};

}

#endif
