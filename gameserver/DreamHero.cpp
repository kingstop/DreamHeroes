#include "stdafx.h"
#include "DreamHero.h"
#include "session.h"
#include "RecordManager.h"
#define _SAVE_PLAYER_TIME_  (10 * _TIME_SECOND_MSEL_)
#define _REMOVE_PLAYER_TIME_  (3 * _TIME_SECOND_MSEL_)
#define _HERO_PING_NOTIFY_ (1 * _TIME_SECOND_MSEL_)
DreamHero::DreamHero()
{
	_info.set_gold(0);
	_info.set_name("test");
	_online = false;
	_day_offset_time = gGameConfig.getGlobalConfig().day_Refresh_time_ * 60 * 60;
	_current_chapter = -1;
	_current_section = -1;

	_current_task_count = 0;
	_last_task_advertisement_time = 0;
	_day_offset_time = 0;
	_gm_level = 0;
	_account = 0;
	_destroy_clock = false;
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
	_account = info->account();
	_info.CopyFrom(info->data());
	_current_chapter = info->current_chapter();
	_current_section = info->current_section();
	_last_task_advertisement_time = info->last_task_advertisement_time();
	_current_task_count = info->free_task_count();
	_last_recover_spirit_time = info->last_recover_spirit_time();
	_last_buy_spirit_time = info->last_buy_spirit_time();
	int size_special_kills =  info->special_kills_size();
	_last_day_lottery_time = info->last_lottery_time();
	_gm_level = info->gm_level();
	_daily_game_time = info->daily_game_time();
	_daily_game_prize_time = info->daily_game_prize_time();
	_last_daily_reset_game_time = info->last_daily_reset_game_time();
	_special_kills.clear();
	for (int i = 0; i < size_special_kills; i ++)
	{
		const message::MsgMapSpecialKill& entry = info->special_kills(i);
		std::pair<int, int> pair_entry;
		pair_entry.first = entry.chapter_id();
		pair_entry.second = entry.section_id();
		int kills_length = entry.kills_size();
		for (size_t j = 0; j < kills_length; j++)
		{
			const message::MsgObjConfig entry_config = entry.kills(j);
			_special_kills[pair_entry].push_back(entry_config);
		}		
	}

	//int hero_deal_length = info->hero_deal_infos_size();
	//for (size_t i = 0; i < hero_deal_length; i++)
	//{
	//	const message::MsgHeroDealInfo entry = info->hero_deal_infos(i);
	//	_hero_deals[entry.order_id()] = entry;
	//	gDreamHeroManager.AddHeroOrder(_account, entry.order_id().c_str());
	//}
	int deal_length = info->deals_size();
	for (size_t i = 0; i < deal_length; i++)
	{
		const message::MsgDealInfo DealInfoEntry = info->deals(i);
		std::string key_code_;
		int status_;
		int price_;
		int order_id_;
		DealStatusType type_;
		DealWaitToPay pay_entry;
		pay_entry.key_code_ = DealInfoEntry.product_id().c_str();
		pay_entry.order_id_ = DealInfoEntry.order();
		pay_entry.price_ = 0;
		pay_entry.status_ = DealInfoEntry.status();
		pay_entry.receipt_ = DealInfoEntry.cur_receipt().c_str();
		pay_entry.type_ = (DealStatusType)DealInfoEntry.complete_status();
		_deals_wait_to_pay.insert(DEALSWAITTOPAY::value_type(pay_entry.order_id_, pay_entry));
	}

	//int deal_info_length = info->hero_deal_infos_size();
	//for (size_t i = 0; i < deal_info_length; i++)
	//{
	//	const message::MsgHeroDealInfo& entry_deal = info->hero_deal_infos(i);
	//	_hero_deals[entry_deal.order_id().c_str()] = entry_deal;
	//}


	_special_creatures.clear();
	int special_creatures_size =  info->special_creatures_size();
	for (int i = 0; i < special_creatures_size; i ++)
	{
		const message::MsgIntPair entry = info->special_creatures(i);
		_special_creatures[entry.number_1()] = entry.number_2();
	}

	DEALSWAITTOPAY::iterator it_deal = _deals_wait_to_pay.begin();
	for (; it_deal != _deals_wait_to_pay.end(); ++ it_deal)
	{
		const DealWaitToPay& WaitToPay = it_deal->second;
		switch (it_deal->second.type_)
		{
		case DealStatusType_WaitToPay:
		case DealStatusType_WaitPrepareToPay:
		{
			addDealPay(WaitToPay.key_code_.c_str(), WaitToPay.status_, WaitToPay.order_id_, message::Error_NO, false);
		}
		break;
		case DealStatusType_WaitPrepareToVerify:
		{

			VerifyDealHttpTaskIOS* entry = new VerifyDealHttpTaskIOS();
			entry->init(_account, _info.name().c_str(), WaitToPay.receipt_.c_str(), WaitToPay.order_id_);
			gHttpManager.addHttpTask(entry);
		}
		break;
		default:
			break;
		}

		
	}

	if (g_server_time > _last_recover_spirit_time)
	{		
		int recorver_times = (int) float(float(g_server_time) - float(_last_recover_spirit_time)) / float(gGameConfig.getGlobalConfig().config_recover_spirit_minute_ * 60);
		int recorver_spirit = gGameConfig.getGlobalConfig().config_recover_spirit_ * recorver_times;
		int temp_spirit = recorver_spirit + _info.spirit();
		if (temp_spirit > gGameConfig.getGlobalConfig().config_max_spirit_)
		{
			temp_spirit = gGameConfig.getGlobalConfig().config_max_spirit_;
		}
		_info.set_spirit(temp_spirit);
	}
	//std::map<std::string, message::MsgHeroDealInfo>::iterator deal_it = _hero_deals.begin();
	//for (; deal_it != _hero_deals.end(); ++ deal_it)
	//{
	//	const message::MsgHeroDealInfo& entry = deal_it->second;
	//	if (entry.type() == message::HeroDealTypeWaitToPay)
	//	{
	//		completeDealByOrder(deal_it->first.c_str(), true,false);
	//	}
	//}
	TryToGetGamePrize(false);

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




void DreamHero::StartDestroyTime()
{
	_destroy_clock = true;
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_) == true)
	{
		gEventMgr.removeEvents(this, EVENT_DESTROY_PLAYER_);
		
	}
	gEventMgr.addEvent(this, &DreamHero::Destroy, EVENT_DESTROY_PLAYER_, _REMOVE_PLAYER_TIME_, -1, 0);
}

void DreamHero::StopDestroyClock()
{
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_)== true)
	{
		gEventMgr.removeEvents(this, EVENT_DESTROY_PLAYER_);
	}

	_destroy_clock = false;
}

void DreamHero::Destroy()
{
	gRecordManager.loginOutRecord(_account, _info.name().c_str());
	if (_session)
	{
		_session->setDreamHero(NULL);

	}
	_parent->DestroyHero(this);
}

void DreamHero::set_parent(DreamHeroManager* p)
{
	_parent = p;
}

void DreamHero::set_online(bool online)
{
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_) == false)
	{
		gEventMgr.removeEvents(this, EVENT_DESTROY_PLAYER_);
	}
	_online = online;
}

bool DreamHero::is_online()
{
	return _online;
}

int DreamHero::getGMLevel()
{
	return _gm_level;
}


void DreamHero::SetGMLevel(int level)
{
	_gm_level = level;
}

