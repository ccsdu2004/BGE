//! 2009.08.17
#ifndef BGE_SYSTEM_MATH_H
#define BGE_SYSTEM_MATH_H
#include <BGE/Config.h>
#include <cmath>
#include <limits>
#include <utility>
#include <ostream>
#if defined(_MSC_VER)
#undef min
#undef max 
#endif

namespace bge
{

template <typename T>
class Vector2
{
public :
    Vector2();
    Vector2(T x,T y);
    Vector2(const Vector2<T>& vector);

    T x()const{return x_;}
    T y()const{return y_;}

    T width()const{return x_;}
    T height()const{return y_;}

    void setX(T x){x_ = x;}
    void setY(T y){y_ = y;}

    T size()const{return x_*y_;}

    Vector2<T>& round()
	{
	    x_ = floorf(x_ + 0.5f);
	    y_ = floorf(y_ + 0.5f);
	    return *this;
	}

    T x_; ///< X coordinate of the vector
    T y_; ///< Y coordinate of the vector
};

template <typename T>
Vector2<T> operator -(const Vector2<T>& right);

template <typename T>
Vector2<T>& operator +=(Vector2<T>& left,const Vector2<T>& right);

template <typename T>
Vector2<T>& operator -=(Vector2<T>& left,const Vector2<T>& right);

template <typename T>
Vector2<T> operator +(const Vector2<T>& left,const Vector2<T>& right);

template <typename T>
Vector2<T> operator -(const Vector2<T>& left,const Vector2<T>& right);

template <typename T>
Vector2<T> operator *(const Vector2<T>& left,T right);

template <typename T>
Vector2<T> operator *(T left,const Vector2<T>& right);

template <typename T>
Vector2<T>& operator *=(Vector2<T>& left,T right);

template <typename T>
Vector2<T> operator /(const Vector2<T>& left,T right);

template <typename T>
Vector2<T>& operator /=(Vector2<T>& left,T right);

template <typename T>
bool operator ==(const Vector2<T>& left,const Vector2<T>& right);

template <typename T>
bool operator !=(const Vector2<T>& left, const Vector2<T>& right);

template<typename T>
std::ostream& operator << (std::ostream& os, const Vector2<T>& vector);

#include <BGE/System/priv/Vector2.inl>

typedef Vector2<int>      Vector2i;
typedef Vector2<int32_t> Vector2u;
typedef Vector2<float>    Vector2f;

template <typename T>
class Vector3
{
public :
    Vector3();
    Vector3(T x,T y,T z);
    template <typename U>
    Vector3(const Vector3<U>& vector);

    T x_; ///< X coordinate of the vector
    T y_; ///< Y coordinate of the vector
    T z_; ///< Z coordinate of the vector
};

template <typename T>
Vector3<T> operator -(const Vector3<T>& left);

template <typename T>
Vector3<T>& operator +=(Vector3<T>& left,const Vector3<T>& right);

template <typename T>
Vector3<T>& operator -=(Vector3<T>& left,const Vector3<T>& right);

template <typename T>
Vector3<T> operator +(const Vector3<T>& left,const Vector3<T>& right);

template <typename T>
Vector3<T> operator -(const Vector3<T>& left,const Vector3<T>& right);

template <typename T>
Vector3<T> operator *(const Vector3<T>& left,T right);

template <typename T>
Vector3<T> operator *(T left,const Vector3<T>& right);

template <typename T>
Vector3<T>& operator *=(Vector3<T>& left,T right);

template <typename T>
Vector3<T> operator /(const Vector3<T>& left,T right);

template <typename T>
Vector3<T>& operator /=(Vector3<T>& left,T right);

template <typename T>
bool operator ==(const Vector3<T>& left,const Vector3<T>& right);

template <typename T>
bool operator !=(const Vector3<T>& left,const Vector3<T>& right);

template<typename T>
std::ostream& operator << (std::ostream& os,const Vector3<T>& vector);

#include <BGE/System/priv/Vector3.inl>

typedef Vector3<int>   Vector3i;
typedef Vector3<float> Vector3f;

template <typename T>
class Rect
{
public :
    Rect();
    Rect(T left,T top,T width,T height);
    Rect(const Vector2<T>& position, const Vector2<T>& size);
    Rect(const Rect<T>& rectangle);
public:
    bool isNull() const
    {
        return width_ == 0.0f && height_ == 0.0f;
    }
    bool isEmpty() const;
    bool isValid() const
    {
        return width_ >= 0.0f && height_ >= 0.0f;
    }
    Rect<T> normalized() const;

