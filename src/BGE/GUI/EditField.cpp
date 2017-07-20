#include <BGE/System/Serialization.h>
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/EditField.h>
#include <BGE/GUI/WindowManager.h>
#include <BGE/GUI/PropertyScheme.h>
#include <BGE/GUI/Message.h>

namespace bge
{

class EditFieldMouseListener : public MouseListener, NonCopyable
{
public:
    EditFieldMouseListener(EditField& anEditField):
        editField_(anEditField) {}
public:
    virtual bool clicked(MouseClickMessage& aMessage)
    {
        bool isHandled = false;
        if(isLeftClick(aMessage))
        {
            WindowManager::instance().setMouseCapture(&editField_);
            const float xPos = aMessage.position().x() - editField_.position().x();
            if( (xPos > 0.0f) && (xPos < editField_.size().x()) )
            {
                const int hitCharacterIndex = hitCharacter(xPos - editField_.xOffset_);
                editField_.setCaretPosition((unsigned long)(hitCharacterIndex));
                editField_.removeSelection();
                editField_.selectionStart_ = hitCharacterIndex;
            }
            isHandled = true;
        }
        else if(isLeftRelease(aMessage))
        {
            if(WindowManager::instance().captureWindow() == &editField_)
            {
                WindowManager::instance().releaseMouseCapture();
            }
            isHandled = true;
        }
        return isHandled;
    }

    virtual bool moved(MouseMessage& message)
    {
        bool isHandled = false;
        if(WindowManager::instance().captureWindow() == &editField_)
        {
            const float xPos = message.position().x() - editField_.position().x();
            if((xPos > 0.0f) && (xPos < editField_.size().x()) )
            {
                int selectionStart = editField_.selectionStart_;
                int selectionEnd = hitCharacter(xPos - editField_.xOffset_);
                editField_.setCaretPosition(selectionEnd);
                if(selectionStart > selectionEnd) std::swap(selectionStart, selectionEnd);
                editField_.setCurrentSelection(selectionStart, selectionEnd);
            }
            else
            {
                if(xPos <= 0.0f)
                {
                    const int s = editField_.selection_.startIndex;
                    if(s > 0)
                    {
                        editField_.setCurrentSelection(s-1, editField_.selectionStart_);
                        editField_.setCaretPosition(s-1);
                    }
                }
                else if(xPos >= editField_.size().x())
                {
                    unsigned int e = editField_.selection_.endIndex;
                    if(e < editField_.text().length())
                    {
                        editField_.setCurrentSelection(editField_.selectionStart_, e+1);
                        editField_.setCaretPosition(e+1);
                    }
                }
            }
            isHandled = true;
        }
        return isHandled;
    }
private:
    bool isLeftClick(MouseClickMessage& aMessage) const
    {
        return
            (aMessage.button() == MouseClickMessage::left) &&
            ((aMessage.event() == MouseClickMessage::pressed) ||
             (aMessage.event() == MouseClickMessage::doubleClicked));
    }

    bool isLeftRelease(MouseClickMessage& aMessage) const
    {
        return
            (aMessage.button() == MouseClickMessage::left) &&
             (aMessage.event() == MouseClickMessage::released);
    }

    int hitCharacter(float aPosition)
    {
        int result = editField_.renderDesc().textDesc().hitCharacterIndex(editField_.text(), aPosition);
        if(result < 0)
            result = editField_.text().length();
        return result;
    }

    EditField& editField_;
};

IMPLEMENT_OBJECT_CREATE(EditField,Window)

EditField::EditField(Window* parent,const String& text):
    Window(parent,text),
    caretPosition_(text.length()),
    selectionStart_(-1),
    xOffset_(0.0f),
    renderDesc_(0)
{
    selection_.startIndex = -1;
    selection_.endIndex = -1;
    mouseListener_ = new EditFieldMouseListener(*this);
    addMessageListener(mouseListener_);
    addMessageListener(this);
}

EditField::~EditField()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
    removeMessageListener(this);
}

