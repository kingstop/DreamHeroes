#include "stdafx.h"
#include "BattleUnit.h"


BattleUnit::BattleUnit()
{
	_objType = GameObjType_BattleUnit;
}


BattleUnit::~BattleUnit()
{
}

void BattleUnit::_create()
{
	_s64ValueCount = message::AObjMax + message::AUnitMax;
}


void BattleUnit::setMaxHp(s64 value)
{
	int index = message::AMaxHp + message::AObjMax;
	SetS64Value(index, value);

}
void BattleUnit::setHp(s64 value)
{
	int index = message::AHp + message::AObjMax;
	SetS64Value(index, value);
}
void BattleUnit::setManaType(s64 value)
{
	int index = message::AManaType + message::AObjMax;
	SetS64Value(index, value);
}
void BattleUnit::setMaxMana(s64 value)
{
	int index = message::AMaxMana + message::AObjMax;
	SetS64Value(index, value);
}
void BattleUnit::setMana(s64 value)
{
	int index = message::AMana + message::AObjMax;
	SetS64Value(index, value);
}
//////////////////


s64 BattleUnit::getMaxHp()
{
	int index = message::AMaxHp + message::AObjMax;
	return GetS64Value(index);

}
s64 BattleUnit::getHp()
{
	int index = message::AHp + message::AObjMax;
	return GetS64Value(index);
}
s64 BattleUnit::getManaType()
{
	int index = message::AManaType + message::AObjMax;
	return GetS64Value(index);
}
s64 BattleUnit::getMaxMana()
{
	int index = message::AMaxMana + message::AObjMax;
	return GetS64Value(index);
}
s64 BattleUnit::getMana()
{
	int index = message::AMana + message::AObjMax;
	return GetS64Value(index);
}

