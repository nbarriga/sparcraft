#pragma once

#include "Common.h"
#include <ctime>
#include <boost/array.hpp>
#include "Random.hpp"

namespace SparCraft
{
namespace Hash
{
	typedef std::vector<HashType> HashVec;

	class HashValues
	{
		HashType	unitPositionHash[Constants::Num_Players];
		HashType	timeCanAttackHash[Constants::Num_Players];
		HashType	timeCanMoveHash[Constants::Num_Players];
		HashType	unitTypeHash[Constants::Num_Players];
		HashType	currentHPHash[Constants::Num_Players];

	public:

		HashValues(int seed = 0);
		
		 HashType getAttackHash		(const size_t & player, const size_t & value) const;
		 HashType getMoveHash			(const size_t & player, const size_t & value) const;
		 HashType getUnitTypeHash		(const size_t & player, const size_t & value) const;
		 HashType getCurrentHPHash		(const size_t & player, const size_t & value) const;
		 HashType positionHash			(const IDType & player, const PositionType & x, const PositionType & y) const;
	};

	// some data storage
	extern HashType			unitIndexHash[Constants::Num_Players][Constants::Max_Units];
	extern HashValues		values[Constants::Num_Hashes];
	
	// good hashing functions
	void			initHash();
	int				hash32shift(int key);
	 size_t	jenkinsHash( size_t a);
	 size_t	magicHash(const HashType & hash, const size_t & player, const size_t & index);

	 int jenkinsHashCombine(const HashType & hash, const int val);
};
}