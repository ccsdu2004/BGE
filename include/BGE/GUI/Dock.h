#ifndef BGE_GUI_DOCK_H
#define BGE_GUI_DOCK_H
#include <BGE/Config.h>
#include <BGE/System/Math.h>
#include <BGE/System/Object.h>

namespace bge
{

class BGE_EXPORT_API Dock : public Object
{
    DECLARE_OBJECT(Dock)
public:
    Dock() : docked_(false) {}
    virtual ~Dock() {}
public:
    void setDocked(bool d) { docked_ = d; }
    void setTop(bool t)  { top_ = t;  }
    void setLeft(bool l) { left_ = l; }
    void setMargin(const Vector2f& m) { margin_ = m; }

    bool docked() const { return docked_; }
    bool top()  const { return top_;  }
    bool left() const { return left_; }
    const Vector2f& margin() const { return margin_; }

    void dock(bool t,bool l,const Vector2f& m)
    {
        setDocked(true);
        setTop(t);
        setLeft(l);
        setMargin(m);
    }
    void undock(){ setDocked(false); }
    void computePosition(Vector2f& position,const Vector2f& size,const Vector2f& parentSize) const;
private:
    bool     docked_;
    bool     top_;
    bool     left_;
    Vector2f margin_;
};

}

#endif // BGE_GUI_DOCK_H
