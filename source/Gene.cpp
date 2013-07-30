/*
 * Gene.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#include "Gene.h"
#include "ga/garandom.h"

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

void Gene::mutate(){
	_pos+=BWAPI::TilePosition(GARandomInt(-2,2),GARandomInt(-2,2));
	_pos.makeValid();
}

std::ostream& operator<< (std::ostream& stream, const Gene& building){
	stream<<"Type: "<<building._type.getName()<<", "<<
			building._pos.x() << ", " << building._pos.y() << ")"<<
			std::endl;
	return stream;
}

bool operator== (Gene &b1, Gene &b2){
	return false;//todo:actually implement comparator
}

bool operator!= (Gene &b1, Gene &b2){
	return true;//todo:actually implement comparator
}

} /* namespace SparCraft */
