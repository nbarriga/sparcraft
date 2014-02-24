/*
 * Position.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: nbarriga
 */


#include "Position.h"

namespace SparCraft
{


Position::Position()
: _x(0)
, _y(0)
{
}

Position::Position(const PositionType & x, const PositionType & y)
: _x(x)
, _y(y)
{
}

Position::Position(const BWAPI::Position & p)
: _x(p.x())
, _y(p.y())
{

}

bool Position::operator < (const Position & rhs) const
{
    return (x() < rhs.x()) || ((x() == rhs.x()) && y() < rhs.y());
}

bool Position::operator == (const Position & rhs) const
            {
    return x() == rhs.x() && y() == rhs.y();
            }

Position Position::operator + (const Position & rhs) const
{
    return Position(x() + rhs.x(), y() + rhs.y());
}

Position Position::operator - (const Position & rhs) const
{
    return Position(x() - rhs.x(), y() - rhs.y());
}

Position Position::scale(const float & f) const
{
    return Position((PositionType)(f * x()), (PositionType)(f * y()));
}

void Position::scalePosition(const float & f)
{
    _x = (PositionType)(f * _x);
    _y = (PositionType)(f * _y);
}

void Position::addPosition(const Position & rhs)
{
    _x += rhs.x();
    _y += rhs.y();
}

void Position::subtractPosition(const Position & rhs)
{
    _x -= rhs.x();
    _y -= rhs.y();
}

void Position::moveTo(const Position & pos)
{
    _x = pos.x();
    _y = pos.y();
}

void Position::addPosition(const PositionType & x, const PositionType & y)
{
    _x += x;
    _y += y;
}

void Position::moveTo(const PositionType & x, const PositionType & y)
{
    _x = x;
    _y = y;
}

PositionType Position::x() const
{
    return _x;
}

PositionType Position::y() const
{
    return _y;
}

Position Position::flipX() const
{
    return Position(-_x,_y);
}

Position Position::flipY() const
{
    return Position(_y,_x);
}

float Position::Q_rsqrt( float number ) const
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}

Position Position::flip() const
{
    return Position(-_x, -_y);
}

void Position::print() const
{
    printf("Position = (%d, %d)\n", _x, _y);
}

const std::string Position::getString() const
{
    std::stringstream ss;
    ss << "(" << x() << ", " << y() << ")";
    return ss.str();
}
std::ostream& operator<< (std::ostream &out, const Position &pos){
    out<<pos.getString();
    return out;
}


}
