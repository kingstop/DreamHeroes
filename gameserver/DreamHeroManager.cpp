#include "stdafx.h"
#include "DreamHeroManager.h"
#include "DreamHero.h"
#include "session.h"
#define _SAVE_COLLECT_TIME_  (10* _TIME_MINUTE_ * _TIME_SECOND_MSEL_)
DreamHeroManager::DreamHeroManager()
{
	char a = 'a';
	for (char a = 'a'; a < ('a' + 26); a++)
	{
		_char_configs.push_back(a);
	}
	_last_save_time = 0;
	_save_all_heroes_ok = false;
	_day_create_heroes_count = 0;
	_deal_order_id = 0;
}

bool DreamHeroManager::AddHeroOrder(account_type acc, const char* order)
{
	bool ret = false;
	MAPSTRACCPAIR::iterator it = _heroes_orders.find(order);
	if (it == _heroes_orders.end())
	{
		ret = true;
		_heroes_orders[order] = acc;		
	}
	return ret;
}

account_type DreamHeroManager::getOrderAcc(const char* order)
{
	account_type acc = 0;
	MAPSTRACCPAIR::iterator it = _heroes_orders.find(order);
	if (it != _heroes_orders.end())
	{
		acc = it->second;	
	}
	return acc;
}


DreamHeroManager::~DreamHeroManager()
{
}

void DreamHeroManager::Load(DBQuery* p)
{
	if (p)
	{
		DBQuery& query = *p;
		query << "select *  ,UNIX_TIMESTAMP(`save_time`),UNIX_TIMESTAMP(`daily_game_begin_time`) from `heroes_status`;";
		SDBResult sResult = query.store();
		int rows_length = sResult.num_rows();
		if (sResult.size() > 0)
		{
			DBRow& row = sResult[0];
			_day_create_heroes_count = row["day_create_heroes_count"];
			std::string str_number = row["day_number"].c_str();
			std::vector<std::string> outVC;
			std::vector<std::string> outVC1;
			std::vector<std::string> outVC2;
			SplitStringA(str_number, ";", outVC);
			int length = outVC.size();
			for (size_t i = 0; i < length; i++)
			{
				std::string numbers_str = outVC[i];
				SplitStringA(numbers_str, ",", outVC1);
				int length_1 = outVC1.size();
				for (size_t j = 0; j < length_1; j++)
				{
					_day_number[i][j] = atoi(outVC1[j].c_str());
				}
			}
			_last_save_time = row["UNIX_TIMESTAMP(`save_time`)"];
			_deal_order_id = row["deal_order_id"];
			gRankManager.setDailyGameBeginTime(row["UNIX_TIMESTAMP(`daily_game_begin_time`)"]);
		}

	}
}

void DreamHeroManager::recoverSpirit()
{
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* Hero = it->second;
		if (Hero)
		{
			Hero->recoverSpirit();
		}
	}
}

