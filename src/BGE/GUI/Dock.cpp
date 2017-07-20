#include <BGE/GUI/Dock.h>

namespace bge
{

IMPLEMENT_OBJECT_CREATE(Dock,Object)

void Dock::computePosition(Vector2f& position,const Vector2f& size,const Vector2f& parentSize) const
{
    if(!docked())
        return;

    if(top_)
        position.y_ = margin_.y_;
    else
        position.y_ = parentSize.y_ - size.y_ - margin_.y_;

    if(left_)
        position.x_ = margin_.x_;
    else
        position.x_ = parentSize.x_ - size.x_ - margin_.x_;
}

}
