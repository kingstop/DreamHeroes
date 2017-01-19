#include "stdafx.h"
#include "RankManager.h"
#include "DreamHero.h"

struct DailyRank
{
	bool operator()(const DailyGameRankTg& t1, const DailyGameRankTg& t2)
	{
		bool ret = false;
		if (t1.score_ > t2.score_)
		{
			ret = true;
		}
		else if(t1.score_ == t2.score_)
		{
			if (t1.time_ < t2.time_)
			{
				ret = true;
			}
			
		}
		return ret;
	}
		
};
RankManager::RankManager()
{
	_daily_game_begin_time = 0;
}


RankManager::~RankManager()
{
}

void RankManager::Init()
{
	if (_daily_game_begin_time == 0)
	{
		DailyGameUpdate();
	}

	if (gEventMgr.hasEvent(this, EVENT_SAVE_DAILY_GAME_RANK_) == false)
	{
		gEventMgr.addEvent(this, &RankManager::save, EVENT_SAVE_DAILY_GAME_RANK_, 5 * 60 * _TIME_SECOND_MSEL_, -1, 0);
	}
}

void RankManager::ClearRankList()
{
	_heroes_daily_rank.clear();
	_heroes_rank.clear();
}

RankManager::HEROESDAILYRANK* RankManager::getHeroesRank()
{
	return &_heroes_daily_rank;
}

void RankManager::Load(DBQuery* p)
{
	if (p)
	{
		_heroes_daily_rank.clear();
		_heroes_rank.clear();
		DBQuery& query = *p;
		query << "select *, UNIX_TIMESTAMP(`time`) from `hero_daily_game_rank` ORDER BY `score` DESC;";
		SDBResult sResult = query.store();
		int rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			u32 temp_time = row["UNIX_TIMESTAMP(`time`)"];			
			if (isInToday(temp_time))
			{
				DailyGameRankTg entry;
				entry.acc_ = row["account_id"];
				entry.name_ = row["name"].c_str();
				entry.score_ = row["score"];
				entry.rank_ = row["rank"];
				entry.time_ = row["UNIX_TIMESTAMP(`time`)"];
				_heroes_daily_rank.push_back(entry);
				_heroes_rank[entry.acc_] = entry.rank_;
			}
		}

	}

}


int RankManager::getHeroDailyRank(account_type acc)
{
	int rank = _daily_max_rank_size + 1;
	std::map<account_type, int>::iterator it = _heroes_rank.find(acc);
	if (it != _heroes_rank.end())
	{
		rank = it->second;
	}
	return rank;
}

u32 RankManager::getDailyGameBeginTime()
{
	return _daily_game_begin_time;
}
void RankManager::setMaxDailyProgress(int progress)
{
	_max_daily_progress = progress;
}
int RankManager::getMaxDailyProgress()
{
	return _max_daily_progress;
}

int RankManager::GetDailyGamePrize(int rank)
{
	int prize_gold = 0;
	const DAILYGAMEPRIZECONFIGS* daily_configs = gGameConfig.getDailyGamePrizeConfigs();
	DAILYGAMEPRIZECONFIGS::const_iterator it_daily_configs = daily_configs->begin();
	for (; it_daily_configs != daily_configs->end(); ++it_daily_configs)
	{
		const DailyGamePrizeConfig& config = (*it_daily_configs);
		if (rank >= config.begin_rank_ && rank <= config.end_rank_)
		{
			prize_gold = config.prize_gold_;
			break;
		}
	}
	return prize_gold;
}


