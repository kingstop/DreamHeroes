#include "stdafx.h"
#include "HttpManager.h"
#include "json_help.h"
//#include "DreamHero.h"

//void HttpProcess()
//{
//	while (gHttpManager.isStop() == false)
//	{
//		gHttpManager.update();
//#ifdef WIN32
//		Sleep(2);
//#else
//		usleep(2);
//#endif
//	}
//
//}

BaseHttpTask::BaseHttpTask()
{


}

BaseHttpTask::~BaseHttpTask()
{

}

void BaseHttpTask::set_acc(account_type acc)
{
	_acc = acc;
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
