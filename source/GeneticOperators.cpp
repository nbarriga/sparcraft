/*
 * GeneticOperators.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: nbarriga
 */

#include "GeneticOperators.h"
#include "Gene.h"

namespace SparCraft {
BWAPI::TilePosition GeneticOperators::_basePos;
std::vector<SparCraft::Unit> GeneticOperators::_buildings=std::vector<SparCraft::Unit>();
std::vector<SparCraft::Unit> GeneticOperators::_defenders=std::vector<SparCraft::Unit>();
std::vector<SparCraft::Unit> GeneticOperators::_attackers=std::vector<SparCraft::Unit>();
Map* GeneticOperators::_map=NULL;
Display* GeneticOperators::_display=NULL;

float GeneticOperators::Objective(GAGenome &g) {
	std::cout<<"calling Objective\n";


	GAListGenome<Gene>& genome=(GAListGenome<Gene>&)g;

	GameState state;
	state.checkCollisions=true;

	for(Gene *gene=genome.head();
			gene!=genome.tail();
			gene=genome.next()){
		BWAPI::Position pos(gene->_pos);
		state.addUnit(gene->_type,1,SparCraft::Position(pos.x(),pos.y()));
	}
	for(std::vector<SparCraft::Unit>::const_iterator it=_defenders.begin();
			it!=_defenders.end();it++){
		assert(it->player()==1);
		state.addUnit(*it);
	}
	for(std::vector<SparCraft::Unit>::const_iterator it=_attackers.begin();
			it!=_attackers.end();it++){
		assert(it->player()==0);
		state.addUnit(*it);
	}

	state.setMap(*_map);

	PlayerPtr ptr1(new Player_Assault(0));
	PlayerPtr ptr2(new Player_Defend(1));
	Game game(state, ptr1, ptr2, 20000);
#ifdef USING_VISUALIZATION_LIBRARIES
	if (_display!=NULL)
	{
		game.disp = _display;
		//				_display->SetExpDesc(getExpDescription(0, 0, _state));
	}
#endif

	// play the game to the end
	game.play();
	return game.getState().eval(Players::Player_Two, SparCraft::EvaluationMethods::LTD2).val();

}

void GeneticOperators::Initializer(GAGenome& g)//todo: better initializer
{
	std::cout<<"calling Initializer\n";
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;
	while(genome.head()) genome.destroy(); // destroy any pre-existing list

	for(std::vector<SparCraft::Unit>::const_iterator it=_buildings.begin();
			it!=_buildings.end();it++){
		Gene gene(*it);
		gene.mutate();
		genome.insert(gene,GAListBASE::TAIL);
	}

}

void GeneticOperators::configure(BWAPI::TilePosition& basePos,
		std::vector<SparCraft::Unit>& buildings,
		std::vector<SparCraft::Unit>& defenders,
		std::vector<SparCraft::Unit>& attackers,
		Map* map,
		Display* display) {
	_basePos=basePos;
	_buildings=buildings;
	_defenders=defenders;
	_attackers=attackers;
	_map=map;
	_display=display;
}


int GeneticOperators::Mutator(GAGenome& g, float pmut)
{
	std::cout<<"calling Mutator\n";
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;


	if(pmut <= 0.0) return 0;


	int nMut = 0;

	for(int i=0; i<genome.size(); i++){
		if(GAFlipCoin(pmut)){
			Gene* gene=genome[i];
			std::cout<<"mutating"<<std::endl;
			gene->mutate();
			nMut++;
		}
	}

	return nMut;

}

} /* namespace SparCraft */
