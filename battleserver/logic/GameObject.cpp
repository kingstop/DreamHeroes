#include "stdafx.h"
#include "GameObject.h"


GameObject::GameObject()
{
	_objType = GameObjType_Obj;
	_create();
	_s64Value = new s64[_s64ValueCount];	
}


GameObject::~GameObject()
{
	delete[] _s64Value;
	_s64ValueCount = 0;
}

void GameObject::_create()
{
	_s64ValueCount = (int)message::AObjMax;
}

s64 GameObject::GetS64Value(u32 index)
{
	if (index + 1 >= _s64ValueCount)
	{
		assert(0);
		return 0;
	}
	return _s64Value[index];
}
void GameObject::SetS64Value(u32 index, s64 value)
{
	if (index + 1 >= _s64ValueCount)
	{
		assert(0);
		return;
	}

	if (_s64Value[index] ==  value)
	{
		return;
	}
	_s64Value[index] = value;
	_s64UpdateMask.SetBit(index);
}


void GameObject::SetAliveState(message::AliveState t)
{
	SetS64Value(message::ALiveState, (s64)t);
}
message::AliveState GameObject::GetAliveState()
{
	message::AliveState t = (message::AliveState)GetS64Value(message::ALiveState);
	return t;	
}

void GameObject::SetFaction(s64 value)
{
	SetS64Value(message::AFaction, (s64)value);
}

s64 GameObject::GetFaction()
{
	return GetS64Value(message::AFaction);
}

u64 GameObject::GetID()
{
	return _id;
}
GameObjType GameObject::GetType()
{
	return _objType;
}
