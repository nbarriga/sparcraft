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

	bvv						_unitData;	            // true if unit on build tile [x][y]
	bvv						_buildingData;          // true if building on build tile [x][y]

	SparCraft::Position 	_goal;
	bool					_hasGoal;				//true if a goal has been set
	std::map<SparCraft::Position, DistanceMap> _distanceMaps;//distances from every walk tile to a given position

	const Position getWalkPosition(const Position & pixelPosition) const
	{
		return Position(pixelPosition.x() / 8, pixelPosition.y() / 8);
	}

    void resetVectors()
    {
        _mapData =          bvv(_walkTileWidth,  std::vector<bool>(_walkTileHeight,  true));
		_unitData =         bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_buildingData =     bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_distanceMaps.clear();
		_hasGoal = false;
    }

public:

	Map() 
        : _walkTileWidth(0)
		, _walkTileHeight(0)
		, _buildTileWidth(0)
		, _buildTileHeight(0)
		, _hasGoal(false)
    {
    }

    // constructor which sets a completely walkable map
    Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY)
        : _walkTileWidth(bottomRightBuildTileX * 4)
		, _walkTileHeight(bottomRightBuildTileY * 4)
		, _buildTileWidth(bottomRightBuildTileX)
		, _buildTileHeight(bottomRightBuildTileY)
    {
        resetVectors();
    }

	Map(BWAPI::Game * game) 
        : _walkTileWidth(game->mapWidth() * 4)
		, _walkTileHeight(game->mapHeight() * 4)
		, _buildTileWidth(game->mapWidth())
		, _buildTileHeight(game->mapHeight())
	{
		resetVectors();

		for (size_t x(0); x<_walkTileWidth; ++x)
		{
			for (size_t y(0); y<_walkTileHeight; ++y)
			{
				setMapData(x, y, game->isWalkable(x, y));
			}
		}
	}

	const size_t & getWalkTileWidth() const
	{
		return _walkTileWidth;
	}

	const size_t & getWalkTileHeight() const
	{
		return _walkTileHeight;
	}

	const size_t & getBuildTileWidth() const
	{
		return _buildTileWidth;
	}

	const size_t & getBuildTileHeight() const
	{
		return _buildTileHeight;
	}

	void invalidateDistances(){
		_distanceMaps.clear();
	}

	void calculateDistances(DistanceMap& dmap,int width, int height, int xGoal, int yGoal, int tileSize){
		int fringeSize(1);
		int fringeIndex(0);

		// the size of the map
		int size = width*height;

		std::vector<int> fringe(size, 0);;
		fringe[0] = xGoal + yGoal*width;
//		std::cerr<<"fringe[0]: "<<fringe[0]<<"size "<<fringe.size()<<std::endl;

		// temporary variables used in search loop
		int currentIndex, nextIndex;
		int newDist;


		// while we still have things left to expand
		while (fringeIndex < fringeSize)
		{
			// grab the current index to expand from the fringe
			currentIndex = fringe[fringeIndex++];
			newDist = dmap[currentIndex] + 1;

			// search up
			nextIndex = (currentIndex > width) ? (currentIndex - width) : -1;
			if ((nextIndex != -1) && dmap[nextIndex] == -1 &&
					!_buildingData[(nextIndex%width)*tileSize/TILE_SIZE][(nextIndex/width)*tileSize/TILE_SIZE]&&
					_mapData[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'D');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}

			// search down
			nextIndex = (currentIndex + width < size) ? (currentIndex + width) : -1;
			if ((nextIndex != -1) && dmap[nextIndex] == -1 &&
					!_buildingData[(nextIndex%width)*tileSize/TILE_SIZE][(nextIndex/width)*tileSize/TILE_SIZE] &&
					_mapData[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'U');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}

			// search left
			nextIndex = (currentIndex % width > 0) ? (currentIndex - 1) : -1;
			if ((nextIndex != -1) && dmap[nextIndex] == -1
					&& !_buildingData[(nextIndex%width)*tileSize/TILE_SIZE][(nextIndex/width)*tileSize/TILE_SIZE] &&
					_mapData[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'R');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}

			// search right
			nextIndex = (currentIndex % width < width - 1) ? (currentIndex + 1) : -1;
			if ((nextIndex != -1) && dmap[nextIndex] == -1
					&& !_buildingData[(nextIndex%width)*tileSize/TILE_SIZE][(nextIndex/width)*tileSize/TILE_SIZE] &&
					_mapData[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'L');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}
		}
	}

	void setGoal(const SparCraft::Position & goal){
		_goal=goal;
		_hasGoal=true;
		invalidateDistances();
	}

	const SparCraft::Position & getGoal() const{
		return _goal;
	}

	bool hasGoal() const{
		return _hasGoal;
	}

	const int getDistanceToGoal(const SparCraft::Position & pixelPosition){
		if(!hasGoal()){
			SparCraft::System::FatalError("Goal not set");
		}
		return getDistance(pixelPosition,_goal);
	}

	const int getDistance(const SparCraft::Position & fromPosition, const SparCraft::Position & toPosition){
		//todo: A*
		std::map<SparCraft::Position, DistanceMap>::const_iterator it=_distanceMaps.find(toPosition);
		if(it!=_distanceMaps.end()){
			return it->second[fromPosition];
		}else if((it=_distanceMaps.find(fromPosition))!=_distanceMaps.end()){
			return it->second[toPosition];
		}else{
			_distanceMaps.insert(std::pair<SparCraft::Position,DistanceMap>(toPosition,DistanceMap(getWalkTileWidth(),getWalkTileHeight(),8)));
			calculateDistances(_distanceMaps[toPosition],_walkTileWidth,_walkTileHeight,toPosition.x()/8,toPosition.y()/8, 8);
		}
	}

	const bool doesCollide(const BWAPI::UnitType & type, const SparCraft::Position & pixelPosition) const{
		//todo: check against other units
		//todo: check the way, not just the end position
		BWAPI::Position dest(pixelPosition.x(),pixelPosition.y());
		BWAPI::TilePosition tDest(dest);


		int startX = (dest.x() - type.dimensionLeft()) / TILE_SIZE;
		int endX   = (dest.x() + type.dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
		int startY = (dest.y() - type.dimensionUp()) / TILE_SIZE;
		int endY   = (dest.y() + type.dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;

		for (int x = startX; x <= endX && x < (int)getBuildTileWidth(); ++x)
		{
			for (int y = startY; y <= endY && y < (int)getBuildTileHeight(); ++y)
			{
				if(_buildingData[x][y] == true){
					return true;
				}
			}
		}
		return false;
	}

	const bool isWalkable(const SparCraft::Position & pixelPosition) const
	{
		const Position & wp(getWalkPosition(pixelPosition));

		return	isWalkable(wp.x(), wp.y());
	}
    
    const bool isFlyable(const SparCraft::Position & pixelPosition) const
	{
		const Position & wp(getWalkPosition(pixelPosition));

		return isFlyable(wp.x(), wp.y());
	}

	const bool isWalkable(const size_t & walkTileX, const size_t & walkTileY) const
	{
		return	walkTileX >= 0 && walkTileX < (PositionType)getWalkTileWidth() && 
				walkTileY >= 0 && walkTileY < (PositionType)getWalkTileHeight() &&
				getMapData(walkTileX, walkTileY);
	}

    const bool isFlyable(const size_t & walkTileX, const size_t & walkTileY) const
	{
		return	walkTileX >= 0 && walkTileX < (PositionType)getWalkTileWidth() && 
				walkTileY >= 0 && walkTileY < (PositionType)getWalkTileHeight();
	}

	const bool getMapData(const size_t & walkTileX, const size_t & walkTileY) const
	{
		return _mapData[walkTileX][walkTileY];
	}

	const bool getUnitData(const size_t & buildTileX, const size_t & buildTileY) const
	{
		return _unitData[buildTileX][buildTileY];
	}

	void setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val)
	{
		_mapData[walkTileX][walkTileY] = val;
	}

	void setUnitData(BWAPI::Game * game)
	{
		_unitData = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

		BOOST_FOREACH (BWAPI::Unit * unit, game->getAllUnits())
		{
			if (!unit->getType().isBuilding())
			{
				addUnit(unit);
			}
		}
	}

	const bool canBuildHere(BWAPI::TilePosition pos)
	{
		return pos.x()>=0 && pos.y()>=0 && pos.x()<_buildTileWidth && pos.y()<_buildTileHeight &&
				!_unitData[pos.x()][pos.y()] &&
				!_buildingData[pos.x()][pos.y()] &&
				_mapData[pos.x()*4][pos.y()*4];
	}

	const bool canBuildHere(const BWAPI::UnitType & type, const SparCraft::Position & pos)
	{
		if(!type.isBuilding()){
			System::FatalError("Map::canBuildHere(UnitType,Position) is only meant for building types.");
		}
		int tx = pos.x() / TILE_SIZE;
		int ty = pos.y() / TILE_SIZE;
		int sx = type.tileWidth();
		int sy = type.tileHeight();
		for(int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
		{
			for(int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
			{
				if(!canBuildHere(BWAPI::TilePosition(x,y))){
					return false;
				}
			}
		}
		return true;
	}

	void setBuildingData(BWAPI::Game * game)
	{
		_buildingData = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

		BOOST_FOREACH (BWAPI::Unit * unit, game->getAllUnits())
		{
			if (unit->getType().isBuilding())
			{
				addUnit(unit);
				invalidateDistances();
			}
		}
	}

	void addUnit(BWAPI::Unit * unit)
	{
		if (unit->getType().isBuilding())
		{
			int tx = unit->getPosition().x() / TILE_SIZE;
			int ty = unit->getPosition().y() / TILE_SIZE;
			int sx = unit->getType().tileWidth(); 
			int sy = unit->getType().tileHeight();
			for(int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
			{
				for(int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
				{
					if(!canBuildHere(BWAPI::TilePosition(x,y))){
						System::FatalError("Trying to place a "+unit->getType().getName()+
								" in an already occupied tile");
					}
					_buildingData[x][y] = true;
				}
			}
			invalidateDistances();
		}
		else
		{
			int startX = (unit->getPosition().x() - unit->getType().dimensionLeft()) / TILE_SIZE;
			int endX   = (unit->getPosition().x() + unit->getType().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
			int startY = (unit->getPosition().y() - unit->getType().dimensionUp()) / TILE_SIZE;
			int endY   = (unit->getPosition().y() + unit->getType().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
			for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
			{
				for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
				{
					_unitData[x][y] = true;
				}
			}
		}
	}

	void addUnit(const SparCraft::Unit & unit)
	{
		if (unit.type().isBuilding())
		{
			int tx = unit.pos().x() / TILE_SIZE;
			int ty = unit.pos().y() / TILE_SIZE;
			int sx = unit.type().tileWidth();
			int sy = unit.type().tileHeight();
			for(int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
			{
				for(int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
				{
					if(!canBuildHere(BWAPI::TilePosition(x,y))){
						std::cerr<<x<<" "<<y<<" "<<_buildingData[x][y]<<" "<<_unitData[x][y]<<" "<<_mapData[x*4][y*4]<<std::endl;
						System::FatalError("Trying to place a "+unit.type().getName()+
								" in an already occupied tile");
					}
					_buildingData[x][y] = true;
				}
			}
			invalidateDistances();
		}
		else
		{
			int startX = (unit.position().x() - unit.type().dimensionLeft()) / TILE_SIZE;
			int endX   = (unit.position().x() + unit.type().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
			int startY = (unit.position().y() - unit.type().dimensionUp()) / TILE_SIZE;
			int endY   = (unit.position().y() + unit.type().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
			for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
			{
				for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
				{
					_unitData[x][y] = true;
				}
			}
		}
	}

	void removeUnit(const SparCraft::Unit & unit)
	{
		if (unit.type().isBuilding())
		{
			int tx = unit.pos().x() / TILE_SIZE;
			int ty = unit.pos().y() / TILE_SIZE;
			int sx = unit.type().tileWidth();
			int sy = unit.type().tileHeight();
			for(int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
			{
				for(int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
				{
					_buildingData[x][y] = false;
				}
			}
			invalidateDistances();
		}
		else
		{
			int startX = (unit.position().x() - unit.type().dimensionLeft()) / TILE_SIZE;
			int endX   = (unit.position().x() + unit.type().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
			int startY = (unit.position().y() - unit.type().dimensionUp()) / TILE_SIZE;
			int endY   = (unit.position().y() + unit.type().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
			for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
			{
				for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
				{
					//todo: this won't work if more than one unit occupies the same build tile
					_unitData[x][y] = false;
				}
			}
		}
	}

	unsigned int * getRGBATexture()
	{
		unsigned int * data = new unsigned int[getWalkTileWidth() * getWalkTileHeight()];
		for (size_t x(0); x<getWalkTileWidth(); ++x)
		{
			for (size_t y(0); y<getWalkTileHeight(); ++y)
			{
				if (!isWalkable(x, y))
				{
					data[y*getWalkTileWidth() + x] = 0xffffffff;
				}
				else
				{
					data[y*getWalkTileWidth() + x] = 0x00000000;
				}
			}
		}

		return data;
	}

	void write(const std::string & filename)
	{
		std::ofstream fout(filename.c_str());
		fout << getWalkTileWidth() << "\n" << getWalkTileHeight() << "\n";

		for (size_t y(0); y<getWalkTileHeight(); ++y)
		{
			for (size_t x(0); x<getWalkTileWidth(); ++x)
			{
				fout << (isWalkable(x, y) ? 1 : 0);
			}

			fout << "\n";
		}

		fout.close();
	}

	void load(const std::string & filename)
	{
		std::ifstream fin(filename.c_str());
		if(!fin){
			std::stringstream ss;
			ss << "Can't open map file: " << filename;
			System::FatalError(ss.str());
		}
		std::string line;
		
		getline(fin, line);
		_walkTileWidth = atoi(line.c_str());

		getline(fin, line);
		_walkTileHeight = atoi(line.c_str());

        _buildTileWidth = _walkTileWidth/4;
        _buildTileHeight = _walkTileHeight/4;

		resetVectors();

		for (size_t y(0); y<getWalkTileHeight(); ++y)
		{
			getline(fin, line);

			for (size_t x(0); x<getWalkTileWidth(); ++x)
			{
				_mapData[x][y] = line[x] == '1' ? true : false;
			}
		}

		fin.close();
	}
};
}