bool DreamHero::isInToday(u32 time)
{
	bool ret = false;
	u64 temp_time = g_server_time - _day_offset_time;
	if (same_day(temp_time, time) == true)
	{
		ret = true;
	}
	return ret;
}
void DreamHero::dayRefresh(bool need_send_msg)
{

	bool can_Refresh_task = false;
	if (_last_task_advertisement_time != 0)
	{
		if (gGameConfig.isInToday(_last_task_advertisement_time) == false)
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
		_current_task_count = 0;
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

	bool can_Refresh_spirit = false;
	if (_last_buy_spirit_time != 0)
	{
		if (gGameConfig.isInToday(_last_buy_spirit_time) == false)
		{
			can_Refresh_spirit = true;
		}
	}
	else
	{
		can_Refresh_spirit = true;
	}

	if (can_Refresh_spirit)
	{
		_info.set_day_buy_spirit(0);
	}
}


void DreamHero::ReqResetDailyGameProgress(const message::MsgC2SReqResetDailyGameProgress* msg)
{
	int reset_game_count = _info.daily_reset_game_count();
	message::GameError error = message::Error_NO;
	if (gGameConfig.isInToday(_daily_game_time) == false || _info.daily_game_progress() == 0)
	{
		error = message::Error_FailedToResetDailyGameTheProgressIsZero;
	}
	else
	{
		if (gGameConfig.isInToday(_last_daily_reset_game_time) == false)
		{
			reset_game_count = 0;
		}
		else
		{
			reset_game_count += 1;
		}
		const globalConfig& global = gGameConfig.getGlobalConfig();
		if (reset_game_count >= global.daily_game_reset_jewel_config_.size())
		{
			error = message::Error_FailedToResetDailyGameUseOut;
		}
		else
		{
			int jewel = global.daily_game_reset_jewel_config_[reset_game_count];
			if (jewel > _info.jewel())
			{
				error = message::Error_FailedToResetDailyGameNotEnoughJewel;
			}
			else
			{
				if (_info.daily_game_hp_pct() != 100)
				{
					_info.set_daily_game_progress(0);
					_info.set_daily_game_record_progress(0);
					_info.set_daily_game_hp_pct(100);
					int cur_jewel = _info.jewel() - jewel;
					_info.set_jewel(cur_jewel);
				}
				else
				{

				}

				_last_daily_reset_game_time = g_server_time;
			}
		}
	}
	_info.set_daily_reset_game_count(reset_game_count);
	message::MsgS2CResetDailyGameProgressACK msgACK;
	msgACK.set_error(error);
	msgACK.set_daily_game_progress(_info.daily_game_progress());
	msgACK.set_daily_reset_game_count(_info.daily_reset_game_count());
	msgACK.set_last_reset_daily_game_time(_last_daily_reset_game_time);
	sendPBMessage(&msgACK);
}

message::MsgTaskConfigInfo DreamHero::RadnomTaskInfo(int give_up_task)
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
			if (_info.tasks(i).taskid() == config_entry.taskid() || give_up_task == config_entry.taskid())
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
		require_section = config_entry.require_unlock_section();
		if (require_chapter != 0)
		{
			need_continue = true;
			int record_size = _info.records_size();
			for (int i = 0; i < record_size; i ++)
			{
				message::MsgIntPair pair_entry = _info.records(i);
				if (pair_entry.number_1() ==  require_chapter)
				{
					if (require_section <= pair_entry.number_2())
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
	int current_gold = _info.gold();
	msgACK.set_current_gold(current_gold);
	
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

	if (en_error == message::Error_NO)
	{
		int unlock_gold_record = current_gold - _info.gold();
		gRecordManager.chapterUnlockRecord(_account, _info.name().c_str(), chapter_id_temp, unlock_gold_record, _info.gold());
		
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
	int current_gold = _info.gold();
	int all_task_gold = 0;
	int cur_complete_task_count = _info.complete_task_count();
	if (chapter_id_temp == -1&& section_id_temp == -1 || chapter_id_temp != _current_chapter || section_id_temp != _current_section)
	{
		msgACK.set_error(message::Error_NotEnterTheExitGame);
	}
	else
	{
		int use_lotion_size = msg->use_lotions_size();
		if (use_lotion_size != 0)
		{
			std::vector<int> vclotions;
			int lostion_size = _info.lotions_size();
			for (size_t i = 0; i < lostion_size; i++)
			{
				vclotions.push_back(_info.lotions(i));
			}
			for (size_t i = 0; i < use_lotion_size; i++)
			{
				if (vclotions.empty())
				{
					msgACK.set_error(message::Error_FailedExitGameNotFoundTheUseLotion);
					break;
				}
				int current_lotion_id = msg->use_lotions(i);
				bool find_ret = false;
				std::vector<int>::iterator it = vclotions.begin();
				for (; it != vclotions.end(); ++it)
				{
					if (current_lotion_id == (*it))
					{
						find_ret = true;
						break;
					}
				}

				if (find_ret == false)
				{
					msgACK.set_error(message::Error_FailedExitGameNotFoundTheUseLotion);
					break;
				}

			}
		}
		if (msgACK.error() == message::Error_NO)
		{
			if (use_lotion_size != 0)
			{
				std::vector<int> vc_current_lotions;
				int current_lotion_size = _info.lotions_size();
				for (size_t i = 0; i < current_lotion_size; i++)
				{
					bool use_lotion = false;
					int current_lotion = _info.lotions(i);
					int use_lotion_size = msg->use_lotions_size();
					for (size_t j = 0; j < use_lotion_size; j++)
					{
						if (msg->use_lotions(j) == current_lotion)
						{
							use_lotion = true;
							break;
						}						
					}
					if (use_lotion == false)
					{
						vc_current_lotions.push_back(current_lotion);
					}
				}
				std::vector<int>::iterator it_current_lotions = vc_current_lotions.begin();
				_info.clear_lotions();
				msgACK.clear_current_lotions();
				for (; it_current_lotions != vc_current_lotions.end(); ++ it_current_lotions)
				{
					int current_lotion_id = *it_current_lotions;
					_info.add_lotions(current_lotion_id);
					msgACK.add_current_lotions(current_lotion_id);
				}

			}
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
							cur_complete_task_count++;
							temp_repeated->erase(it);
							all_task_gold += task_gift_gold;

							gRecordManager.taskCompleteRecord(_account, _info.name().c_str(), _current_chapter, _current_section, task_id, task_gift_gold, all_task_gold + _info.gold());
							break;
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
			if (msg->success())
			{
				bool find_chapter = false;
				int record_size = _info.records_size();
				for (int i = 0; i < record_size; i++)
				{
					message::MsgIntPair* pair_entry = _info.mutable_records(i);
					if (pair_entry->number_1() == chapter_id_temp)
					{
						if (section_id_temp == pair_entry->number_2() + 1)
						{
							pair_entry->set_number_2(section_id_temp);
							find_chapter = true;
						}
					}
				}
			}
			msgACK.set_current_gold(gold_entry);
			_info.set_gold(gold_entry);
			_info.set_complete_task_count(cur_complete_task_count);
			std::pair<int, int> pair_entry;
			pair_entry.first = _current_chapter;
			pair_entry.second = _current_section;
			SPECIALKILLS::iterator it_special_kill = _special_kills.find(pair_entry);
			if (it_special_kill == _special_kills.end())
			{
				std::vector<message::MsgObjConfig> vc_temp;
				_special_kills.insert(SPECIALKILLS::value_type(pair_entry, vc_temp));
			}
			int special_kill_size = msg->special_kill_list_size();
			for (int i = 0; i < special_kill_size; i++)
			{
				const message::MsgObjConfig obj_config_entry = msg->special_kill_list(i);
				bool need_add = true;
				std::vector<message::MsgObjConfig>::iterator it_special_temp = _special_kills[pair_entry].begin();
				for (; it_special_temp != _special_kills[pair_entry].end(); ++it_special_temp)
				{
					if (it_special_temp->id() == obj_config_entry.id() && it_special_temp->type() == obj_config_entry.type())
					{
						need_add = false;
						break;
					}
				}
				if (need_add)
				{
					_special_kills[pair_entry].push_back(obj_config_entry);
				}
			}
			int special_creatures_size = msg->special_creatures_size();
			for (int i = 0; i < special_creatures_size; i++)
			{
				const message::MsgIntPair entry_pair = msg->special_creatures(i);
				_special_creatures[entry_pair.number_1()] = entry_pair.number_2();
			}
		}
	}
	if (msgACK.error() == message::Error_NO)
	{
		int gold_modify_leave_record = _info.gold() - current_gold;
		int monster_gold = gold_modify_leave_record - all_task_gold;
		
		gRecordManager.leaveGameRecord(_account, _info.name().c_str(), _current_chapter,
			_current_section, msg->success(), gold_modify_leave_record, _info.gold());
		gRecordManager.goldModifyRecord(_account, _info.name().c_str(), monster_gold, _info.gold() - all_task_gold, RecordManager::GoldModify_KillMonster);
		gRecordManager.goldModifyRecord(_account, _info.name().c_str(), all_task_gold, _info.gold(), RecordManager::GoldModify_CompleteTask);
		gRecordManager.goldModifyRecord(_account, _info.name().c_str(), gold_modify_leave_record, _info.gold(), RecordManager::GoldModify_LeaveGame);
	}
	_current_chapter = -1;
	_current_section = -1;
	
	msgACK.set_complete_task_count(cur_complete_task_count);
	fillSpecialCreatureList(msgACK.mutable_special_creatures());	
	sendPBMessage(&msgACK);	
}

void DreamHero::fillSpecialCreatureList(::google::protobuf::RepeatedPtrField< ::message::MsgIntPair >* list)
{
	SPECIALCREATURES::iterator it_special_creatures = _special_creatures.begin();
	for (; it_special_creatures != _special_creatures.end(); ++it_special_creatures)
	{
		message::MsgIntPair* entry_pair = list->Add();
		entry_pair->set_number_1(it_special_creatures->first);
		entry_pair->set_number_2(it_special_creatures->second);
	}
}

void DreamHero::RefreshTask(int give_up_task_id, bool gold)
{

	message::GameError msgError = message::Error_NO;
	int current_gold = _info.gold();
	if (current_gold < gGameConfig.getGlobalConfig().refresh_task_gold_ && gold == true)
	{
		msgError = message::Error_RefreshAdvertisementTaskFailedNotEnoughGold;
	}
	else if (_current_task_count > gGameConfig.getGlobalConfig().day_free_task_count_)
	{
		s64 diff_time = g_server_time - _last_task_advertisement_time;
		if (diff_time < gGameConfig.getGlobalConfig().day_task_advertisement_task_cd_)
		{
			msgError = message::Error_RefreshAdvertisementTaskFailedInCD;
		}
	}

	google::protobuf::RepeatedPtrField< message::MsgTaskInfo >* repeated_tasks = _info.mutable_tasks();
	if (msgError == message::Error_NO)
	{		
		if (give_up_task_id != 0)
		{
			msgError = message::Error_RefreshAdvertisementTaskFailedNotFoundGiveUpTaskID;
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
				_current_task_count++;
				_last_task_advertisement_time = g_server_time;
			}
			else
			{
				msgError = message::Error_RefreshAdvertisementTaskFailedUnknow;
			}
		}		
	}
	message::MsgTaskConfigInfo info_task_config = RadnomTaskInfo(give_up_task_id);
	bool have_task = false;
	if (info_task_config.taskid() != 0 && msgError == message::Error_NO)
	{
		have_task = true;
	}

	if (have_task)
	{
		message::MsgTaskInfo* entry = repeated_tasks->Add();
		entry->set_taskid(info_task_config.taskid());
		entry->set_argument_1(0);
		entry->set_usetime(0);
	}

	if (msgError == message::Error_NO)
	{
		if (info_task_config.taskid() != 0)
		{
			int refresh_gold = 0;
			if (gold == true)
			{
				refresh_gold = gGameConfig.getGlobalConfig().refresh_task_gold_;
			}
			current_gold = current_gold - gGameConfig.getGlobalConfig().refresh_task_gold_;
			_info.set_gold(current_gold);
			gRecordManager.goldModifyRecord(_account, _info.name().c_str(), -refresh_gold, current_gold, RecordManager::GoldModify_AcceptTask);
			gRecordManager.taskAccepteRecordRecord(_account, _info.name().c_str(), refresh_gold,current_gold, info_task_config.taskid());
			
		}		
	}

	if (give_up_task_id != 0)
	{
		gRecordManager.taskGiveUpRecord(_account, _info.name().c_str(), give_up_task_id);
		message::MsgS2CAdvertisementRefreshTaskACK msgACK;
		msgACK.set_give_up_task_id(give_up_task_id);
		if (have_task)
		{
			message::MsgTaskConfigInfo* info = msgACK.add_infos();
			info->CopyFrom(info_task_config);
		}
		
		msgACK.set_current_gold(current_gold);
		msgACK.set_error(msgError);
		sendPBMessage(&msgACK);
	}
	else
	{
		message::MsgS2CAdvertisementApplyTaskACK msgACK;
		msgACK.set_error(msgError);
		if (have_task)
		{
			message::MsgTaskConfigInfo* info = msgACK.add_infos();
			info->CopyFrom(info_task_config);
		}
		msgACK.set_current_gold(current_gold);
		sendPBMessage(&msgACK);
	}

}

void DreamHero::ReqReliveReq(const message::MsgC2SReliveReq* msg)
{
	message::GameError error = message::Error_NO;
	int current_gold = _info.gold();
	if (current_gold >= gGameConfig.getGlobalConfig().relive_gold_)
	{
		
		current_gold = current_gold - gGameConfig.getGlobalConfig().relive_gold_;
		_info.set_gold(current_gold);
		gRecordManager.reliveRecord(_account, 
			_info.name().c_str(),
			gGameConfig.getGlobalConfig().relive_gold_, current_gold);

		gRecordManager.goldModifyRecord(_account, _info.name().c_str(),
			-gGameConfig.getGlobalConfig().relive_gold_, current_gold, RecordManager::GoldModify_ReliveRecord);

	}
	else
	{
		error = message::Error_ReliveFailedNotEnoughGod;
	}
	
	message::MsgS2CReliveACK msgACK;
	msgACK.set_current_gold(_info.gold());
	msgACK.set_error(error);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqAdvertisementApplyTask(const message::MsgC2SReqAdvertisementApplyTask* msg)
{
	RefreshTask(0, msg->gold());
}


void DreamHero::ReqModifyTaskCompleteCount(const message::MsgC2SCmdReqModifyTaskCompleteCount* msg)
{
	int count = msg->task_complete_count();
	if (count < 0)
	{
		count = 0;
	}
	_info.set_complete_task_count(count);
	message::MsgS2CCmdModifyTaskCompleteCountACK msgACK;
	msgACK.set_current_task_count(_info.complete_task_count());
	sendPBMessage(&msgACK);
}

void DreamHero::ReqReplaceTask(const message::MsgS2CCmdReqReplaceTask* msg)
{
	int task_count = msg->task_count();
	if (task_count > gGameConfig.getGlobalConfig().hero_max_tasks_count_)
	{
		task_count = gGameConfig.getGlobalConfig().hero_max_tasks_count_;
	}
	_info.clear_tasks();

	message::MsgS2CCmdReplaceTaskACK msgACK;
	for (size_t i = 0; i < task_count; i++)
	{
		message::MsgTaskConfigInfo info_task_config = RadnomTaskInfo();
		if (info_task_config.taskid() != 0)
		{
			message::MsgTaskConfigInfo* entry = msgACK.add_infos();
			entry->CopyFrom(info_task_config);

			message::MsgTaskInfo* entry_info = _info.mutable_tasks()->Add();
			entry_info->set_taskid(info_task_config.taskid());
			entry_info->set_argument_1(0);
			entry_info->set_usetime(0);
		}
	}
	msgACK.set_error(message::Error_NO);
	sendPBMessage(&msgACK);

}
void DreamHero::ReqModifyCurrentHero(int grid_id)
{
	int current_grid_id = grid_id;
	int heroes_length = _info.heroes_size();
	message::GameError en = message::Error_NO;
	message::MsgS2CModifyCurrentHeroACK msg;	
	if (heroes_length > current_grid_id)
	{
		bool b_character = _info.heroes(current_grid_id);
		if (b_character)
		{
			_info.set_current_hero(current_grid_id);
		}
		else
		{
			en = message::Error_ModifyCurrentFailedTheCharacterIsLock;
		}

	}
	else
	{
		en = message::Error_ModifyCurrentFailedTheCharacterIsLock;
	}
	msg.set_current_grid(_info.current_hero());
	msg.set_error(en);
	sendPBMessage(&msg);
}

void DreamHero::ReqResetMap(const message::MsgC2SCmdReqResetMap* msg)
{
	msg->chapter_id();
	msg->section_id();
	std::pair<int, int> pair_entry;
	pair_entry.first = msg->chapter_id();
	pair_entry.second = msg->section_id();
	SPECIALKILLS::iterator it = _special_kills.find(pair_entry);

	if (it != _special_kills.end())
	{
		_special_kills.erase(it);
	}
	message::MsgS2CCmdResetMapACK msgACK;
	msgACK.set_chapter_id(msg->chapter_id());
	msgACK.set_section_id(msg->section_id());
	msgACK.set_error(message::Error_NO);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqModifyGold(const message::MsgC2SCmdReqModifyGold* msg)
{
	int current_gold = _info.gold();
	message::MsgS2CCmdModifyGoldACK msgACK;
	current_gold += msg->gold();
	if (current_gold < 0)
	{
		current_gold = 0;
	}
	_info.set_gold(current_gold);
	msgACK.set_gold(current_gold);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqBuyHero(const message::MsgC2SReqBuyHero* msg)
{
	int buy_grid =  msg->grid();
	int buy_gold =  msg->gold();
	const message::MsgShopConfigInfo* buy_grid_config = gGameConfig.getShopConfig(buy_grid);
	message::MsgS2CBuyHeroACK msgACK;
	msgACK.set_grid(buy_grid);
	msgACK.set_current_gold(_info.gold());
	int current_gold = _info.gold();
	message::GameError en = message::Error_NO;
	if (buy_grid_config == NULL)
	{
		en = message::Error_BuyHeroFailedNotFoundGrid;
	}
	else
	{
		
		int require_chapter_id = buy_grid_config->require_chapter_id();
		int require_section_id = buy_grid_config->require_section_id();
		if (require_chapter_id != 0 && require_section_id != 0)
		{
			en = message::Error_BuyHeroFailedNotPassRequiredSection;
			int record_size = _info.records_size();
			for (int i = 0; i < record_size; i ++)
			{
				if (_info.records(i).number_1() == require_chapter_id && require_section_id <= _info.records(i).number_2())
				{
					en = message::Error_NO;
					break;
				}
				
			}
		}

		if (en == message::Error_NO)
		{
			int cheap_gold = gShopSalesPromotionManager.getCheapGold(buy_grid);
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
					if (heroes_length <= buy_grid + 1)
					{
						for (size_t i = 0; i <= buy_grid + 1 - heroes_length; i++)
						{
							_info.add_heroes(false);
						}
					}
					_info.set_heroes(buy_grid, true);
					heroes_length = _info.heroes_size();
				}
			}
		}
	}
	if (en == message::Error_NO)
	{
		int buy_hero_gold_record = current_gold - _info.gold();
		gRecordManager.buyHeroRecord(_account, _info.name().c_str(), buy_grid, buy_hero_gold_record, _info.gold());
		

	}
	msgACK.set_current_gold(_info.gold());
	msgACK.set_error(en);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqModifyTutorialFlag(const message::MsgC2SReqModifyNewTutorial* msg)
{
	_info.set_new_tutorial(msg->flag());
	message::MsgS2CModifyNewTutorialACK msgACK;
	msgACK.set_flag(_info.new_tutorial());
	sendPBMessage(&msgACK);
}

void DreamHero::ReqAdvertisementRefreshTask(const message::MsgC2SReqAdvertisementRefreshTask* msg)
{

	int give_up_task_id_temp = msg->give_up_task_id();
	RefreshTask(give_up_task_id_temp, msg->gold());
}


void DreamHero::EnterGame(int chapter_id, int section_id, bool admin)
{
	
	int use_spirit_config = gGameConfig.getGlobalConfig().config_enter_game_use_spirit_;
	int spirit = _info.spirit();
	int chapter_id_temp = chapter_id;
	int section_id_temp = section_id;
	int records_length = _info.records_size();
	message::MsgS2CEnterGameACK msgACK;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_section_id(section_id_temp);
	message::GameError en_error = message::Error_CanNotEnterGameTheInstanceIsLock;
	if (admin)
	{
		en_error = message::Error_NO;
	}
	else
	{
		en_error = message::Error_CanNotEnterGameTheInstanceIsLock;
		if (chapter_id_temp == -1 && section_id_temp == -1)
		{

		}
		else
		{
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
						en_error = message::Error_NO;
					}
					else
					{
						en_error = message::Error_CanNotEnterGameTheSectionIsLock;
					}
					break;
				}
			}

			if (en_error == message::Error_NO)
			{
				if (use_spirit_config != 0)
				{
					if (spirit < use_spirit_config)
					{
						en_error = message::Error_CanNotEnterGameNotEnoughSpirit;
					}
				}
			}


		}
	}

	if (en_error == message::Error_NO)
	{
		std::pair<int, int> pair_entry;
		pair_entry.first = chapter_id_temp;
		pair_entry.second = section_id_temp;
		SPECIALKILLS::iterator it = _special_kills.find(pair_entry);
		if (it != _special_kills.end())
		{
			std::vector<message::MsgObjConfig>::iterator it_obj_config = it->second.begin();
			for (; it_obj_config != it->second.end(); ++ it_obj_config)
			{				
				message::MsgObjConfig* temp_entry = msgACK.add_kill_list();
				temp_entry->CopyFrom((*it_obj_config));
			}
		}

		const MAPTYPEDROPBOXCONFIGS* type_map_dropboxs = gGameConfig.getMapDropBox(chapter_id_temp, section_id_temp);
		if (type_map_dropboxs != NULL)
		{
			MAPTYPEDROPBOXCONFIGS::const_iterator it_type_drop = type_map_dropboxs->begin();
			for (; it_type_drop != type_map_dropboxs->end(); ++it_type_drop)
			{
				MAPDROPBOXCONFIGS::const_iterator it_map_drop = it_type_drop->second.begin();
				for (; it_map_drop != it_type_drop->second.end(); ++it_map_drop)
				{
					message::MsgDropBoxConfig* drop_box_entry = msgACK.add_drop_box_configs();
					ObjDropBoxConfig box_entry = it_map_drop->second;
					drop_box_entry->set_base_gold(box_entry.base_gold_);
					drop_box_entry->set_random_gold(box_entry.random_gold_);
					drop_box_entry->mutable_obj()->set_id(box_entry.obj_id_);
					drop_box_entry->mutable_obj()->set_type(box_entry.type_);
				}
			}
		}
		_current_chapter = chapter_id_temp;
		_current_section = section_id_temp;
				
		spirit = spirit - use_spirit_config;
		if (spirit < 0)
		{
			spirit = 0;
		}
		_info.set_spirit(spirit);
		
	}
	if (en_error == message::Error_NO)
	{
		gRecordManager.enterGameRecord(_account, _info.name().c_str(), chapter_id_temp, section_id_temp, _info.gold());
	}
	msgACK.set_spirit(_info.spirit());
	msgACK.set_error(en_error);
	sendPBMessage(&msgACK);
}

int	DreamHero::getSpirit()
{
	return _info.spirit();
}
void DreamHero::setSpirit(int spirit)
{
	_info.set_spirit(spirit);
}
int	DreamHero::getJewel()
{
	return _info.jewel();
}
void DreamHero::setJewel(int jewel)
{
	_info.set_jewel(jewel);
}


void DreamHero::SendResetGameACK(message::GameError en)
{
	message::MsgS2CCmdResetGameACK msg;
	msg.mutable_info()->CopyFrom(_info);
	msg.set_error(en);
	msg.set_current_advertisement_count(_current_task_count);
	msg.set_last_advertisement_time(_last_task_advertisement_time);
	sendPBMessage(&msg);
}

void DreamHero::addDealPay(std::string key_code, int status, int order_id, message::GameError error,bool send_msg)
{
	message::MsgS2CVerifyDealACK msg;
	//message::GameError error = message::Error_NO;
	int add_gold = 0;
	int current_gold = _info.gold();
	if (error == message::Error_NO )
	{
		const GoldShopConfigInfo* entry_config = gGameConfig.getGoldShopConfigInfo(key_code.c_str());
		
		if (entry_config)
		{
			DEALSWAITTOPAY::iterator it = _deals_wait_to_pay.find(order_id);
			if (it != _deals_wait_to_pay.end())
			{
				if (status == 0)
				{
					if (_deals_wait_to_pay[order_id].type_ == DealStatusType_WaitToPay || _deals_wait_to_pay[order_id].type_ == DealStatusType_WaitPrepareToPay)
					{
						_deals_wait_to_pay[order_id].type_ = DealStatusType_Complete;

						add_gold = entry_config->info_.gold();
						int gold_entry = current_gold + add_gold;
						_info.set_gold(gold_entry);
						error = message::Error_NO;
					}
					else
					{
						error = message::Error_BuyGoldFailedTheOrderHaveBeenCompleted;

					}
				}
				else
				{
					_deals_wait_to_pay[order_id].type_ = DealStatusType_Failed;
					error = message::Error_Unknow;
				}
			}
			else
			{
				error = message::Error_BuyGoldFailedNotFoundOrder;
			}
		}
		else
		{
			error = message::Error_BuyGoldFailedNotFoundConfig;
		}
	}
	else
	{
		error = message::Error_Unknow;
	}

	if (send_msg)
	{
		msg.set_error(error);
		msg.set_order_id(order_id);
		msg.set_current_gold(_info.gold());
		msg.set_product_id(key_code.c_str());
		msg.set_status(status);
		msg.set_error(error);
		sendPBMessage(&msg);
	}	
	if (error == message::Error_NO)
	{
		gRecordManager.goldModifyRecord(_account, _info.name().c_str(), add_gold, _info.gold(), RecordManager::GoldModify_BuyHero);
		gRecordManager.dealPayRecord(_account, key_code.c_str(), status, order_id, add_gold, _info.gold());
	}
}

void DreamHero::addDealWaitToPay(std::string key_code, int status, int price, int order_id, const char* notify_url, message::GameError error)
{
	if (error == message::Error_NO)
	{
		DEALSWAITTOPAY::iterator it = _deals_wait_to_pay.find(order_id);
		if (it != _deals_wait_to_pay.end())
		{

		}
		else
		{
			DealWaitToPay entry;
			entry.key_code_ = key_code;
			entry.status_ = status;
			entry.price_ = price;
			entry.order_id_ = order_id;
			entry.type_ = DealStatusType_WaitToPay;
			_deals_wait_to_pay.insert(DEALSWAITTOPAY::value_type(entry.order_id_, entry));

		}
	}
	
	message::MsgS2CCrearteDealACK msg;
	msg.set_key_code(key_code.c_str());
	msg.set_status(status);
	msg.set_price(price);
	msg.set_order_id(order_id);
	msg.set_error(error);
	msg.set_platform_url(notify_url);
	sendPBMessage(&msg);
	if (error == message::Error_NO)
	{
		gRecordManager.dealWaitToPayRecord(_account, key_code.c_str(), status, price, order_id);
	}

}

void DreamHero::ResetGame()
{
	LoadFromConfig();
	SendResetGameACK(message::Error_NO);
}

void DreamHero::ReqEnterGame(const message::MsgC2SReqEnterGame* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int section_id_temp = msg->section_id();	
	EnterGame(chapter_id_temp, section_id_temp, false);
}

void DreamHero::ReqCrearteDeal(const message::MsgC2SReqCrearteDeal* msg)
{
	DEALSWAITTOPAY::iterator it_deal = _deals_wait_to_pay.begin();
	while (it_deal != _deals_wait_to_pay.end())
	{
		if (it_deal->second.type_ == DealStatusType_WaitToPay)
		{
			gRecordManager.giveUpDealRecord(_account, it_deal->second.key_code_.c_str(), it_deal->second.status_, 
				it_deal->second.price_, it_deal->second.order_id_);
#ifdef WIN32
			it_deal = _deals_wait_to_pay.erase(it_deal);
#else
			_deals_wait_to_pay.erase(it_deal);
			++it_deal;
#endif
		}
		else
		{
			++it_deal;
		}
		

	}
	std::string key_code = msg->key_code();
	CreateDealHttpTask* entry = new CreateDealHttpTask();
	entry->init(_account, _info.name().c_str(), key_code.c_str(), msg->channel());
	gHttpManager.addHttpTask(entry);
}
void DreamHero::ReqVerifyDeal(const message::MsgC2SReqVerifyDeal* msg)
{
	std::string receipt = msg->receipt().c_str();
	VerifyDealHttpTaskIOS* entry = new VerifyDealHttpTaskIOS();
	
	char sz_temp[10240];
	gRecordManager.dealWaitToVerifyRecord(_account, _info.name().c_str(), msg->order_id(), receipt.c_str());
	sprintf(sz_temp, "replace into deal_wait_to_pay(`order_id`, `complete_status`, `receipt`) \
				values(%d, %d, '%s') ", msg->order_id(), DealStatusType_WaitPrepareToVerify, receipt.c_str());
	gDreamHeroManager.addSql(sz_temp);
	entry->init(_account, _info.name().c_str(), receipt.c_str(), msg->order_id());
	gHttpManager.addHttpTask(entry);
}

void DreamHero::SendClientInit()
{
	StopDestroyClock();
	dayRefresh(false);
	StartSave();
	message::MsgS2CHeroesInit msg;
	msg.mutable_info()->CopyFrom(_info);
	int length = _info.tasks_size();
	for (size_t i = 0; i < length; i++)
	{
		const message::MsgTaskInfo entry =  _info.tasks(i);
		if (entry.taskid() != 0)
		{
			const message::MsgTaskConfigInfo* entry_config = gGameConfig.getMapTask(entry.taskid());
			if (entry_config != NULL)
			{
				message::MsgTaskConfigInfo* cur_entry = msg.add_task_config_infos();
				cur_entry->CopyFrom(*entry_config);
			}
			else
			{
				//need log;
			}
		}
	}
	const MAPSPIRITSHOP* spirit_shop_infos = gGameConfig.getSpiritShop();
	MAPSPIRITSHOP::const_iterator it_spirit = spirit_shop_infos->begin();
	for (; it_spirit != spirit_shop_infos->end(); ++ it_spirit)
	{
		msg.add_sprit_shop_infos()->CopyFrom(it_spirit->second);
		//msg.
	}

	const MAPLOTIONSHOPCONFIGS* lotion_shop_configs = gGameConfig.getLotionShop();
	MAPLOTIONSHOPCONFIGS::const_iterator it_lotion_shop_config = lotion_shop_configs->begin();
	for (; it_lotion_shop_config != lotion_shop_configs->end(); ++ it_lotion_shop_config)
	{
		msg.add_lotion_shop_configs()->CopyFrom(it_lotion_shop_config->second);
	}
	msg.set_current_advertisement_count(_current_task_count);
	msg.set_last_advertisement_time(_last_task_advertisement_time);
	msg.set_gm_level(getGMLevel());
	const globalConfig& globalConfig = gGameConfig.getGlobalConfig();

	msg.set_free_advertisement_config_count(globalConfig.day_free_task_count_);
	msg.set_advertisement_time_cd(globalConfig.day_task_advertisement_task_cd_);
	msg.set_refresh_task_gold(globalConfig.refresh_task_gold_);
	msg.set_relive_gold(globalConfig.relive_gold_);
	msg.set_max_spirit(globalConfig.config_max_spirit_);
	msg.set_config_recover_spirit_minute(globalConfig.config_recover_spirit_minute_);
	msg.set_config_recover_spirit(globalConfig.config_recover_spirit_);
	msg.set_config_enter_game_use_spirit(globalConfig.config_enter_game_use_spirit_);
	msg.set_config_day_buy_spirit(globalConfig.config_day_buy_spirit_);
	msg.set_daily_game_time(_daily_game_time);
	msg.set_last_buy_spirit_time(_last_buy_spirit_time);
	msg.set_last_recover_spirit_time(_last_recover_spirit_time);
	msg.set_day_refresh_time(globalConfig.day_Refresh_time_);
	msg.set_last_lottery_time(_last_day_lottery_time);
	msg.set_server_time(g_server_time);
	msg.set_last_reset_daily_game_time(_last_daily_reset_game_time);
	fillSpecialCreatureList(msg.mutable_special_creatures());
	int daily_jewel_sonfig_size = globalConfig.daily_game_reset_jewel_config_.size();
	for (int i = 0; i < daily_jewel_sonfig_size; i ++)
	{
		msg.add_reset_jewel_configs(globalConfig.daily_game_reset_jewel_config_[i]);
	}

	int daily_game_record_size = globalConfig.daily_game_record_config_.size();
	for (int i = 0; i < daily_game_record_size; i ++)
	{
		msg.add_daily_game_record_configs(globalConfig.daily_game_record_config_[i]);
	}
	
	sendPBMessage(&msg);

	
	_online = true;
	Mylog::log_player(LOG_INFO, "send account init account[%d] name[%s]", _account, _info.name().c_str());
	//StartPing();
	gRecordManager.loginRecord(_account, _info.name().c_str());
}


void DreamHero::LoadFromConfig()
{
	_special_kills.clear();
	_special_creatures.clear();
	_current_chapter = 0;
	_current_section = 0;
	_current_task_count = 0;
	_last_task_advertisement_time = 0;
	_info.mutable_tasks()->Clear();
	_info.mutable_records()->Clear();
	int length = gGameConfig.getGlobalConfig().hero_unlock_count_;
	_info.clear_heroes();
	for (size_t i = 0; i < length; i++)
	{
		_info.add_heroes(true);
	}
	message::MsgIntPair* pair_entry = _info.mutable_records()->Add();
	pair_entry->set_number_1(1);
	pair_entry->set_number_2(1);
	message::MsgIntPair* pair_entry_1 = _info.mutable_records()->Add();
	pair_entry_1->set_number_1(0);
	pair_entry_1->set_number_2(0);
	_info.set_gold(gGameConfig.getGlobalConfig().config_gold_);
	_info.set_complete_task_count(0);
	
	_info.set_current_hero(0);
	std::string name = _parent->generateName();
	_info.set_name(name.c_str());
	_info.set_new_tutorial(0);
	_ping_count = 0;
	_info.set_jewel(gGameConfig.getGlobalConfig().config_jewel_);
	_info.set_spirit(gGameConfig.getGlobalConfig().config_max_spirit_);
	_last_recover_spirit_time = g_server_time;
	SetGMLevel(1);
}


void DreamHero::ResetDailyGame()
{
	_info.set_daily_game_hp_pct(100);
	//_info.set_daily_game_gold(0);
	_info.set_daily_game_progress(0);
	_info.set_daily_game_record_progress(0);
	_info.set_daily_reset_game_count(0);
	_daily_game_time = 0;
	_daily_game_prize_time = 0;
	_last_daily_reset_game_time = 0;
}
void DreamHero::ResetDailyLottery()
{
	_last_day_lottery_time = 0;
}


void DreamHero::ReqApplyHeroDeal(const message::MsgC2SReqApplyDeal* msg)
{
	message::MsgS2CApplyDealACK msgACK;
	int id = msg->id();
	msgACK.set_id(id);
	msgACK.set_order_id("");
	msgACK.set_product_id("");
	msgACK.set_error(message::Error_NO);
	const GoldShopConfigInfo* config_entry = gGameConfig.getGoldShopConfigInfo(id);
	if (config_entry != NULL)
	{
		std::string order_id = gDreamHeroManager.generateDealOrderID(_account);
		CreateDealHttpTask* entry = new CreateDealHttpTask();
		entry->init(_account, _info.name().c_str(), config_entry->appstore_product_id_.c_str(), gHttpManager.getChannel());
		gHttpManager.addHttpTask(entry);		
	}
	else
	{
		msgACK.set_error(message::Error_FailedToApplyDealOrderNotFooundConfig);
	}
	sendPBMessage(&msgACK);
}

void DreamHero::ReqBuySpirit(const message::MsgC2SReqBuySpirit* msg)
{
	message::GameError error = message::Error_NO;
	message::MsgS2CBuySpiritACK msgACK;
	int day_buy_spirit = _info.day_buy_spirit();
	int config_day_buy_spirit = gGameConfig.getGlobalConfig().config_day_buy_spirit_;
	if (day_buy_spirit >= config_day_buy_spirit)
	{
		error = message::Error_BuySpiritFailedCanNotBuyMoreSpirit;
	}
	else
	{
		const message::MsgSpiritShopInfo* config_info = gGameConfig.getSpiritShop(msg->index());
		if (config_info != NULL)
		{
			int need_jewel = config_info->need_jewel();
			int spirit_config = config_info->spirit();
			if (need_jewel <= _info.jewel())
			{

				int jewel = _info.jewel() - need_jewel;
				int spirit = _info.spirit() + spirit_config;
				_info.set_jewel(jewel);
				_info.set_spirit(spirit);
			}
			else
			{
				error = message::Error_BuySpiritFailedNotEnoughJewel;
			}
		}
		else
		{
			error = message::Error_BuySpiritFailedNotFoundConfig;
		}
	}

	msgACK.set_index(msg->index());
	msgACK.set_jewel(_info.jewel());
	msgACK.set_spirit(_info.spirit());
	msgACK.set_error(error);
	
	sendPBMessage(&msgACK);

}

void DreamHero::ReqReceiveDailyGamePrize()
{
	message::MsgS2CReceiveDailyGamePrizeACK msg;
	int daily_game_gold = _info.daily_game_gold();
	message::GameError error = message::Error_NO;
	if (daily_game_gold == 0)
	{
		error = message::Error_FailedToReceivedDailyGamePrizeNOPrize;
	}
	else
	{
		int gold = _info.gold() + daily_game_gold;
		_info.set_gold(gold);
	}

	msg.set_current_gold(_info.gold());
	msg.set_error(error);
	sendPBMessage(&msg);
}

void DreamHero::SaveHero()
{
	std::string sql_temp;
	int length = _info.records_size();
	std::string record_temp;
	std::string heroes_temp;
	std::string tasks_temp;
	std::string special_kill_temp;
	std::string special_creatures;
	char sz_temp[256];
	char sz_temp_1[128];
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

	SPECIALKILLS::iterator it = _special_kills.begin();
	special_kill_temp.clear();
	for (; it != _special_kills.end(); ++ it)
	{
		std::string temp_str;
		std::vector<message::MsgObjConfig>::iterator it_config = it->second.begin();
		for (; it_config != it->second.end(); ++ it_config)
		{
			temp_str += ":";			
			sprintf(sz_temp_1, "%d,%d", (*it_config).id(), (int)(*it_config).type());
			temp_str += sz_temp_1;
		}

		if (temp_str.empty() == false)
		{
			if (special_kill_temp.empty() == false)
			{
				special_kill_temp += ";";
			}
			sprintf(sz_temp, "%d,%d", it->first.first, it->first.second);
			special_kill_temp += sz_temp;
			special_kill_temp += temp_str;
		}
	}
	char temp[4096];
	std::string last_task_advertisement_time_temp;
	std::string last_recover_spirit_time;
	std::string last_buy_spirit_time;
	std::string last_lottery_time;
	std::string daily_game_time;
	std::string daily_game_prize_time;
	std::string last_reset_daily_game_time;

	SPECIALCREATURES::iterator it_special_creatures = _special_creatures.begin();
	for (int i = 0; it_special_creatures != _special_creatures.end(); ++ it_special_creatures, i ++)
	{
		if (i != 0)
		{
			special_creatures += ";";
		}
		sprintf(sz_temp_1, "%d,%d", it_special_creatures->first, it_special_creatures->second);
		special_creatures += sz_temp_1;
	}
	build_unix_time_to_string(_last_daily_reset_game_time, last_reset_daily_game_time);
	build_unix_time_to_string(_last_task_advertisement_time, last_task_advertisement_time_temp);
	build_unix_time_to_string(_last_recover_spirit_time, last_recover_spirit_time);
	build_unix_time_to_string(_last_buy_spirit_time, last_buy_spirit_time);
	build_unix_time_to_string(_last_day_lottery_time, last_lottery_time);
	build_unix_time_to_string(_daily_game_time, daily_game_time);
	build_unix_time_to_string(_daily_game_prize_time, daily_game_prize_time);

	int lotion_size =  _info.lotions_size();
	std::string str_lotion_status;
	char sz_lotion_status[128];
	for (int i = 0; i < lotion_size; i ++)
	{
		if (i != 0)
		{
			str_lotion_status += ",";
		}

		sprintf(sz_lotion_status, "%d", _info.lotions(i));
		str_lotion_status += sz_lotion_status;
	}
	
//#ifdef WIN32
	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,`special_kill`,\
		`current_hero`, `current_chapter`, `current_section`, `complete_task_count`, `special_creatures`, \
		 `free_task_count`,`last_task_advertisement_time`,`gm_level`, `current_task_count`, `tutorial_flag`,\
		 `jewel`, `spirit`,`last_recover_spirit_time`,`day_buy_spirit`, `last_buy_spirit_time`, `lotions`, `last_lottery_time`,\
		 `daily_game_progress`, `daily_game_score`, `daily_game_gold`, `daily_game_time`, `daily_game_prize_time`,`daily_game_hp_pct`,\
		 `daily_game_record_progress`, `daily_reset_game_count`, `last_daily_reset_game_time`, `daily_top_grogress`) values \
		(%llu, '%s', %d, '%s', '%s', '%s', '%s', %d, %d, %d, %d, '%s',%d, '%s', %d, %d, %d, %d, %d,\
		 '%s', %d, '%s', '%s', '%s', %d, %d, %d, '%s','%s', %d, %d, %d, '%s', %d);",
		_account, _info.name().c_str(), _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), 
		special_kill_temp.c_str(), _info.current_hero(), _current_chapter,
		_current_section, _info.complete_task_count(), special_creatures.c_str(), _current_task_count,
		last_task_advertisement_time_temp.c_str(), _gm_level, _current_task_count, _info.new_tutorial(),
		_info.jewel(), _info.spirit(), last_recover_spirit_time.c_str(), _info.day_buy_spirit(), 
		last_buy_spirit_time.c_str(), str_lotion_status.c_str(), last_lottery_time.c_str(), _info.daily_game_progress(),
		 _info.daily_game_score(), _info.daily_game_gold(), daily_game_time.c_str(), daily_game_prize_time.c_str(), _info.daily_game_hp_pct(),
		_info.daily_game_record_progress(), _info.daily_reset_game_count(), last_reset_daily_game_time.c_str(), _info.daily_top_grogress());
