#pragma once
#include "GameObject.h"
class MoveGameObject :
	public GameObject
{
public:
	MoveGameObject();
	virtual ~MoveGameObject();
	void setMoveSpeed(float speed);
	void setMoving(bool moving);
	float getMoveSpeed();
	bool isMoving();

protected:
	float _moveSpeed;
	bool _isMoving;
};

