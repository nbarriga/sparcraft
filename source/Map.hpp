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
	DistanceMap				_distanceMapBuild;			// distances from every build tile to a goal tile
	DistanceMap				_distanceMapWalk;			// distances from every walk tile to a goal tile
	bool					_validDistances;		// true if distances are valid, false if buildings have changed

	const Position getWalkPosition(const Position & pixelPosition) const
	{
		return Position(pixelPosition.x() / 8, pixelPosition.y() / 8);
	}

    void resetVectors()
    {
        _mapData =          bvv(_walkTileWidth,  std::vector<bool>(_walkTileHeight,  true));
		_unitData =         bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_buildingData =     bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_distanceMapBuild.reset(_buildTileWidth,_buildTileHeight);
		_distanceMapWalk.reset(_walkTileWidth,_walkTileHeight);
		_validDistances = false;
    }

public:

	Map() 
        : _walkTileWidth(0)
		, _walkTileHeight(0)
		, _buildTileWidth(0)
		, _buildTileHeight(0)
		, _distanceMapBuild(0, 0)
		, _distanceMapWalk(0, 0)
		, _validDistances(false)
    {
    }

    // constructor which sets a completely walkable map
    Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY)
        : _walkTileWidth(bottomRightBuildTileX * 4)
		, _walkTileHeight(bottomRightBuildTileY * 4)
		, _buildTileWidth(bottomRightBuildTileX)
		, _buildTileHeight(bottomRightBuildTileY)
    	, _distanceMapBuild(bottomRightBuildTileX, bottomRightBuildTileY)
    	, _distanceMapWalk(bottomRightBuildTileX * 4, bottomRightBuildTileY * 4)
    {
        resetVectors();
    }

	Map(BWAPI::Game * game) 
        : _walkTileWidth(game->mapWidth() * 4)
		, _walkTileHeight(game->mapHeight() * 4)
		, _buildTileWidth(game->mapWidth())
		, _buildTileHeight(game->mapHeight())
		, _distanceMapBuild(game->mapWidth(), game->mapHeight())
		, _distanceMapWalk(game->mapWidth() * 4, game->mapHeight() * 4)
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
	
    const size_t getPixelWidth() const
    {
        return getWalkTileWidth() * 4;
    }

    const size_t getPixelHeight() const
    {
        return getWalkTileHeight() * 4;
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

	void calculateDistances(){
		if(!_validDistances){
//			SparCraft::System::printStackTrace(0);
			calculateDistances(_distanceMapBuild,_buildTileWidth,_buildTileHeight,_goal.x()/TILE_SIZE,_goal.y()/TILE_SIZE, 1);
			calculateDistances(_distanceMapWalk,_walkTileWidth,_walkTileHeight,_goal.x()/8,_goal.y()/8, 4);
		}
	}
	void calculateDistances(DistanceMap& dmap,int width, int height, int xGoal, int yGoal, int factor){
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
			if ((nextIndex != -1) && dmap[nextIndex] == -1
					&& !_buildingData[(nextIndex%width)/factor][(nextIndex/width)/factor])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'D');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}

			// search down
			nextIndex = (currentIndex + width < size) ? (currentIndex + width) : -1;
			if ((nextIndex != -1) && dmap[nextIndex] == -1
					&& !_buildingData[(nextIndex%width)/factor][(nextIndex/width)/factor])
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
					&& !_buildingData[(nextIndex%width)/factor][(nextIndex/width)/factor])
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
					&& !_buildingData[(nextIndex%width)/factor][(nextIndex/width)/factor])
			{
				// set the distance based on distance to current cell
				dmap.setDistance(nextIndex, newDist);
				dmap.setMoveTo(nextIndex, 'L');

				// put it in the fringe
				fringe[fringeSize++] = nextIndex;
			}
		}

		_validDistances=true;
//		for(int j=0;j<height;j++){
//			for(int i=0;i<width;i++){
//				std::cout<<dmap[i+width*j]<<" ";
//			}
//			std::cout<<std::endl;
//		}
//		std::cout<<std::endl;
	}

	void setGoal(const SparCraft::Position & goal){
		_goal=goal;
		_validDistances=false;
	}

	const int getDistanceToGoal(const SparCraft::Position & pixelPosition) const{
		if(!_validDistances){
			SparCraft::System::FatalError("Distances not updated on Map");
		}
		return _distanceMapWalk[pixelPosition.x()/8+pixelPosition.y()/8*_walkTileWidth];
//		return _distanceMapBuild[pixelPosition.x()/32+pixelPosition.y()/32*_buildTileWidth];
	}

	const int getDistance(const SparCraft::Position & fromPosition, const SparCraft::Position & toPosition) const{
		//todo: A*
		SparCraft::System::FatalError("Not yet Implemented");
	}

	const bool doesCollide(const SparCraft::Unit & unit, const SparCraft::Position & pixelPosition) const{
		//todo: check against other units
		//todo: check the way, not just the end position
		BWAPI::Position start(unit.pos().x(),unit.pos().y());
		BWAPI::Position dest(pixelPosition.x(),pixelPosition.y());
		BWAPI::TilePosition tStart(start);
		BWAPI::TilePosition tDest(dest);


		int startX = (dest.x() - unit.type().dimensionLeft()) / TILE_SIZE;
		int endX   = (dest.x() + unit.type().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
		int startY = (dest.y() - unit.type().dimensionUp()) / TILE_SIZE;
		int endY   = (dest.y() + unit.type().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;

//		std::cerr<<unit.type().dimensionLeft()<<" "<<unit.type().dimensionRight()
//				<<" "<<unit.type().dimensionUp()<<" "<<unit.type().dimensionDown()<<std::endl;
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
//		return _buildingData[tDest.x()][tDest.y()];
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
		return !_unitData[pos.x()][pos.y()] && !_buildingData[pos.x()][pos.y()];
	}

	void setBuildingData(BWAPI::Game * game)
	{
		_buildingData = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

		BOOST_FOREACH (BWAPI::Unit * unit, game->getAllUnits())
		{
			if (unit->getType().isBuilding())
			{
				addUnit(unit);
				_validDistances = false;
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
					_validDistances = false;
				}
			}
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
//		std::cerr<<"adding unit "<<unit.type().getName()<<std::endl;
//		System::printStackTrace();
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
						std::cerr<<x<<" "<<y<<" "<<_buildingData[x][y]<<std::endl;
						System::FatalError("Trying to place a "+unit.type().getName()+
								" in an already occupied tile");
					}
					_buildingData[x][y] = true;
					_validDistances = false;
				}
			}
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
					_validDistances = false;
				}
			}
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
