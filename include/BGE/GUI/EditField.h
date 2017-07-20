#ifndef BGE_GUI_EDITFIELD_H
#define BGE_GUI_EDITFIELD_H
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Window.h>
#include <BGE/GUI/Message.h>

namespace bge
{

class EditFieldDesc;
class EditFieldMouseListener;
class TextDesc;

class BGE_EXPORT_API EditField : public Window, public KeyListener
{
    DECLARE_OBJECT(EditField)
public:
    struct Selection
    {
        int startIndex;
        int endIndex;
    };
public:
    EditField(Window* parent = 0,const String& text = L"");
    virtual ~EditField();
public:
    virtual bool onCharacterInput(uint16_t character);
    virtual bool onKeyPressed(uint16_t key,bool alt,bool ctrl,bool shift);
public:
    uint32_t caretPosition() const { return caretPosition_; }
    String selectedText() const;
    Selection currentSelection() const;
    int currentSelectionStart() const { return selection_.startIndex; }
    int currentSelectionEnd() const { return selection_.endIndex; }

    EditField& setCurrentSelection(const  Selection& selection);
    EditField& setCurrentSelection(int startIndex, int endIndex);
    EditField& removeSelection();
    bool isSelectionValid() const;

    EditField& selectAll();

    virtual Vector2f recommendedSize() const;
    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
public:
    Signal0 returnPressed;
    Signal1<const String&> textChanged;
protected:
    virtual Window& onDraw();
private:
    EditField& doBackspace();
    EditField& doDelete();
    EditField& deleteSelection();

    String stringUpToCaret() const;
    String stringFromCaret() const;

    EditField& setCaretPosition(uint32_t pos);

    float caretXPos()const;

    int lastSpaceBefore(int pos) const;
    int nextWordStartPosAfter(int pos) const;

    EditFieldDesc& renderDesc() const;

    void moveCaretMaintainSelection(int offset);
    void moveCaretDiscardSelection(int offset);
    bool isValidCaretPosition(int position)const;

    friend class EditFieldMouseListener;
    EditFieldMouseListener* mouseListener_;

    uint32_t caretPosition_;
    Selection selection_;
    int selectionStart_;
    float xOffset_;

    EditFieldDesc* renderDesc_;
    friend class EditFieldDesc;
};

class BGE_EXPORT_API EditFieldDesc : public RenderDesc
{
public:
    EditFieldDesc();
    virtual ~EditFieldDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const;
    EditFieldDesc& setEditField(EditField* anEditField)
    {
        editField_ = anEditField;
        return *this;
    }

    TextDesc& textDesc() { return *textDesc_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    void drawTextWithSelection(const FloatRect& area,Canvas* canvas) const;
    void drawTextWithoutSelection(const FloatRect& area,Canvas* canvas) const;
    float drawPreSelectedText(const FloatRect& area,Canvas* canvas) const;
    float drawSelectedText(const FloatRect& area, const FloatRect& aVisibleArea, float aPreSelectedWidth,Canvas* canvas) const;
    void drawPostSelectedText(const FloatRect& area,Canvas* canvas) const;
    float textDescTextWidth() const;
    void drawSelectionBackground(const FloatRect& visible,float startx)const;
    void drawCaret() const;
private:
    RenderDesc* background_;
    RenderDesc* border_;
    TextDesc*   textDesc_;
    Color       selectionColor_;
    Color       selectedTextColor_;
    EditField*  editField_;
};

}

#endif
