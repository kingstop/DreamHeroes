#include "stdafx.h"
#include "HttpManager.h"
#include "json_help.h"


BaseHttpTask::BaseHttpTask()
{
	_en = HttpType_NO;
	_complete = false;
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



bool CreateDealHttpTaskIOS::excute()
{
	int channel_id = gHttpManager.getChannel();
	//sprintf(_Http, "http://121.43.187.139/paygateway/index.php?action= third_preorder&channel=%d&user_id=%llu&ud=%s& product_id=%s", channel_id, _acc, );
	//http://121.43.187.139/paygateway/index.php?action= third_preorder&channel=1001&user_id=1000001&ud=fahxgyhghjk& product_id=com.bodhiworld.dh.xxx
	//sprintf(_Http, "http:// 121.43.187.139/paygateway/index.php?action=third_confirm& \
	//		channel=%d&user_id=%llu& receipt ={}", channel_id, _acc);
	//gHttpManager.Posts();
	
	std::string post_url;
	std::string respone_url;
	sprintf(_Http, "http://121.43.187.139:8080/paygateway/index.php?action=third_preorder&channel=%d&user_id=%llu&ud=fahxgyhghjk&product_id=%s",
		channel_id, _acc, _name.c_str(), _key_code.c_str());
	
	gHttpManager.Posts(_Http, post_url, respone_url);
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
			//need log;
		}
		else
		{
			bret = value["status"].empty();
			if (bret == true)
			{

			}
			bret = value["product_id"].empty();
			if (bret == true)
			{

			}
			bret = value["price"].empty();
			if (bret == true)
			{

			}
			bret = value["order_id"].empty();
			if (bret == true)
			{

			}
			_status = value["status"].asInt();
			std::string product_id = value["product_id"].asString();
			_price = value["price"].asInt();
			_order_id = value["order_id"].asInt();
			if (product_id == _key_code)
			{

			}
			else
			{
				//need log;
			}
		}
	}
	catch (std::exception &ex)
	{

	}

	return true;
}


VerifyDealHttpTaskIOS::VerifyDealHttpTaskIOS()
{

}
VerifyDealHttpTaskIOS::~VerifyDealHttpTaskIOS()
{

}
bool VerifyDealHttpTaskIOS::excute()
{
	int channel_id = gHttpManager.getChannel();
	
	std::string post_url;
	std::string respone_url;
	sprintf(_Http, "http://121.43.187.139:8080/paygateway/index.php?action=third_confirm&channel=%d&user_id=%llu&receipt={%s}", channel_id, _acc, _receipt.c_str());
	gHttpManager.Posts(_Http, post_url, respone_url);
	try
	{
		bool bret = false;
		std::string strTemp;
		Json::Reader reader;
		Json::Value value;
		Json::Value arrayObj;
		Json::Value item;
		bret = reader.parse(respone_url.c_str(), value);
		bret = value["status"].empty();
		if (bret == true)
		{

		}

		bret = value["product_id"].empty();
		if (bret == true)
		{

		}

		bret = value["order_id"].empty();
		if (bret == true)
		{

		}

		_product_id = value["product_id"].asString();
		_status = value["status"].asInt();
		_order_id = value["order_id"].asInt();
	}
	catch (const std::exception& ex)
	{

	}
	return true;
}


int HttpManager::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char* pCaPath)
{
	_http_client->Posts(strUrl, strPost, strResponse, pCaPath);
	return 0;
}



HttpManager::HttpManager()
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
void HttpManager::setChannel(int channel)
{
	_channel = channel;
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

	while (_execute_https.size() != 0)
	{
		BaseHttpTask* http_client = _execute_https.front();
		http_client->excute();
	}
	
}
