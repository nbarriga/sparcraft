/*
 * GeneticOperators.h
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#ifndef GENETICOPERATORS_H_
#define GENETICOPERATORS_H_

#include <ga/GAListGenome.h>
#include "Common.h"
#include "SparCraft.h"
namespace SparCraft {

class GeneticOperators {
public:
	static BWAPI::TilePosition _basePos;
	static std::vector<SparCraft::Unit> _buildings;
	static std::vector<SparCraft::Unit> _defenders;
	static std::vector<SparCraft::Unit> _attackers;
	static Map* _map;
	static Display* _display;
	static svv _expDesc;
	static void configure( BWAPI::TilePosition& basePos,
			 std::vector<SparCraft::Unit>& buildings,
			 std::vector<SparCraft::Unit>& defenders,
			 std::vector<SparCraft::Unit>& attackers,
			 Map* map,
			 Display* display,
			 svv expDesc);
	static float Objective(GAGenome &g);
	static void	Initializer(GAGenome& g);
	static int Mutator(GAGenome& g, float pmut);
	static int Crossover(const GAGenome&, const GAGenome&,
		      GAGenome*, GAGenome*);
};

} /* namespace SparCraft */
#endif /* GENETICOPERATORS_H_ */
