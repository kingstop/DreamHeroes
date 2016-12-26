#pragma once
#include "tcpsession.h"
class PlatformClient :
	 public tcp_client, public ProtocMsgBase<PlatformClient>
{
public:
	PlatformClient();
	virtual ~PlatformClient();

};

