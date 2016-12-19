#pragma once
#include "cur_http_client.h"
#include "common_type.h"
#include "message/server_define.h"
#include <queue>
#include "iostream"
#include <boost/thread.hpp>
//void HttpProcess();

class BaseHttpTask
{
public:
	BaseHttpTask();
	virtual ~BaseHttpTask();
	void set_acc(account_type acc);
	virtual bool excute() = 0;
	virtual bool logicExcute() = 0;
protected:
	int _en;
	account_type _acc;
	int _error;
	std::string _url_config;
	char _Http[512];
};



class HttpManager
{
public:
	HttpManager();
	virtual ~HttpManager();
	void addHttpTask(BaseHttpTask* task);
	void update();	
	void logicUpdate();
	int getChannel();
	int getGameID();
	void setChannel(int channel);
	void setGameID(int game_id);
	void setStop(bool stop);
	bool isStop();
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char* pCaPath = NULL);
protected:
	std::queue<BaseHttpTask*> _https;
	std::queue<BaseHttpTask*> _execute_https;
	std::queue<BaseHttpTask*> _complete_task;
	boost::mutex _http_mutex;
	boost::mutex _http_complete_mutex;
	CurlHttpClient* _http_client;
	int _channel;
	int _game_id;
	volatile bool _stop;
};

