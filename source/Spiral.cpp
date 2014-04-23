/*
 * Spiral.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: nbarriga
 */

#include "Spiral.h"

namespace SparCraft {

Spiral::Spiral(int x, int y, int tileSize):_layer(1),_leg(-1),_tileSize(tileSize),_x(x),_y(y),_dx(0),_dy(0){}

BWAPI::Position Spiral::getNext(){
    switch(_leg){
    case -1://get center first
        ++_leg;
        break;
    case 0:
        ++_dx;
        if(_dx  == _layer)
            ++_leg;
        break;
    case 1:
        ++_dy;
        if(_dy  == _layer)
            ++_leg;
        break;
    case 2:
        --_dx;
        if(-_dx == _layer)
            ++_leg;
        break;
    case 3:
        --_dy;
        if(-_dy == _layer){
            _leg = 0;
            ++_layer;
        }
        break;
    }
    return BWAPI::Position(_x+_dx*_tileSize,_y+_dy*_tileSize);
}

} /* namespace SparCraft */
