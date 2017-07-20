template <typename T>
inline Vector2<T>::Vector2() :
x_(.0f),
y_(.0f)
{
}

template <typename T>
inline Vector2<T>::Vector2(T x,T y):
x_(x),
y_(y)
{
}

template <typename T>
inline Vector2<T>::Vector2(const Vector2<T>& vector) :
x_(vector.x_),
y_(vector.y_)
{
}

template <typename T>
inline Vector2<T> operator -(const Vector2<T>& right)
{
    return Vector2<T>(-right.x_,-right.y_);
}

template <typename T>
inline Vector2<T>& operator +=(Vector2<T>& left,const Vector2<T>& right)
{
    left.x_ += right.x_;
    left.y_ += right.y_;
    return left;
}

template <typename T>
inline Vector2<T>& operator -=(Vector2<T>& left,const Vector2<T>& right)
{
    left.x_ -= right.x_;
    left.y_ -= right.y_;
    return left;
}

template <typename T>
inline Vector2<T> operator +(const Vector2<T>& left,const Vector2<T>& right)
{
    return Vector2<T>(left.x_ + right.x_, left.y_ + right.y_);
}

template <typename T>
inline Vector2<T> operator -(const Vector2<T>& left,const Vector2<T>& right)
{
    return Vector2<T>(left.x_ - right.x_, left.y_ - right.y_);
}

template <typename T>
inline Vector2<T> operator *(const Vector2<T>& left,T right)
{
    return Vector2<T>(left.x_ * right, left.y_ * right);
}

template <typename T>
inline Vector2<T> operator *(T left, const Vector2<T>& right)
{
    return Vector2<T>(right.x_ * left, right.y_ * left);
}

template <typename T>
inline Vector2<T>& operator *=(Vector2<T>& left, T right)
{
    left.x_ *= right;
    left.y_ *= right;

    return left;
}

template <typename T>
inline Vector2<T> operator /(const Vector2<T>& left, T right)
{
    return Vector2<T>(left.x_ / right, left.y_ / right);
}

template <typename T>
inline Vector2<T>& operator /=(Vector2<T>& left, T right)
{
    left.x_ /= right;
    left.y_ /= right;
    return left;
}

template <typename T>
inline bool operator ==(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.x_ == right.x_) && (left.y_ == right.y_);
}

template <typename T>
inline bool operator !=(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.x_ != right.x_) || (left.y_ != right.y_);
}
