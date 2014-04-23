/*
 * Spiral.h
 *
 *  Created on: Apr 23, 2014
 *      Author: nbarriga
 */

#ifndef SPIRAL_H_
#define SPIRAL_H_
#include "Position.h"

namespace SparCraft {

class Spiral{
protected:
    unsigned int _layer;
    unsigned int _leg;
    unsigned int _tileSize;
    int _x, _y, _dx, _dy; //read these as output from next, do not modify.

public:
    Spiral(int x, int y, int tileSize=0);
    Position getNext();
};

} /* namespace SparCraft */

#endif /* SPIRAL_H_ */
