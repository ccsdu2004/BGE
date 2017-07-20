#ifndef BGE_GUI_LISTCONTROL_H
#define BGE_GUI_LISTCONTROL_H
#include <BGE/GUI/ListWindow.h>
#include <vector>
#include <list>

namespace bge
{

class ListControlDesc;
class TextDesc;

class BGE_EXPORT_API ListControl : public ListWindow
{
    DECLARE_OBJECT(ListControl)
public:
    class Row
    {
    public:
        typedef std::vector<String> Strings;
    public:
		Row(ListControl& list): listControl_(list)
        {
            strings_.resize(list.numberOfColumns());
        }

        Row(const Row& row): listControl_(row.listControl_)
        {
            strings_ = row.strings_;
        }
    public:
        Row& setEntry(uint32_t column,const String& string)
        {
            //ASSERT ( aColumn < listControl_.numberOfColumns() );
            strings_[column] = string;
            return *this;
        }

        const String& stringAt(int column)const
        {
            return strings_[column];
        }
    private:
        ListControl& listControl_;
        Strings strings_;
        Row& operator = (const Row& aRow);
    };

    typedef std::list<Row> Rows;
public:
    ListControl(Window* parent = 0,const FloatRect& rect = FloatRect(0,0,64,128),uint32_t numColumns = 0);
    virtual ~ListControl();
public:
    Signal0 leftClicked;
    Signal0 rightClicked;
    Signal1<int> selectionChanged;
public:
    uint32_t numberOfColumns() const { return int(columnWidths_.size()); }
    ListControl& setColumnWidths(const std::vector<float>& widths);
    float columnWidth(uint32_t index) const { return columnWidths_[index]; }
    ListControl& addRow(const std::vector<String>& strings);
    ListControl& removeRowAt(uint32_t pos);
    uint32_t numberOfRows() const { return int(rows_.size()); }
    size_t numberOfEntries() const { return numberOfRows(); }
    const Row& rowAt(uint32_t pos) const;

    virtual bool loadAppearance(const std::string& file);
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual Window& onDraw();
    virtual void onResize();
    uint32_t columnIndexAt(const Vector2f& p) const;

    virtual float entryHeight() const;

    Rows rows_;
    std::vector<float> columnWidths_;
    bool usingDefaultWidths_;
    Window& resizeColumns();
private:
    ListControlDesc* renderDesc_;
};

class BGE_EXPORT_API ListControlDesc : public ListWindowDesc
{
public:
    ListControlDesc();
    virtual ~ListControlDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual void drawRow(size_t row,const FloatRect& rectangle) const;
private:
    float separatorWidth_;
    Color separatorColor_;
};

}

#endif
