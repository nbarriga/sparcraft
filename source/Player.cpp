#include "Player.h"

using namespace SparCraft;

 IDType Player::ID() 
{ 
	return _playerID; 
}

void Player::setID(const IDType & playerID)
{
	_playerID = playerID;
}