    bool contains(T x,T y) const;
    bool contains(const Vector2<T>& point) const;
    bool intersects(const Rect<T>& rectangle)const;
    bool intersects(const Rect<T>& rectangle, Rect<T>& intersection) const;

    void setSize(const Vector2<T>& size)
    {
        width_  = size.x_;
        height_ = size.y_;
    }

    void setPosition(const Vector2<T>& position)
    {
        left_ = position.x_;
        top_  = position.y_;
    }

    Vector2<T> position() const {return Vector2<T>(left_,top_);}
    Vector2<T> size() const {return Vector2<T>(width_,height_);}

    T left()const{return left_;}
    T top()const{return top_;}
    T right()const{return left_+width_;}
    T bottom()const{return height_+top_;}

    Vector2<T> topRight()const
    {
        return Vector2<T>(left_+width_,top_);
    }

    Vector2<T> topLeft()const
    {
        return Vector2<T>(left_,top_);
    }

    Vector2<T> bottomRight()const
    {
        return Vector2<T>(left_+width_,top_+height_);
    }

    Vector2<T> bottomLeft()const
    {
        return Vector2<T>(left_,top_+height_);
    }

    Vector2<T> center()const
    {
        return Vector2<T>(left_+width_*0.5f,top_+height_*0.5f);
    }

    void translate(T dx,T dy)
    {
        left_ += dx;
        top_ += dy;
    }
    void translate(const Vector2<T>& p)
    {
        left_ += p.x();
        top_ += p.y();
    }

    void moveTo(T x,T y)
    {
        left_ = x;
        top_ = y;
    }
    void moveTo(const Vector2<T>& p)
    {
        left_ = p.x_;
        top_ = p.y_;
    }

	void expandBy(T amount)
	{
	    left_ -= amount;
	    top_ -= amount;
	    width_ += 2*amount;
	    height_ += 2*amount;
	}

	void expandWidthBy(T amount)
	{
	    left_ -= amount;
	    width_ += amount;
	}
public:
    T left_;   ///< Left coordinate of the rectangle
    T top_;    ///< Top coordinate of the rectangle
    T width_;  ///< Width of the rectangle
    T height_; ///< Height of the rectangle
};

template <typename T>
bool operator ==(const Rect<T>& left, const Rect<T>& right);

template <typename T>
bool operator !=(const Rect<T>& left, const Rect<T>& right);

#include <BGE/System/priv/Rect.inl>

typedef Rect<int>    IntRect;
typedef Rect<float>  FloatRect;

class BGE_EXPORT_API Line
{
public:
    static Line fromPolar(float length,float angle);
public:
    Line(){}
    Line(const Vector2f& pt1,const Vector2f& pt2):
        pt1_(pt1),
        pt2_(pt2)
    {
    }
    Line(float x1,float y1,float x2,float y2):
        pt1_(x1,y1),
        pt2_(x2,y2)
    {
    }
    Line(const Line &line):
        pt1_(line.p1()),
        pt2_(line.p2())
    {
    }

    bool isNull() const;

    Vector2f p1() const{return pt1_;}
    Vector2f p2() const{return pt2_;}

    float x1() const{return pt1_.x();}
    float y1() const{return pt1_.y();}

    float x2() const{return pt2_.x();}
    float y2() const{return pt2_.y();}