void DreamHeroManager::init()
{
	_recover_spirit_minute = gGameConfig.getGlobalConfig().config_recover_spirit_minute_;
	_recover_spirit = gGameConfig.getGlobalConfig().config_recover_spirit_;
	_save_all_heroes_ok = false;
	if (gEventMgr.hasEvent(this, EVENT_COLLECT_INFO_) == false)
	{
		gEventMgr.addEvent(this, &DreamHeroManager::CollectInfo, EVENT_COLLECT_INFO_, _SAVE_COLLECT_TIME_, -1, 0);
	}
	
	if (_recover_spirit != 0 && _recover_spirit_minute != 0)
	{
		if (gEventMgr.hasEvent(this, EVENT_RECOVER_SPIRIT_) == false)
		{
			int time_entry = _recover_spirit_minute * _TIME_MINUTE_ * _TIME_SECOND_MSEL_;
			gEventMgr.addEvent(this, &DreamHeroManager::recoverSpirit, EVENT_RECOVER_SPIRIT_, time_entry, -1, 0);
		}
	}
	else
	{
		//Mylog::log_server()
	}
	time_t server_time = g_server_start_time;

	tm* p1 = localtime(&server_time);
	if (gEventMgr.hasEvent(this, EVENT_PER_MIN) == false)
	{
		int next_min = (60 - p1->tm_sec) * _TIME_SECOND_MSEL_;
		gEventMgr.addEvent(this, &DreamHeroManager::eventPerMin, EVENT_PER_MIN, next_min, -1, 0);
	}

	//if (gEventMgr.hasEvent(this, EVENT_PER_HOUR) == false)
	//{
	//	int next_hour_second = ((59 - p1->tm_min) * 60 + (60 - p1->tm_sec))* _TIME_SECOND_MSEL_;
	//	gEventMgr.addEvent(this, &DreamHeroManager::eventPerHour, EVENT_PER_HOUR, next_hour_second, -1, 0);
	//}

	if (gEventMgr.hasEvent(this, EVENT_SAVE_HEROES_STATU) == false)
	{
		gEventMgr.addEvent(this, &DreamHeroManager::save, EVENT_SAVE_HEROES_STATU, 60 * _TIME_SECOND_MSEL_, -1, 0);
	}

	refrashHeroTitle();
	if (same_day(_last_save_time, g_server_time) == false)
	{
		refrashDayNumber();
	}
	
	gRankManager.Init();

}

void DreamHeroManager::save()
{
	std::string number_str;
	std::string number_temp;
	char sz_temp[64];
	for (size_t i = 0; i < 6; i++)
	{
		if (number_str.empty() == false)
		{
			number_str += ";";
		}
		for (size_t j = 0; j < 10; j++)
		{
			if (number_temp.empty() == false)
			{
				number_temp += ',';
				
			}
			sprintf(sz_temp, "%d", _day_number[i][j]);
			number_temp += sz_temp;			
		}
		number_str += number_temp;
		number_temp.clear();
	}
	char sz_sql[1024];
	int count = _heroes.size();
	_last_save_time = g_server_time;
	std::string last_time_temp;
	build_unix_time_to_string(_last_save_time, last_time_temp);
	char server_id = gGameConfig.getServerID();

	std::string daily_game_begin_time;
	build_unix_time_to_string(gRankManager.getDailyGameBeginTime(), daily_game_begin_time);
	std::string str_server_id;
	str_server_id.push_back(server_id);
	str_server_id.push_back('\0');
	sprintf(sz_sql, "replace into `heroes_status`(`sid`, `server_id`, `day_create_heroes_count`, `day_number`, `heroes_online_count`, \
					`save_time`, `deal_order_id`, `daily_game_begin_time`)\
	 values (%d, '%s', %d, '%s', %d, '%s', %d, '%s');",0, str_server_id.c_str() , _day_create_heroes_count, number_str.c_str(),
		count, last_time_temp.c_str(), _deal_order_id, daily_game_begin_time.c_str());
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sz_sql);
	
	gGSDBClient.sendPBMessage(&msg_db, 0);
}

void DreamHeroManager::eventPerMin()
{
	time_t server_time = g_server_time;
	tm* p1 = localtime(&server_time);
	if (p1->tm_min == 0 )
	{
		if (p1->tm_hour == gGameConfig.getGlobalConfig().day_Refresh_time_)
		{
			dayRefresh();
		}

		if (p1->tm_hour == 0)
		{
			refrashDayNumber();
		}
	}
	if (gEventMgr.hasEvent(this,EVENT_PER_MIN) == true)
	{
		gEventMgr.removeEvents(this, EVENT_PER_MIN);
	}
	int next_min = (60 - p1->tm_sec) * _TIME_SECOND_MSEL_;
	gEventMgr.addEvent(this, &DreamHeroManager::eventPerMin, EVENT_PER_MIN, next_min, -1, 0);
	std::string cur_time;
	build_unix_time_to_string(g_server_time, cur_time);
	Mylog::log_server(LOG_INFO, "server per min time[%s]", cur_time.c_str());

}

