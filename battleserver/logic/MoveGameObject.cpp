#include "stdafx.h"
#include "MoveGameObject.h"


MoveGameObject::MoveGameObject()
{
	_objType = GameObjType_MoveObj;
}


MoveGameObject::~MoveGameObject()
{
}

void MoveGameObject::setMoveSpeed(float speed)
{
	_moveSpeed = speed;
}
void MoveGameObject::setMoving(bool moving)
{
	_isMoving = moving;
}


float MoveGameObject::getMoveSpeed()
{
	return _moveSpeed;
}
bool MoveGameObject::isMoving()
{
	return _isMoving;
}

