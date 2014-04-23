/*
 * Map.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: nbarriga
 */


#include "Map.h"
#include "Spiral.h"

namespace SparCraft
{

Position Map::getWalkPosition(const Position & pixelPosition) const
{
	return Position(pixelPosition.x() / 8, pixelPosition.y() / 8);
}

void Map::resetVectors()
{
	_mapData =          bvv(_walkTileWidth,  std::vector<bool>(_walkTileHeight,  true));
	_unitDataBuildTile =         bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
	_buildingDataBuildTile =     bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
	_unitDataWalkTile =         bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
	_buildingDataWalkTile =     bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
	_distanceMaps.clear();
}

void Map::invalidateDistances(){
	_distanceMaps.clear();
}

void Map::calculateDistances(DistanceMap& dmap,int width, int height, int xGoal, int yGoal, int tileSize) const{
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

Map::Map()
: _walkTileWidth(0)
, _walkTileHeight(0)
, _buildTileWidth(0)
, _buildTileHeight(0)
{
}

// constructor which sets a completely walkable map
Map::Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY)
: _walkTileWidth(bottomRightBuildTileX * 4)
, _walkTileHeight(bottomRightBuildTileY * 4)
, _buildTileWidth(bottomRightBuildTileX)
, _buildTileHeight(bottomRightBuildTileY)
{
	resetVectors();
}

Map::Map(BWAPI::Game * game)
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

const size_t & Map::getWalkTileWidth() const
{
	return _walkTileWidth;
}

const size_t & Map::getWalkTileHeight() const
{
	return _walkTileHeight;
}

const size_t & Map::getBuildTileWidth() const
{
	return _buildTileWidth;
}

const size_t & Map::getBuildTileHeight() const
{
	return _buildTileHeight;
}



bool Map::canWalkStraight(Position from ,Position to, int range) const{
	Position wpFrom = getWalkPosition(from);
	Position wpTo = getWalkPosition(to);
	int xDiff,yDiff;
	do{
		if(!isWalkable(wpFrom.x(),wpFrom.y())||_buildingDataWalkTile[wpFrom.x()][wpFrom.y()]){
			return false;
		}
		xDiff=wpTo.x()-wpFrom.x();
		yDiff=wpTo.y()-wpFrom.y();
		if(abs(xDiff)>abs(yDiff)){
			wpFrom=wpFrom+Position((xDiff > 0) - (xDiff < 0),0);
		}else{
			wpFrom=wpFrom+Position(0,(yDiff > 0) - (yDiff < 0));
		}
	}while(xDiff*yDiff>range*range);
	return true;
}

std::pair<int,int> Map::getClosestLegal(int xGoal, int yGoal) const{
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

int Map::getDistance(const SparCraft::Position & fromPosition, const SparCraft::Position & toPosition) const{
	std::map<SparCraft::Position, DistanceMap>::const_iterator it=_distanceMaps.find(toPosition);
	if(it!=_distanceMaps.end()){
		return it->second[fromPosition];
	}else if((it=_distanceMaps.find(fromPosition))!=_distanceMaps.end()){
		return it->second[toPosition];
	}else{
		_distanceMaps.insert(std::pair<SparCraft::Position,DistanceMap>(toPosition,DistanceMap(getWalkTileWidth(),getWalkTileHeight(),8)));
		//			std::cout<<"calculating new distance to building at: "<<toPosition.x()<<" "<<toPosition.y()<<std::endl;
		calculateDistances(_distanceMaps[toPosition],_walkTileWidth,_walkTileHeight,toPosition.x()/8,toPosition.y()/8, 8);
		it=_distanceMaps.find(toPosition);
		if(it!=_distanceMaps.end()){
			//				std::cout<<"dist= "<<it->second[fromPosition]<<std::endl;
			return it->second[fromPosition];
		}else{
			System::FatalError("Couldn't find distance we just calculated");
			return -1;//to avoid warning
		}
	}
}

bool Map::doesCollide(const BWAPI::UnitType & type, const SparCraft::Position & pixelPosition) const{
	//todo: check against other units
	//todo: check the way, not just the end position

	int startX = floorDiv(pixelPosition.x() - type.dimensionLeft(),8);
	int endX   = ceilDiv(pixelPosition.x() + type.dimensionRight(),8);
	int startY = floorDiv(pixelPosition.y() - type.dimensionUp(),8);
	int endY   = ceilDiv(pixelPosition.y() + type.dimensionDown(),8);

    if(startX<0 || endX>(int)_walkTileWidth || startY<0 || endY>(int)_walkTileHeight){
        return true;
    }

	for (int x = startX; x < endX && x < (int)getWalkTileWidth(); ++x)
	{
		for (int y = startY; y < endY && y < (int)getWalkTileHeight(); ++y)
		{
			if(_buildingDataWalkTile[x][y] == true){
				return true;
			}
		}
	}
	return false;
}

bool Map::isWalkable(const SparCraft::Position & pixelPosition) const
{
	const Position & wp(getWalkPosition(pixelPosition));

	return	isWalkable(wp.x(), wp.y());
}

bool Map::isFlyable(const SparCraft::Position & pixelPosition) const
{
	const Position & wp(getWalkPosition(pixelPosition));

	return isFlyable(wp.x(), wp.y());
}

bool Map::isWalkable(const size_t & walkTileX, const size_t & walkTileY) const
{
	return	/*walkTileX >= 0 &&*/ walkTileX < getWalkTileWidth() &&
			/*walkTileY >= 0 &&*/ walkTileY < getWalkTileHeight() &&
			getMapData(walkTileX, walkTileY);
}

bool Map::isFlyable(const size_t & walkTileX, const size_t & walkTileY) const
{
	return	/*walkTileX >= 0 &&*/ walkTileX < getWalkTileWidth() &&
			/*walkTileY >= 0 &&*/ walkTileY < getWalkTileHeight();
}

bool Map::getMapData(const size_t & walkTileX, const size_t & walkTileY) const
{
	return _mapData[walkTileX][walkTileY];
}

bool Map::getUnitData(const size_t & buildTileX, const size_t & buildTileY) const
{
	return _unitDataBuildTile[buildTileX][buildTileY];
}

void Map::setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val)
{
	_mapData[walkTileX][walkTileY] = val;
}

void Map::setUnitData(BWAPI::Game * game)
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

bool Map::canBuildHere(BWAPI::TilePosition pos) const
{
    System::FatalError("Buggy function, if needed, reimplement");
    //need to check buildings against unit walk tiles not unit build tiles
	return pos.x()>=0 && pos.y()>=0 && pos.x()<(int)_buildTileWidth && pos.y()<(int)_buildTileHeight &&
			!_unitDataBuildTile[pos.x()][pos.y()] &&
			!_buildingDataBuildTile[pos.x()][pos.y()] &&
			_mapData[pos.x()*4][pos.y()*4];
}

bool Map::canBuildHere(const BWAPI::UnitType & type, const SparCraft::Position & pos) const
{
	if(!type.isBuilding()){
		System::FatalError("Map::canBuildHere(UnitType,Position) is only meant for building types.");
	}

	int startX = floorDiv((pos.x() - type.dimensionLeft()),TILE_SIZE);
	int endX   = ceilDiv((pos.x() + type.dimensionRight()),TILE_SIZE);
	int startY = floorDiv((pos.y() - type.dimensionUp()),TILE_SIZE);
	int endY   = ceilDiv((pos.y() + type.dimensionDown()),TILE_SIZE);

	if(startX<0 || endX>(int)_buildTileWidth || startY<0 || endY>(int)_buildTileHeight){
	    return false;
	}

	for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
	{
	    for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
	    {
	        if(_buildingDataBuildTile[x][y]){
	            return false;
	        }
	    }
	}

	startX = floorDiv((pos.x() - type.dimensionLeft()),8);
	endX   = ceilDiv((pos.x() + type.dimensionRight()),8);
	startY = floorDiv((pos.y() - type.dimensionUp()),8);
	endY   = ceilDiv((pos.y() + type.dimensionDown()),8);

	for (int x = startX; x < endX && x < (int)getWalkTileWidth(); ++x)
	{
	    for (int y = startY; y < endY && y < (int)getWalkTileHeight(); ++y)
	    {
	        if(_unitDataWalkTile[x][y] || !_mapData[x][y]){
	            return false;
	        }
	    }
	}
	return true;
}

void Map::setBuildingData(BWAPI::Game * game)
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

void Map::addUnit(BWAPI::Unit * unit)
{
	SparCraft::Position pos(unit->getPosition().x(),unit->getPosition().y());
	SparCraft::Unit spunit(unit->getType(),unit->getPlayer()->getID(),pos);
	addUnit(spunit);
}

void Map::addUnit(const SparCraft::Unit & unit)
{
	int startX = floorDiv(unit.position().x() - unit.type().dimensionLeft(), TILE_SIZE);
	int endX   = ceilDiv(unit.position().x() + unit.type().dimensionRight(), TILE_SIZE); // Division - round up
	int startY = floorDiv(unit.position().y() - unit.type().dimensionUp(), TILE_SIZE);
	int endY   = ceilDiv(unit.position().y() + unit.type().dimensionDown(),TILE_SIZE);

	if(_buildTileWidth==0||_buildTileHeight==0){
	    //map has not been set yet, accept everything
	    return;
	}
	if(startX<0 || endX>(int)_buildTileWidth || startY<0 || endY>(int)_buildTileHeight){
	    std::stringstream ss;
	    ss<<"Trying to place a "<<unit.type().getName()<<" outside the map at pos: X:"<<startX<<"-"<<endX<<" Y:"<<startY<<"-"<<endY;
	    ss<<std::endl<<"Map size: "<<_buildTileWidth<<"X"<<_buildTileHeight;
	    System::FatalError(ss.str());
	}

	for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
	{
		for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
		{
			if (unit.type().isBuilding()){
			    if(_buildingDataBuildTile[x][y]){
					std::cerr<<"\nwrong place:"<<x<<" "<<y<<" build tile."<<std::endl;
					System::FatalError("Trying to place a "+unit.type().getName()+
							" in an already occupied build tile");
				}
				_buildingDataBuildTile[x][y] = true;
				invalidateDistances();
			}else{
				_unitDataBuildTile[x][y] = true;
			}
		}
	}

	startX = floorDiv(unit.position().x() - unit.type().dimensionLeft(), 8);
	endX   = ceilDiv(unit.position().x() + unit.type().dimensionRight(), 8); // Division - round up
	startY = floorDiv(unit.position().y() - unit.type().dimensionUp(), 8);
	endY   = ceilDiv(unit.position().y() + unit.type().dimensionDown(), 8);
	for (int x = startX; x < endX && x < (int)getWalkTileWidth(); ++x)
	{
		for (int y = startY; y < endY && y < (int)getWalkTileHeight(); ++y)
		{
			if (unit.type().isBuilding()){
			    if(_unitDataWalkTile[x][y] || !_mapData[x][y]){
			        std::cerr<<"\nwrong place:"<<x<<" "<<y<<" walk tile."<<std::endl;
			        System::FatalError("Trying to place a "+unit.type().getName()+
			                " in an already occupied walk tile");
			    }
				//won't check _buildingDataWalkTile because we already did
				_buildingDataWalkTile[x][y] = true;
				invalidateDistances();
			}else{
				_unitDataWalkTile[x][y] = true;
			}
		}
	}
}

void Map::addUnitClosestLegalPos(SparCraft::Unit& unit) {
    if(unit.type().isBuilding()){
        Spiral sp(unit.x(),unit.y(),TILE_SIZE);
        Position newPos;

        do{
            newPos=sp.getNext();
        }while(!canBuildHere(unit.type(),newPos));
        if(!(newPos==unit.pos())){
            std::cout<<"Adding building "<<unit.type().getName()<<" in legal pos "<<newPos<<" (from "<<unit.pos()<<")"<<std::endl;
            unit.resetPosition(newPos);
        }
        addUnit(unit);
    }else{
        Spiral sp(unit.x(),unit.y(),8);
        Position newPos;
        do{
            newPos=sp.getNext();
        }while(doesCollide(unit.type(),newPos));
        if(!(newPos==unit.pos())){
            std::cout<<"Adding unit "<<unit.type().getName()<<" in legal pos "<<newPos<<" (from "<<unit.pos()<<")"<<std::endl;
            unit.resetPosition(newPos);
        }
        addUnit(unit);
    }
}

void Map::removeUnit(const SparCraft::Unit & unit)
{
	int startX = floorDiv(unit.position().x() - unit.type().dimensionLeft(), TILE_SIZE);
	int endX   = ceilDiv(unit.position().x() + unit.type().dimensionRight() ,TILE_SIZE); // Division - round up
	int startY = floorDiv(unit.position().y() - unit.type().dimensionUp(), TILE_SIZE);
	int endY   = ceilDiv(unit.position().y() + unit.type().dimensionDown(), TILE_SIZE);
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

	startX = floorDiv(unit.position().x() - unit.type().dimensionLeft(),8);
	endX   = ceilDiv(unit.position().x() + unit.type().dimensionRight(), 8); // Division - round up
	startY = floorDiv(unit.position().y() - unit.type().dimensionUp(), 8);
	endY   = ceilDiv(unit.position().y() + unit.type().dimensionDown(), 8);
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

void Map::clearAllUnits(){
    _unitDataBuildTile =         bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
    _buildingDataBuildTile =     bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
    _unitDataWalkTile =         bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
    _buildingDataWalkTile =     bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, false));
    _distanceMaps.clear();
}

unsigned int * Map::getRGBATexture() const
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

void Map::write(const std::string & filename) const
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

void Map::load(const std::string & filename)
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


int Map::ceilDiv(int a, int b) {
    return std::ceil(a/(float)b);
}

int Map::floorDiv(int a, int b) {
    return std::floor(a/(float)b);
}

}