void RankManager::save()
{
	char sz_temp[1024];
	std::string sql_delete = "DELETE * FROM hero_daily_game_rank;";
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sql_delete.c_str());
	gGSDBClient.sendPBMessage(&msg_db, 0);
	HEROESDAILYRANK::const_iterator it = _heroes_daily_rank.begin();
	std::string sql_head = "replace into hero_daily_game_rank(`account_id`, `name`, `score`, `rank`, `time`) values";
	std::string sql_rank;
	std::string rank_time;
	int number = 0;
	for (int i = 0; it != _heroes_daily_rank.end(); ++it, number++)
	{
		if (number == 0)
		{
			sql_rank = "replace into hero_daily_game_rank(`account_id`, `name`, `score`, `rank`, `time`) values";
		}
		else
		{
			sql_rank += ",";
		}
		const DailyGameRankTg& entry = (*it);
		int prize_gold = GetDailyGamePrize(entry.rank_);
		DreamHero* hero = gDreamHeroManager.GetHero(entry.acc_);
		build_unix_time_to_string(entry.time_, rank_time);
		sprintf(sz_temp, "(%llu, '%s', %d, %d, '%s')", entry.acc_, entry.name_.c_str(),
			entry.score_, entry.rank_, rank_time.c_str());
		sql_rank += sz_temp;
		if (number >= 20)
		{
			number = 0;
			msg_db.set_sql(sql_rank.c_str());
			gGSDBClient.sendPBMessage(&msg_db, 0);
			sql_rank.clear();
		}
	}
	if (sql_rank.empty() == false)
	{
		msg_db.set_sql(sql_rank.c_str());
		gGSDBClient.sendPBMessage(&msg_db, 0);
		sql_rank.clear();
	}
}
void RankManager::DailyGameUpdate()
{
	const DAILYGAMEPRIZECONFIGS* daily_configs = gGameConfig.getDailyGamePrizeConfigs();
	HEROESDAILYRANK::const_iterator it = _heroes_daily_rank.begin();
	char sz_temp[1024];
	std::string sql;
	std::string game_time;
	build_unix_time_to_string(_daily_game_begin_time, game_time);
	std::string sql_delete = "DELETE FROM hero_daily_game_rank;";
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sql_delete.c_str());
	gGSDBClient.sendPBMessage(&msg_db, 0);

	std::string sql_head = "replace into hero_daily_game_rank(`account_id`, `name`, `score`, `rank`, `time`) values";
	std::string sql_rank;
	std::string rank_time;
	int number = 0;
	for (int i = 0; it != _heroes_daily_rank.end(); ++ it, number ++)
	{
		if (number == 0)
		{
			sql_rank = "replace into hero_daily_game_rank(`account_id`, `name`, `score`, `rank`, `time`) values";
		}
		else
		{
			sql_rank += ",";
		}
		const DailyGameRankTg& entry = (*it);
		int prize_gold = GetDailyGamePrize(entry.rank_);
		DreamHero* hero = gDreamHeroManager.GetHero(entry.acc_);
		build_unix_time_to_string(entry.time_, rank_time);
		sprintf(sz_temp, "(%llu, '%s', %d, %d, '%s')", entry.acc_, entry.name_.c_str(), 
			entry.score_, entry.rank_, rank_time.c_str());
		sql_rank += sz_temp;
		if (number >= 20)
		{
			number = 0;
			msg_db.set_sql(sql_rank.c_str());
			gGSDBClient.sendPBMessage(&msg_db, 0);
			sql_rank.clear();
		}

		if (hero)
		{							
			hero->DailyGamePrize(prize_gold);
		}
		else
		{			
			i++;
			sprintf(sz_temp, "update `character` set `daily_game_gold`=%d, `daily_game_prize_time`='%s' where `account_id`=%llu;", prize_gold,
				game_time.c_str(), entry.acc_);
			sql += sz_temp;
			if (i >= 20)
			{
				//message::MsgSaveDataGS2DB msg_db;
				msg_db.set_sql(sql.c_str());
				gGSDBClient.sendPBMessage(&msg_db, 0);
				sql.clear();
				i = 0;
			}
		}
	}
	if (sql_rank.empty() == false)
	{
		msg_db.set_sql(sql_rank.c_str());
		gGSDBClient.sendPBMessage(&msg_db, 0);
		sql_rank.clear();
	}
	
	if (sql.empty() == false)
	{		
		msg_db.set_sql(sql.c_str());
		gGSDBClient.sendPBMessage(&msg_db, 0);
		sql.clear();
	}
	DreamHeroManager::MAPHEROS* heroes = gDreamHeroManager.GetHeroes();
	DreamHeroManager::MAPHEROS::iterator it_heroes = heroes->begin();
	for (; it_heroes != heroes->end(); ++it_heroes)
	{
		DreamHero* hero = it_heroes->second;
		hero->TryToGetGamePrize(true);
	}
	_heroes_daily_rank.clear();
	_heroes_rank.clear();
	_daily_game_begin_time = g_server_time;

	std::string day_refresh_time = "";
	build_unix_time_to_string(g_server_time, day_refresh_time);
	Mylog::log_server(LOG_INFO, "daily game day refresh [%s]", day_refresh_time.c_str());
}


void RankManager::setDailyGameBeginTime(u32 begin_time)
{
	_daily_game_begin_time = begin_time;
}

void RankManager::setDailyMaxRankSize(int rank_size)
{
	_daily_max_rank_size = rank_size;
}

void RankManager::removeHero(account_type acc)
{
	_heroes_rank.clear();
	std::list<DailyGameRankTg>::iterator it = _heroes_daily_rank.begin();
	for (int temp_rank = 1; it != _heroes_daily_rank.end(); ++it)
	{
		DailyGameRankTg& entry = (*it);
		entry.rank_ = temp_rank;
		_heroes_rank[entry.acc_] = temp_rank;
		if (entry.acc_ == acc)
		{
			it = _heroes_daily_rank.erase(it);		
		}
		else
		{
			temp_rank++;
			_heroes_rank[entry.acc_] = temp_rank;
		}
	}
	

}

void RankManager::updateHeroDailyRank(account_type acc, const char* name, int score,int& rank)
{
	rank = _daily_max_rank_size + 1;
	bool full = true;
	if (_heroes_rank.size() < _daily_max_rank_size)
	{
		full = false;
	}

	bool need_add = false;
	bool need_rank = false;
	if (score <= _min_daily_score)
	{
		if (full == false)
		{
			need_add = true;
		}
	}
	else
	{
		need_add = true;
		need_rank = true;
	}
	std::list<DailyGameRankTg>::iterator it = _heroes_daily_rank.begin();
	for (; it != _heroes_daily_rank.end(); ++ it)
	{
		DailyGameRankTg& entry = (*it);
		if (entry.acc_ == acc)
		{
			entry.score_ = score;
			entry.name_ = name;
			need_add = false;
			break;
		}
	}

	if (need_add)
	{
		rank = _heroes_daily_rank.size() + 1;
		_heroes_rank[acc] = rank;
		DailyGameRankTg entry;
		entry.acc_ = acc;
		entry.name_ = name;
		entry.rank_ = rank;
		entry.score_ = score;
		entry.time_ = g_server_time;
		_min_daily_score = score;
		_heroes_daily_rank.push_back(entry);
	}

	if (need_rank)
	{
		_heroes_rank.clear();
		_heroes_daily_rank.sort(DailyRank());
		it = _heroes_daily_rank.begin();
		for (int temp_rank = 1; it != _heroes_daily_rank.end(); ++ it, temp_rank++)
		{
			DailyGameRankTg& entry = (*it);
			entry.rank_ = temp_rank;
			_heroes_rank[entry.acc_] = temp_rank;
			if (entry.acc_ == acc)
			{
				rank = entry.rank_;
			}
		}
	}

}
