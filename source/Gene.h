/*
 * Gene.h
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#ifndef GENE_H_
#define GENE_H_

#include "Common.h"
#include "Position.hpp"
#include "Unit.h"

namespace SparCraft {

class Gene{
public:
	BWAPI::UnitType     _type;
	BWAPI::TilePosition            _pos;

	Gene(SparCraft::Unit building);
//	Gene();
	void mutate();

	friend std::ostream& operator<< (std::ostream& stream, const Gene& building);
	friend bool operator== (Gene &b1, Gene &b2);
	friend bool operator!= (Gene &b1, Gene &b2);


};
} /* namespace SparCraft */
#endif /* GENE_H_ */
