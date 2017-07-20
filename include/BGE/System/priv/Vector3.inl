template <typename T>
inline Vector3<T>::Vector3() :
x_(0),
y_(0),
z_(0)
{
}

////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T>::Vector3(T x, T y, T z):
x_(x),
y_(y),
z_(z)
{

}


////////////////////////////////////////////////////////////
template <typename T>
template <typename U>
inline Vector3<T>::Vector3(const Vector3<U>& vector) :
x_(static_cast<T>(vector.x_)),
y_(static_cast<T>(vector.y_)),
z_(static_cast<T>(vector.z_))
{
}

////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator -(const Vector3<T>& left)
{
    return Vector3<T>(-left.x_, -left.y_, -left.z_);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T>& operator +=(Vector3<T>& left, const Vector3<T>& right)
{
    left.x_ += right.x_;
    left.y_ += right.y_;
    left.z_ += right.z_;
    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T>& operator -=(Vector3<T>& left, const Vector3<T>& right)
{
    left.x_ -= right.x_;
    left.y_ -= right.y_;
    left.z_ -= right.z_;
    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator +(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x + right.x, left.y + right.y, left.z + right.z);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator -(const Vector3<T>& left, const Vector3<T>& right)
{
    return Vector3<T>(left.x - right.x, left.y - right.y, left.z - right.z);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator *(const Vector3<T>& left, T right)
{
    return Vector3<T>(left.x * right, left.y * right, left.z * right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator *(T left, const Vector3<T>& right)
{
    return Vector3<T>(right.x * left, right.y * left, right.z * left);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T>& operator *=(Vector3<T>& left, T right)
{
    left.x *= right;
    left.y *= right;
    left.z *= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T> operator /(const Vector3<T>& left, T right)
{
    return Vector3<T>(left.x / right, left.y / right, left.z / right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Vector3<T>& operator /=(Vector3<T>& left, T right)
{
    left.x /= right;
    left.y /= right;
    left.z /= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator ==(const Vector3<T>& left, const Vector3<T>& right)
{
    return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator !=(const Vector3<T>& left, const Vector3<T>& right)
{
    return (left.x != right.x) || (left.y != right.y) || (left.z != right.z);
}
