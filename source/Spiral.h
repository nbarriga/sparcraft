/*
 * Spiral.h
 *
 *  Created on: Apr 23, 2014
 *      Author: nbarriga
 */

#ifndef SPIRAL_H_
#define SPIRAL_H_
#include "BWAPI/Position.h"

namespace SparCraft {

class Spiral{
protected:
    int _layer;
    int _leg;
    int _tileSize;
    int _x, _y, _dx, _dy; //read these as output from next, do not modify.

public:
    Spiral(int x, int y, int tileSize=0);
    BWAPI::Position getNext();
};

} /* namespace SparCraft */

#endif /* SPIRAL_H_ */
