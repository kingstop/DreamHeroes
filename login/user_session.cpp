#include "stdafx.h"
#include "user_session.h"
#include "message/login.pb.h"
#include "account_manager.h"
#include "LGHttpManager.h"

enum
{
    _SESSION_WAIT_ACCOUNT_ = 30* _TIME_SECOND_MSEL_,
    _SESSION_CHECK_ACCOUNT_ = 60 * _TIME_SECOND_MSEL_,
    _SESSION_WAIT_GATE_ = 60 * _TIME_SECOND_MSEL_,
    _SESSION_WAIT_CLOSE = 5 * _TIME_SECOND_MSEL_,
	_SESSION_WAIT_HTTP_ = 5 * _TIME_SECOND_MSEL_,
	_SESSION_WAIT_LOGIN_ = 5 * _TIME_SECOND_MSEL_,
};

void UserLoginSession::initPBModule()
{
	ProtocMsgBase<UserLoginSession>::registerSDFun(&UserLoginSession::send_message, NULL);
	ProtocMsgBase<UserLoginSession>::registerCBFun(PROTOCO_NAME(message::LoginRequest),  &UserLoginSession::parseLoginGame);
	ProtocMsgBase<UserLoginSession>::registerCBFun(PROTOCO_NAME(message::RegisterAccountRequest),  &UserLoginSession::parseRegister);
	ProtocMsgBase<UserLoginSession>::registerCBFun(PROTOCO_NAME(message::C2SPlatformVerifyReq), &UserLoginSession::parsePlatformVerify);
}

void UserLoginSession::parsePlatformVerify(google::protobuf::Message* p, pb_flag_type flag)
{
	message::C2SPlatformVerifyReq* msg = static_cast<message::C2SPlatformVerifyReq*>(p);
	if (msg)
	{
		if (_platform_user_id.empty() || _platform_user_id == msg->user_id())
		{
			LoginHttpTask* httpEntry = new LoginHttpTask();
			httpEntry->init(msg->user_id().c_str(), msg->app_id().c_str(),
				msg->app_key().c_str(),
				msg->token_id().c_str(), msg->extra_data().c_str(), msg->channel_id(), this);
			gHttpManager.addHttpTask(httpEntry);
			setState(UserLoginSession::_wait_platform_);
		}
	}
}

void UserLoginSession::parseLoginGame(google::protobuf::Message* p, pb_flag_type flag)
{
	
	message::LoginRequest* msg = static_cast<message::LoginRequest*>(p);
	if (msg)
	{
		bool check_account = true;
		if (gLoginConfig.IsNeedPlatformVerify()&& msg->channel()!= 0 )
		{
			if (m_state != UserLoginSession::_platform_success_ || msg->name() != _platform_user_id)
			{
				message::LoginResponse msg;
				msg.set_result(message::enumLoginResult_ErrorNeedThirdVerify);
				sendPBMessage(&msg);
			}
			else
			{

				check_account = false;
			}
		}

		if (check_account == true)
		{
			int verstion_number_1 = msg->version().number_1();
			int verstion_number_2 = msg->version().number_2();
			int verstion_number_3 = msg->version().number_3();
			int channel = msg->channel();
			if (gLoginConfig.CompareVersion(verstion_number_1, verstion_number_2, verstion_number_3) == false)
			{
				message::LoginResponse msg;
				msg.set_result(message::enumLoginResult_ErrorVersion);
				msg.mutable_version()->set_number_1(gLoginConfig.GetVersion()->versionNumber_1);
				msg.mutable_version()->set_number_2(gLoginConfig.GetVersion()->versionNumber_2);
				msg.mutable_version()->set_number_3(gLoginConfig.GetVersion()->versionNumber_3);
				sendPBMessage(&msg);
			}
			else if (gLoginConfig.CompareChannel(channel) == false)
			{
				message::LoginResponse msg;
				msg.set_result(message::enumLoginResult_ErrorChannel);
				sendPBMessage(&msg);
			}
			else if (msg->name().size() >= 128 || msg->pwd().size() >= 128)
			{
				message::LoginResponse msg;
				msg.set_result(message::enumLoginResult_NameFail);
				sendPBMessage(&msg);

			}
			else
			{
				gLGAccountMgr.checkAccount(msg->name(), msg->pwd(), msg->channel(),this, CheckAcct::_login_check_, "check account");
			}


		}
		

	}
	
}


void UserLoginSession::parseRegister(google::protobuf::Message* p, pb_flag_type flag)
{
	
	message::RegisterAccountRequest* msg = (message::RegisterAccountRequest *)p;
	if (msg)
	{
		gLGAccountMgr.checkAccount(msg->name(), msg->pwd(), 0,this, CheckAcct::_new_acc_check_, "new account");
	}
	

}


UserLoginSession::UserLoginSession(void) : tcp_session( *net_global::get_io_service() )
{
	m_accid = 0;
	_proto_user_ptr = this;
	_base64 = true;
	_channel = 0;
}

UserLoginSession::~UserLoginSession(void)
{
}

u8 UserLoginSession::getState()
{
	return m_state;
}

void UserLoginSession::setState(u8 s)
{
    m_state = s;
    switch(s)
    {
    case _disable_:
        {
            gEventMgr.removeEvents(this, EVENT_USER_SESSION_CLOSE);
        }
		break;
    case _wait_data_:
        {
            gEventMgr.addEvent(this, &UserLoginSession::setClose,EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_ACCOUNT_ ,1,EVENT_FLAG_DELETES_OBJECT);
        }
		break;
    case  _checking_data_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_CHECK_ACCOUNT_);
            }
    
        }
		break;
    case _wait_gate_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_GATE_);
            }
        }
		break;
    case _wait_close_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_CLOSE);
            }
        }
		break;
	case _wait_platform_:
		{
			if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
			{
				gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_HTTP_);
			}
		}
		break;

	case _platform_success_:
		{
			if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
			{
				gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_LOGIN_);
			}
		}
		break;
    default:
        break;
    }
}
void UserLoginSession::on_accept( tcp_server* p )
{
    setState(_wait_data_);
	tcp_session::on_accept( p );
    Mylog::log_player(LOG_INFO, "add a user session");
}

void UserLoginSession::on_close( const boost::system::error_code& error )
{
    
    setState(_disable_);
	_platform_user_id.clear();
	tcp_session::on_close( error );
	Mylog::log_player(LOG_INFO, "remove a user session");
}

const char* UserLoginSession::getPlatformUserId()
{
	return _platform_user_id.c_str();
}

void UserLoginSession::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len, msg.base64);
}

void UserLoginSession::reset()
{
	m_state = _disable_;
	m_accid = 0;
	tcp_session::reset();
}

void UserLoginSession::run()
{
	tcp_session::run();
}


void UserLoginSession::set_channel(int channel)
{
	_channel = channel;
}

int UserLoginSession::get_channel()
{
	return _channel;
}