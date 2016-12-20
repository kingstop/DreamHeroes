#pragma once
#include <vector>

class DreamHero;
class DreamHeroManager : public EventableObject
{
public:
	typedef std::map<account_type, DreamHero*> MAPHEROS;
	typedef std::map<std::string, account_type> MAPSTRACCPAIR;
public:
	DreamHeroManager();
	virtual ~DreamHeroManager();
	void Load(DBQuery* p);
	void init();	
	DreamHero* CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session);
	DreamHero* CreateHero(account_type acc, Session* session);
	DreamHero* GetHeroByName(const char* name);
	DreamHero* GetHero(account_type account);
	void DestroyHero(DreamHero* p);
	void SaveDreamHeroes();
	bool is_save_all_heroes_ok();
	void save_all_heroes_ok();
	void CollectInfo();
	std::string generateName();
	void addSql(const char* sql);
protected:
	void eventPerHour();
	void save();
	void dayRefresh();
	void refrashDayNumber();
	void refrashHeroTitle();
	void recoverSpirit();
protected:
	MAPHEROS _heroes;
	u8 _day_number[6][10];
	bool _save_all_heroes_ok;
	int _day_create_heroes_count;
	std::string _hero_day_title;
	std::vector<char> _char_configs;
	u64 _last_save_time;
	std::vector<std::string> _vc_sql;
	int _recover_spirit_minute;
	int _recover_spirit;
};

