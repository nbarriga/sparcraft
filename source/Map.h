#pragma once

#include "Common.h"
#include "Array.hpp"
#include "Unit.h"
#include "DistanceMap.hpp"

#include <boost/foreach.hpp>

namespace SparCraft
{

typedef std::vector< std::vector<bool> > bvv;

class Map
{
	size_t					_walkTileWidth;
	size_t					_walkTileHeight;
	size_t					_buildTileWidth;
	size_t					_buildTileHeight;
	bvv						_mapData;	            // true if walk tile [x][y] is walkable

	bvv						_unitDataBuildTile;	            // true if unit on build tile [x][y]
	bvv						_buildingDataBuildTile;          // true if building on build tile [x][y]

	bvv						_unitDataWalkTile;	            // true if unit on walk tile [x][y]
	bvv						_buildingDataWalkTile;          // true if building on walk tile [x][y]

	SparCraft::Position 	_goal;
	bool					_hasGoal;				//true if a goal has been set

	mutable std::map<SparCraft::Position, DistanceMap> _distanceMaps;//distances from every walk tile to a given position


	void resetVectors();
	void invalidateDistances();
	void calculateDistances(DistanceMap& dmap,int width, int height, int xGoal, int yGoal, int tileSize) const;

public:

	Map() ;
	// constructor which sets a completely walkable map
	Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY);
	Map(BWAPI::Game * game) ;

	const size_t & getWalkTileWidth() const;
	const size_t & getWalkTileHeight() const;
	const size_t & getBuildTileWidth() const;
	const size_t & getBuildTileHeight() const;
	Position getWalkPosition(const Position & pixelPosition) const;



	bool canWalkStraight(Position from ,Position to, int range) const;
	std::pair<int,int> getClosestLegal(int xGoal, int yGoal) const;
	bool doesCollide(const BWAPI::UnitType & type, const SparCraft::Position & pixelPosition) const;


	void setGoal(const SparCraft::Position & goal);
	const SparCraft::Position & getGoal() const;
	bool hasGoal() const;

	int getDistanceToGoal(const SparCraft::Position & pixelPosition) const;
	int getDistance(const SparCraft::Position & fromPosition, const SparCraft::Position & toPosition) const;


	bool isWalkable(const SparCraft::Position & pixelPosition) const;
	bool isFlyable(const SparCraft::Position & pixelPosition) const;
	bool isWalkable(const size_t & walkTileX, const size_t & walkTileY) const;
	bool isFlyable(const size_t & walkTileX, const size_t & walkTileY) const;

	bool getMapData(const size_t & walkTileX, const size_t & walkTileY) const;
	bool getUnitData(const size_t & buildTileX, const size_t & buildTileY) const;

	void setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val);
	void setUnitData(BWAPI::Game * game);

	bool canBuildHere(BWAPI::TilePosition pos) const;
	bool canBuildHere(const BWAPI::UnitType & type, const SparCraft::Position & pos) const;

	void setBuildingData(BWAPI::Game * game);

	void addUnit(BWAPI::Unit * unit);
	void addUnit(const SparCraft::Unit & unit);

	void removeUnit(const SparCraft::Unit & unit);
	void clearAllUnits();

	unsigned int * getRGBATexture() const;
	void write(const std::string & filename) const;
	void load(const std::string & filename);
};
}
