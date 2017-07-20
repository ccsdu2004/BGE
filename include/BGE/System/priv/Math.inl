
//! 获取较小者
template<class T>
inline T Math::min(const T& a, const T& b)
{
    return a > b ? b : a;
}

//! 获取较小者
template<class T>
inline T Math::min(const T& a, const T& b, const T& c)
{
    return min(min(a,b),c);
}

//! 获取较大者
template<class T>
inline T Math::max(const T& a, const T& b)
{
    return a > b ? a : b;
}

template<class T>
inline T Math::max(const T& a, const T& b, const T& c)
{
    return Max(Max(a,b),c);
}

//! 获取插值点(0.0 <= t <= 1.0)
template<class T>
T  Math::lerp(const T& a, const T& b, const float t)
{
    return a + (b-a)*t;
}

//! 取符号
template<class T>
inline T Math::sign(const T& value)
{
    return value < T(0) ? T(-1) : (value > T(0) ? T(1) : T(0));
}

//! 取立方
template<class T>
inline T Math::cube(const T& value)
{
    return value*value*value;
}

///////////////////////////////////////////////////////
/// 数据交换
///////////////////////////////////////////////////////
template<class T>
inline void Math::swap(T &t1, T &t2)
{
    T  temp = t1;
    t1 = t2;
    t2 = temp;
}

///////////////////////////////////////////////////////
/// 求取绝对值
///////////////////////////////////////////////////////
template<class T>
inline T Math::abs(T value)
{
    return value > T(0) ? value:-value;
}

template<class T>
inline T  Math::clamp(const T &value, const T &min, const T &max)
{
    if(value<min)
	    return min;
	if(value>max)
	    return max;
	return value;
}
/*
////////////////////////////////////////////////////////
/// 给定差量t(0<=t<=1)获取线性插值点p(0) = p1,p(1) = p2
////////////////////////////////////////////////////////
template<class T>
Vector3<T> Math::getPoint(const Vector3<T> &from, const Vector3<T> &to, float t)
{
    t = clamp<float>(t,0,1);
    if(from == to)
	    return from;
    else
    {
	    Vector3<T>  p(from);
	    T  length = p.Length(to);
	    return Vector3<T>(from.x+(to.x-from.x)*t,from.y+(to.y-from.y)*t,from.z+(to.z-from.z)*t);
	}
}
*/
////////////////////////////////////////////////////////
/// 获取给定值的剪贴值
////////////////////////////////////////////////////////
template<class T>
T  Math::truncate(const T& t, const T& target)
{
    return t > target ? t - target : t;
}












