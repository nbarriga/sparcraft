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
	state.setMap(*_map);
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
	int score = game.getState().evalBuildingPlacement(Players::Player_One,Players::Player_Two);
	std::cout<<"score: "<<score<<std::endl;
	return score;

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
	Mutator(genome,0.5,20);
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

int GeneticOperators::Mutator(GAGenome& g, float pmut){
	return Mutator(g,pmut,2);
}

bool GeneticOperators::moveIfLegal(GAListGenome<Gene>& genome, int pos,
		BWAPI::TilePosition& offset) {


	BWAPI::TilePosition newPos=genome[pos]->getPos()+offset;
	if(_map->canBuildHere(newPos)){
		genome[pos]->move(offset);
		bool legal=true;
		for(int j=0; j<genome.size(); j++){
			if(pos!=j){
				if(genome[pos]->collides(*genome[j])){
					legal=false;
					break;
				}
			}
		}
		if(!legal){//undo
			genome[pos]->undo(offset);
		}
		return legal;
	}else{
		return false;
	}
}

int GeneticOperators::Mutator(GAGenome& g, float pmut, int maxJump)
{
	std::cout<<"calling Mutator\n";
	GAListGenome<Gene>& genome = (GAListGenome<Gene>&)g;


	if(pmut <= 0.0) return 0;


	int nMut = 0;

	for(int i=0; i<genome.size(); i++){
		if(GAFlipCoin(pmut)){
			Gene* gene=genome[i];
			BWAPI::TilePosition offset(GARandomInt(-maxJump,maxJump),GARandomInt(-maxJump,maxJump));
			if(moveIfLegal(genome,i,offset)){
				std::cout<<"mutating"<<std::endl;
				nMut++;
			}
		}
	}
	if(nMut!=0) genome.swap(0,0);//_evaluated = gaFalse;
	return nMut;

}

void GeneticOperators::repair(GAListGenome<Gene>& genome, int pos) {
	int a=0,b=0,distance=0;//start with distance 0, to check if it is currently legal
	do{
		for(int a=0;a<=distance;a++){
			for(int b=0;b<=distance;b++){
				if(std::max(a,b)==distance){
					BWAPI::TilePosition offset(a,b);
					if(moveIfLegal(genome,pos,offset)){
						if(distance>0){
							std::cout<<"repaired\n";
						}
						return;
					}else{
						distance++;
					}
				}
			}
		}
	}while(true);
}

void GeneticOperators::repair(GAListGenome<Gene>& genome) {
	for(int i=0; i<genome.size(); i++){
		repair(genome,i);
	}
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
	if(children==2){
		GAListGenome<Gene>& c1 = *(GAListGenome<Gene>*)child1;
		GAListGenome<Gene>& c2 = *(GAListGenome<Gene>*)child2;
		for(int i=0; i<c1.size(); i++){
			if(GAFlipCoin(0.5)){
				Gene* temp1=c1[i];
				Gene* temp2=c2[i];
				temp1->move(temp2->getPos()-temp1->getPos());
				temp2->move(temp1->getPos()-temp2->getPos());
				std::cout<<"exchanging\n";
			}
		}
		repair(c1);
		repair(c2);
	}
	return children;
}

} /* namespace SparCraft */
