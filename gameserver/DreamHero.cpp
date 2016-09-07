#include "stdafx.h"
#include "DreamHero.h"
#include "session.h"
#define _SAVE_PLAYER_TIME_  (10 * _TIME_SECOND_MSEL_)
DreamHero::DreamHero()
{
	_info.set_gold(0);
	_info.set_name("test");
	_online = false;
	_day_offset_time = gGameConfig.getGlobalConfig().day_Refresh_time_ * 60 * 60;
}


DreamHero::~DreamHero()
{
	StopSave();
}


message::MsgHeroData DreamHero::get_info()
{
	return _info;
}


u64 DreamHero::get_account()
{
	return _account;
}

void DreamHero::set_account(u64 account)
{
	_account = account;
	
}

void DreamHero::set_info(const message::MsgHeroDataDB2GS* info)
{
	_info.CopyFrom(info->data());
	_current_chapter = info->current_chapter();
	_current_section = info->current_section();
	_current_gold = info->current_gold();
	_last_task_advertisement_time = info->last_task_advertisement_time();
	_current_free_task_count = info->free_task_count();
}


void DreamHero::set_session(Session* session)
{
	_session = session;
}

Session* DreamHero::get_session()
{
	return _session;
}


std::vector<int> MapVersionFormat(std::string cur_version)
{
	std::vector<std::string> vcstr;
	std::vector<int> vcret;
	SplitStringA(cur_version, ".", vcstr);
	for (int i = 0; i < vcstr.size(); i ++)
	{
		if (isIntger(vcstr[i].c_str()))
		{
			vcret.push_back(atoi(vcstr[i].c_str()));
		}
		else
		{
			break;
		}
	}
	return vcret;
}


void DreamHero::StartSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == false)
	{
		gEventMgr.addEvent(this, &DreamHero::SaveHero, EVENT_SAVE_PLAYER_DATA_, _SAVE_PLAYER_TIME_, -1, 0);
	}
}

void DreamHero::StopSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == true)
	{
		gEventMgr.removeEvents(this, EVENT_SAVE_PLAYER_DATA_);
	}
}



void DreamHero::set_online(bool online)
{
	_online = online;
}

bool DreamHero::is_online()
{
	return _online;
}

void DreamHero::dayRefresh(bool need_send_msg)
{
	bool can_Refresh_task = false;
	if (_last_task_advertisement_time != 0)
	{
		u64 temp_time = g_server_time - _day_offset_time;
		if (same_day(temp_time, _last_task_advertisement_time))
		{			
			can_Refresh_task = true;
		}		 
	}
	else
	{
		can_Refresh_task = true;
	}

	if (can_Refresh_task)
	{
		_current_free_task_count = 0;
		if (_info.tasks_size() < gGameConfig.getGlobalConfig().hero_max_tasks_count_)
		{
			message::MsgTaskConfigInfo task_config_entry = RadnomTaskInfo();
			if (task_config_entry.taskid() != 0)
			{
				if (need_send_msg)
				{
					message::MsgS2CNewTaskNotify msg;
					msg.mutable_task()->CopyFrom(task_config_entry);
					sendPBMessage(&msg);
				}
				message::MsgTaskInfo* task_entry = _info.add_tasks();
				task_entry->set_argument_1(0);
				task_entry->set_taskid(task_config_entry.taskid());
				task_entry->set_usetime(0);				
			}
		}
	}
}


