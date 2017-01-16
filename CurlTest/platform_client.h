#pragma once
#include "tcpsession.h"
class TimeTest : public EventableObject
{
public:
	TimeTest();
	~TimeTest();

private:
	EventableObjectHolder m_EventHold;
};

