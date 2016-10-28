#pragma once
#include "cur_http_client.h"
enum HttpType
{
	HttpType_NO,
	HttpType_CreateDealIOS,
	HttpType_DealIOS,
};
class BaseHttpTask
{
public:
	BaseHttpTask();
	virtual ~BaseHttpTask();
	void set_acc(account_type acc);
	virtual bool excute() = 0;
protected:
	HttpType _en;
	account_type _acc;
	bool _complete;
	std::string _url_config;
	char _Http[512];
};

class CreateDealHttpTaskIOS : public BaseHttpTask
{
public:
	CreateDealHttpTaskIOS();
	virtual ~CreateDealHttpTaskIOS();
	virtual bool excute();

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
	virtual bool excute();
private:
	std::string _receipt;
	std::string _product_id;
	int _status;
	int _order_id;
};



class HttpManager
{
public:
	HttpManager();
	virtual ~HttpManager();
	void addHttpTask(BaseHttpTask* task);
	void update();
	int getChannel();
	void setChannel(int channel);
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char* pCaPath = NULL);
protected:
	std::queue<BaseHttpTask*> _https;
	std::queue<BaseHttpTask*> _execute_https;
	std::queue<BaseHttpTask*> _complete_task;
	boost::mutex _http_mutex;
	CurlHttpClient* _http_client;
	int _channel;
};