bool EditField::onKeyPressed(uint16_t key,bool alt,bool ctrl,bool shift)
{
    bool isHandled = false;
    if(key == KeyMessage::Key_LEFT)
    {
        int offset = -1;
        if(ctrl)
            offset = lastSpaceBefore(caretPosition()) - caretPosition();
        if(shift)
            moveCaretMaintainSelection(offset);
        else
            moveCaretDiscardSelection(offset);
        isHandled = true;
    }
    else if(key == KeyMessage::Key_RIGHT)
    {
        int offset = 1;
        if(ctrl)
            offset = nextWordStartPosAfter(caretPosition()) - caretPosition();
        if(shift)
            moveCaretMaintainSelection(offset);
        else
            moveCaretDiscardSelection(offset);
        isHandled = true;
    }
    else if(key == KeyMessage::Key_HOME)
    {
        if(shift)
        {
            selectionStart_ = caretPosition();
            setCurrentSelection(0, caretPosition());
        }
        else
            removeSelection();

        setCaretPosition(0);
        isHandled = true;
    }
    else if(key == KeyMessage::Key_END)
    {
        if(shift)
        {
            selectionStart_ = caretPosition();
            setCurrentSelection(caretPosition(),text().length());
        }
        else
            removeSelection();
        setCaretPosition(text().length());
        isHandled = true;
    }
    else if(key == KeyMessage::Key_BACKSPACE)
    {
        doBackspace();
        textChanged(text());
        isHandled = true;
    }

    return isHandled;
}

bool EditField::onCharacterInput(uint16_t character)
{
    if(character < 32)
    {
        switch(character)
        {
        case 1: // ctrl-a !?!?!!
            setCaretPosition(int(text().length()));
            selectionStart_ = 0;
            selectAll();
            break;
        case 3: // ctrl-c !?!?!!
            if(!selectedText().empty())
                WindowManager::instance().copyToClipboard(selectedText());
            break;
        case 22: // ctrl-v !?!?!!
            {
                if(isSelectionValid())
                    deleteSelection();
                std::basic_string<uint32_t> str = text().get();
                std::basic_string<uint32_t> pasteText = WindowManager::instance().clipboardContents().get();
                str.insert(caretPosition(),pasteText);
                setText(str);
                textChanged(text());
                setCaretPosition(caretPosition() + pasteText.length());
            }
            break;
        case 24: // ctrl-x !?!?!!
            if(isSelectionValid())
            {
                WindowManager::instance().copyToClipboard(selectedText());
                auto str = text().get();
                str.erase(selection_.startIndex, selection_.endIndex - selection_.startIndex);
                setCaretPosition(selection_.startIndex);
                removeSelection();
                setText(str);
                textChanged(text());
            }
            break;
        case 8:
            doBackspace();
            textChanged(text());
            break;
        case 13:
            returnPressed();
        default:
            break;
        }
    }
    else if(character == 127)
    {
        doDelete();
        textChanged(text());
    }
    else
    {
        if(isSelectionValid())
            deleteSelection();
        setText(stringUpToCaret() + toBGEString(character) + stringFromCaret());
        setCaretPosition(caretPosition()+1);
        textChanged(text());
    }

    return true;
}

EditField& EditField::doBackspace()
{
    if(isSelectionValid())
    {
        deleteSelection();
    }
    else if(caretPosition() > 0)
    {
        auto str = stringUpToCaret().get();
        str = str.substr(0,str.length()-1);
        setText(str + stringFromCaret());
        setCaretPosition(caretPosition_-1);
    }

    removeSelection();
    return *this;
}

EditField& EditField::doDelete()
{
    if(isSelectionValid())
    {
        deleteSelection();
    }
    else if(caretPosition() < text().length())
    {
        std::basic_string<uint32_t> str = stringFromCaret().get();
        str = str.substr(1, str.length()-1);
        setText(stringUpToCaret() + str);
    }

    removeSelection();
    return *this;
}

EditField& EditField::deleteSelection()
{
    std::basic_string<uint32_t> str = text().get();
    str = str.substr(0, selection_.startIndex) + str.substr(selection_.endIndex, str.length() - selection_.endIndex);
    setCaretPosition(selection_.startIndex);
    removeSelection();
    setText(str);
    return *this;
}

bool EditField::isSelectionValid() const
{
    const int s = selection_.startIndex;
    const int e = selection_.endIndex;
    return
       ((s >= 0) && (e > 0) && (s < e) &&
        (s < int(text().length())) && (e <= int(text().length())));
}

