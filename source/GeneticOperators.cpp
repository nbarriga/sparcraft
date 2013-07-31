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
svv GeneticOperators::_expDesc=svv();

float GeneticOperators::Objective(GAGenome &g) {
	std::cout<<"calling Objective\n";


	GAListGenome<Gene>& genome=(GAListGenome<Gene>&)g;
std::cout<<"genome: "<<genome<<std::endl;
	GameState state;
	state.checkCollisions=true;

	for(int i=0;i<genome.size();i++){
		Gene *gene=genome[i];
		BWAPI::Position pos(gene->getPos());
//		std::cout<<"building unit: "<<gene->getType().getName()<<std::endl;
		state.addUnit(gene->getType(),Players::Player_Two,SparCraft::Position(pos.x(),pos.y()));
	}
	for(std::vector<SparCraft::Unit>::const_iterator it=_defenders.begin();
			it!=_defenders.end();it++){
		assert(it->player()==Players::Player_Two);
//		std::cout<<"defender unit: "<<it->type().getName()<<std::endl;
		state.addUnit(*it);
	}
	for(std::vector<SparCraft::Unit>::const_iterator it=_attackers.begin();
			it!=_attackers.end();it++){
		assert(it->player()==Players::Player_One);
//		std::cout<<"attacker unit: "<<it->type().getName()<<std::endl;
		state.addUnit(*it);
	}

	state.setMap(*_map);

	PlayerPtr ptr1(new Player_Assault(Players::Player_One));
	PlayerPtr ptr2(new Player_Defend(Players::Player_Two));
	Game game(state, ptr1, ptr2, 20000);
#ifdef USING_VISUALIZATION_LIBRARIES
	if (_display!=NULL)
	{
		game.disp = _display;
		_display->SetExpDesc(_expDesc);

	}
#endif

	// play the game to the end
	game.play();
	int score = game.getState().eval(Players::Player_Two, SparCraft::EvaluationMethods::LTD2).val();
	std::cout<<"score: "<<score<<std::endl;
	return score+110000;

}

void GeneticOperators::Initializer(GAGenome& g)//todo: better initializer
{
	std::cout<<"calling Initializer\n";
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;
	while(genome.head()) genome.destroy(); // destroy any pre-existing list

	for(std::vector<SparCraft::Unit>::const_iterator it=_buildings.begin();
			it!=_buildings.end();it++){
		Gene gene(*it);

		genome.insert(gene,GAListBASE::TAIL);
	}
	Mutator(genome,0.5);
}

void GeneticOperators::configure(BWAPI::TilePosition& basePos,
		std::vector<SparCraft::Unit>& buildings,
		std::vector<SparCraft::Unit>& defenders,
		std::vector<SparCraft::Unit>& attackers,
		Map* map,
		Display* display,
		 svv expDesc) {
	_basePos=basePos;
	_buildings=buildings;
	_defenders=defenders;
	_attackers=attackers;
	_map=map;
	_display=display;
	_expDesc=expDesc;
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
			BWAPI::TilePosition offset(GARandomInt(-2,2),GARandomInt(-2,2));
			gene->move(offset);
			bool legal=true;
			for(int j=0; j<genome.size(); j++){
				if(i!=j){
					if(gene->collides(*genome[j])){
						legal=false;
					}
				}
			}
			if(!legal){
				gene->move(BWAPI::TilePosition(0,0)-offset);
			}else{
				std::cout<<"mutating"<<std::endl;
			}
			nMut++;
		}
	}
	if(nMut!=0) genome.swap(0,0);//_evaluated = gaFalse;
	return nMut;

}

int GeneticOperators::Crossover(const GAGenome& parent1, const GAGenome& parent2,
		GAGenome* child1, GAGenome* child2) {
	std::cout<<"calling Crossover\n";
	int children=0;
	if(child1!=NULL){
	child1->copy(parent1);
	children++;
	}
	if(child2!=NULL){
	child2->copy(parent2);
	children++;
	}
	std::cout<<children<<" children created"<<std::endl;
	return children;
}

} /* namespace SparCraft */