message::MsgTaskConfigInfo DreamHero::RadnomTaskInfo()
{
	
	std::vector<int> vcTemp;
	const MAPTASKS* map_tasks = gGameConfig.getMapTasks();
	MAPTASKS::const_iterator it = map_tasks->begin();
	int require_chapter = 0;
	int require_section = 0;
	int require_complete_task_count = 0;
	for (; it != map_tasks->end(); ++it)
	{
		const message::MsgTaskConfigInfo& config_entry = it->second;
		int tasks_length = _info.tasks_size();
		bool need_continue = false;
		for (int i = 0; i < tasks_length; i ++)
		{
			if (_info.tasks(i).taskid() == config_entry.taskid())
			{
				need_continue = true;
				break;
			}
		}

		if (need_continue)
		{
			continue;
		}

		require_complete_task_count = config_entry.require_unlock_complete_task_count();
		if (_info.complete_task_count() <require_complete_task_count)
		{
			continue;
		}
		

		require_chapter = config_entry.require_unlock_chapter();
		require_section = config_entry.require_unlock_chapter();
		if (require_chapter != 0)
		{
			need_continue = true;
			int record_size = _info.records_size();
			for (int i = 0; i < record_size; i ++)
			{
				message::MsgIntPair pair_entry = _info.records(i);
				if (pair_entry.number_1() ==  require_chapter)
				{
					if (pair_entry.number_2() < require_section)
					{
						need_continue = false;
					}					
					break;
				}								
			}
			if (need_continue)
			{
				continue;
			}
		}
		vcTemp.push_back(config_entry.taskid());
	}

	message::MsgTaskConfigInfo TaskConfigEntry;
	TaskConfigEntry.set_taskid(0);
	if (vcTemp.size() != 0)
	{
		int index = rand() % vcTemp.size();
		int id_task = vcTemp[index];
		const message::MsgTaskConfigInfo* task_config_entry = gGameConfig.getMapTask(id_task);
		if (task_config_entry != NULL)
		{
			TaskConfigEntry.CopyFrom(*task_config_entry);
		}
	}	
	return TaskConfigEntry;
}



