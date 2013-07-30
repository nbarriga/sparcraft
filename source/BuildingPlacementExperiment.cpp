#include "BuildingPlacementExperiment.h"

#include <ga/GASimpleGA.h>
#include <ga/GAListGenome.h>
#include "Gene.h"
#include "GeneticOperators.h"

namespace SparCraft {
BuildingPlacementExperiment::BuildingPlacementExperiment(const std::string & configFile):
							SearchExperiment(configFile),_display(NULL){
	// TODO Auto-generated constructor stub

}

BuildingPlacementExperiment::~BuildingPlacementExperiment(){
	// TODO Auto-generated destructor stub
}


void BuildingPlacementExperiment::runExperiment(){
	int width    = 10;
	int height   = 5;
	int popsize  = 10;
	int ngen     = 20;
	float pmut   = 0.1;
	float pcross = 0.9;
	GARandomSeed(time(NULL));

	// set the map file for all states
	for (size_t state(0); state < states.size(); ++state)
	{
		//todo: set actual goal from map
		map->setGoal(SparCraft::Position(1200,400));
		states[state].setMap(*map);
	}

#ifdef USING_VISUALIZATION_LIBRARIES
	_display = NULL;
	if (showDisplay)
	{
		_display = new Display(map ? map->getBuildTileWidth() : 40, map ? map->getBuildTileHeight() : 22);
		_display->SetImageDir(imageDir);
		_display->OnStart();
		_display->LoadMapTexture(map, 19);
	}
#endif
	// for each player one player
	for (size_t p1Player(0); p1Player < players[0].size(); p1Player++)
	{
		// for each player two player
		for (size_t p2Player(0); p2Player < players[1].size(); p2Player++)
		{
			// for each state we care about
			for (size_t state(0); state < states.size(); ++state)
			{
				char buf[255];
				fprintf(stderr, "%s  ", configFileSmall.c_str());
				fprintf(stderr, "%5d %5d %5d %5d", (int)p1Player, (int)p2Player, (int)state, (int)states[state].numUnits(Players::Player_One));


				resultsPlayers[0].push_back(p1Player);
				resultsPlayers[1].push_back(p2Player);
				resultsStateNumber[p1Player][p2Player].push_back(state);
				resultsNumUnits[p1Player][p2Player].push_back(states[state].numUnits(Players::Player_One));

				// get player one
				PlayerPtr playerOne(players[0][p1Player]);

				// give it a new transposition table if it's an alpha beta player
				Player_AlphaBeta * p1AB = dynamic_cast<Player_AlphaBeta *>(playerOne.get());
				if (p1AB)
				{
					p1AB->setTranspositionTable(TTPtr(new TranspositionTable()));
				}

				// get player two
				PlayerPtr playerTwo(players[1][p2Player]);
				Player_AlphaBeta * p2AB = dynamic_cast<Player_AlphaBeta *>(playerTwo.get());
				if (p2AB)
				{
					p2AB->setTranspositionTable(TTPtr(new TranspositionTable()));
				}

				// Now create the GA and run it.  First we create a genome of the type that
				// we want to use in the GA.  The ga doesn't operate on this genome in the
				// optimization - it just uses it to clone a population of genomes.

				BWAPI::TilePosition goal(BWAPI::Position(1200,400));
				std::vector<Unit> emptyList;
				GeneticOperators::configure(goal,
						emptyList,
						emptyList,
						emptyList,
						map,_display);

				GAListGenome<Gene> genome(GeneticOperators::Objective);

				// Now that we have the genome, we create the genetic algorithm and set
				// its parameters - number of generations, mutation probability, and crossover
				// probability.  And finally we tell it to evolve itself.
				genome.initializer(GeneticOperators::Initializer);
				genome.mutator(GeneticOperators::Mutator);





				GASimpleGA ga(genome);
				ga.populationSize(popsize);
				ga.nGenerations(ngen);
				ga.pMutation(pmut);
				ga.pCrossover(pcross);
				ga.evolve();

				// Now we print out the best genome that the GA found.

				std::cout << "The GA found:\n" << ga.statistics().bestIndividual() << "\n";

			}
		}
	}





}
}

