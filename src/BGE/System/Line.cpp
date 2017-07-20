#include <BGE/System/Math.h>
#include <cmath>

namespace bge
{

bool Line::isNull() const
{
    return (Math::fuzzyCompare(pt1_.x(), pt2_.x()) &&
            Math::fuzzyCompare(pt1_.y(), pt2_.y())) ? true : false;
}

float Line::length() const
{
    float x = pt2_.x() - pt1_.x();
    float y = pt2_.y() - pt1_.y();
    return sqrt(x*x + y*y);
}

float Line::angle() const
{
    const float dx = pt2_.x() - pt1_.x();
    const float dy = pt2_.y() - pt1_.y();

    const float theta = Math::RADTODEG32*atan2f(-dy, dx);
    const float theta_normalized = theta < 0 ? theta + 360 : theta;

    if(Math::fuzzyCompare(theta_normalized,360.0f))
        return .0f;
    else
        return theta_normalized;
}

void Line::setAngle(float angle)
{
    const float angleR = Math::DEGTORAD32*angle;
    const float l = length();

    const float dx = cos(angleR) * l;
    const float dy = -sin(angleR) * l;

    pt2_.x_ = pt1_.x() + dx;
    pt2_.y_ = pt1_.y() + dy;
}

Line Line::fromPolar(float length,float angle)
{
    const float angleR = Math::DEGTORAD32*angle;
    return Line(0, 0,cos(angleR) * length, -sin(angleR) * length);
}

Line Line::unitVector() const
{
    float x = pt2_.x() - pt1_.x();
    float y = pt2_.y() - pt1_.y();

    float len = sqrt(x*x + y*y);
    Line line(p1(),Vector2f(pt1_.x() + x/len, pt1_.y() + y/len));
    return line;
}

float Line::angleTo(const Line& other) const
{
    if(isNull() || other.isNull())
        return 0;

    const float a1 = angle();
    const float a2 = other.angle();

    const float delta = a2 - a1;
    const float delta_normalized = delta < 0 ? delta + 360 : delta;

    if(Math::fuzzyCompare(delta,float(360)))
        return 0;
    else
        return delta_normalized;
}

float Line::angle(const Line& l)const
{
    if (isNull() || l.isNull())
        return 0;
    float cos_line = (dx()*l.dx() + dy()*l.dy()) / (length()*l.length());
    float rad = 0;
    // only accept cos_line in the range [-1,1], if it is outside, use 0 (we return 0 rather than PI for those cases)
    if (cos_line >= float(-1.0) && cos_line <= 1.0f)
        rad = cos(cos_line);
    return Math::RADTODEG32*rad;
}

}
