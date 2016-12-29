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
			DailyGameRankTg entry;
			entry.acc_ = row["account_id"];
			entry.name_ = row["name"].c_str();
			entry.score_ = row["score"];
			entry.time_ = row["UNIX_TIMESTAMP(`time`)"];
			entry.rank_ = row["rank"];
			_heroes_daily_rank.push_back(entry);
			_heroes_rank[entry.acc_] = entry.rank_;
		}

	}

}


void RankManager::setDailyMaxRankSize(int rank_size)
{
	_daily_max_rank_size = rank_size;
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
void RankManager::DailyGameUpdate()
{
	const DAILYGAMEPRIZECONFIGS* daily_configs = gGameConfig.getDailyGamePrizeConfigs();
	HEROESDAILYRANK::const_iterator it = _heroes_daily_rank.begin();
	char sz_temp[1024];
	std::string sql;
	std::string game_time;
	build_unix_time_to_string(_daily_game_begin_time, game_time);
	for (int i = 0; it != _heroes_daily_rank.end(); ++ it)
	{
		const DailyGameRankTg& entry = (*it);
		int prize_gold = GetDailyGamePrize(entry.rank_);
		DreamHero* hero = gDreamHeroManager.GetHero(entry.acc_);
		if (hero)
		{
							
			hero->DailyGamePrize(prize_gold);
		}
		else
		{
			
			sprintf(sz_temp, "update `character` set `daily_game_gold`=%d, `daily_game_prize_time`='%s' where `account_id`=%llu;", prize_gold,
				game_time.c_str(), entry.acc_);
			sql += sz_temp;
			if (i >= 20)
			{
				message::MsgSaveDataGS2DB msg_db;
				msg_db.set_sql(sql.c_str());
				gGSDBClient.sendPBMessage(&msg_db, 0);
				sql.clear();
				i = 0;
			}
		}
	}
	if (sql.empty() == false)
	{
		message::MsgSaveDataGS2DB msg_db;
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
	_daily_game_begin_time = g_server_time;
}

u32 RankManager::getDailyRankMaxSize()
{
	return _daily_max_rank_size;
}

void RankManager::setDailyGameBeginTime(u32 begin_time)
{
	_daily_game_begin_time = begin_time;
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
		std::list<DailyGameRankTg>::iterator it = _heroes_daily_rank.begin();
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
