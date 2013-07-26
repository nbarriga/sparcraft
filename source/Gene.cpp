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
				_pos(building.pos()){
	if(!building.type().isBuilding()){
		System::FatalError("Only buildings are allowed");
	}
}

Gene::Gene():
				_type(BWAPI::UnitTypes::None),
				_pos(Position(GARandomInt(-10,10),GARandomInt(-10,10))){
}

void Gene::mutate(){
	_pos.addPosition(Position(GARandomInt(-10,10),GARandomInt(-10,10)));
}

std::ostream& operator<< (std::ostream& stream, const Gene& building){
	stream<<"Type: "<<building._type.getName()<<", "<<building._pos.getString()<<std::endl;
	return stream;
}

bool operator== (Gene &b1, Gene &b2){
	return false;//todo:actually implement comparator
}

bool operator!= (Gene &b1, Gene &b2){
	return true;//todo:actually implement comparator
}

} /* namespace SparCraft */