void DreamHero::ReqUnlockChapter(const message::MsgC2SReqUnlockChapter* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int record_size_temp = _info.records_size();
	message::MsgS2CUnlockChapterACK msgACK;
	message::GameError en_error = message::Error_NO;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_current_gold(_info.gold());
	const message::MsgChapterConfigInfo* info_config = gGameConfig.getChapterConfigInfo(chapter_id_temp);
	if (info_config == NULL)
	{
		en_error = message::Error_UnlockChapterFailedNotFoundTheUnlockChapter;
	}
	else
	{
		if (_info.complete_task_count() < info_config->required_task_complete_count())
		{
			en_error = message::Error_UnlockChapterFailedYouHaveToCompleteEnoughTasks;
		}
		else
		{
			info_config->required_task_complete_count();
			en_error = message::Error_UnlockChapterFailedTheRequiredSectionNotPass;
			bool bret_find_required_chapter_section = false;
			for (int i = 0; i < record_size_temp; i++)
			{
				message::MsgIntPair pair_entry = _info.records(i);
				if (pair_entry.number_1() == chapter_id_temp)
				{
					en_error = message::Error_TheChapterIsNotLock;
					return;
				}

				if (info_config->required_chapter_id() == pair_entry.number_1())
				{
					if (pair_entry.number_2() >= info_config->required_section_id())
					{
						en_error = message::Error_NO;
					}
				}
			}

			if (en_error == message::Error_NO)
			{
				int gold_current = _info.gold();
				gold_current -= info_config->require_gold();
				if (gold_current < 0)
				{
					en_error = message::Error_UnlockChapterFailedYouNotHaveEnoughGold;
				}
				else
				{
					message::MsgIntPair* pair_entry = _info.add_records();
					pair_entry->set_number_1(chapter_id_temp);
					pair_entry->set_number_2(0);
					_info.set_gold(gold_current);
					msgACK.set_current_gold(gold_current);
				}
			}
		}		
	}
	msgACK.set_error(en_error);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqExitGame(const message::MsgC2SReqExitGame* msg)
{
	
	int chapter_id_temp = msg->chapter_id();
	int section_id_temp = msg->section_id();
	int task_size = msg->task_infos_size();
	int complete_tasks_temp_size = msg->complete_tasks_size();
	message::MsgS2CExitGameACK msgACK;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_section_id(section_id_temp);
	msgACK.set_current_gold(0);
	msgACK.set_success(msg->success());
	msgACK.set_error(message::Error_NO);
	

	if (chapter_id_temp != _current_chapter || section_id_temp != _current_section)
	{
		msgACK.set_error(message::Error_NotEnterTheExitGame);
	}
	else
	{

		google::protobuf::RepeatedPtrField< ::message::MsgTaskInfo >* temp_repeated = NULL;
		::google::protobuf::RepeatedPtrField< ::message::MsgTaskInfo >::iterator it;
		temp_repeated = _info.mutable_tasks();
		for (int i = 0; i < task_size; i++)
		{
			message::MsgTaskInfo task_entry = msg->task_infos(i);
			int current_task_size = _info.tasks_size();
			for (int i = 0; i < current_task_size; i++)
			{
				message::MsgTaskInfo* cur_info = _info.mutable_tasks(i);
				if (cur_info->taskid() == task_entry.taskid())
				{
					cur_info->CopyFrom(task_entry);
					break;
				}
			}
		}

		for (int i = 0; i < complete_tasks_temp_size; i++)
		{
			int task_id = msg->complete_tasks(i);
			const message::MsgTaskConfigInfo* info_task_config = gGameConfig.getMapTask(task_id);
			bool try_to_find_task_info = false;
			int task_gift_gold = 0;
			if (info_task_config != NULL)
			{
				temp_repeated = _info.mutable_tasks();
				it = temp_repeated->begin();
				for (; it != temp_repeated->end(); ++it)
				{
					message::MsgTaskInfo task_entry = (*it);
					if (task_entry.taskid() == task_id)
					{
						try_to_find_task_info = true;
						task_gift_gold = info_task_config->gift_gold();
						temp_repeated->erase(it);
						return;
					}
				}
			}
			else
			{
				//need error log;
			}
			message::MsgIntPair* entry_pair = msgACK.add_task_gift();
			entry_pair->set_number_1(task_id);
			entry_pair->set_number_2(task_gift_gold);
		}
		int gold_entry = _info.gold();
		if (gold_entry < 0)
		{
			gold_entry = 0;
		}

		// need modify
		gold_entry += msg->gold();
		int length_task_gift = msgACK.task_gift_size();
		for (int i = 0; i < length_task_gift; i++)
		{
			const message::MsgIntPair entry = msgACK.task_gift(i);
			gold_entry += entry.number_2();
		}

		temp_repeated = _info.mutable_tasks();
		it = temp_repeated->begin();
		for (; it != temp_repeated->end(); ++it)
		{
			message::MsgTaskInfo* info_entry = msgACK.add_task_infos();
			info_entry->CopyFrom(*it);
		}
		
		bool find_chapter = false;
		int record_size = _info.records_size();
		for (int i = 0; i < record_size; i ++)
		{
			message::MsgIntPair* pair_entry = _info.mutable_records(i);
			if (pair_entry->number_1() == chapter_id_temp)
			{
				int modify_section = section_id_temp + 1;
				if (modify_section == pair_entry->number_2())
				{
					pair_entry->set_number_2(modify_section);
					find_chapter = true;
				}
			}
		}

		msgACK.set_current_gold(gold_entry);
	}
	sendPBMessage(&msgACK);	
}

void DreamHero::RefreshTask(int give_up_task_id)
{
	message::GameError msgError = message::Error_NO;
	
	if (_current_free_task_count > gGameConfig.getGlobalConfig().day_free_task_count_)
	{
		u64 diff_time = g_server_start_time - _last_task_advertisement_time;
		if (diff_time < gGameConfig.getGlobalConfig().day_task_advertisement_task_cd_)
		{
			msgError = message::Error_RefreshAdvertisementTaskFailedInCD;
		}
	}

	google::protobuf::RepeatedPtrField< message::MsgTaskInfo >* repeated_tasks = _info.mutable_tasks();
	if (msgError == message::Error_NO)
	{
		msgError = message::Error_RefreshAdvertisementTaskFailedNotFoundGiveUpTaskID;
		if (give_up_task_id != 0)
		{
			google::protobuf::RepeatedPtrField< message::MsgTaskInfo >::iterator it = repeated_tasks->begin();
			for (; it != repeated_tasks->end(); ++it)
			{
				if (it->taskid() == give_up_task_id)
				{
					msgError = message::Error_NO;
					repeated_tasks->erase(it);
					break;
				}
			}
		}

		if (msgError == message::Error_NO)
		{
			if (repeated_tasks->size() <= gGameConfig.getGlobalConfig().hero_max_tasks_count_ - 1)
			{
				message::MsgTaskConfigInfo info_task_config = RadnomTaskInfo();
				if (info_task_config.taskid() != 0)
				{
					message::MsgTaskInfo* entry = repeated_tasks->Add();
					entry->set_taskid(info_task_config.taskid());
					entry->set_argument_1(0);
					entry->set_usetime(0);
					if (give_up_task_id != 0)
					{
						message::MsgS2CAdvertisementRefreshTaskACK msgACK;
						msgACK.set_give_up_task_id(give_up_task_id);											
						message::MsgTaskConfigInfo* info = msgACK.add_infos();
						info->CopyFrom(info_task_config);
						msgACK.set_error(msgError);
						sendPBMessage(&msgACK);
					}
					else
					{
						message::MsgS2CAdvertisementApplyTaskACK msgACK;
						msgACK.set_error(msgError);
						message::MsgTaskConfigInfo* info = msgACK.add_infos();
						info->CopyFrom(info_task_config);
						sendPBMessage(&msgACK);
					}					
				}
				_current_free_task_count++;
				_last_task_advertisement_time = g_server_start_time;
			}
		}
	}
}

void DreamHero::ReqAdvertisementApplyTask(const message::MsgC2SReqAdvertisementApplyTask* msg)
{
	RefreshTask(0);
}


void DreamHero::ReqBuyHero(const message::MsgC2SReqBuyHero* msg)
{
	int buy_grid =  msg->grid();
	int buy_gold =  msg->gold();
	const message::MsgShopConfigInfo* buy_grid_config = gGameConfig.getShopConfig(buy_grid);
	message::MsgS2CBuyHeroACK msgACK;
	msgACK.set_grid(buy_grid);
	msgACK.set_current_gold(_info.gold());
	message::GameError en = message::Error_NO;
	if (buy_grid_config == NULL)
	{
		en = message::Error_BuyHeroFailedNotFoundGrid;
	}
	else
	{
		int cheap_gold =  gShopSalesPromotionManager.getCheapGold(buy_grid);
		int current_require_gold = buy_grid_config->require_gold() - cheap_gold;

		if (current_require_gold < 0)
		{
			current_require_gold = 0;
		}
		if (current_require_gold != msg->gold())
		{
			en = message::Error_BuyHeroFailedThePriceIsOld;
		}
		else
		{
			int current_gold = _info.gold();
			int modify_gold = current_gold - current_require_gold;
			if (modify_gold < 0)
			{
				en = message::Error_BuyHeroFailedNotEnoughGold;
			}
			else
			{
				_info.set_gold(modify_gold);
				int heroes_length = _info.heroes_size();
				if (heroes_length <= buy_grid)
				{
					for (size_t i = 0; i <= buy_grid - heroes_length; i++)
					{
						_info.add_heroes(false);
					}
				}
				_info.set_heroes(buy_grid, true);

			}
		}
	}
	sendPBMessage(&msgACK);
}


void DreamHero::ReqAdvertisementRefreshTask(const message::MsgC2SReqAdvertisementRefreshTask* msg)
{

	int give_up_task_id_temp = msg->give_up_task_id();
	RefreshTask(give_up_task_id_temp);
}

void DreamHero::ReqEnterGame(const message::MsgC2SReqEnterGame* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int section_id_temp = msg->section_id();
	int current_grid_id = msg->current_grid();

	int records_length = _info.records_size();
	message::MsgS2CEnterGameACK msgACK;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_section_id(section_id_temp);
	msgACK.set_current_grid(current_grid_id);
	message::GameError en_error = message::Error_CanNotEnterGameTheInstanceIsLock;

	for (int i = 0; i < records_length; i++)
	{
		const message::MsgIntPair record_entry = _info.records(i);
		int chapter_config_id = record_entry.number_1();
		int section_config_id = record_entry.number_2();
		if (chapter_config_id == chapter_id_temp)
		{
			int section = section_config_id + 1;
			if (section_id_temp <= section)
			{
				 int heroes_length = _info.heroes_size();
				 if (heroes_length < current_grid_id)
				 {
					 bool b_character = _info.heroes(current_grid_id);
					 if (b_character)
					 {
						 en_error = message::Error_NO;
						 _current_chapter = chapter_id_temp;
						 _current_section = section_id_temp;
						 _current_gold = 0;
					 }
					 else
					 {
						 en_error = message::Error_CanNotEnterGameTheCharacterIsLock;
					 }

				 }
				 else
				 {
					 en_error = message::Error_CanNotEnterGameTheCharacterIsLock;
				 }
			}

			return;
		}
	}

	msgACK.set_error(en_error);
	sendPBMessage(&msgACK);
}

void DreamHero::SendClientInit()
{
	StartSave();
	message::MsgS2CHeroesInit msg;
	msg.mutable_info()->CopyFrom(_info);

	sendPBMessage(&msg);
	_online = true;
}


void DreamHero::LoadFromConfig()
{
	int length = gGameConfig.getGlobalConfig().hero_unlock_count_;
	_info.clear_heroes();
	for (size_t i = 0; i < length; i++)
	{
		_info.add_heroes(true);
	}

	_info.set_gold(gGameConfig.getGlobalConfig().config_gold_);
	_info.set_complete_task_count(0);
	_info.set_current_hero(0);
	_info.set_name("normal");
	
}

void DreamHero::SaveHero()
{
	std::string sql_temp;
	int length = _info.records_size();
	std::string record_temp;
	std::string heroes_temp;
	std::string tasks_temp;
	char sz_temp[256];
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			record_temp += ";";
		}
		message::MsgIntPair pair_entry =  _info.records(i);
		sprintf(sz_temp, "%d,%d", pair_entry.number_1(), pair_entry.number_2());
		record_temp += sz_temp;
	}

	length = _info.heroes_size();
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			heroes_temp += ";";
		}

		int number_ = 0;
		bool b_temp =  _info.heroes(i);
		if (b_temp)
		{
			number_ = 1;
		}
		sprintf(sz_temp, "%d", number_);
		heroes_temp += sz_temp;
	}

	length = _info.tasks_size();
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			tasks_temp += ";";
		}
		message::MsgTaskInfo info_entry = _info.tasks(i);
		sprintf(sz_temp, "%d,%d,%d", info_entry.taskid(), info_entry.argument_1(), info_entry.usetime());
		tasks_temp += sz_temp;
	}
	char temp[4096];
	std::string last_task_advertisement_time_temp;


	build_unix_time_to_string(_last_task_advertisement_time, last_task_advertisement_time_temp);