    float dx() const{return x2() - x1();}
    float dy() const{return y2() - y1();}

    float length() const;
    void setLength(float len)
    {
	    if(isNull())
	        return;
	    Line v = unitVector();
	    pt2_ = Vector2f(pt1_.x() + v.dx() * len, pt1_.y() + v.dy() * len);
    }

    float angle() const;
    void setAngle(float angle);

    float angleTo(const Line& other) const;

    Line normalVector() const
    {
        return Line(p1(), p1() + Vector2f(dy(), -dx()));
    }

    Line unitVector() const;

    float angle(const Line& other) const;

    Vector2f pointAt(float t) const
    {
	    float vx = pt2_.x() - pt1_.x();
	    float vy = pt2_.y() - pt1_.y();
	    return Vector2f(pt1_.x() + vx * t, pt1_.y() + vy * t);
    }
    void translate(const Vector2f& p)
    {
        pt1_ += p;
        pt2_ += p;
    }
    void translate(float dx,float dy)
    {
        translate(Vector2f(dx,dy));
    }

    Line translated(const Vector2f& p) const
    {
        return Line(pt1_ + p, pt2_ + p);
    }

    Line translated(float dx,float dy) const
    {
        return translated(Vector2f(dx,dy));
    }

    void setP1(const Vector2f& p1)
    {
        pt1_ = p1;
    }

    void setP2(const Vector2f& p2)
    {
        pt2_ = p2;
    }

    void setPoints(const Vector2f& p1,const Vector2f& p2)
    {
        pt1_ = p1;
        pt2_ = p2;
    }

    void setLine(float x1,float y1,float x2,float y2)
    {
        pt1_ = Vector2f(x1,y1);
        pt2_ = Vector2f(x2,y2);
    }

    bool operator==(const Line& d) const
    {
        return pt1_ == d.pt1_ && pt2_ == d.pt2_;
    }
    bool operator!=(const Line& d) const { return !(*this == d); }
public:
    /*template<typename Archive>
    void serialize(Archive& archive)
    {
        archive &TAGGED_OBJECT(pt1)
                &TAGGED_OBJECT(pt2);
    }*/
private:
    Vector2f pt1_;
    Vector2f pt2_;
};

class BGE_EXPORT_API Math
{
public:
	template<class T>
	inline static T sign(const T& value);

    template<class T>
    inline static void swap(T &t1,T &t2);

	template<class T>
	inline static T clamp(const T& value,const T& min,const T& max);

	template<class T>
	inline static T min(const T& a,const T& b);

	template<class T>
	inline static T min(const T& a, const T& b, const T& c);

	template<class T>
	inline static T max(const T& a, const T& b);

	template<class T>
	inline static T max(const T& a, const T& b, const T& c);

	template<class T>
	inline static T abs(T a);

    //! (0.0 <= t <= 1.0)
	template<class T>
	inline static T lerp(const T& a, const T& b, const float t);

	template<class T>
	inline static T cube(const T& value);

	template<class T>
	static bool equal(T a,T b){return abs<T>(a-b) < 1e-6;}

    static bool fuzzyCompare(float p1,float p2)
    {
        return (abs(p1 - p2) <= 0.0000000001f * min(abs(p1),abs(p2)));
    }

    template<class T>
	static Vector3<T> getPoint(const Vector3<T> &from,const Vector3<T> &to,float t);

    template<class T>
	static T  truncate(const T& t,const T& target);

	static float fabs(float x)
	{
		unsigned int data = ((unsigned int&)(x))&0x7fffffff;
		return (float&)data;
	}

    static Vector3f bezier(Vector3f* p,int n,float t);

    static void transform(const float pos[3],const float r[12],float matrix[16]);

    const static float  PI32;
    const static float  RECIPROCAL_PI32;
    const static float  DEGTORAD32;
    const static float  RADTODEG32;
};

#include <BGE/System/priv/Math.inl>

}

#endif








