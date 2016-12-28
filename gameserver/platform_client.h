#pragma once
#include "tcpsession.h"
class PlatformClient :
	 public tcp_client, public ProtocMsgBase<PlatformClient>
{
public:
	static void initPBModule();
public:
	PlatformClient();
	virtual ~PlatformClient();
	virtual void on_connect();
	virtual void on_close(const boost::system::error_code& error);
	virtual void on_connect_failed(boost::system::error_code error);
	virtual void proc_message(const message_t& msg);
	void parseMsg(google::protobuf::Message* p, pb_flag_type flag);
public:
	void parseClinchADealNotify(google::protobuf::Message* p, pb_flag_type flag);
	void registerRegisterACK(google::protobuf::Message* p, pb_flag_type flag);
};