#ifdef WIN32
	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,\
		`current_hero`, `current_chapter`, `current_section`, `current_gold`, `complete_task_count`, `free_task_count`,`last_task_advertisement_time`) values \
		(%llu, 'normal', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s');",
		_account, _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), _info.current_hero(), _current_chapter,
		_current_section, _info.gold(), _info.complete_task_count(), _current_free_task_count, last_task_advertisement_time_temp.c_str());
#elif LINUX
	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,\
		`current_hero`, `current_chapter`, `current_section`, `current_gold`, `complete_task_count`, `free_task_count`,`last_task_advertisement_time`) values \
		(%lu, 'normal', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s');",
		_account, _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), _info.current_hero(), _current_chapter,
		_current_section, _info.gold(), _info.complete_task_count(), _current_free_task_count, last_task_advertisement_time_temp.c_str());
#endif


	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(temp);
	gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
	/*
	std::string sql_temp;
	char temp[2048];
	int suits_size_temp = _info.suits_size();
	char suit_temp[256];
	std::string suits_sql;
	for (int i = 0; i < suits_size_temp; i ++)
	{
		if (i != 0)
		{
			suits_sql += ":";
		}
		const message::MsgSuitData temp_data = _info.suits(i);

		sprintf(suit_temp, "%d,%s", temp_data.suit_id(), temp_data.suit_name().c_str());
		suits_sql += suit_temp;
		for (int i = 0; i < temp_data.equip_ids_size(); i ++)
		{
			suits_sql += ",";
			u64 equip_id = temp_data.equip_ids(i);
#ifdef WIN32
			sprintf(suit_temp, "%llu", equip_id);
#else
			sprintf(suit_temp, "%llu", equip_id);
#endif
			suits_sql += suit_temp;
		}		
	}

#ifdef WIN32
	sprintf(temp, "replace into `character`(`account_id`, `level`, `name`,`action_point`, `diamand`, `gold`, `suits_name`) values(%llu, %d, '%s', %d, %d, %d, '%s')",
		_info.account(), _info.level(), _info.name().c_str(), _info.action_point(), _info.diamand(), _info.gold(), suits_sql.c_str());

#else
	sprintf(temp, "replace into `character`(`account_id`, `level`, `name`,`action_point`, `diamand`, `gold`, `suits_name`) values(%lu, %d, '%s', %d, %d, %d, '%s')",
		_info.account(), _info.level(), _info.name().c_str(), _info.action_point(), _info.diamand(), _info.gold(), suits_sql.c_str());

#endif // WIN32
	sql_temp = temp;
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sql_temp.c_str());
	sql_temp.clear();
 
	gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
	//gGSDBClient.parseQueryChar()
	if (_hero_equips.size() != 0)
	{
		std::string sql_item = "replace into `character_equip`(`equip_id`, `account_id`, `equip_config_id`, `level`, `equipped`, `client_save_flag`,`equip_count`) values";
		HEROEQUIPS::iterator it = _hero_equips.begin();
		int item_count = 0;
		for (; it != _hero_equips.end(); ++ it, item_count ++)
		{
			if (item_count >= 20)
			{
				msg_db.clear_sql();
				msg_db.set_sql(sql_temp.c_str());
				gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
				item_count = 0;
			}

			if (item_count == 0)
			{
				sql_temp += sql_item;
			}
			else
			{
				sql_temp += ",";
			}
			message::MsgEquipData entry = it->second; 
#ifdef WIN32
			sprintf(temp, "(%llu, %llu, %d, %d, %d, %d, %d)", entry.id(), _info.account(), entry.equip_id(), entry.level(), (int)entry.equipped(), entry.client_save_flag(), entry.count());
#else
			sprintf(temp, "(%lu, %lu, %d, %d, %d, %d, %d)", entry.id(), _info.account(), entry.equip_id(), entry.level(), (int)entry.equipped(), entry.client_save_flag(), entry.count());
#endif // 
			sql_temp += temp;

		}

		if (sql_temp.empty() == false)
		{
			msg_db.clear_sql();
			msg_db.set_sql(sql_temp.c_str());
			gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
		}
	}

	if (_hero_toys.size() != 0)
	{
		std::string sql_head = "replace into `character_toy`(`toy_cd_key`, `account`, `toy_config_id`, `toy_config_type`, `toy_level`, `verify_time`) values";
		std::string sql_excute = "";
		HEROTOYS::iterator it_toy = _hero_toys.begin();
		int toy_save_count = 0;
		for (; it_toy != _hero_toys.end(); ++it_toy, toy_save_count++)
		{
			if (toy_save_count > 20)
			{
				toy_save_count = 0;
				msg_db.clear_sql();
				msg_db.set_sql(sql_excute.c_str());
				gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
				sql_excute.clear();
			}

			if (toy_save_count == 0)
			{
				sql_excute += sql_head;
			}
			else
			{
				sql_excute += ",";
			}

			char sztemp[512];
			std::string verify_time = get_time(it_toy->second.time_stamp());
			sprintf(sztemp, "('%s', '%llu', %d, %d, %d, '%s')", it_toy->second.toy_cd_key().c_str(), _account,
				it_toy->second.toy().toy_config_id(), it_toy->second.toy().toy_config_type(), it_toy->second.toy().toy_level(), verify_time.c_str());
			sql_excute += sztemp;
		}

		if (sql_excute.empty() == false)
		{
			msg_db.clear_sql();
			msg_db.set_sql(sql_excute.c_str());
			gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
		}
	}
	*/
}

void DreamHero::set_name(const char* name)
{
	_info.set_name(name);
}

//int DreamHero::get_level()
//{
//	return _info.level();
//}
const char* DreamHero::get_name()
{
	return _info.name().c_str();
}




void DreamHero::sendPBMessage(google::protobuf::Message* p)
{
	if (_session)
	{
		_session->sendPBMessage(p);
	}
	
}
