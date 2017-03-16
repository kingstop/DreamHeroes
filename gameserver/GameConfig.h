#pragma once
#include <vector>
#include <map>
#include <list>

struct TaskConfig
{
	int task_id_;
	message::TaskType en_;
	int argument_;
	int argument_1_;
	int need_time_;
	int require_chapter_;
	int require_section_;
	int require_complete_task_count_;
	int gift_gold_;
};


struct globalConfig
{
	int config_gold_;
	int hero_unlock_count_;
	int day_Refresh_time_;
	int day_free_task_count_;
	int day_task_advertisement_task_cd_;
	int hero_max_tasks_count_;
	int channel_id_;
	int game_id_;
	int refresh_task_gold_;
	int relive_gold_;
	int config_max_spirit_;
	int config_jewel_;
	int config_recover_spirit_minute_;
	int config_recover_spirit_;
	int config_enter_game_use_spirit_;
	int config_day_buy_spirit_;
	int config_weixin_gold;
	std::vector<int> daily_game_record_config_;
	std::vector<int> daily_game_reset_jewel_config_;
	std::string config_weixin_cd_key;	
};
struct ShopSalesPromotionConfig
{
	int id_;
};

enum TimeShopPromotionType
{
	HeroPromotionType,
	RechargePromotionType,
	OpenHideHeroType
};


struct BaseShopPromotion
{
	TimeShopPromotionType PromotionType_;

};

struct HeroPromotion : public BaseShopPromotion
{

	HeroPromotion()
	{
		PromotionType_ = HeroPromotionType;
	}
	std::map<int, int> cheap_hero_;
	
};

struct OpenHideHeroPromotion : public BaseShopPromotion
{
	OpenHideHeroPromotion()
	{
		PromotionType_ = OpenHideHeroType;
	}
	std::list<int> hide_grid_ids_;
};

struct RechargeRattingPromotion : public BaseShopPromotion
{
	RechargeRattingPromotion()
	{
		PromotionType_ = RechargePromotionType;
	}
	enum 
	{
		ALL,
		GOLD,
		JEWEL
	};
	int recharge_ratting_;
	int rechagre_type;
};

enum TimeEventType
{
	Time_Normal,
	Time_Week,
	Time_OpenServerOffsetTime,

};
struct EventTimeBase
{
	TimeEventType type_;
};

struct TimeNormal : public EventTimeBase
{
	TimeNormal()
	{
		type_ = Time_Normal;
	}
	s64 show_time_;
	s64 begin_time_;
	s64 end_time_;
};

struct TimeWeek : public TimeNormal
{
	TimeWeek()
	{
		type_ = Time_Week;
	}
};

struct TimeOpenServerOffsetTime : public TimeNormal
{
	TimeOpenServerOffsetTime()
	{
		type_ = Time_OpenServerOffsetTime;
	}
};



class TimeShopSalesPromotionConfig : public ShopSalesPromotionConfig
{
public:
	TimeShopSalesPromotionConfig() : _time(NULL), _promotion(NULL)
	{

	}
	~TimeShopSalesPromotionConfig()
	{
		if (_time != NULL)
		{
			delete _time;
		}
		if (_promotion != NULL)
		{
			delete _promotion;
		}
	}
	void set_time(EventTimeBase* p)
	{
		_time = p;
	}
	void set_promotion(BaseShopPromotion* promotion)
	{
		_promotion = promotion;
	}

	const EventTimeBase* GetTime() const
	{
		return _time;
	}
	const BaseShopPromotion* GetPromotion() const
	{
		return _promotion;
	}
	void setName(const char* name)
	{
		_name = name;
	}
	void setTimeDescribe(const char* time_describe)
	{
		_time_describe = time_describe;
	}
	void setDescribe(const char* describe)
	{
		_describe = describe;
	}

	const char* getTimeDescribe() const
	{
		return _time_describe.c_str();
	}
	const char* getName() const
	{
		return _name.c_str();
	}
	const char* getDescribe() const
	{
		return _describe.c_str();
	}
protected:
	EventTimeBase* _time;
	BaseShopPromotion* _promotion;
	std::string _name;
	std::string _time_describe;
	std::string _describe;
};



struct ObjDropBoxConfig
{
	int obj_id_;
	message::SubType type_;
	int base_gold_;
	int random_gold_;
};

struct IntPair
{
	int number_1_;
	int number_2_;
};
struct MapBehaviorConfig
{
	std::pair<int, int> key_;
	int obj_behavior_id_;
	int obj_count_;
	int max_pos_count_;

};


struct MapRandomObjConfig
{
	message::SubType type_;
	int obj_id_;
};

struct MapRandomObjsConfig
{
	std::pair<int, int> key_;
	int obj_behavior_id_;
	std::vector<MapRandomObjConfig> objs_;
};

struct GoldShopConfigInfo
{
	std::string appstore_product_id_;
	message::MsgGoldShopConfigInfo info_;
};

enum enLotteryBox
{
	Lottery_Jewel,
	Lottery_Gold,
	Lottery_Lotion
};

struct LotteryDrawBoxConfig
{
	message::enLotteryBox lottery_type_;
	int sub_index_;
	int base_count_;
	int random_count_;
	int rating_;
};

struct DailyGamePrizeConfig
{
	int begin_rank_;
	int end_rank_;
	int prize_gold_;
};



typedef std::map<int, message::MsgTaskConfigInfo> MAPTASKS;
typedef std::map<int, message::MsgShopConfigInfo> MAPSHOPHEROCONFIGS;
typedef std::map<int, message::MsgChapterConfigInfo> MAPCHAPTERCONFIGINFOS;
typedef std::map<int, GoldShopConfigInfo> MAPGOLDSHOPCONFIGINFOS;

