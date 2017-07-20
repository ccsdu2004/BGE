
//! ��ȡ��С��
template<class T>
inline T Math::min(const T& a, const T& b)
{
    return a > b ? b : a;
}

//! ��ȡ��С��
template<class T>
inline T Math::min(const T& a, const T& b, const T& c)
{
    return min(min(a,b),c);
}

//! ��ȡ�ϴ���
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

//! ��ȡ��ֵ��(0.0 <= t <= 1.0)
template<class T>
T  Math::lerp(const T& a, const T& b, const float t)
{
    return a + (b-a)*t;
}

//! ȡ����
template<class T>
inline T Math::sign(const T& value)
{
    return value < T(0) ? T(-1) : (value > T(0) ? T(1) : T(0));
}

//! ȡ����
template<class T>
inline T Math::cube(const T& value)
{
    return value*value*value;
}

///////////////////////////////////////////////////////
/// ���ݽ���
///////////////////////////////////////////////////////
template<class T>
inline void Math::swap(T &t1, T &t2)
{
    T  temp = t1;
    t1 = t2;
    t2 = temp;
}

///////////////////////////////////////////////////////
/// ��ȡ����ֵ
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
/// ��������t(0<=t<=1)��ȡ���Բ�ֵ��p(0) = p1,p(1) = p2
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
/// ��ȡ����ֵ�ļ���ֵ
////////////////////////////////////////////////////////
template<class T>
T  Math::truncate(const T& t, const T& target)
{
    return t > target ? t - target : t;
}












