#ifndef BGE_GUI_MULTITEXTBUTTON_H
#define BGE_GUI_MULTITEXTBUTTON_H
#include <BGE/GUI/BasicButton.h>

namespace bge
{

class MTButtonMouseListener;

class BGE_EXPORT_API MultiTextButton : public BasicButton
{
    typedef std::list<String> Strings;
    DECLARE_OBJECT(MultiTextButton)
public:
    MultiTextButton(Window* parent = 0, const String& text = L"");
    virtual ~MultiTextButton();
public:
    MultiTextButton& addString(const String& string)
    {
        strings_.push_back(string);
        return *this;
    }

    uint32_t numberOfStrings() const { return int(strings_.size()); }
    String stringAt(int index) const
    {
        Strings::const_iterator it = strings_.begin();
        std::advance(it, index);
        return *it;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
public:
    Signal1<const String&> selectionChanged;
protected:
    virtual Window& onDraw();
    MultiTextButton& onPressedRight();
    MultiTextButton& onPressedLeft();

    uint32_t stringIndex_;

    friend class MTButtonMouseListener;
    MTButtonMouseListener* mtButtonMouseListener_;
private:
    Strings strings_;
};

}

#endif
