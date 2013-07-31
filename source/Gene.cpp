/*
 * Gene.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#include "Gene.h"
#include "ga/garandom.h"
#include "GeneticOperators.h"

namespace SparCraft {

Gene::Gene(SparCraft::Unit building):
				_type(building.type()),
				_pos(BWAPI::Position(building.pos().x(),building.pos().y())){
	if(!building.type().isBuilding()){
		System::FatalError("Only buildings are allowed");
	}
}

//Gene::Gene():
//				_type(BWAPI::UnitTypes::None),
//				_pos(BWAPI::TilePosition(GARandomInt(0,10),GARandomInt(0,10))){
//}

void Gene::move(BWAPI::TilePosition offset){
	_pos+=offset;
}

std::ostream& operator<< (std::ostream& stream, const Gene& building){
	stream<<"Type: "<<building._type.getName()<<", "<<
			building._pos.x() << ", " << building._pos.y() << ")"<<
			std::endl;
	return stream;
}

bool Gene::collides(const Gene& other) const{
	int otherLeft=other.getPos().x();
	int otherRight=otherLeft+other.getType().tileWidth();
	int otherTop=other.getPos().y();
	int otherBottom=otherTop+other.getType().tileHeight();

	int thisLeft=getPos().x();
		int thisRight=thisLeft+getType().tileWidth();
		int thisTop=getPos().y();
		int thisBottom=thisTop+getType().tileHeight();

		if(
				((thisLeft<otherRight&&thisLeft>otherLeft)||
						(thisRight>otherLeft&&thisRight<otherRight)
				)&&
				(
						(thisTop<otherBottom&&thisTop>otherTop)||
						(thisBottom>otherTop&&thisBottom<otherBottom)
				)){
			return true;
		}else{
			return false;
	}
}

bool operator== (Gene &b1, Gene &b2){
	return b1.getType()==b2.getType() && b1.getPos()==b2.getPos();
}

const BWAPI::UnitType Gene::getType() const {
	return _type;
}

const BWAPI::TilePosition Gene::getPos() const {
	return _pos;
}

bool operator!= (Gene &b1, Gene &b2){
	return !(b1==b2);
}

} /* namespace SparCraft */