typedef std::map<int, MAPGOLDSHOPCONFIGINFOS> CHANNELMAPGOLDSHOPCONFIGINFOS;
typedef std::map<int, TimeShopSalesPromotionConfig*> MAPTIMESHOPSALESPROMOTIONCONFIGS;
typedef std::map<int, ObjDropBoxConfig> MAPDROPBOXCONFIGS;
typedef std::map<message::SubType, MAPDROPBOXCONFIGS> MAPTYPEDROPBOXCONFIGS;
typedef std::map<int, MapBehaviorConfig> MAPBEHAVIORCONFIGS;
typedef std::map<std::pair<int, int>, MAPBEHAVIORCONFIGS> MAPALLBEHAVIORCONFIGS;
typedef std::map<int, MapRandomObjsConfig> MAPRANDOMOBJCONFIGS;
typedef std::map<std::pair<int, int>, MAPRANDOMOBJCONFIGS> MAPALLRANDOMOBJCONFIGS;
typedef std::map<std::pair<int, int>, MAPTYPEDROPBOXCONFIGS> MAPMAPDROPBOXCONFIGS;
typedef std::map<int, message::MsgLotionShopConfigInfo> MAPLOTIONSHOPCONFIGS;

typedef std::map<int, message::MsgSpiritShopInfo> MAPSPIRITSHOP;
typedef std::vector<LotteryDrawBoxConfig> VCLOTTERYDRAWBOXSCONFIGS;
typedef std::vector<DailyGamePrizeConfig> DAILYGAMEPRIZECONFIGS;

class GameConfig
{
public:
	GameConfig();
	virtual ~GameConfig();
	void Load(DBQuery* p);
	const MAPSHOPHEROCONFIGS* getShopConfigs();
	const message::MsgShopConfigInfo* getShopConfig(int  grid_id);
	void setHeroShopConfig(int  grid_id, bool open);
	const MAPTASKS* getMapTasks();
	const message::MsgTaskConfigInfo* getMapTask(int id);
	const MAPCHAPTERCONFIGINFOS* getChapterConfigInfos();
	const MAPTIMESHOPSALESPROMOTIONCONFIGS* getTimeShopSalesPromotionConfigs();
	const TimeShopSalesPromotionConfig* getTimeShopSalesPromotionConfig(int id);
	const  message::MsgChapterConfigInfo* getChapterConfigInfo(int id);
	const MAPGOLDSHOPCONFIGINFOS* getGoldShopConfigInfos(int channel);
	const GoldShopConfigInfo* getGoldShopConfigInfo(int channel, int id);
	const GoldShopConfigInfo* getGoldShopConfigInfo(int channel, const char* appstore_product_id);
	const globalConfig& getGlobalConfig();
	const MAPTYPEDROPBOXCONFIGS* getMapDropBox(int chapter_id, int section_id);
	const MAPSPIRITSHOP* getSpiritShop();
	const message::MsgSpiritShopInfo* getSpiritShop(int index);
	const MAPLOTIONSHOPCONFIGS* getLotionShop();
	const message::MsgLotionShopConfigInfo* getLotionShop(int index);
	const VCLOTTERYDRAWBOXSCONFIGS* getLotteryDrawBoxs();
	const DAILYGAMEPRIZECONFIGS* getDailyGamePrizeConfigs();

	void setServerOpenTime(u64 time_open);
	void setServerID(char server_id);
	u64 getServerOpenTime();
	int getOpenServerPassedDay(s64 curTime);
	char getServerID();
	void setPlatformHttpUrl(const char* http);
	const char* getPlatformHttpUrl();

	void setNeedConnectPlatform(bool b);
	void setPlatformServerIp(const char* ip);
	void setPlatformServerPort(int port);
	void setCanConcernWeiXin(bool concern_weixin);
	bool isConcernWeiXin();

	bool isNeedConnectPlatform( );
	const char* getPlatformServerIp();
	int getPlatformServerPort();
	void setGameID(int game_id);
	int getGameID();

	void setServerType(int t);
	int getServerType();
	const char* getServerTitle();
	void generateTitle();
	bool isInToday(u32 time);

protected:
	MAPTASKS _tasks;
	MAPSHOPHEROCONFIGS _shop_heroes;
	MAPTIMESHOPSALESPROMOTIONCONFIGS _shop_time_sales_promotion;
	MAPCHAPTERCONFIGINFOS _chapter_config_infos;
	CHANNELMAPGOLDSHOPCONFIGINFOS _gold_shop_config_infos;
	MAPTYPEDROPBOXCONFIGS _drop_box_configs;
	MAPALLBEHAVIORCONFIGS _map_behavior_config;
	MAPALLRANDOMOBJCONFIGS _map_random_obj_configs;
	MAPMAPDROPBOXCONFIGS _map_drop_box_configs;
	MAPSPIRITSHOP _map_spirit_shop;
	MAPLOTIONSHOPCONFIGS _map_lotion_shop_configs;
	VCLOTTERYDRAWBOXSCONFIGS _lottery_draw_configs;
	DAILYGAMEPRIZECONFIGS _daily_game_prize_configs;
	globalConfig _global_config;
	u64 _server_open_time;
	char _server_char;
	std::string _platform_http;
	bool _need_connect_platform;
	std::string _platform_ip;
	int _platform_port;
	int _game_id;
	int _server_type;
	bool _can_concern_weixin;
	std::string _server_title;
};

