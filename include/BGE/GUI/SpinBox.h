#ifndef BGE_GUI_SPINBOX_H
#define BGE_GUI_SPINBOX_H
#include <BGE/System/Timer.h>
#include <BGE/GUI/Slideable.h>

namespace bge
{

class EditField;
class SpinBoxButton;
class SpinBoxMouseListener;

class BGE_EXPORT_API SpinBox : public Slideable
{
    DECLARE_OBJECT(SpinBox)
public:
    SpinBox(Window* parent = 0,int value = 0);
    virtual ~SpinBox();
public:
    virtual bool process(const Message& message);
    virtual Vector2f recommendedSize()const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
protected:
    virtual Window& onDraw();
    virtual void onResize();
    virtual void onMove();
    virtual void onGainedFocus();
    virtual void onLostFocus();
    virtual Window* windowBelow(const Vector2f& p);

    SpinBox& arrange();

    void incrementButtonPressed();
    void decrementButtonPressed();
    void incrementButtonReleased();
    void decrementButtonReleased();

    void textEdited();
    void updateDisplay();
    void focusEditField();
    void unFocusEditField();

    bool isTextNumeric();
private:
    friend class SpinBoxMouseListener;
    Timer incrementTimer_;
    Timer decrementTimer_;
    SpinBoxButton* incrementButton_;
    SpinBoxButton* decrementButton_;
    EditField*     editField_;
    SpinBoxMouseListener* mouseListener_;
};

}

#endif
