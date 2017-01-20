#include "stdafx.h"
#include "RecordManager.h"


RecordManager::RecordManager()
{
	_sql_head[RecordTypeLogin] = "insert into `login_record`(`account_id`, `nick_name`, `record_time`) values "; 
	_sql_head[RecordTypeLoginOut] = "insert into `login_out_record`(`account_id`, `nick_name`, `record_time`) values ";
	_sql_head[RecordTypeEnterGame] = "insert into `enter_game_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `current_gold`, `record_time`) values ";
	_sql_head[RecordTypeLeaveGame] = "insert into `leave_game_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `success`, `gold`, `current_gold`, `record_time`) values";
	_sql_head[RecordTypeTaskComplete] = "insert into `task_complete_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `task_id`, `gold`, `current_gold`, `record_time`) values";
	_sql_head[RecordTypeTaskAccepte] = "insert into `task_accepte_record`(`account_id`, `nick_name`, `gold`, `current_gold`, `task_id`, `record_time`) values";
	_sql_head[RecordTypeTaskGiveUp] = "insert into `task_give_up_record`(`account_id`, `nick_name`, `task_id`, `record_time`) values";
	_sql_head[RecordTypeChapterUnlock] = "insert into `chapter_unlock_record`(`account_id`, `nick_name`, `chapter_id`, `gold`,  `current_gold`,`record_time`) values";
	_sql_head[RecordTypeGoldModify] = "insert into `modify_gold_record`(`account_id`, `nick_name`, `gold`,  `current_gold`,`modify_type`, `record_time`) values";
	_sql_head[RecordTypeBuyHero] = "insert into `buy_hero_record`(`account_id`, `nick_name`, `grid_hero`, `gold`, `current_gold`, `record_time`) values";
	_sql_head[RecordTypeDealWaitToPay] = "insert into `deal_wait_to_pay`(`account_id`, `status`, `price`, `order_id`, `product_id`, `record_time`) values";
	_sql_head[RecordTypeDealToPay] = "insert into `deal_to_pay`(`account_id`, `status`, `order_id`, `modify_gold`, `current_gold`, `product_id`, `modify_jewel`, `current_jewel`, `record_time`) values";
	_sql_head[RecordTypeGiveUpDeal] = "insert into `give_up_deal`(`account_id`, `status`, `price`, `order_id`, `product_id`, `record_time`) values";
	_sql_head[RecordTypeWaitToVerify] = "insert into `deal_wait_to_verify_reocrd`(`account_id`, `order_id`, `name`, `receipt`, `record_time`) values";
	_sql_head[RecordTypeRelive] = "insert into `relive_record`(`account_id`, `nick_name`, `gold`, `current_gold`, `record_time`) values";
	_sql_head[RecordTypeOnlineCount] = "insert into `game_online_record`(`online_player_count`, `offline_player_count`, `record_time`) values";
	_sql_head[RecordTypeBuySpirit] = "insert into `buy_spirit_record`(`account_id`, `nick_name`,`spirit`, `day_buy_count`, `current_spirit`, `use_jewel`, `current_jewel`, `record_time`) values";
	_sql_head[RecordDailyGame] = "insert into `daily_game_record`(`account_id`, `nick_name`, `daily_game_rank`, `daily_game_score`, `record_time`) values";
	_sql_head[RecordDailyGameReward] = "insert into `daily_game_reward`(`account_id`, `nick_name`, `daily_game_rank`, `daily_gold`, `current_gold`, `record_time`) values";
	_sql_head[RecordDailyLotteryRecord] = "insert into `daily_lottery_record`(`account_id`, `nick_name`, `daily_gold`, `current_gold`, `daily_jewel`, `current_jewel`, `lottery_lotion`, `record_time`) values";
	_sql_head[RecordCreateName] = "insert into `create_name_record`(`name`, `day_create_count`, `day_title`, `record_time`) values";
}

void RecordManager::generateNameRecord(const char* nick_name, int day_create_count, const char* day_title)
{
	sprintf(_szTemp, "('%s', %d, '%s', '%s')", nick_name, day_create_count, day_title, getCurTime());
	_record[RecordCreateName].push_back(_szTemp);
}