//#else
//	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,\
//		`current_hero`, `current_chapter`, `current_section`, `current_gold`, `complete_task_count`, `free_task_count`,`last_task_advertisement_time`) values \
//		(%llu, 'normal', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s');",
//		_account, _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), _info.current_hero(), _current_chapter,
//		_current_section, _info.gold(), _info.complete_task_count(), _current_task_count, last_task_advertisement_time_temp.c_str());
//#endif


	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(temp);
	gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());

	DEALSWAITTOPAY::iterator it_deal = _deals_wait_to_pay.begin();
	std::string sql_head_deal = "replace into deal_wait_to_pay(`order_id`, `account_id`, `key_code`, `status`, `price`, `deal_time`,`complete_status`) \
				values";
	sql_temp.clear();
	std::string cur_time_entry;
	build_unix_time_to_string(g_server_time, cur_time_entry);
	if (_deals_wait_to_pay.size() != 0)
	{
		for (int i = 0; it_deal != _deals_wait_to_pay.end(); ++it_deal, i++)
		{
			if (i == 0)
			{
				sql_temp = sql_head_deal;
			}
			else
			{
				sql_temp += ",";
			}
			sprintf(sz_temp, "(%d, %llu, '%s', %d, %d, '%s', %d)",
				it_deal->second.order_id_, _account, it_deal->second.key_code_.c_str(),
				it_deal->second.status_, it_deal->second.price_, cur_time_entry.c_str(), it_deal->second.type_);
			sql_temp += sz_temp;
		}
		msg_db.set_sql(sql_temp.c_str());
		gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
		it_deal != _deals_wait_to_pay.begin();
		while (it_deal != _deals_wait_to_pay.end())
		{
			if (it_deal->second.type_ == DealStatusType_Complete)
			{
#ifdef WIN32
				it_deal = _deals_wait_to_pay.erase(it_deal);
#else
				_deals_wait_to_pay.erase(it_deal);
				++it_deal;
#endif
			}
			else
			{
				++it_deal;
			}
		}


	}
}

