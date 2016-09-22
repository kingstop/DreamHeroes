#pragma once
#include <vector>
#include <map>

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
};
struct ShopSalesPromotionConfig
{
	int id_;
};

struct TimeShopSalesPromotionConfig : public ShopSalesPromotionConfig
{
	int grid_id_;
	int cheap_gold_;
	s64 begin_time_;
	s64 end_time_;
};

typedef std::map<int, message::MsgTaskConfigInfo> MAPTASKS;
typedef std::map<int, message::MsgShopConfigInfo> MAPSHOPHEROCONFIGS;
typedef std::map<int, message::MsgChapterConfigInfo> MAPCHAPTERCONFIGINFOS;
typedef std::map<int, message::MsgGoldShopConfigInfo> MAPGOLDSHOPCONFIGINFOS;
typedef std::map<int, TimeShopSalesPromotionConfig> MAPTIMESHOPSALESPROMOTIONCONFIGS;


enum obj_type
{
	obj_creature,
	obj_item,
};

struct ObjDropBoxConfig
{
	int obj_id_;
	obj_type type_;
	int base_gold_;
	int random_gold_;
};


class GameConfig
{
public:
	GameConfig();
	virtual ~GameConfig();
	void Load(DBQuery* p);
	const MAPSHOPHEROCONFIGS* getShopConfigs();
	const message::MsgShopConfigInfo* getShopConfig(int  grid_id);
	const MAPTASKS* getMapTasks();
	const message::MsgTaskConfigInfo* getMapTask(int id);
	const MAPCHAPTERCONFIGINFOS* getChapterConfigInfos();
	const MAPTIMESHOPSALESPROMOTIONCONFIGS* getTimeShopSalesPromotionConfigs();
	const TimeShopSalesPromotionConfig* getTimeShopSalesPromotionConfig(int id);
	const  message::MsgChapterConfigInfo* getChapterConfigInfo(int id);
	const MAPGOLDSHOPCONFIGINFOS* getGoldShopConfigInfos();
	const message::MsgGoldShopConfigInfo* getGoldShopConfigInfo(int id);

	const globalConfig& getGlobalConfig();
	
protected:
	MAPTASKS _tasks;
	MAPSHOPHEROCONFIGS _shop_heroes;
	MAPTIMESHOPSALESPROMOTIONCONFIGS _shop_time_sales_promotion;
	MAPCHAPTERCONFIGINFOS _chapter_config_infos;
	MAPGOLDSHOPCONFIGINFOS _gold_shop_config_infos;
	
	globalConfig _global_config;
};

