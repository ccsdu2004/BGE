template <typename T>
Rect<T>::Rect():
left_(0),
top_(0),
width_(0),
height_(0)
{
}

template <typename T>
Rect<T>::Rect(T rectLeft,T rectTop,T rectWidth,T rectHeight) :
left_(rectLeft),
top_(rectTop),
width_(rectWidth),
height_(rectHeight)
{
}

template <typename T>
Rect<T>::Rect(const Vector2<T>& position,const Vector2<T>& size):
left_(position.x_),
top_(position.y_),
width_(size.x_),
height_(size.y_)
{
}

template <typename T>
Rect<T>::Rect(const Rect<T>& rectangle):
left_(rectangle.left_),
top_ (rectangle.top_),
width_(rectangle.width_),
height_(rectangle.height_)
{
}

template <typename T>
bool Rect<T>::contains(T x,T y)const
{
    return (x >= left_) && (x < left_ + width_) && (y >= top_) && (y < top_ + height_);
}

template <typename T>
bool Rect<T>::contains(const Vector2<T>& point) const
{
    return contains(point.x_, point.y_);
}

template <typename T>
bool Rect<T>::intersects(const Rect<T>& rectangle) const
{
    Rect<T> intersection;
    return intersects(rectangle, intersection);
}

template <typename T>
bool Rect<T>::intersects(const Rect<T>& rectangle, Rect<T>& intersection) const
{
    // Compute the intersection boundaries
    T interLeft   = std::max(left_,         rectangle.left_);
    T interTop    = std::max(top_,          rectangle.top_);
    T interRight  = std::min(left_ + width_, rectangle.left_ + rectangle.width);
    T interBottom = std::min(top_ + height_, rectangle.top_ + rectangle.height);

    // If the intersection is valid (positive non zero area), then there is an intersection
    if ((interLeft < interRight) && (interTop < interBottom))
    {
        intersection = Rect<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
        return true;
    }
    else
    {
        intersection = Rect<T>(0, 0, 0, 0);
        return false;
    }
}

template <typename T>
inline bool operator ==(const Rect<T>& left, const Rect<T>& right)
{
    return (left.left == right.left) && (left.width == right.width) &&
           (left.top == right.top) && (left.height == right.height);
}

template <typename T>
inline bool operator !=(const Rect<T>& left, const Rect<T>& right)
{
    return !(left == right);
}
