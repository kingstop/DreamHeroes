#include "stdafx.h"
#include "platform_client.h"


PlatformClient::PlatformClient() :tcp_client(*net_global::get_io_service())
{
	_proto_user_ptr = this;
	m_isreconnect = true;
}


PlatformClient::~PlatformClient()
{
}
