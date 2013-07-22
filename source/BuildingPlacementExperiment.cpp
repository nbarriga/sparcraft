#include "BuildingPlacementExperiment.h"

namespace SparCraft {

BuildingPlacementExperiment::BuildingPlacementExperiment(const std::string & configFile):
			SearchExperiment(configFile){
	// TODO Auto-generated constructor stub

}

BuildingPlacementExperiment::~BuildingPlacementExperiment(){
	// TODO Auto-generated destructor stub
}

void BuildingPlacementExperiment::initialize() {

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

}

void BuildingPlacementExperiment::iterate() {
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

				// construct the game
				states[state].checkCollisions = checkCollisions;
				Game g(states[state], playerOne, playerTwo, 20000);
#ifdef USING_VISUALIZATION_LIBRARIES
				if (showDisplay)
				{
					g.disp = _display;
					_display->SetExpDesc(getExpDescription(p1Player, p2Player, state));
				}
#endif

				// play the game to the end
				g.play();

				ScoreType gameEval = g.getState().eval(Players::Player_One, SparCraft::EvaluationMethods::LTD2).val();

				numGames[p1Player][p2Player]++;
				if (gameEval > 0)
				{
					numWins[p1Player][p2Player]++;
				}
				else if (gameEval < 0)
				{
					numLosses[p1Player][p2Player]++;
				}
				else if (gameEval == 0)
				{
					numDraws[p1Player][p2Player]++;
				}

				double ms = g.getTime();
				fprintf(stderr, "%12d %12.2lf\n", gameEval, ms);

				resultsEval[p1Player][p2Player].push_back(gameEval);
				resultsRounds[p1Player][p2Player].push_back(g.getRounds());
				resultsTime[p1Player][p2Player].push_back(ms);


			}
		}
	}
}

void BuildingPlacementExperiment::runExperiment(){
	initialize();

	bool done=true;
	do{
		//set state

		//evaluate
		iterate();
	}while(!done);

}
}

