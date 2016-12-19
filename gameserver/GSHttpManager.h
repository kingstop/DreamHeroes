#pragma once
#include "cur_http_client.h"
#include "HttpManager.h"

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
	DealStatusType_Complete,
	DealStatusType_WaitPrepareToVerify,
};

class BaseGSHttpTask : public BaseHttpTask
{
public:
	BaseGSHttpTask();
	virtual ~BaseGSHttpTask();
public:
	void set_acc(account_type acc);
protected:
	account_type _acc;

};


class CreateDealHttpTaskIOS : public BaseGSHttpTask
{
public:
	CreateDealHttpTaskIOS();
	void init(account_type acc, const char* name, const char* key_code);
	virtual ~CreateDealHttpTaskIOS();
	virtual bool excute();
	virtual bool logicExcute();

protected:
	std::string _name;
	std::string _key_code;
	
	int _status;	
	int _price;
	int _order_id;
};

class VerifyDealHttpTaskIOS : public BaseGSHttpTask
{
public:
	VerifyDealHttpTaskIOS();
	virtual ~VerifyDealHttpTaskIOS();
	void init(account_type acc, const char* name, const char* receipt, int order_id);
	virtual bool excute();
	virtual bool logicExcute();
private:
	std::string _receipt;
	std::string _product_id;	
	int _status;
	int _order_id;
	std::string _name;
};