void DreamHero::set_name(const char* name)
{
	_info.set_name(name);
}

void DreamHero::ReqRemoveAllSpecialCreatureList()
{
	_special_creatures.clear();
	message::MsgS2CCmdNotifySpecialCreatureHisModify msg;
	sendPBMessage(&msg);

}

void DreamHero::ReqSetSpecialCreatureList(int creature_id, int status)
{
	_special_creatures[creature_id] = status;
	message::MsgS2CCmdNotifySpecialCreatureHisModify msg;
	fillSpecialCreatureList(msg.mutable_special_creatures());
	sendPBMessage(&msg);	
}

//void DreamHero::completeDealByOrder(const char* order_id, bool success,bool needmsg)
//{
//	std::map<std::string, message::MsgHeroDealInfo>::iterator it = _hero_deals.find(order_id);
//	if (it != _hero_deals.end())
//	{
//		message::MsgHeroDealInfo& entry = it->second;
//		entry.set_type(message::HeroDealTypeComplete);
//		const GoldShopConfigInfo* config = gGameConfig.getGoldShopConfigInfo(entry.product_id().c_str());
//		if (config != NULL)
//		{
//			if (success)
//			{
//				int gold = _info.gold() + config->info_.gold();
//				int jewel = _info.jewel() + config->info_.jewel();
//				_info.set_gold(gold);
//				_info.set_jewel(jewel);
//			}
//			if (needmsg)
//			{
//				message::MsgS2CNotifyDealComplete msg;
//				msg.set_product_id(entry.product_id().c_str());
//				msg.set_order_id(order_id);
//				msg.set_current_gold(_info.gold());
//				msg.set_current_jewel(_info.jewel());
//				msg.set_success(success);
//				sendPBMessage(&msg);
//
//			}
//		}
//	}
//	else
//	{
//
//	}
//}

