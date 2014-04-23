#pragma once

#include "Common.h"
#include <algorithm>
#include "MoveArray.h"
#include "Hash.h"
#include "Map.h"
#include "Unit.h"
#include "GraphViz.hpp"
#include "Array.hpp"

typedef boost::shared_ptr<SparCraft::Map> MapPtr;

namespace SparCraft
{
class GameState 
{
    Map                                                             _map;

    Array2D<Unit, Constants::Num_Players, Constants::Max_Units>     _units;             
    Array2D<int, Constants::Num_Players, Constants::Max_Units>      _unitIndex;        
    Array<Unit, 1>                                                  _neutralUnits;

    Array<UnitCountType, Constants::Num_Players>                    _numUnits;
    Array<UnitCountType, Constants::Num_Players>                    _prevNumUnits;

    Array<float, Constants::Num_Players>                            _totalLTD;
    Array<float, Constants::Num_Players>                            _totalSumSQRT;

    Array<int, Constants::Num_Players>                              _numMovements;
    Array<int, Constants::Num_Players>                              _prevHPSum;
	
    TimeType                                                        _currentTime;
    size_t                                                          _maxUnits;
    TimeType                                                        _sameHPFrames;

    // checks to see if the unit array is full before adding a unit to the state
    bool              checkFull(const IDType & player)                                        const;
    bool              checkUniqueUnitIDs()                                                    const;

    void                    performUnitAction(const UnitAction & theMove);

public:

    bool 					checkCollisions;
    GameState();
    GameState(const std::string & filename);

	// misc functions
    void                    finishedMoving();
    void					beforeMoving();
    void                    updateGameTime();
    bool              playerDead(const IDType & player)                                       const;
    bool              isTerminal()                                                            const;

    // unit data functions
     size_t            numUnits(const IDType & player)                                         const;
     size_t            prevNumUnits(const IDType & player)                                     const;
     size_t            numNeutralUnits()                                                       const;
     size_t            closestEnemyUnitDistance(const Unit & unit)                             const;

    // Unit functions
    void                    sortUnits();
    void                    addUnit(const Unit & u);
    void                    addUnitClosestLegalPos(const Unit & u);
    void                    addUnit(const BWAPI::UnitType unitType, const IDType playerID, const Position & pos);
    void                    addUnitWithID(const Unit & u);
    void                    addNeutralUnit(const Unit & unit);
    const Unit &            getUnit(const IDType & player, const UnitCountType & unitIndex)         const;
    const Unit &            getUnitByID(const IDType & unitID)                                      const;
          Unit &            getUnit(const IDType & player, const UnitCountType & unitIndex);
    const Unit &            getUnitByID(const IDType & player, const IDType & unitID)               const;
          Unit &            getUnitByID(const IDType & player, const IDType & unitID);
    boost::optional<const Unit&> getUnitByIDOpt(const IDType & player, const IDType & unitID) const;
    boost::optional<const Unit&> getUnitByIDOpt(const IDType & unitID) const;

    const Unit& getClosestEnemyUnit(const IDType & player, const IDType & unitIndex) const;
    const Unit& getClosestOurUnit(const IDType & player, const IDType & unitIndex) const;

    bool isPowered(const SparCraft::Position &pos, const IDType & player) const;
    const boost::optional<const Unit&> getClosestEnemyUnitOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestEnemyBuildingOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurUnitOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurBuildingOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurDamagedBuildingOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurWoundedUnitOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurPylonOpt(const IDType & player, const IDType & unitIndex) const;
    const boost::optional<const Unit&> getClosestOurPylonOpt(const IDType & player, const SparCraft::Position &pos) const;

    std::vector<IDType> getUnitIDs(const IDType & player) const;
    std::vector<IDType> getBuildingIDs(const IDType & player) const;
    std::vector<IDType> getAliveUnitIDs(const IDType & player) const;
    std::vector<IDType> getAliveBuildingIDs(const IDType & player) const;

    bool hasMobileAttackUnits(IDType player) const;
    bool hasDamageDealingUnits(IDType player) const;

    std::vector<IDType> getAliveUnitsInCircleIDs(IDType player, const Position &pos, int radius) const;


    const Unit &            getUnitDirect(const IDType & player, const IDType & unit)               const;
    const Unit &            getNeutralUnit(const size_t & u)                                        const;
    
    // game time functions
    void                    setTime(const TimeType & time);
     TimeType          getTime()                                                               const;

    // evaluation functions
    StateEvalScore    eval(   const IDType & player, const IDType & evalMethod,
                                    const IDType p1Script = PlayerModels::NOKDPS,
                                    const IDType p2Script = PlayerModels::NOKDPS)                   const;
     ScoreType         evalLTD(const IDType & player)                                        const;
     ScoreType         evalLTD2(const IDType & player)                                       const;
     ScoreType         LTD(const IDType & player)                                            const;
     ScoreType         LTD2(const IDType & player)                                           const;
     StateEvalScore    evalSim(const IDType & player, const IDType & p1, const IDType & p2)    const;
     IDType            getEnemy(const IDType & player)                                         const;

    // unit hitpoint calculations, needed for LTD2 evaluation
    void                    calculateStartingHealth();
    void                    setTotalLTD(const float & p1, const float & p2);
    void                    setTotalLTD2(const float & p1, const float & p2);
    const float &           getTotalLTD(const IDType & player)                                    const;
    const float &           getTotalLTD2(const IDType & player)                                   const;

    // move related functions
    void                    generateMoves(MoveArray & moves, const IDType & playerIndex)            const;
    void                    makeMoves(const std::vector<UnitAction> & moves);
    const int &             getNumMovements(const IDType & player)                                  const;
     IDType            whoCanMove()                                                            const;
    bool              bothCanMove()                                                           const;
		  
    // map-related functions
    void                    setMap(const Map & map);
    const Map &             		getMap() const;
    bool              isWalkable(const Position & pos)                                        const;
    bool              isFlyable(const Position & pos)                                         const;

    // hashing functions
     HashType          calculateHash(const size_t & hashNum)                                   const;

    // state i/o functions
    void                    print(int indent = 0) const;
    void                    write(const std::string & filename)                                     const;
    void                    read(const std::string & filename);
};

}

