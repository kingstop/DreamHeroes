#include "stdafx.h"
#include "lg_platform_client.h"


void LGPlatformClient::initPBModule()
{
	ProtocMsgBase<LGPlatformClient>::registerSDFun(&LGPlatformClient::send_message, &LGPlatformClient::parseMsg);
	//ProtocMsgBase<LGPlatformClient>::registerCBFun(PROTOCO_NAME(IPlatformPayProto::MsgP2GOrderNotifyReq), &LGPlatformClient::parseClinchADealNotify);
	//ProtocMsgBase<LGPlatformClient>::registerCBFun(PROTOCO_NAME(IPlatformPayProto::MsgP2GRegisterRsp), &LGPlatformClient::registerRegisterACK);
	//ProtocMsgBase<LGPlatformClient>::registerCBFun(PROTOCO_NAME(IPlatformPayProto::MsgP2GKickOffAck), &LGPlatformClient::parseKick);

}

void LGPlatformClient::parseMsg(google::protobuf::Message* p, pb_flag_type flag)
{

}
LGPlatformClient::LGPlatformClient() :tcp_client(*net_global::get_io_service())
{
	_proto_user_ptr = this;
	m_isreconnect = true;
}


LGPlatformClient::~LGPlatformClient()
{
}

void LGPlatformClient::on_connect()
{
	tcp_client::on_connect();
	//Mylog::log_server(LOG_INFO, "connect platform server [%s] success!", get_remote_address_string().c_str());
	//IPlatformPayProto::MsgG2PRegisterReq msg;
	//msg.set_game_id(gGameConfig.getGameID());
	//msg.set_server_id(gGameConfig.getServerID());
	//msg.set_server_flag(gGameConfig.getServerType());
	//sendPBMessage(&msg, gGameConfig.getGameID());
}
void LGPlatformClient::on_connect_failed(boost::system::error_code error)
{
	Mylog::log_server(LOG_ERROR, "connect platform server failed!");
}
void LGPlatformClient::proc_message(const message_t& msg)
{
	parsePBMessage(msg.data, msg.len, msg.base64);
}

void LGPlatformClient::on_close(const boost::system::error_code& error)
{
	Mylog::log_server(LOG_INFO, "platform  client close");
	tcp_session::on_close(error);

}

void LGPlatformClient::registerRegisterACK(google::protobuf::Message* p, pb_flag_type flag)
{
	//IPlatformPayProto::MsgP2GRegisterRsp* msg = (IPlatformPayProto::MsgP2GRegisterRsp*)p;
	//if (msg->err_code() == IPlatformPayProto::IPAY_ERR_OK)
	//{
	//	Mylog::log_server(LOG_INFO, "Register platform server success!");
	//}
	//else
	//{
	//	Mylog::log_server(LOG_INFO, "Register platform server failed!");
	//}
	
	
}

void LGPlatformClient::parseKick(google::protobuf::Message* p, pb_flag_type flag)
{
	//IPlatformPayProto::MsgP2GKickOffAck* msg = (IPlatformPayProto::MsgP2GKickOffAck*)p;
	//Mylog::log_server(LOG_ERROR, "platform server kick!");
	m_isreconnect = false;
}

void LGPlatformClient::parseClinchADealNotify(google::protobuf::Message* p, pb_flag_type flag)
{
	//try
	//{
	//	if (p)
	//	{
	//		IPlatformPayProto::MsgP2GOrderNotifyReq* msg = (IPlatformPayProto::MsgP2GOrderNotifyReq*)p;
	//		account_type acc = msg->user_id();
	//		IPlatformPayProto::MsgG2POrderNotifyRsp msgACK;
	//		msgACK.set_order_id(msg->order_id());
	//		msgACK.set_err_code((s32)IPlatformPayProto::IPAY_ERR_OK);
	//		sendPBMessage(&msgACK, gGameConfig.getGameID());

	//		if (acc != 0)
	//		{
	//			DreamHero* hero = gDreamHeroManager.GetHero(acc);
	//			if (hero)
	//			{
	//				hero->addDealPay(msg->product_id(), 0, msg->order_id(), message::Error_NO);
	//			}
	//			else
	//			{
	//				gDreamHeroManager.OfflineHeroDealWaitToPay(msg->order_id(), acc, msg->product_id().c_str(), 0);
	//			}
	//		}

	//	}
	//}
	//catch (const std::exception& x)
	//{
	//	Mylog::log_server(LOG_CRITICAL, "parse clinch dael crash[%s]!", x.what());
	//}


}