void DreamHero::ReqDayLottery(const message::MsgC2SReqDayLottery* msg)
{
	//u64 temp_time = g_server_time - _day_offset_time;
	message::MsgS2CDayLotteryACK msgACK;
	msgACK.set_count(0);
	msgACK.set_current_gold(_info.gold());
	msgACK.set_current_jewel(_info.jewel());
	msgACK.set_type(message::Lottery_Jewel);
	msgACK.set_index(0);
	msgACK.set_error(message::Error_NO);
	int self_lotion_size = _info.lotions_size();
	for (size_t i = 0; i < self_lotion_size; i++)
	{
		msgACK.add_current_lotions(_info.lotions(i));
	}

	
	if (gGameConfig.isInToday(_last_day_lottery_time) == false)
	{
		const VCLOTTERYDRAWBOXSCONFIGS* configs = gGameConfig.getLotteryDrawBoxs();
		VCLOTTERYDRAWBOXSCONFIGS vc_lottery_random;
		VCLOTTERYDRAWBOXSCONFIGS::const_iterator it = configs->begin();
		for (; it != configs->end(); ++ it)
		{
			const LotteryDrawBoxConfig& lotteryConfig = (*it);
			if (lotteryConfig.lottery_type_ == message::Lottery_Lotion)
			{
				int self_lotion_size = _info.lotions_size();
				bool ret_find = false;
				for (size_t i = 0; i < self_lotion_size; i++)
				{
					if (lotteryConfig.sub_index_ == _info.lotions(i))
					{
						ret_find = true;
						break;
					}
				}
				if (ret_find == false)
				{
					vc_lottery_random.push_back(lotteryConfig);
				}				
			}
			else
			{
				vc_lottery_random.push_back(lotteryConfig);
			}			
		}
		if (vc_lottery_random.empty() == false)
		{
			LotteryDrawBoxConfig entry = vc_lottery_random[0];
			std::map<int, LotteryDrawBoxConfig> map_random_lottery;
			it = vc_lottery_random.begin();
			int rating = 0;
			for (; it != vc_lottery_random.end(); ++it)
			{
				const LotteryDrawBoxConfig& entry = (*it);
				map_random_lottery[rating] = entry;
				rating += entry.rating_;
			}
			if (rating > 0)
			{
				_last_day_lottery_time = g_server_time;
				int current_rating = rand() % rating;
				std::map<int, LotteryDrawBoxConfig>::iterator it_map = map_random_lottery.begin();

				for (; it_map != map_random_lottery.end(); ++it_map)
				{
					if (current_rating >= it_map->first)
					{
						break;
					}
					entry = it_map->second;
				}

				if (entry.lottery_type_ == message::Lottery_Lotion)
				{
					if (entry.sub_index_>=0 && entry.sub_index_ <= 3)
					{
						_info.add_lotions(entry.sub_index_);
						msgACK.add_current_lotions(entry.sub_index_);
					}
					else
					{
						msgACK.set_error(message::Error_FailedToLotteryErrorLotionConfig);
					}

				}
				else
				{
					int random_count = 0;
					if (entry.random_count_ > 0)
					{
						random_count = rand() % entry.random_count_;
					}
					int add_count = entry.base_count_ + random_count;
					if (add_count != 0)
					{
						if (entry.lottery_type_ == message::Lottery_Gold)
						{
							int gold = _info.gold();
							gold += add_count;
							_info.set_gold(gold);
							
							msgACK.set_current_gold(_info.gold());
						}
						else if (entry.lottery_type_ == message::Lottery_Jewel)
						{
							int jewel = _info.jewel();
							jewel += add_count;
							_info.set_jewel(jewel);
							msgACK.set_current_jewel(_info.jewel());
						}
						msgACK.set_count(add_count);
					}
					
				}

			}
			else
			{
				msgACK.set_error(message::Error_FailedToLotteryErrorRating);
			}
		}
		else
		{
			msgACK.set_error(message::Error_FailedToLotteryNothingCanBeLotteried);
		}

	}
	else
	{
		msgACK.set_error(message::Error_FailedToLotteryTodayHaveBeenLotteried);
	}
	msgACK.set_last_lottery_time(_last_day_lottery_time);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqBuyLotion(const message::MsgC2SReqBuyLotion* msg)
{
	message::GameError error = message::Error_NO;
	int lotion_id = msg->lotion_id();
	if (lotion_id >= 0 && lotion_id <= 3)
	{		
		const message::MsgLotionShopConfigInfo* shop_config = gGameConfig.getLotionShop(lotion_id);
		if (shop_config != NULL)
		{
			int lotion_size = _info.lotions_size();
			for (size_t i = 0; i < lotion_size; i++)
			{
				if (_info.lotions(i) == lotion_id)
				{
					error = message::Error_FailedToBuyLotionAlreadyHaveLotion;
					break;
				}

			}
			if (error == message::Error_NO)
			{
				switch (shop_config->use_type())
				{
				case message::LotionUseType_jewel:
				{
					int jewel = _info.jewel();
					if (jewel < shop_config->use_count())
					{
						error = message::Error_FailedToBuyLotionNotEnoughJewel;
					}
					else
					{
						jewel -= shop_config->use_count();
						_info.set_jewel(jewel);
					}

				}
				break;
				case message::LotionUseType_gold:
				{
					int gold = _info.gold();
					if (gold < shop_config->use_count())
					{
						error = message::Error_FailedToBuyLotionNotEnoughGold;
					}
					else
					{
						gold -= shop_config->use_count();
						_info.set_gold(gold);
					}
				}
				break;
				default:
					break;
				}

				if (error == message::Error_NO)
				{
					_info.add_lotions(lotion_id);
				}
			}
		}
		else
		{
			error = message::Error_FailedToBuyNotFoundThisLotion;
		}
	}
	else
	{
		error = message::Error_FailedToBuyNotFoundThisLotion;
	}
	message::MsgS2CBuyLotionACK msgACK;
	msgACK.set_lotion_id(lotion_id);
	msgACK.set_current_gold(_info.gold());
	msgACK.set_current_jewel(_info.jewel());
	msgACK.set_error(error);
	sendPBMessage(&msgACK);
}


void DreamHero::ReqGoldShopConfigs()
{
	message::MsgS2CGoldShopConfigsACK msg;
	const MAPGOLDSHOPCONFIGINFOS* infos = gGameConfig.getGoldShopConfigInfos();
	MAPGOLDSHOPCONFIGINFOS::const_iterator it = infos->begin();
	for (; it != infos->end(); ++ it)
	{
		message::MsgGoldShopConfigInfo* entry_info = msg.add_infos();
		entry_info->CopyFrom(it->second.info_);
	}
	sendPBMessage(&msg);
}

const char* DreamHero::get_name()
{
	return _info.name().c_str();
}

void DreamHero::recoverSpirit()
{
	int spirit = _info.spirit();
	int config_max_spirit = gGameConfig.getGlobalConfig().config_max_spirit_;
	int config_spirit = gGameConfig.getGlobalConfig().config_recover_spirit_;
	if (spirit < config_max_spirit)
	{
		spirit += config_spirit;
		if (spirit >= config_max_spirit)
		{
			spirit = config_max_spirit;
		}
	}

	_info.set_spirit(spirit);
	_last_recover_spirit_time = g_server_time;
	message::MsgS2CRecoverSpiritNotify notify;
	notify.set_current_spirit(_info.spirit());
	sendPBMessage(&notify);
	
}

void DreamHero::sendPBMessage(google::protobuf::Message* p)
{
	if (_session)
	{
		_session->sendPBMessage(p);
	}	
}

void DreamHero::ReqReqEnterDailyGame(const message::MsgC2SReqEnterDailyGame* msg)
{
	message::MsgS2CEnterDailyGameACK msgACK;
	message::GameError error = message::Error_NO;
	
	//if (gRankManager.getDailyGameBeginTime() != _daily_game_prize_time)
	//{
	//	_daily_game_time = gRankManager.getDailyGameBeginTime();
	//	_info.set_daily_game_score(0);
	//	_info.set_daily_game_progress(0);
	//	_info.set_daily_game_hp_pct(100);
	//	
	//}
	//else
	//{
	//	error = message::Error_FailedToEnterDailyGameYouHaveBeenAlreadyBeginGame;
	//}
	//msgACK.set_error(error);
	//msgACK.set_daily_game_time(_daily_game_time);
	//msgACK.set_daily_game_progress(_info.daily_game_progress());
	//msgACK.set_score(_info.daily_game_score());		
	//sendPBMessage(&msgACK);
}

void DreamHero::DailyGamePrize(int gold)
{
	if (_daily_game_prize_time != _daily_game_time)
	{
		_daily_game_time = _daily_game_prize_time;
		_info.set_daily_game_gold(gold);
		message::MsgS2CNotifyDailyGamePrize msg;
		msg.set_gold(_info.daily_game_gold());
		msg.set_time(_daily_game_time);
		sendPBMessage(&msg);
	}

}

void DreamHero::TryToGetGamePrize(bool sendmsg)
{
	if (_daily_game_time != _daily_game_prize_time)
	{
		int rank = gRankManager.getDailyRankMaxSize() + 1;
		_info.set_daily_game_gold(gRankManager.GetDailyGamePrize(rank));
		_daily_game_prize_time = _daily_game_time;
		if (sendmsg)
		{
			message::MsgS2CNotifyDailyGamePrize msg;
			msg.set_gold(_info.daily_game_gold());
			msg.set_time(_daily_game_time);
			sendPBMessage(&msg);
		}
	}
}
void DreamHero::ReqUpdateDailyGameProgress(const message::MsgC2SReqUpdateDailyGameProgress* msg)
{
	message::MsgS2CUpdateDailyGameProgressACK msgACK;
	message::GameError error = message::Error_NO;
	int rank = 101;
	if (msg->daily_game_progress() == 1)
	{
		
		if (gRankManager.getDailyGameBeginTime() != _daily_game_prize_time)
		{
			_daily_game_time = gRankManager.getDailyGameBeginTime();
			_info.set_daily_game_hp_pct(100);
			_info.set_daily_game_progress(0);
			_info.set_daily_game_hp_pct(100);
		}
		else if(_info.daily_game_hp_pct() != 0)
		{
			if (_info.daily_game_progress() == 0)
			{

			}
		}
		else
		{
			error = message::Error_FailedToUpdateDailyProgressNoHp;
		}
	}

	if (error == message::Error_NO)
	{
		int progress_temp = _info.daily_game_progress() + 1;
		if (_info.daily_game_hp_pct() != 0)
		{
			if (progress_temp == msg->daily_game_progress())
			{
				int score = msg->score();
				_info.set_daily_game_score(score);
				_info.set_daily_game_progress(msg->daily_game_progress());
				_info.set_daily_game_hp_pct(msg->hp_pct());
				gRankManager.updateHeroDailyRank(_account, _info.name().c_str(), score, rank);

			}
			else
			{
				error = message::Error_FailedToUpdateDailyProgressErrorProgress;
				
			}
		}
		else
		{
			error = message::Error_FailedToUpdateDailyProgressNoHp;
		}

	}

	if (error != message::Error_NO)
	{
		int daily_game_progress = msg->daily_game_progress();
		if (daily_game_progress > _info.daily_top_grogress())
		{
			_info.set_daily_top_grogress(daily_game_progress);
		}
		const globalConfig global_config = gGameConfig.getGlobalConfig();
		int record_size = global_config.daily_game_record_config_.size();
		for (size_t i = record_size - 1; i >= 0; i++)
		{
			if (global_config.daily_game_record_config_[i] == _info.daily_game_progress())
			{
				_info.set_daily_game_record_progress(_info.daily_game_progress());
				break;
			}
		}
		_info.set_daily_game_hp_pct(msg->hp_pct());
		rank = gRankManager.getHeroDailyRank(_account);
	}
	msgACK.set_score(_info.daily_game_score());
	msgACK.set_error(error);
	msgACK.set_rank(rank);
	msgACK.set_daily_game_progress(_info.daily_game_progress());
	msgACK.set_daily_game_progress_record(_info.daily_game_record_progress());
	msgACK.set_hp_pct(_info.daily_game_hp_pct());
	sendPBMessage(&msgACK);
}


