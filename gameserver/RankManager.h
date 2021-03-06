#pragma once
#include <iostream>
#include <list>
struct DailyGameRankTg
{
	std::string name_;
	account_type acc_;
	int score_;
	int rank_;
	u32 time_;
};
class RankManager : public EventableObject
{
public:
	
	typedef std::list<DailyGameRankTg> HEROESDAILYRANK;
public:
	RankManager();
	virtual ~RankManager();
	void Init();
	void Load(DBQuery* p);
	void updateHeroDailyRank(account_type acc, const char* name, int score,int& rank);
	void removeHero(account_type acc);
	int getHeroDailyRank(account_type acc);
	void DailyGameUpdate();
	int GetDailyGamePrize(int rank);
	u32 getDailyGameBeginTime();
	void setMaxDailyProgress(int progress);
	int getMaxDailyProgress();
	void setDailyGameBeginTime(u32 begin_time);
	void setDailyMaxRankSize(int rank_size);
	HEROESDAILYRANK* getHeroesRank();
	void ClearRankList();
	void save();
public:
	HEROESDAILYRANK _heroes_daily_rank;
	std::map<account_type, int> _heroes_rank;
	int _daily_max_rank_size;
	int _min_daily_score;
	u32 _daily_game_begin_time;
	int _max_daily_progress;
};