EditField& EditField::setCaretPosition(uint32_t pos)
{
    caretPosition_ = pos;

    const float xPos = caretXPos();
    const float leftSideX = position().x() + renderDesc().textDesc().margin();
    const float rightSideX = leftSideX + size().x() - (renderDesc().textDesc().margin() * 2.0f);
    if(xPos < leftSideX)
    {
        xOffset_ += (leftSideX - xPos);
    }
    else if(xPos > rightSideX)
    {
        xOffset_ -= (xPos - rightSideX);
    }

    return *this;
}

bool EditField::isValidCaretPosition(int position) const
{
    return (position >= 0) && (position <= int(text().length()));
}

void EditField::moveCaretMaintainSelection(int anOffset)
{
    if(selectionStart_ < 0)
        selectionStart_ = caretPosition();

    const int newCaretPosition = caretPosition() + anOffset;
    if(isValidCaretPosition(newCaretPosition)) setCaretPosition(newCaretPosition);
    if(selectionStart_ > int(caretPosition()))
        setCurrentSelection(caretPosition(), selectionStart_);
    else
        setCurrentSelection(selectionStart_, caretPosition());
}

void EditField::moveCaretDiscardSelection(int anOffset)
{
    removeSelection();
    const int newCaretPosition = caretPosition() + anOffset;
    if(isValidCaretPosition(newCaretPosition))
        setCaretPosition(newCaretPosition);
}

String EditField::stringUpToCaret() const
{
    String ret;
    if(caretPosition() > 0)
        ret = text().get().substr(0, caretPosition());
    return ret;
}

String EditField::stringFromCaret() const
{
    String ret;
    if(caretPosition() < text().length())
        ret = text().get().substr(caretPosition(), text().length());
    return ret;
}

EditField::Selection EditField::currentSelection() const
{
    return selection_;
}

EditField& EditField::setCurrentSelection(const Selection& selection)
{
    setCurrentSelection(selection.startIndex,selection.endIndex);
    return *this;
}

EditField& EditField::setCurrentSelection(int startIndex, int endIndex)
{
    selection_.startIndex = startIndex;
    selection_.endIndex   = endIndex;
    return *this;
}

EditField& EditField::removeSelection()
{
    setCurrentSelection(0, 0);
    selectionStart_ = -1;
    return *this;
}

EditField& EditField::selectAll()
{
    setCurrentSelection(0,int(text().length()));
    return *this;
}

float EditField::caretXPos() const
{
    TextDesc& textDesc = renderDesc().textDesc();
    textDesc.setText(stringUpToCaret());
    Vector2f sz = textDesc.textSize();
    return sz.x() + xOffset_ + geometry().left() + textDesc.margin();
}

int EditField::lastSpaceBefore(int pos) const
{
    auto str = text().get().substr(0,pos);
    int ret = 0;
    if(str.length() > 0)
    {
        std::basic_string<uint32_t>::size_type n = str.find_last_not_of(' ');
        if(n != std::basic_string<uint32_t>::npos)
            str.erase(n, str.length()-n);

        std::basic_string<uint32_t>::size_type p = str.find_last_of(' ');
        if(p != std::basic_string<uint32_t>::npos)
            ret = int(p+1);
    }
    return ret;
}

int EditField::nextWordStartPosAfter(int pos) const
{
    auto str = text().get().substr(pos, text().length()-pos);
    const int32_t firstNonSpacePos = str.find_first_not_of(' ');

    int ret = int(text().length());
    if(firstNonSpacePos != std::basic_string<uint32_t>::npos)
    {
        str.erase(0, firstNonSpacePos);
        auto p = str.find_first_of(' ');

        if(p != std::basic_string<uint32_t>::npos)
        {
            std::basic_string<uint32_t> str2 = str.substr(p, str.length());
            std::basic_string<uint32_t>::size_type p2 = str2.find_first_not_of(' ');
            if(p2 != std::basic_string<uint32_t>::npos)
                ret = int(p2) + int(p) + pos;
        }
    }
    return ret;
}

