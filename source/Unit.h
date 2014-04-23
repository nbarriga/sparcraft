#pragma once

#include "Common.h"

#include "UnitAction.hpp"
#include "Position.h"
#include "Hash.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "AnimationFrameData.h"
#include <iostream>

namespace SparCraft
{

class UnitAction;

class Unit 
{
    BWAPI::UnitType     _unitType;				// the BWAPI unit type that we are mimicing
    PositionType        _range;
	
	Position            _position;				// current location in a possibly infinite space
	
	IDType              _unitID;				// unique unit ID to the state it's contained in
    IDType              _playerID;				// the player who controls the unit
	
	HealthType          _currentHP;				// current HP of the unit
	HealthType          _currentEnergy;

	TimeType            _timeCanMove;			// time the unit can next move
	TimeType            _timeCanAttack;			// time the unit can next attack

	UnitAction          _previousAction;		// the previous action that the unit performed
	TimeType            _previousActionTime;	// the time the previous move was performed
	Position            _previousPosition;

    mutable TimeType    _prevCurrentPosTime;
    mutable Position    _prevCurrentPos;

public:

	Unit();
	//Unit(BWAPI::Unit * unit, BWAPI::Game * game, const IDType & playerID, const TimeType & gameTime);
	Unit(const BWAPI::UnitType unitType, const IDType & playerID, const Position & pos);
	Unit(const BWAPI::UnitType unitType, const Position & pos, const IDType & unitID, const IDType & playerID, 
		 const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta);

	bool operator < (const Unit & rhs) const;

    // action functions
	void                    setPreviousAction(const UnitAction & m, const TimeType & previousMoveTime);
	void                    updateAttackActionTime(const TimeType & newTime);
	void                    updateMoveActionTime(const TimeType & newTime);
	void                    attack(const UnitAction & move, const Unit & target, const TimeType & gameTime);
	void                    heal(const UnitAction & move, const Unit & target, const TimeType & gameTime);
	void                    move(const UnitAction & move, const TimeType & gameTime) ;
	void                    waitUntilAttack(const UnitAction & move, const TimeType & gameTime);
	void                    pass(const UnitAction & move, const TimeType & gameTime);
	void                    takeAttack(const Unit & attacker);
	void                    takeHeal(const Unit & healer);

	// conditional functions
	bool			    isMobile()                  const;
	bool			    isOrganic()                 const;
	bool              isAlive()                   const;
	bool			    canAttackNow()              const;
	bool			    canMoveNow()                const;
	bool			    canHealNow()                const;
	bool			    canKite()                   const;
	bool			    canHeal()                   const;
	bool              equalsID(const Unit & rhs)  const;
	bool              canAttackTarget(const Unit & unit, const TimeType & gameTime) const;
	bool              canHealTarget(const Unit & unit, const TimeType & gameTime) const;

    // id related
	void                    setUnitID(const IDType & id);
	 IDType		    ID()                        const;
	 IDType		    player()                    const;

    // position related functions
	const Position &        position()                  const;
	const Position &        pos()                       const;
	 PositionType      x()                         const;
	 PositionType      y()                         const;
	 PositionType      range()                     const;
	 PositionType      healRange()                 const;
	 PositionType      getDistanceSqToUnit(const Unit & u, const TimeType & gameTime) const;
	 PositionType      getDistanceSqToPosition(const Position & p, const TimeType & gameTime) const;
    const Position &        currentPosition(const TimeType & gameTime) const;
    void                    setPreviousPosition(const TimeType & gameTime);
    void      resetPosition(const Position & p);

    // health and damage related functions
	 HealthType        damage()                    const;
	 HealthType        healAmount()                const;
	 HealthType	    maxHP()                     const;
	 HealthType	    currentHP()                 const;
	 HealthType	    currentEnergy()             const;
	 HealthType	    maxEnergy()                 const;
	 HealthType	    healCost()                  const;
     HealthType        getArmor()                  const;
	 float			    dpf()                       const;
	void                    updateCurrentHP(const HealthType & newHP);
    const BWAPI::UnitSizeType getSize()                 const;
    const BWAPI::WeaponType getWeapon(BWAPI::UnitType target) const;
     HealthType        getDamageTo(const Unit & unit) const;
     PlayerWeapon      getWeapon(const Unit & target) const;

    // time and cooldown related functions
	 TimeType		    moveCooldown()              const;
	 TimeType		    attackCooldown()            const;
	 TimeType		    healCooldown()              const;
	 TimeType		    nextAttackActionTime()      const;
	 TimeType		    nextMoveActionTime()        const;
	 TimeType		    previousActionTime()        const;
	 TimeType		    firstTimeFree()             const;
	 TimeType 		    attackInitFrameTime()       const;
	 TimeType 		    attackRepeatFrameTime()     const;
	void                    setCooldown(TimeType attack, TimeType move);

    // other functions
	 int			    typeID()                    const;
	 double		    speed()                     const;
	const BWAPI::UnitType   type()                      const;
	const UnitAction &	    previousAction()            const;
	const std::string       name()                      const;
	void                    print()                     const;
	const std::string       debugString()               const;

	// hash functions
	 HashType          calculateHash(const size_t & hashNum, const TimeType & gameTime) const;
	void                    debugHash(const size_t & hashNum, const TimeType & gameTime) const;
};

class UnitPtrCompare
{
public:
	bool operator() (Unit * u1, Unit * u2) const
	{
		return *u1 < *u2;
	}
};
}
