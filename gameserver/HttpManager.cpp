#include "stdafx.h"
#include "HttpManager.h"
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

BaseHttpTask::BaseHttpTask()
{
	_en = HttpType_NO;
	_error = message::Error_NO;
	_url_config = "http:// 121.43.187.139/paygateway/index.php?";

}

BaseHttpTask::~BaseHttpTask()
{

}

void BaseHttpTask::set_acc(account_type acc)
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
		hero->addDealWaitToPay(_key_code.c_str(), _status, _price, _order_id, _error);
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
	//sprintf(_Http, "http://121.43.187.139/paygateway/index.php?action= third_preorder&channel=%d&user_id=%llu&ud=%s& product_id=%s", channel_id, _acc, );
	//http://121.43.187.139/paygateway/index.php?action= third_preorder&channel=1001&user_id=1000001&ud=fahxgyhghjk& product_id=com.bodhiworld.dh.xxx
	//sprintf(_Http, "http:// 121.43.187.139/paygateway/index.php?action=third_confirm& \
	//		channel=%d&user_id=%llu& receipt ={}", channel_id, _acc);
	//gHttpManager.Posts();
	
	std::string post_url;
	std::string respone_url;
	char sz_temp[1024];
	sprintf(sz_temp, "http://121.43.187.139:8080/paygateway/index.php?action=third_preorder&channel_id=%d&game_id=%d&user_id=%llu&ud=%s&product_id=%s",
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
		hero->addDealPay(_product_id.c_str(), _status, _order_id, _error);
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
		sprintf((char*)sz_temp, "http://121.43.187.139:8080/paygateway/index.php?action=third_confirm&channel_id=%d&game_id=%d&user_id=%llu&order_id=%d&receipt=%s",
			channel_id, game_id, _acc, _order_id, _receipt.c_str());
		Mylog::log_server(LOG_INFO, "http verify [%s] ", sz_temp);
		Mylog::log_server(LOG_INFO, "http encode verify [%s] ", _receipt.c_str());
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
					sprintf((char*)sz_temp, "http://121.43.187.139:8080/paygateway/index.php?action=order_finish&channel_id=%d&game_id=%d&user_id=%llu&order_id=%d",
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


int HttpManager::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char* pCaPath)
{
	_http_client->Posts(strUrl, strPost, strResponse, pCaPath);
	return 0;
}



HttpManager::HttpManager()
	:_stop(false)
{
	_http_client = new CurlHttpClient();
}


HttpManager::~HttpManager()
{

}

int HttpManager::getChannel()
{
	return _channel;
}

int HttpManager::getGameID()
{
	return _game_id;
}

void HttpManager::setChannel(int channel)
{
	_channel = channel;
}

void HttpManager::setGameID(int game_id)
{
	_game_id = game_id;
}

void HttpManager::addHttpTask(BaseHttpTask* task)
{
	boost::mutex::scoped_lock lock(_http_mutex);
	_https.push(task);
}

void HttpManager::update()
{
	
	_http_mutex.lock();
	while (_https.size() != 0)
	{
		_execute_https.push(_https.front());
		_https.pop();
	}
	_http_mutex.unlock();

	_http_complete_mutex.lock();
	while (_execute_https.size() != 0)
	{
		BaseHttpTask* http_client = _execute_https.front();
		http_client->excute();
		_complete_task.push(http_client);
		_execute_https.pop();
	}
	_http_complete_mutex.unlock();		
}

void HttpManager::logicUpdate()
{
	std::queue<BaseHttpTask*> queue_temp;
	_http_complete_mutex.lock();
	while (_complete_task.size() != 0)
	{
		BaseHttpTask* http_client = _complete_task.front();
		queue_temp.push(http_client);
		_complete_task.pop();
	}
	_http_complete_mutex.unlock();

	while (queue_temp.size() != 0)
	{
		BaseHttpTask* http_client = queue_temp.front();
		http_client->logicExcute();
		delete http_client;
		queue_temp.pop();
	}
}

void HttpManager::setStop(bool stop)
{
	_stop = stop;
}
bool HttpManager::isStop()
{
	return _stop;
}