String EditField::selectedText() const
{
    String ret;
    if(isSelectionValid())
        ret = text().get().substr(selection_.startIndex, selection_.endIndex - selection_.startIndex);

    return ret;
}

Vector2f EditField::recommendedSize() const
{
    EditFieldDesc* renderDesc = renderDesc_;
    if(renderDesc == 0)
        renderDesc = &WindowManager::instance().propertyScheme()->editFieldDesc_;

    renderDesc->textDesc().setText(text());
    return renderDesc->textDesc().textSize() + Vector2f(8, 8);
}

Window& EditField::onDraw()
{
    renderDesc().setEditField(this);
    renderDesc().draw(geometry(),WindowManager::instance().canvas());
    return *this;
}

WINDOW_LOAD_APPAREARANCE(EditField)

bool EditField::loadProperty(TiXmlNode* node)
{
    DESERIALIZE_OBJECT(caretPosition,node,uint32_t)
    //DESERIALIZE_OBJECT(selection,node,Selection)
    DESERIALIZE_OBJECT(selectionStart,node,int)
    DESERIALIZE_OBJECT(xOffset,node,float)
    TiXmlNode* window = node->FirstChildElement("Window");
    Window::loadProperty(window);
    return true;
}

bool EditField::saveProperty(TiXmlNode* node)
{
    SERIALIZE_OBJECT(caretPosition,node)
    //SERIALIZE_OBJECT(selection,node)
    SERIALIZE_OBJECT(selectionStart,node)
    SERIALIZE_OBJECT(xOffset,node)
    TiXmlElement element("Window");
    Window::saveProperty(&element);
    node->InsertEndChild(element);
    return true;
}

void EditField::deleteLater()
{
    removeMessageListener(mouseListener_);
    delete mouseListener_;
    removeMessageListener(this);
	Window::deleteLater();
}

EditFieldDesc& EditField::renderDesc() const
{
    return (renderDesc_ != 0) ? *renderDesc_ : WindowManager::instance().propertyScheme()->editFieldDesc_;
}

EditFieldDesc::EditFieldDesc() :
    background_(new SolidSurfaceDesc()),
    border_(new RectangleDesc()),
    textDesc_(new TextDesc()) ,
    selectionColor_(Color(0.85f, 0.85f, 0.85f, 1)),
    selectedTextColor_(Color(0.35f, 0.35f, 0.35f, 1))
{
    textDesc_->setHorizontalAlignment(Horizontal_left);
}

EditFieldDesc::~EditFieldDesc()
{
    delete background_;
    delete border_;
    delete textDesc_;
}

bool EditFieldDesc::loadProperty(TiXmlNode* node)
{
	TiXmlElement* thisNode = node->FirstChildElement("EditFieldDesc");
	if(thisNode)
	{
		CREATE_BY_NODE(background,thisNode,RenderDesc)
		CREATE_BY_NODE(border,thisNode,RenderDesc)
		CREATE_BY_NODE(textDesc,thisNode,TextDesc)
		TiXmlNode* window = thisNode->FirstChildElement("RenderDesc");
		RenderDesc::loadProperty(window);
	}
	return true;
}

bool EditFieldDesc::saveProperty(TiXmlNode* node)
{
    TiXmlElement thisNode("EditFieldDesc");
    background_->saveProperty(&thisNode,"background");
    border_->saveProperty(&thisNode,"border");
    textDesc_->saveProperty(&thisNode,"textDesc");
    TiXmlElement element("RenderDesc");
    RenderDesc::saveProperty(&element);
    thisNode.InsertEndChild(element);
    node->InsertEndChild(thisNode);
    return true;
}

void EditFieldDesc::draw(const FloatRect& area,Canvas* canvas) const
{
    textDesc_->setHorizontalAlignment(Horizontal_left);

    if(background_ != 0)
        background_->draw(area,canvas);

    if(editField_->text().length() != 0)
    {
        FloatRect clipRectangle(area);
        clipRectangle.expandBy(-textDesc_->margin());
        canvas->enableClipRectangle(clipRectangle);

        const EditField::Selection& selection = editField_->selection_;
        if(selection.endIndex > selection.startIndex)
            drawTextWithSelection(area,canvas);
        else
            drawTextWithoutSelection(area,canvas);
    }

    drawCaret();
    canvas->disableClipRectangle();

    if(border_ != 0)
        border_->draw(area,WindowManager::instance().canvas());

    canvas->setColor(Color(1, 1, 1, 1));
}

