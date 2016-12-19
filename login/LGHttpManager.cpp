#include "stdafx.h"
#include "LGHttpManager.h"
#include "json_help.h"
#include "user_session.h"
void HttpProcess()
{
	while (gHttpManager.isStop() == false)
	{
		gHttpManager.update();
#ifdef WIN32
		Sleep(2);
#else
		usleep(2);
#endif
	}

}


LoginHttpTask::LoginHttpTask()
{


}
LoginHttpTask::~LoginHttpTask()
{

}
void LoginHttpTask::init(const char* user_id, const char* app_id,
	const char* app_key, const char* token, const char* extra_data, int channel, UserLoginSession* session)
{
	_user_id = user_id;
	_app_id = app_id;
	_app_key = app_key;
	_token = token;
	_extra_data = extra_data;
	_channel_id = channel;
	_session = session;
}
bool LoginHttpTask::excute()
{
	std::string post_url;
	std::string respone_url;
	char sz_temp[10240];
	sprintf(sz_temp, "%s/logincheck/account_service.php?action=third_login_check&channel_id=%d&app_id=%s&app_key=%s&user_id=%s&token=%s&extra_data=%s",
		gLoginConfig.GetPlatformUrl(),
		_channel_id, _app_id.c_str(), _app_key.c_str(), _user_id.c_str(),
		_token.c_str(), _extra_data.c_str());

	gHttpManager.Posts(sz_temp, post_url, respone_url);

	try
	{
		bool bret = false;
		std::string strTemp;
		Json::Reader reader;
		Json::Value value;
		Json::Value arrayObj;
		Json::Value item;
		bret = reader.parse(respone_url.c_str(), value);
		if (bret == false)
		{
			Mylog::log_server(LOG_ERROR,"failed to parse login http respone[%s]", respone_url.c_str());
		}
		else
		{
			bool bret_status = value["status"].empty();
			if (bret_status)
			{
				_status = value["status"].asInt();
				if (_status == 0)
				{
					_user_id = value["user_id"].asString();
					_nick_name = value["nick_name"].asString(); 
					_s_token = value["token"].asString();
				}
			}
		}

	}
	catch (std::exception &ex)
	{
		_error = message::Error_Unknow;
		Mylog::log_server(LOG_ERROR, "parse login http respone[%s] crash ex[%s]", respone_url.c_str(), ex.what());

	}
	return true;
}
bool LoginHttpTask::logicExcute()
{
	
	bool success = false;
	if (strcmp(_session->getPlatformUserId(), _user_id.c_str()) == 0)
	{
		if (_status == 0)
		{
			_session->setState(UserLoginSession::_platform_success_);
			success = true;
		}		
	}
	message::S2CPlatformVerifyACK msg;
	msg.set_nick_name(_nick_name.c_str());
	msg.set_user_id(_user_id.c_str());
	msg.set_token(_s_token.c_str());
	msg.set_success(success);
	return true;
}