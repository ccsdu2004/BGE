#ifndef BGE_GUI_Layout_H
#define BGE_GUI_Layout_H
#include <BGE/System/Object.h>
#include <BGE/System/Math.h>
#include <BGE/System/Enum.h>
#include <BGE/System/Sigslot.h>
#include <string>
#include <list>
#include <vector>

namespace bge
{

class Window;

class BGE_EXPORT_API LayoutInfo : public Object
{
    DECLARE_OBJECT(LayoutInfo)
public:
    LayoutInfo(){}
    virtual ~LayoutInfo(){}
};

class BGE_EXPORT_API Layout : public Object
{
    DECLARE_OBJECT(Layout)
public:
    Layout(Window* parent = 0);
    virtual ~Layout();
public:
    virtual Layout& addWindow(Window* window) = 0;
    virtual Layout& addWindow(Window* window,const LayoutInfo& info)
    {
        addWindow(window);
        return *this;
    }

    virtual Layout& removeWindow(Window* window) = 0;
    virtual Layout& arrangeWithin() = 0;

    virtual std::list<Window*> windows()const = 0;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);

	Window* parse(TiXmlElement* node);
public:
    float spacing() const { return spacing_; }
    void  setSpacing(float spacing) { spacing_ = spacing; }
private:
    float   spacing_;
};

class BGE_EXPORT_API BoxLayout : public Layout
{
    DECLARE_OBJECT(BoxLayout)
public:
    explicit BoxLayout(Orientation justification = Orientation_horizontal,Window* parent = 0);
    BoxLayout(Orientation justification,float spacing,bool respectChildSizes);
    virtual ~BoxLayout(){}
public:
    Orientation justification() const { return justification_; }
    BoxLayout& setJustification(Orientation justification)
    {
        justification_ = justification;
        return *this;
    }

    bool isChildSizeRespected() const { return isChildSizeRespected_; }
    BoxLayout& respectChildSize() { isChildSizeRespected_ = true; return *this; }
    BoxLayout& doNotRespectChildSize() { isChildSizeRespected_ = false; return *this; }

    std::list<Window*> windows()const{return windows_;}
public:
    virtual Layout& addWindow(Window* window);
    virtual Layout& removeWindow(Window* window);
    virtual Layout& arrangeWithin();
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    Orientation justification_;
    bool isChildSizeRespected_;
    std::list<Window*> windows_;
};

class BGE_EXPORT_API FlowLayout : public Layout
{
    DECLARE_OBJECT(FlowLayout)
public:
    explicit FlowLayout(Window* parent = 0,Horizontal alignment = Horizontal_left);
    FlowLayout(Horizontal alignment,float spacing);
    virtual ~FlowLayout(){}
public:
    Horizontal alignment() const { return alignment_; }
    FlowLayout& setAlignment(Horizontal alignment)
    {
        alignment_ = alignment;
        return *this;
    }
public:
    virtual Layout& addWindow(Window* window);
    virtual Layout& removeWindow(Window* window);
    virtual Layout& arrangeWithin();
    virtual std::list<Window*> windows()const{return windows_;}
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    Horizontal alignment_;
    std::list<Window*> windows_;
};

class BGE_EXPORT_API StackLayout : public Layout
{
    DECLARE_OBJECT(StackLayout)
public:
    StackLayout(Window* parent = NULL);
    ~StackLayout();
public:
    virtual Layout& addWindow(Window* window);
    virtual Layout& removeWindow(Window* window);

    std::list<Window*> windows()const{return windows_;}
    Layout& insertWindow(int index,Window* window);
    Window* currentWindow()const;
    int currentIndex()const;
    Window* window(int index = -1)const;
    int count()const;

    Layout& setCurrentIndex(int index);
    Layout& setCurrentWidget(Window* window);

    Layout& arrangeWithin();
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
public:
    Signal1<int> windowRemoved;
    Signal1<int> currentChanged;
private:
    std::list<Window*> windows_;
    int current_;
};

class BGE_EXPORT_API AbsoluteLayout : public Layout
{
    DECLARE_OBJECT(AbsoluteLayout)
public:
    AbsoluteLayout(Window* parent = NULL);
    ~AbsoluteLayout();
public:
    Layout& addWindow(Window* window);
    Layout& removeWindow(Window* window);
    int count()const;
    Layout& arrangeWithin();
    std::list<Window*> windows()const{return windows_;}
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    std::list<Window*> windows_;
};

class BGE_EXPORT_API GridCellInfo : public LayoutInfo
{
    DECLARE_OBJECT(GridCellInfo)
public:
    GridCellInfo();
    virtual ~GridCellInfo(){}
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
public:
    uint32_t horizontalSpan_;
    uint32_t verticalSpan_;
    uint32_t columnIndex_;
    uint32_t rowIndex_;
    static const uint32_t relativeToPrevious;
};

class BGE_EXPORT_API GridLayout : public Layout
{
    DECLARE_OBJECT(GridLayout)
private:
    struct Cell
    {
        Window* window_;
        GridCellInfo info_;

        Cell(Window* window,const GridCellInfo& cellInfo)
        {
            window_ = window;
            info_ = cellInfo;
        }
    };
public:
    explicit GridLayout(const Vector2f& border = Vector2f(2.0f,2.0f)):
        border_(border),
		numCols_(0),
		numRows_(0)
    {
    }
    virtual ~GridLayout(){}
public:
    GridLayout& setColumnWidths(const std::vector<float>& widths);
    GridLayout& setRowHeights(const std::vector<float>& heights);
    float columnWidth(int index)const;
    float rowHeight(int index)const;
public:
    virtual Layout& addWindow(Window* window);
    virtual Layout& addWindow(Window* window,const LayoutInfo& info);
    virtual Layout& removeWindow(Window* window);

    virtual Layout& arrangeWithin();

    std::list<Window*> windows()const
    {
        std::list<Window*> list;
        auto itr = cells_.begin();
        while(itr != cells_.end())
        {
            list.push_back(itr->window_);
            itr ++;
        }
        return list;
    }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
private:
    struct Block
    {
        uint32_t row;
        uint32_t col;
        uint32_t hSpan;
        uint32_t vSpan;
        float hSpace;
        float vSpace;
        bool windowStart;
        Window* window;
    };

	void parseItemNode(TiXmlElement* node);
    void calculateBlocks();
    void calculateColWidths(std::vector<float>& colWidths,float width);
    void calculateRowHeights(std::vector<float>& rowHeights,float height);
    void addBlockToMap(const Block& block)
    {
        blockMap_[block.row*256 + block.col] = block;
    }
private:
    Vector2f border_;
    std::vector<float> columnWidths_;
    std::vector<float> rowHeights_;
    uint32_t numCols_;
    uint32_t numRows_;
    std::list<Cell> cells_;
    std::map<uint32_t,Block> blockMap_;
};

}

#endif
