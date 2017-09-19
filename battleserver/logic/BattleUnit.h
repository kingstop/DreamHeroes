#pragma once
#include "MoveGameObject.h"

class BattleUnit :
	public MoveGameObject
{
public:
	BattleUnit();
	~BattleUnit();
	void setMaxHp(s64 value);
	void setHp(s64 value);
	void setManaType(s64 value);
	void setMaxMana(s64 value);
	void setMana(s64 value);

	s64 getMaxHp();
	s64 getHp();
	s64 getManaType();
	s64 getMaxMana();
	s64 getMana();
protected:
	virtual void _create();

};