void EditFieldDesc::drawTextWithSelection(const FloatRect& anArea,Canvas* canvas) const
{
    FloatRect area(anArea);

    const float margin = textDesc_->margin();
    textDesc_->setMargin(0.0f);
    area.expandWidthBy(-margin);

    FloatRect textRectangle(area);
    textRectangle.translate(Vector2f(editField_->xOffset_, 0.0f));
    const float preSelectedWidth = drawPreSelectedText(textRectangle,canvas);

    textRectangle.translate(Vector2f(preSelectedWidth, 0.0f));
    const float selectedWidth = drawSelectedText(textRectangle, area, preSelectedWidth,canvas);

    textRectangle.translate(Vector2f(selectedWidth, 0.0f));
    drawPostSelectedText(textRectangle,canvas);

    textDesc_->setMargin(margin);
}

void EditFieldDesc::drawTextWithoutSelection(const FloatRect& area,Canvas* canvas) const
{
    FloatRect textRectangle(area);
    textRectangle.translate(Vector2f(editField_->xOffset_, 0.0f));
    (*textDesc_)
        .setText(editField_->text())
        .draw(textRectangle,WindowManager::instance().canvas());
}

float EditFieldDesc::drawPreSelectedText(const FloatRect& area,Canvas* canvas) const
{
    const auto text = editField_->text().get().substr(0, editField_->selection_.startIndex);
    textDesc_->setText(text);
    (*textDesc_).draw(area,WindowManager::instance().canvas());
    return textDescTextWidth();
}

float EditFieldDesc::drawSelectedText(const FloatRect& anArea, const FloatRect& aVisibleArea, float aPreSelectedWidth,Canvas* canvas) const
{
    const EditField::Selection& selection = editField_->selection_;
    const int selectionSize = selection.endIndex-selection.startIndex;

    const auto selected = editField_->text().get().substr(selection.startIndex, selectionSize);
    textDesc_->setText(selected);

    drawSelectionBackground(aVisibleArea, editField_->xOffset_ + aPreSelectedWidth);

    const Color unselectedColor = textDesc_->color();
    (*textDesc_)
        .setColor(selectedTextColor_)
        .draw(anArea,canvas)
        ;

    textDesc_->setColor(unselectedColor);

    return textDescTextWidth();
}

void EditFieldDesc::drawPostSelectedText(const FloatRect& anArea,Canvas* canvas) const
{
    const int selectionEnd = editField_->selection_.endIndex;
    const std::basic_string<uint32_t>& text = editField_->text().get();
    std::basic_string<uint32_t> postSelected = text.substr(selectionEnd, text.length() - selectionEnd);

    (*textDesc_).setText(postSelected).draw(anArea,canvas);
}

void EditFieldDesc::drawSelectionBackground(const FloatRect& aVisibleArea, float aStartX) const
{
    WindowManager::instance().canvas()->setColor(selectionColor_);
    const Vector2f topLeft = aVisibleArea.position() + Vector2f(aStartX, 1.0f);
    const Vector2f bottomRight(textDescTextWidth(), aVisibleArea.height_-1.0f);
    WindowManager::instance().canvas()->drawRectangle(FloatRect(topLeft,bottomRight), true);
}

void EditFieldDesc::drawCaret() const
{
    if(editField_->isFocused())
    {
        Canvas* canvas = WindowManager::instance().canvas();
        const float x = editField_->caretXPos();
        const float caretHeightOverTwo = (textDesc_->lineHeight() + 4.0f) * 0.5f;
        const float verticalCenter = (editField_->size().y() * 0.5f) +
                                      editField_->geometry().top_;

        canvas->drawLine(Vector2f(x, verticalCenter - caretHeightOverTwo),
                 Vector2f(x, verticalCenter + caretHeightOverTwo));
    }
}

float EditFieldDesc::textDescTextWidth() const
{
    float width = textDesc_->textSize().x();
    if(width < 0.0f)
        width = 0.0f;
    return width;
}

}
