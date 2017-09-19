#pragma once
#include "../LocationVector.h"
#include <tchar.h>
#include "common_type.h"
#include "message\battle.pb.h"
#include "../UpdateMask.h"
enum  GameObjType
{
	GameObjType_Obj,
	GameObjType_MoveObj,
	GameObjType_BattleUnit,
	GameObjType_BattleCreature
};
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	
	void SetAliveState(message::AliveState t);
	message::AliveState GetAliveState();
	void SetFaction(s64 value);
	s64 GetFaction();
	u64 GetID();
	GameObjType GetType();

protected:
	virtual void _create();
	void SetS64Value(u32 index, s64 value);
	s64 GetS64Value(u32 index);
protected:
	LocationVector _position;
	s64* _s64Value;
	int _s64ValueCount;
	UpdateMask _s64UpdateMask;
	GameObjType _objType;
	u64 _id;
private: 
};