void RecordManager::dailyLotteryRecord(account_type acc, const char* nick_name, int jewel, int current_jewel, int gold, int current_gold, int lotion_id)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d, %d, %d,'%s')", acc, nick_name, jewel, current_jewel, gold, current_jewel, lotion_id, getCurTime());
	_record[RecordDailyLotteryRecord].push_back(_szTemp);
	if (gold != 0)
	{
		goldModifyRecord(acc, nick_name, gold, current_gold, GoldModify_DailyLottery);
	}

}

void RecordManager::dailyGameRewardRecord(account_type acc, const char* nick_name, int daily_game_rank, int daily_gold, int current_gold)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d,'%s')", acc, nick_name, daily_game_rank, daily_gold, current_gold, getCurTime());
	_record[RecordDailyGameReward].push_back(_szTemp);
	if (daily_gold != 0)
	{
		goldModifyRecord(acc, nick_name, daily_gold, current_gold, GoldModify_DailyGameReward);
	}

}

void RecordManager::dailyGameRecord(account_type acc, const char* nick_name, int daily_game_rank, int daily_game_score)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, '%s')", acc, nick_name, daily_game_rank, daily_game_score, getCurTime());
	_record[RecordDailyGame].push_back(_szTemp);
}

void RecordManager::reliveRecord(account_type acc, const char* nick_name, int gold, int current_gold)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d,'%s')", acc, nick_name, gold , current_gold,getCurTime());
	_record[RecordTypeRelive].push_back(_szTemp);
}

void RecordManager::buySpiritRecord(account_type acc, const char* nick_name,int spirit, int day_buy_count, int current_spirit, int use_jewel, int current_jewel)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d, %d, %d, '%s')", acc, nick_name, spirit, day_buy_count, current_spirit, use_jewel, current_jewel, getCurTime());
	_record[RecordTypeBuySpirit].push_back(_szTemp);
}

RecordManager::~RecordManager()
{
}

const char* RecordManager::getCurTime()
{
	build_unix_time_to_string(g_server_time, _cur_time);
	return _cur_time.c_str();
}


void RecordManager::loginRecord(account_type acc, const char* nick_name)
{
	sprintf(_szTemp, "(%llu, '%s','%s')", acc, nick_name, getCurTime());
	_record[RecordTypeLogin].push_back(_szTemp);

}
void RecordManager::loginOutRecord(account_type acc, const char* nick_name)
{
	sprintf(_szTemp, "(%llu, '%s','%s')", acc, nick_name, getCurTime());
	_record[RecordTypeLoginOut].push_back(_szTemp);
}


void RecordManager::enterGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int current_gold)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d,'%s')", acc, nick_name, chapter_id, section_id, current_gold,getCurTime());
	_record[RecordTypeEnterGame].push_back(_szTemp);
}

void RecordManager::leaveGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, bool success, int gold, int current_gold)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d, %d, %d, '%s')", acc, nick_name, chapter_id, section_id,success , gold, current_gold, getCurTime());
	_record[RecordTypeLeaveGame].push_back(_szTemp);
}

void RecordManager::taskCompleteRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int task_id, int gold, int current_gold)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d, %d, %d,'%s')", acc, nick_name, chapter_id, section_id, task_id, gold, current_gold, getCurTime());
	_record[RecordTypeTaskComplete].push_back(_szTemp);
}

void RecordManager::taskAccepteRecordRecord(account_type acc, const char* nick_name,int gold, int current_gold, int task_id)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d,'%s')", acc, nick_name, gold, current_gold, task_id, getCurTime());
	_record[RecordTypeTaskAccepte].push_back(_szTemp);
}

