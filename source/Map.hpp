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
	std::map<SparCraft::Position, DistanceMap> _distanceMaps;//distances from every walk tile to a given position

	const Position getWalkPosition(const Position & pixelPosition) const
	{
		return Position(pixelPosition.x() / 8, pixelPosition.y() / 8);
	}

    void resetVectors()
    {
        _mapData =          bvv(_walkTileWidth,  std::vector<bool>(_walkTileHeight,  true));
		_unitDataBuildTile =         bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_buildingDataBuildTile =     bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
		_unitDataWalkTile =         bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
		_buildingDataWalkTile =     bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
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

	std::pair<int,int> getClosestLegal(int xGoal, int yGoal){
		int distance=0;//start with distance 0, to check if it is currently legal
		do{
			for(int a=-distance;a<=distance;a++){
				for(int b=-distance;b<=distance;b++){
					if(std::max(std::abs(a),std::abs(b))==distance){
						if(isWalkable(xGoal+a,yGoal+b)&&!_buildingDataWalkTile[xGoal+a][yGoal+b]){
							return std::pair<int,int>(xGoal+a,yGoal+b);
						}
					}
				}
			}
			distance++;
		}while(distance<10);
		return std::pair<int,int>(-1,-1);
	}

	void calculateDistances(DistanceMap& dmap,int width, int height, int xGoal, int yGoal, int tileSize){
		std::pair<int,int> legalGoal=getClosestLegal(xGoal,yGoal);
		if(legalGoal.first!=-1 && legalGoal.second!=-1){
			xGoal=legalGoal.first;
			yGoal=legalGoal.second;
		}
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
					!_buildingDataWalkTile[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8]&&
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
					!_buildingDataWalkTile[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8] &&
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
					&& !_buildingDataWalkTile[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8] &&
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
					&& !_buildingDataWalkTile[(nextIndex%width)*tileSize/8][(nextIndex/width)*tileSize/8] &&
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
			std::cout<<"calculating new distance to building at: "<<toPosition.x()<<" "<<toPosition.y()<<std::endl;
			calculateDistances(_distanceMaps[toPosition],_walkTileWidth,_walkTileHeight,toPosition.x()/8,toPosition.y()/8, 8);
			it=_distanceMaps.find(toPosition);
			if(it!=_distanceMaps.end()){
				std::cout<<"dist= "<<it->second[fromPosition]<<std::endl;
				return it->second[fromPosition];
			}else{
				System::FatalError("Couldn't find distance we just calculated");
				return -1;//to avoid warning
			}
		}
	}

	const bool doesCollide(const BWAPI::UnitType & type, const SparCraft::Position & pixelPosition) const{
		//todo: check against other units
		//todo: check the way, not just the end position

		int startX = (pixelPosition.x() - type.dimensionLeft()) / 8;
		int endX   = (pixelPosition.x() + type.dimensionRight() + 8 - 1) / 8; // Division - round up
		int startY = (pixelPosition.y() - type.dimensionUp()) / 8;
		int endY   = (pixelPosition.y() + type.dimensionDown() + 8 - 1) / 8;

		for (int x = startX; x <= endX && x < (int)getWalkTileWidth(); ++x)
		{
			for (int y = startY; y <= endY && y < (int)getWalkTileHeight(); ++y)
			{
				if(_buildingDataWalkTile[x][y] == true){
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
		return _unitDataBuildTile[buildTileX][buildTileY];
	}

	void setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val)
	{
		_mapData[walkTileX][walkTileY] = val;
	}

	void setUnitData(BWAPI::Game * game)
	{
		_unitDataBuildTile = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

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
				!_unitDataBuildTile[pos.x()][pos.y()] &&
				!_buildingDataBuildTile[pos.x()][pos.y()] &&
				_mapData[pos.x()*4][pos.y()*4];
	}

	const bool canBuildHere(const BWAPI::UnitType & type, const SparCraft::Position & pos)
	{
		if(!type.isBuilding()){
			System::FatalError("Map::canBuildHere(UnitType,Position) is only meant for building types.");
		}

		int startX = (pos.x() - type.dimensionLeft()) / TILE_SIZE;
		int endX   = (pos.x() + type.dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
		int startY = (pos.y() - type.dimensionUp()) / TILE_SIZE;
		int endY   = (pos.y() + type.dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
		for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
		{
			for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
			{
				if(!canBuildHere(BWAPI::TilePosition(x,y))){
					return false;
				}
			}
		}
		return true;

//		int tx = pos.x() / TILE_SIZE;
//		int ty = pos.y() / TILE_SIZE;
//		int sx = type.tileWidth();
//		int sy = type.tileHeight();
//		for(int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
//		{
//			for(int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
//			{
//				if(!canBuildHere(BWAPI::TilePosition(x,y))){
//					return false;
//				}
//			}
//		}
//		return true;
	}

	void setBuildingData(BWAPI::Game * game)
	{
		_buildingDataBuildTile = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

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
		SparCraft::Position pos(unit->getPosition().x(),unit->getPosition().y());
		SparCraft::Unit spunit(unit->getType(),unit->getPlayer()->getID(),pos);
		addUnit(spunit);
	}

	void addUnit(const SparCraft::Unit & unit)
	{

		int startX = (unit.position().x() - unit.type().dimensionLeft()) / TILE_SIZE;
		int endX   = (unit.position().x() + unit.type().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
		int startY = (unit.position().y() - unit.type().dimensionUp()) / TILE_SIZE;
		int endY   = (unit.position().y() + unit.type().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
		for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
		{
			for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
			{
				if (unit.type().isBuilding()){
					if(!canBuildHere(BWAPI::TilePosition(x,y))){
						std::cerr<<"\nwrong place:"<<x<<" "<<y<<" "<<
								_buildingDataBuildTile[x][y]<<" "<<
								_unitDataBuildTile[x][y]<<" "<<
								_mapData[x*4][y*4]<<std::endl;
						System::FatalError("Trying to place a "+unit.type().getName()+
								" in an already occupied tile");
					}
					_buildingDataBuildTile[x][y] = true;
					invalidateDistances();
				}else{
					_unitDataBuildTile[x][y] = true;
				}
			}
		}

		startX = (unit.position().x() - unit.type().dimensionLeft()) / 8;
		endX   = (unit.position().x() + unit.type().dimensionRight() + 8 - 1) / 8; // Division - round up
		startY = (unit.position().y() - unit.type().dimensionUp()) / 8;
		endY   = (unit.position().y() + unit.type().dimensionDown() + 8 - 1) / 8;
		for (int x = startX; x < endX && x < (int)getWalkTileWidth(); ++x)
		{
			for (int y = startY; y < endY && y < (int)getWalkTileHeight(); ++y)
			{
				if (unit.type().isBuilding()){
					//won't check if it's a valid location because we already did above.
					_buildingDataWalkTile[x][y] = true;
					invalidateDistances();
				}else{
					_unitDataWalkTile[x][y] = true;
				}
			}
		}
	}

	void removeUnit(const SparCraft::Unit & unit)
	{
		int startX = (unit.position().x() - unit.type().dimensionLeft()) / TILE_SIZE;
		int endX   = (unit.position().x() + unit.type().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
		int startY = (unit.position().y() - unit.type().dimensionUp()) / TILE_SIZE;
		int endY   = (unit.position().y() + unit.type().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
		for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
		{
			for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
			{
				if (unit.type().isBuilding()){
					_buildingDataBuildTile[x][y] = false;
					invalidateDistances();
				}else{
					_unitDataBuildTile[x][y] = false;
				}
			}
		}

		startX = (unit.position().x() - unit.type().dimensionLeft()) / 8;
		endX   = (unit.position().x() + unit.type().dimensionRight() + 8 - 1) / 8; // Division - round up
		startY = (unit.position().y() - unit.type().dimensionUp()) / 8;
		endY   = (unit.position().y() + unit.type().dimensionDown() + 8 - 1) / 8;
		for (int x = startX; x < endX && x < (int)getWalkTileWidth(); ++x)
		{
			for (int y = startY; y < endY && y < (int)getWalkTileHeight(); ++y)
			{
				if (unit.type().isBuilding()){
					_buildingDataWalkTile[x][y] = false;
					invalidateDistances();
				}else{
					_unitDataWalkTile[x][y] = false;
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