void DreamHeroManager::eventPerHour()
{
	//time_t server_time = g_server_time;
	//tm* p1 = localtime(&server_time);
	//int next_hour_second = ((59 - p1->tm_min) * 60 + (60 - p1->tm_sec) )* _TIME_SECOND_MSEL_;
	//if (gEventMgr.hasEvent(this,EVENT_PER_HOUR))
	//{
	//	gEventMgr.removeEvents(this, EVENT_PER_HOUR);
	//}
	//gEventMgr.addEvent(this, &DreamHeroManager::eventPerHour, EVENT_PER_HOUR, next_hour_second, -1, 0);

	////gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_PER_HOUR, next_hour_second);
	//
	//if (p1->tm_hour == gGameConfig.getGlobalConfig().day_Refresh_time_)
	//{
	//	dayRefresh();
	//}

	//if (p1->tm_hour == 0)
	//{
	//	refrashDayNumber();
	//}
}

std::string DreamHeroManager::generateStr(int& key, const char* argu)
{
	
	//int number_entry;
	int number_temp = 1;
	char current_server_char = gGameConfig.getServerID();
	std::string str_name = current_server_char + _hero_day_title;
	int numbers[6];
	for (size_t i = 0; i < 6; i++)
	{
		int cur_number = (key / number_temp) % 10;
		numbers[i] = _day_number[i][cur_number];
		number_temp *= 10;
	}
	char sz_name[256];
	sprintf(sz_name, "%s%d%d%d%d%d%d%s", str_name.c_str(), numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5], argu);

	return sz_name;

}

std::string DreamHeroManager::generateName()
{
	_day_create_heroes_count++;
	//int number_entry;
	return generateStr(_day_create_heroes_count, "");
}

std::string DreamHeroManager::generateDealOrderID(account_type acc)
{
	_deal_order_id++;
	std::string order = generateStr(_deal_order_id, "order");
	AddHeroOrder(acc, order.c_str());
	return order;
}

void DreamHeroManager::refrashHeroTitle()
{
	time_t server_open_time = gGameConfig.getServerOpenTime();
	tm* p_open_time = localtime(&server_open_time);
	_hero_day_title = "aaa";
	if (g_server_time > server_open_time)
	{
		s64 day_spawn = p_open_time->tm_hour * 60 * 60 + p_open_time->tm_min * 60 + p_open_time->tm_sec;
		s64 time_spwan = g_server_time - (server_open_time - day_spawn);		
		s64 number = time_spwan / (60 * 60 * 24);
		int number_char_1 = number % 26;
		int number_char_2 = (number / 26) % 26;
		int number_char_3 = (number / (26 * 26)) % 26;
		_hero_day_title.clear();
		_hero_day_title.push_back(_char_configs[number_char_1]);
		_hero_day_title.push_back(_char_configs[number_char_2]);
		_hero_day_title.push_back(_char_configs[number_char_3]);
		_hero_day_title.push_back('\0');
		_day_create_heroes_count = 0;
	}
}

void DreamHeroManager::refrashDayNumber()
{
	std::vector<int> vc_number;
	for (size_t i = 0; i <= 9; i++)
	{
		vc_number.push_back(i);
	}

	for (size_t i = 0; i < 6; i++)
	{
		std::random_shuffle(vc_number.begin(), vc_number.end());
		for (size_t j = 0; j < 10; j++)
		{
			_day_number[i][j] = vc_number[j];
		}
	}

	refrashHeroTitle();
	save();
}

void DreamHeroManager::dayRefresh()
{	
	gRankManager.DailyGameUpdate();
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end() ; ++ it)
	{
		it->second->dayRefresh();
	}
	std::string day_refresh_time = "";
	build_unix_time_to_string(g_server_time, day_refresh_time);
	Mylog::log_server(LOG_INFO, "server day refresh [%s]", day_refresh_time.c_str());
}

DreamHeroManager::MAPHEROS* DreamHeroManager::GetHeroes()
{
	return &_heroes;
}

