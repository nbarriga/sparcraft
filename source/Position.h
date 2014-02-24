#pragma once

#include "Common.h"
#include <math.h>
#include <sstream>

namespace SparCraft
{
class Position
{
    // x,y location will be used for Units in a 'grid'
    PositionType		_x,
    _y;

public:

    Position();
    Position(const PositionType & x, const PositionType & y);
    Position(const BWAPI::Position & p);

    bool operator < (const Position & rhs) const;
    bool operator == (const Position & rhs) const;
    Position operator + (const Position & rhs) const;
    Position operator - (const Position & rhs) const;
    Position scale(const float & f) const;

    void scalePosition(const float & f);
    void addPosition(const Position & rhs);
    void subtractPosition(const Position & rhs);
    void moveTo(const Position & pos);
    void addPosition(const PositionType & x, const PositionType & y);
    void moveTo(const PositionType & x, const PositionType & y);
    PositionType x() const;
    PositionType y() const;
    Position flipX() const;
    Position flipY() const;
    float Q_rsqrt( float number ) const;
    Position flip() const;

    inline  PositionType getDistance(const Position & p) const
    {
        PositionType dX = x() - p.x();
        PositionType dY = y() - p.y();

        if (dX == 0)
        {
            return abs(dY);
        }
        else if (dY == 0)
        {
            return abs(dX);
        }
        else
        {
            return (PositionType)sqrt((float)(dX*dX + dY*dY));
        }
    }

    inline  PositionType getDistanceSq(const Position & p) const
    {
        return (x()-p.x())*(x()-p.x()) + (y()-p.y())*(y()-p.y());
    }

    void print() const;
    const std::string getString() const;
    friend std::ostream& operator<< (std::ostream &out, const Position &pos);
};


}
