#ifndef BGE_GUI_LISTWINDOW_H
#define BGE_GUI_LISTWINDOW_H
#include <BGE/Graphics/RenderDesc.h>
#include <BGE/GUI/Window.h>

namespace bge
{

class ScrollBar;
class ListWindowMouseListener;
class TextDesc;

class BGE_EXPORT_API ListWindow : public Window
{
    DECLARE_OBJECT(ListWindow)
public:
    ListWindow(Window* parent = 0,const FloatRect& rect = FloatRect());
    virtual ~ListWindow();
public:
    int selectedIndex() const { return selectedIndex_; }
    int highlightedIndex() const { return highlightedIndex_; }
    ListWindow& select(int index);
    virtual size_t numberOfEntries() const = 0;
public:
    Signal0 leftClicked;
    Signal0 rightClicked;
    Signal1<int> selectionChanged;
public:
    float lineHeight() const;
    float lineHeightFactor() const { return lineHeightFactor_; }
    ListWindow& setLineHeightFactor(float factor) { lineHeightFactor_ = factor; return *this; }

    virtual bool process(const Message& message);
    virtual Vector2f recommendedSize() const;
    const ScrollBar* verticalScrollBar() const { return verticalScrollBar_; }
    uint32_t firstVisibleEntry() const;
    void setScrollBarParameters();

    virtual bool containsFocus() const;
    virtual Window* windowBelow(const Vector2f& p);
    virtual bool doesHierarchyContain(Window* window) const;
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
	virtual void deleteLater();
protected:
    virtual void onResize();
    virtual void onMove();
    virtual float entryHeight() const = 0;
    void onContentsChanged();
    uint32_t entryIndexAt(const Vector2f& p) const;
    void highlight(int index);

    ListWindow& positionScrollBar();
    ScrollBar* verticalScrollBar_;

    float heightOfAllContents() const;
    float heightOfVisibleContents() const;
private:
    friend class ListWindowMouseListener;

    ListWindowMouseListener*  listWindowMouseListener_;
    int selectedIndex_;
    int highlightedIndex_;
    float lineHeightFactor_;
};

class BGE_EXPORT_API ListWindowDesc : public RenderDesc
{
public:
    ListWindowDesc();
    virtual ~ListWindowDesc();
public:
    virtual void draw(const FloatRect& area,Canvas* canvas)const;
    ListWindowDesc& setListWindow(ListWindow* list)
    {
        listWindow_ = list;
        return *this;
    }

    TextDesc& textDesc() { return *textDesc_; }
public:
    virtual bool loadProperty(TiXmlNode* node);
    virtual bool saveProperty(TiXmlNode* node);
protected:
    virtual void drawRow(size_t row,const FloatRect& rect)const = 0;

    RenderDesc* background_;
    RenderDesc* border_;
    TextDesc*   textDesc_;
    Color       selectionColor_;
    Color       selectedTextColor_;
    Color       highlightColor_;
    Color       highlightedTextColor_;
    ListWindow* listWindow_;
};

}

#endif