DreamHero* DreamHeroManager::GetHero(account_type account)
{
	DreamHero* hero = NULL;
	MAPHEROS::iterator it = _heroes.find(account);
	if (it != _heroes.end())
	{
		hero = it->second;
		hero->set_parent(this);
		//hero->StopDestroyClock();
	}
	
	
	return hero;
}

void DreamHeroManager::OfflineHeroDealWaitToPay(int order_id, account_type acc, const char* product_id, int status)
{
	std::string create_pay_time;;
	build_unix_time_to_string(g_server_time, create_pay_time);
	char sz_tem[2048];
	DealStatusType type = DealStatusType_WaitPrepareToPay;
	if (status != 0) 
	{
		type = DealStatusType_Failed;
	}
	sprintf(sz_tem, "replace into deal_wait_to_pay(`order_id`, `account_id`, `key_code`, `status`, `price`, `deal_time`,`complete_status`) \
				values(%d, %llu, '%s', %d, %d, '%s', %d) ", order_id, acc, product_id, status, 0, create_pay_time.c_str(), type);
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sz_tem);
	gGSDBClient.sendPBMessage(&msg_db, 0);

}



DreamHero* DreamHeroManager::CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		const message::MsgHeroData* entry_msg = HeroDataMsg->mutable_data();
		hero->set_info(HeroDataMsg, session->get_channel());
		
		_heroes.insert(MAPHEROS::value_type(hero->get_account(), hero));
	}	
	hero->set_account(acc);
	hero->set_session(session);
	hero->set_parent(this);
	hero->StopDestroyClock();
	return hero;
}

DreamHero* DreamHeroManager::CreateHero(account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		hero->set_parent(this);
		hero->LoadFromConfig(session->get_channel());
	}
	else
	{
		hero->set_parent(this);
	}
	hero->set_session(session);
	hero->set_account(acc);
	
	hero->StopDestroyClock();
	_heroes.insert(MAPHEROS::value_type(hero->get_account(), hero));
	return hero;
}



void DreamHeroManager::SaveDreamHeroes()
{
	save();
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* hero = it->second;
		hero->SaveHero();
	}
	Mylog::log_server(LOG_INFO, "game server save all heroes count[%d]!", _heroes.size());
	message::MsgSaveAllHeroesGS2DB msg;
	gGSDBClient.sendPBMessage(&msg, 0);	
}


void DreamHeroManager::DestroyHero(DreamHero* p)
{
	MAPHEROS::iterator it = _heroes.find(p->get_account());
	if (it != _heroes.end())
	{
		DreamHero* p_temp = it->second;
		if (p_temp == p)
		{
			delete p_temp;
			_heroes.erase(it);
		}
		else
		{
			//need log;
		}
	}
	else
	{
		//need log;
	}
}


void DreamHeroManager::addSql(const char* sql)
{
	_vc_sql.push_back(sql);
}

void DreamHeroManager::CollectInfo()
{
	int offline_count = 0;
	int online_count = 0;
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* hero = it->second;
		if (hero)
		{
			if (hero->is_online())
			{
				online_count++;
			}
			else
			{
				offline_count++;
			}			
		}
	}
	int current_player = offline_count + online_count;
	gRecordManager.gameOnlineRecord(online_count, offline_count);
	Mylog::log_server(LOG_INFO, "current player count[%d], online player count[%d], offline player count[%d]", current_player, online_count, offline_count);
}

DreamHero* DreamHeroManager::GetHeroByName(const char* name)
{
	DreamHero* hero = NULL;
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* temp_hero = it->second;
		if (temp_hero != NULL)
		{
			std::string hero_name = temp_hero->get_name();
			int length = strlen(name);
			if (strcmp(temp_hero->get_name(), name) == 0)
			{
				hero = temp_hero;
				break;

			}
			//
			//if (temp_hero->get_name() == name)
			//{
			//}
		}


	}
	//hero->StopDestroyClock();
	return hero;
}

bool DreamHeroManager::is_save_all_heroes_ok()
{
	return _save_all_heroes_ok;
}

void DreamHeroManager::save_all_heroes_ok()
{
	_save_all_heroes_ok = true;
}

