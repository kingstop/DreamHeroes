#pragma once
#include "HttpManager.h"
void HttpProcess();
enum HttpType
{
	HttpType_NO,
	HttpType_Login,
};

class UserLoginSession;

class LoginHttpTask : public BaseHttpTask
{
public:
	LoginHttpTask();
	virtual ~LoginHttpTask();
	void init(const char* user_id, const char* app_id,
		const char* app_key, const char* token, const char* extra_data, 
		int channel, UserLoginSession* session);
	virtual bool excute();
	virtual bool logicExcute();
protected:	
	std::string _user_id;	
	int _channel_id;
	std::string _app_id;
	std::string _app_key;
	std::string _token;
	std::string _extra_data;

	int _status;
	std::string _nick_name;
	std::string _s_token;
	UserLoginSession* _session;

};
