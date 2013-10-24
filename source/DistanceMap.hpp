#pragma once

#include <vector>
#include "BWAPI.h"

class DistanceMap
{
	int					rows,
						cols,
						startRow,
						startCol,
						_tileSize;

	std::vector<int>	dist;
	std::vector<char>	moveTo;

	int getIndex(const int row, const int col) const
	{
		return row * cols + col;
	}

	int getIndex(const BWAPI::Position & p) const
	{
		return getIndex(p.y() / _tileSize, p.x() / _tileSize);
	}

public:

	DistanceMap () 
		: dist(std::vector<int>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), -1))
		, moveTo(std::vector<char>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), 'X'))
		, rows(BWAPI::Broodwar->mapHeight()), cols(BWAPI::Broodwar->mapWidth()), startRow(-1), startCol(-1),
		_tileSize(32)
	{
		//BWAPI::Broodwar->printf("New Distance Map With Dimensions (%d, %d)", rows, cols);
	}

	DistanceMap (const size_t & width, const size_t & height, const size_t tileSize)
	: dist(std::vector<int>(width * height, -1))
	, moveTo(std::vector<char>(width * height, 'X'))
	, rows(height), cols(width), startRow(-1), startCol(-1),
	_tileSize(tileSize)
	{
		//BWAPI::Broodwar->printf("New Distance Map With Dimensions (%d, %d)", rows, cols);
	}

	int & operator [] (const int index)						{ return dist[index]; }
	const int & operator [] (const int index) const			{ return dist[index]; }
	int & operator [] (const BWAPI::Position & pos)			{ return dist[getIndex(pos.y() / _tileSize, pos.x() / _tileSize)]; }
	const int & operator [] (const BWAPI::Position & pos) const{ return dist[getIndex(pos.y() / _tileSize, pos.x() / _tileSize)]; }
	int & operator [] (const SparCraft::Position & pos)			{ return dist[getIndex(pos.y() / _tileSize, pos.x() / _tileSize)]; }
	const int & operator [] (const SparCraft::Position & pos) const{ return dist[getIndex(pos.y() / _tileSize, pos.x() / _tileSize)]; }
	void setMoveTo(const int index, const char val)			{ moveTo[index] = val; }
	void setDistance(const int index, const int val)
	{
		dist[index] = val;
		if(val<0){
			std::cerr<<"Warning, setting -1 distance on DistanceMap!";
		}
	}
	void setStartPosition(const int sr, const int sc)		{ startRow = sr; startCol = sc; }

	// reset the distance map
	void reset(const int & rows, const int & cols, const int tileSize)
	{
		this->rows = rows;
		this->cols = cols;
		dist = std::vector<int>(rows * cols, -1);
		moveTo = std::vector<char>(rows * cols, 'X');
		startRow = -1;
		startCol = -1;
		_tileSize=tileSize;
	}

	// reset the distance map
	void reset()
	{
		std::fill(dist.begin(), dist.end(), -1);
		std::fill(moveTo.begin(), moveTo.end(), 'X');
		startRow = -1;
		startCol = -1;
	}

	bool isConnected(const BWAPI::Position p) const
	{
		return dist[getIndex(p)] != -1;
	}

	// given a position, get the position we should move to to minimize distance
	BWAPI::Position getMoveTo(const BWAPI::Position p, const int lookAhead = 1) const
	{
		// the initial row an column
		int row = p.y() / _tileSize;
		int col = p.x() / _tileSize;
		
		// for each lookahead
		for (int i=0; i<lookAhead; ++i)
		{
			// get the index
			int index = getIndex(row,col);

			// adjust the row and column accordingly
			if (moveTo[index] == 'L')
			{
				col -= 1;
			} 
			else if (moveTo[index] == 'R')
			{
				col += 1;
			} 
			else if (moveTo[index] == 'U')
			{
				row -= 1;
			} 
			else
			{
				row += 1;
			}
		}

		// return the position
		return BWAPI::Position(col * _tileSize + _tileSize/2, row * _tileSize + _tileSize/2);
	}
};
