#include "stdafx.h"
#include "GSHttpManager.h"
#include "json_help.h"
#include "DreamHero.h"

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

BaseGSHttpTask::BaseGSHttpTask()
{

}

BaseGSHttpTask::~BaseGSHttpTask()
{

}

void BaseGSHttpTask::set_acc(account_type acc)
{
	_acc = acc;
}


CreateDealHttpTaskIOS::CreateDealHttpTaskIOS()
{
	_en = HttpType_CreateDealIOS;
}
CreateDealHttpTaskIOS::~CreateDealHttpTaskIOS()
{

}

void CreateDealHttpTaskIOS::init(account_type acc, const char* name, const char* key_code)
{
	_acc = acc;
	_name = name;
	_key_code = key_code;

}

bool CreateDealHttpTaskIOS::logicExcute()
{
	DreamHero* hero = gDreamHeroManager.GetHero(_acc);
	if (hero)
	{
		hero->addDealWaitToPay(_key_code.c_str(), _status, _price, _order_id, (message::GameError)_error);
	}
	else
	{
		if (_error == message::Error_NO)
		{
			std::string create_pay_time;;
			build_unix_time_to_string(g_server_time, create_pay_time);
			char sz_temp[1024];
			sprintf(sz_temp, "replace into deal_wait_to_pay(`order_id`, `account_id`, `key_code`, `status`, `price`, `deal_time`,`complete_status`) \
				values(%d, %llu, '%s', %d, %d, '%s', %d) ", _order_id, _acc, _key_code.c_str(), _status, _price, create_pay_time.c_str(), DealStatusType_WaitToPay);
			gDreamHeroManager.addSql(sz_temp);
		}
	}

	return true;


}

bool CreateDealHttpTaskIOS::excute()
{
	int channel_id = gHttpManager.getChannel();
	int game_id = gHttpManager.getGameID();
	
	std::string post_url;
	std::string respone_url;
	char sz_temp[1024];
	sprintf(sz_temp, "%s/paygateway/index.php?action=third_preorder&channel_id=%d&game_id=%d&user_id=%llu&ud=%s&product_id=%s",
		gGameConfig.getPlatformHttpUrl(),
		channel_id, game_id, _acc, _name.c_str(), _key_code.c_str());
	
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
			_error = message::Error_CreateDealFailedTheHttpErrorRespone;
			//need log;
		}
		else
		{
			bool bret_status = value["status"].empty();
			bool bret_product_id = value["product_id"].empty();
			bool bret_price = value["price"].empty();
			bool bret_order_id = value["order_id"].empty();
			if (bret_status == false && bret_product_id == false && bret_price == false && bret_order_id == false)
			{
				_status = value["status"].asInt();
				if (_status == 0)
				{
					std::string product_id = value["product_id"].asString();
					_price = value["price"].asInt();
					_order_id = value["order_id"].asInt();
					if (product_id == _key_code)
					{

						
					}
					else
					{
						_error = message::Error_CreateDealDailedTheHttpResponeErrorProduct_id;
						//need log;
					}
				}
				else
				{
					_error = message::Error_CreateDealFailedTheHttpResponeFailed;
				}
			}
			else
			{
				_error = message::Error_CreateDealFailedTheHttpErrorRespone;
			}



		}
	}
	catch (std::exception &ex)
	{
		_error = message::Error_Unknow;
	}

	return true;
}


VerifyDealHttpTaskIOS::VerifyDealHttpTaskIOS()
{

}
VerifyDealHttpTaskIOS::~VerifyDealHttpTaskIOS()
{

}

bool VerifyDealHttpTaskIOS::logicExcute()
{
	DreamHero* hero = gDreamHeroManager.GetHero(_acc);
	if (hero)
	{
		hero->addDealPay(_product_id.c_str(), _status, _order_id, (message::GameError)_error);
	}
	else
	{

		if (_error == message::Error_NO)
		{
			char sz_temp[1024];
			std::string create_pay_time;;
			build_unix_time_to_string(g_server_time, create_pay_time);
			sprintf(sz_temp, "replace into deal_wait_to_pay(`order_id`, `account_id`, `key_code`, `status`, `price`, `deal_time`,`complete_status`) \
				values(%d, %llu, '%s', %d, %d, '%s', %d) ", _order_id, _acc, _product_id.c_str(), _status, 0, create_pay_time.c_str(), DealStatusType_WaitPrepareToPay);
			gDreamHeroManager.addSql(sz_temp);
		}
	}

	return true;
}


void VerifyDealHttpTaskIOS::init(account_type acc, const char* name, const char* receipt, int order_id)
{
	_acc = acc;
	_name = name;
	_receipt = receipt;
	_order_id = order_id;
}

bool VerifyDealHttpTaskIOS::excute()
{
	int channel_id = gHttpManager.getChannel();
	int game_id = gHttpManager.getGameID();
	
	if (_receipt.size() > 10000)
	{
		_error = message::Error_Unknow;
	}
	else
	{
		std::string post_url;
		std::string respone_url;
		unsigned char sz_temp[10240];
		unsigned char sz_temp_target[10240];
		//urlencode((unsigned char*)_receipt.c_str(), sz_temp_target);
		sprintf((char*)sz_temp,
			"%s/paygateway/index.php?action=third_confirm&channel_id=%d&game_id=%d&user_id=%llu&order_id=%d&receipt=%s",
			gGameConfig.getPlatformHttpUrl(),
			channel_id, game_id, _acc, _order_id, _receipt.c_str());
		//Mylog::log_server(LOG_INFO, "http verify [%s] ", sz_temp);
		//Mylog::log_server(LOG_INFO, "http encode verify [%s] ", _receipt.c_str());
		gHttpManager.Posts((char*)sz_temp, post_url, respone_url);
		try
		{
			bool bret = false;
			std::string strTemp;
			Json::Reader reader;
			Json::Value value;
			Json::Value arrayObj;
			Json::Value item;
			bret = reader.parse(respone_url.c_str(), value);
			bool bret_status = value["status"].empty();
			bool bret_product_id = value["product_id"].empty();
			bool bret_order_id = value["order_id"].empty();

			if (bret_status == false && bret_product_id == false && bret_order_id == false)
			{
				_status = value["status"].asInt();
				if (_status == 0)
				{
					_product_id = value["product_id"].asString();
					_order_id = value["order_id"].asInt();
					sprintf((char*)sz_temp, "%s/paygateway/index.php?action=order_finish&channel_id=%d&game_id=%d&user_id=%llu&order_id=%d",
						gGameConfig.getPlatformHttpUrl(),
						channel_id, game_id, _acc, _order_id);
					value.clear();
					gHttpManager.Posts((char*)sz_temp, post_url, respone_url);
					bret = reader.parse(respone_url.c_str(), value);
					bool bret_status = value["status"].empty();
					if (bret_status == false)
					{
						int status_entry = value["status"].asInt();
						if (status_entry == 0)
						{

						}
						else
						{
							_error = message::Error_VerifyDealFailedTheHttpResponeFailed;
							//need log
						}
					}
					else
					{
						_error = message::Error_VerifyDealFailedTheHttpErrorRespone;
						//need log
					}
				}
			}
			else
			{
				_error = message::Error_VerifyDealFailedTheHttpResponeFailed;
				//need log
			}
		}
		catch (const std::exception& ex)
		{
			_error = message::Error_Unknow;
		}
	}
	return true;
}



