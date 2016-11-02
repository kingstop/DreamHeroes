#pragma once
#include "cur_http_client.h"

void HttpProcess();

enum HttpType
{
	HttpType_NO,
	HttpType_CreateDealIOS,
	HttpType_DealIOS,
};

enum DealStatusType
{
	DealStatusType_WaitToPay,
	DealStatusType_WaitPrepareToPay,
	DealStatusType_Complete
};
class BaseHttpTask
{
public:
	BaseHttpTask();
	virtual ~BaseHttpTask();
	void set_acc(account_type acc);
	virtual bool excute() = 0;
	virtual bool logicExcute() = 0;
protected:
	HttpType _en;
	account_type _acc;
	message::GameError _error;
	std::string _url_config;
	char _Http[512];
};

class CreateDealHttpTaskIOS : public BaseHttpTask
{
public:
	CreateDealHttpTaskIOS();
	void init(account_type acc, const char* name, const char* key_code);
	virtual ~CreateDealHttpTaskIOS();
	virtual bool excute();
	virtual bool logicExcute();

protected:
	account_type _acc;
	std::string _name;
	std::string _key_code;
	
	int _status;	
	int _price;
	int _order_id;
};

class VerifyDealHttpTaskIOS : public BaseHttpTask
{
public:
	VerifyDealHttpTaskIOS();
	virtual ~VerifyDealHttpTaskIOS();
	void init(account_type acc, const char* name, const char* receipt);
	virtual bool excute();
	virtual bool logicExcute();
private:
	std::string _receipt;
	std::string _product_id;
	int _status;
	int _order_id;
	account_type _acc;
	std::string _name;
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

