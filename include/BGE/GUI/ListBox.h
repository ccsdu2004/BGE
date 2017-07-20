#ifndef BGE_GUI_LISTBOX_H
#define BGE_GUI_LISTBOX_H
#include <BGE/GUI/ListWindow.h>
#include <BGE/GUI/Message.h>

namespace bge
{

class ListBoxDesc;
class TextDesc;

class BGE_EXPORT_API ListBox : public ListWindow
{
    DECLARE_OBJECT(ListBox)
public:
    typedef std::list<String> Strings;
public:
    ListBox(Window* parent = 0,const FloatRect& rect = FloatRect());
    virtual ~ListBox();
public:
    ListBox& addString(const String& string);
    ListBox& removeStringAt(size_t pos);
    ListBox& removeAllStrings();

    String stringAt(size_t position) const;
    size_t numberOfStrings() const { return strings_.size(); }

    virtual size_t numberOfEntries() const { return numberOfStrings(); }

    bool contains(const String& string) const;
    int indexOf(const String& string) const;

    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
    virtual float entryHeight() const;
private:
    Strings strings_;
    ListBoxDesc* renderDesc_;
};

class BGE_EXPORT_API ListBoxDesc : public ListWindowDesc
{
public:
    ListBoxDesc(){}
    virtual ~ListBoxDesc(){}
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const
    {
        ListWindowDesc::draw(area,canvas);
    }
protected:
    virtual void drawRow(size_t row,const FloatRect& rectangle)const;
};

}

#endif