void RecordManager::taskGiveUpRecord(account_type acc, const char* nick_name, int task_id)
{
	sprintf(_szTemp, "(%llu, '%s', %d, '%s')", acc, nick_name, task_id, getCurTime());
	_record[RecordTypeTaskGiveUp].push_back(_szTemp);
}
void RecordManager::chapterUnlockRecord(account_type acc, const char* nick_name, int chapter_id, int gold, int current_gold)
{
	goldModifyRecord(acc, nick_name, gold, current_gold, GoldModify_UnlockChapter);
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d,'%s')", acc, nick_name, chapter_id, gold, current_gold,getCurTime());
	_record[RecordTypeChapterUnlock].push_back(_szTemp);
}
void RecordManager::buyHeroRecord(account_type acc, const char* nick_name, int grid, int gold, int current_gold)
{
	goldModifyRecord(acc, nick_name, gold, current_gold, GoldModify_BuyHero);
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d, '%s')", acc, nick_name, grid, gold, current_gold,getCurTime());
	_record[RecordTypeBuyHero].push_back(_szTemp);
}
void RecordManager::goldModifyRecord(account_type acc, const char* nick_name, int gold, int current_gold, GoldModifyType en)
{
	sprintf(_szTemp, "(%llu, '%s', %d, %d, %d,'%s')", acc, nick_name, gold, current_gold,(int)en, getCurTime());
	_record[RecordTypeGoldModify].push_back(_szTemp);
}

void RecordManager::dealWaitToPayRecord(account_type acc, const char* key_code, int status, int price, int order_id)
{
	//`account_id`, `status`, `price`, `order_id`, `product_id`, `record_time`
	sprintf(_szTemp, "(%llu, %d, %d, %d, '%s', '%s')", acc, status, price, order_id, key_code, getCurTime());
	_record[RecordTypeDealWaitToPay].push_back(_szTemp);
}

void RecordManager::dealWaitToVerifyRecord(account_type acc, const char* name, int order_id, const char* recepit)
{
	sprintf(_szTemp, "(%llu, %d, '%s', '%s', '%s')", acc, order_id, name, recepit,  getCurTime());
	_record[RecordTypeWaitToVerify].push_back(_szTemp);
}

void RecordManager::dealPayRecord(account_type acc, const char* key_code, int status, int order_id, int modify_gold, int current_gold, int modify_jewel, int current_jewel)
{	
	//`account_id`, `status`, `order_id`, `modify_gold`, `current_gold`, `product_id`, `record_time`
	sprintf(_szTemp, "(%llu, %d, %d, %d, %d, '%s', %d, %d,'%s')", acc, status, order_id, modify_gold, current_gold, key_code, modify_jewel, current_jewel,getCurTime());
	_record[RecordTypeDealToPay].push_back(_szTemp);

}
void RecordManager::gameOnlineRecord(int online_count, int offline_count)
{
	sprintf(_szTemp, "(%d, %d, '%s')", online_count, offline_count, getCurTime());
	_record[RecordTypeOnlineCount].push_back(_szTemp);

}

void RecordManager::giveUpDealRecord(account_type acc, const char* key_code, int status, int price, int order_id)
{
	sprintf(_szTemp, "(%llu, %d, %d, %d, '%s', '%s')", acc, status, price, order_id, key_code, getCurTime());
	_record[RecordTypeGiveUpDeal].push_back(_szTemp);
}

void RecordManager::update()
{
	int max_count = 10;
	std::string sql_record;
	for (size_t i = RecordTypeLogin; i < RecordTypeMax; i++)
	{		
		RECORDS::iterator it = _record[i].begin();

		for (int j = 0; it != _record[i].end(); ++ it, j ++)
		{
			if (j >= max_count)
			{
				message::MsgRecordSqlGS2DB msg;
				msg.set_sql(sql_record.c_str());
				gGSDBClient.sendPBMessage(&msg, 0);				
				sql_record.clear();
				j = 0;
			}

			if (j == 0)
			{
				sql_record += _sql_head[i];
			}
			else
			{
				sql_record += ",";
			}
			sql_record += (*it);
		}

		if (sql_record.empty() == false)
		{
			message::MsgRecordSqlGS2DB msg;
			msg.set_sql(sql_record.c_str());
			gGSDBClient.sendPBMessage(&msg, 0);
			sql_record.clear();
		}
		_record[i].clear();
	}
}