#include "stdafx.h"
#include "ShopSalesPromotionManager.h"
#include "session.h"

ShopSalesPromotionManager::ShopSalesPromotionManager()
{
	_recharge_ratting = 0;
}

void ShopSalesPromotionManager::modifyRechargeRatting(int ratting)
{
	_recharge_ratting += ratting;
}

int ShopSalesPromotionManager::getRechargeRatting()
{
	return _recharge_ratting;
}
void ShopSalesPromotionManager::DayPass()
{
	std::list<ShopSalesPromotion*>::iterator it = _shopSalesPromotions.begin();
	for (; it != _shopSalesPromotions.end(); ++ it)
	{
		ShopSalesPromotion* entry = (*it);
		entry->DayPass();
	}

}
ShopSalesPromotionManager::~ShopSalesPromotionManager()
{
}

void ShopSalesPromotionManager::ReqActivityAnnouncement(Session* p)
{
	message::MsgS2CActivityAnnouncementACK msg;
	std::list<ShopSalesPromotion*>::iterator it = _shopSalesPromotions.begin();
	for (; it != _shopSalesPromotions.end(); ++ it)
	{
		ShopSalesPromotion* entry = (*it);
		message::MsgActivity* entryActivity = msg.add_activity();
		entryActivity->set_id(entry->GetID());
		entryActivity->set_name(entry->getName());
		entryActivity->set_time_describe(entry->getTimeDescribe());
		entryActivity->set_describe(entry->getDescribe());
	}
	p->sendPBMessage(&msg);
}

void ShopSalesPromotionManager::init()
{
	const MAPTIMESHOPSALESPROMOTIONCONFIGS* map_sales_promotions = gGameConfig.getTimeShopSalesPromotionConfigs();
	MAPTIMESHOPSALESPROMOTIONCONFIGS::const_iterator it = map_sales_promotions->begin();
	for (; it != map_sales_promotions->end(); ++ it)
	{
		TimeShopPromotion* entry = new TimeShopPromotion();
		if (entry->init(it->first, this))
		{
			_shopSalesPromotions.push_back(entry);
		}
		else
		{
			delete entry;
		}		
	}
}
void ShopSalesPromotionManager::modifyGoldRechargeRatting(int ratting)
{
	_recharge_gold_ratting += ratting;
}
void ShopSalesPromotionManager::modifyJewelRechargeRatting(int ratting)
{
	_recharge_jewel_ratting += ratting;
}


int ShopSalesPromotionManager::getGoldRechargeRatting()
{
	int ratting = _recharge_ratting + _recharge_gold_ratting;
	return ratting;
}
int ShopSalesPromotionManager::getJewelRechargeRatting()
{
	int ratting = _recharge_ratting + _recharge_jewel_ratting;
	return ratting;
}
void ShopSalesPromotionManager::update()
{
	std::list<ShopSalesPromotion*>::iterator it = _shopSalesPromotions.begin();
	while (it != _shopSalesPromotions.end())
	{
		ShopSalesPromotion* entry = (*it);
		if (entry->getType() == ShopSalesPromotionType_Destroy)
		{
			it = _shopSalesPromotions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ShopSalesPromotionManager::modifyCheapGold(int grid, int cheap_gold)
{
	std::map<int, int>::iterator it = _cheap_gold.find(grid);
	if (it == _cheap_gold.end())
	{
		_cheap_gold.insert(std::map<int, int>::value_type(grid,0));
	}
	int number = _cheap_gold[grid];
	number += cheap_gold;
	_cheap_gold[grid] = number;
}

int ShopSalesPromotionManager::getCheapGold(int grid)
{
	int cheap_gold = 0;
	std::map<int, int>::iterator it = _cheap_gold.find(grid);
	if (it != _cheap_gold.end())
	{
		cheap_gold = it->second;
	}
	return cheap_gold;
}

ShopSalesPromotionType ShopSalesPromotion::getType()
{
	return _type;
}

ShopSalesPromotion::ShopSalesPromotion()
{

}

ShopSalesPromotion::~ShopSalesPromotion()
{

}

void ShopSalesPromotion::apply()
{
	if (gEventMgr.hasEvent(this, EVENT_SHOP_SALES_PROMOTION_APPLY))
	{
		gEventMgr.removeEvents(this, EVENT_SHOP_SALES_PROMOTION_APPLY);
	}
	_type = ShopSalesPromotionType_Apply;
}

void DayPass()
{

}

void ShopSalesPromotion::exit()
{
	if (gEventMgr.hasEvent(this, EVENT_SHOP_SALES_PROMOTION_EXIT))
	{
		gEventMgr.removeEvents(this, EVENT_SHOP_SALES_PROMOTION_EXIT);
	
	}
	_type = ShopSalesPromotionType_Wait;
}

void ShopSalesPromotion::DayPass()
{

}
bool ShopSalesPromotion::init(int id, ShopSalesPromotionManager* parent)
{
	_id = id;
	_parent = parent;
	_type = ShopSalesPromotionType_Wait;
	return true;
}


TimeShopPromotion::TimeShopPromotion()
{

}

TimeShopPromotion::~TimeShopPromotion()
{

}

void TimeShopPromotion::DayPass()
{
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	if (config_entry == NULL)
	{

	}

	const EventTimeBase* cur_time = config_entry->GetTime();
	if (_type == ShopSalesPromotionType_Wait)
	{
		switch (cur_time->type_)
		{
		case Time_Week:
		{
			const TimeWeek* timeweek = (const TimeWeek*)cur_time;
			time_t tim_entry = (time_t)g_server_time;
			tm* p1 = localtime(&tim_entry);
			int week_time = p1->tm_wday;
			if (timeweek->show_time_ <= timeweek->end_time_)
			{
				if (week_time >= timeweek->show_time_ && week_time <= timeweek->end_time_)
				{
					apply();
				}
			}
			else
			{
				if (week_time <= timeweek->show_time_ || week_time >= timeweek->end_time_)
				{
					apply();
				}
			}
		}
		break;
		case Time_OpenServerOffsetTime:
		{
			const TimeOpenServerOffsetTime* openserveroffsettime = (const TimeOpenServerOffsetTime*)cur_time;
			int passed_day = gGameConfig.getOpenServerPassedDay(g_server_start_time);
			if (openserveroffsettime->show_time_ > openserveroffsettime->end_time_)
			{
				return;
			}
			if (passed_day >= openserveroffsettime->show_time_ && passed_day <= openserveroffsettime->end_time_)
			{
				apply();
			}

		}
		break;
		default:
			break;
		}
	}
	if (_type == ShopSalesPromotionType_Apply)
	{

		switch (cur_time->type_)
		{
		case Time_Week:
		{
			const TimeWeek* timeweek = (const TimeWeek*)cur_time;
			time_t tim_entry = (time_t)g_server_time;
			tm* p1 = localtime(&tim_entry);
			int week_time = p1->tm_wday;
			if (timeweek->begin_time_ <= timeweek->end_time_)
			{
				if (week_time >= timeweek->begin_time_ && week_time <= timeweek->end_time_)
				{
					excute();
				}
			}
			else
			{
				if (week_time <= timeweek->begin_time_ || week_time >= timeweek->end_time_)
				{
					excute();
				}
			}
		}
		break;
		case Time_OpenServerOffsetTime:
		{
			const TimeOpenServerOffsetTime* openserveroffsettime = (const TimeOpenServerOffsetTime*)cur_time;
			int passed_day = gGameConfig.getOpenServerPassedDay(g_server_time);
			if (openserveroffsettime->show_time_ > openserveroffsettime->end_time_)
			{

			}
			if (passed_day >= openserveroffsettime->begin_time_ && passed_day <= openserveroffsettime->end_time_)
			{
				excute();
			}

		}
		break;
		default:
			break;
		}



	}
	if (_type == ShopSalesPromotionType_Excute)
	{
		switch (cur_time->type_)
		{
		case Time_Week:
		{
			const TimeWeek* timeweek = (const TimeWeek*)cur_time;
			time_t tim_entry = (time_t)g_server_time;
			tm* p1 = localtime(&tim_entry);
			int week_time = p1->tm_wday;
			if (timeweek->begin_time_ <= timeweek->end_time_)
			{
				if (week_time < timeweek->begin_time_ && week_time > timeweek->end_time_)
				{
					exit();
				}
			}
			else
			{
				if (week_time > timeweek->begin_time_ || week_time < timeweek->end_time_)
				{
					exit();
				}
			}
		}
		break;
		case Time_OpenServerOffsetTime:
		{
			const TimeOpenServerOffsetTime* openserveroffsettime = (const TimeOpenServerOffsetTime*)cur_time;
			int passed_day = gGameConfig.getOpenServerPassedDay(g_server_start_time);
			if (openserveroffsettime->show_time_ > openserveroffsettime->end_time_)
			{

			}
			if (passed_day < openserveroffsettime->begin_time_ && passed_day > openserveroffsettime->end_time_)
			{
				exit();
			}
		}
		break;
		default:
			break;
		}
	}

}

bool TimeShopPromotion::init(int id, ShopSalesPromotionManager* parent)
{
	ShopSalesPromotion::init(id, parent);
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	_name = config_entry->getName();
	_time_describe = config_entry->getTimeDescribe();
	_describe = config_entry->getDescribe();
	if (config_entry == NULL)
	{
		return false;
	}

	const EventTimeBase* cur_time = config_entry->GetTime();


	switch (cur_time->type_)
	{
	case Time_Normal:
		{
			const TimeNormal* normal = (const TimeNormal*)cur_time;			
			s64 begin_time = 0;
			s64 end_time = 0;
			s64 show_time = 0;
			begin_time = normal->begin_time_;
			end_time = normal->end_time_;
			show_time = normal->show_time_;
			if (show_time > end_time)
			{
				return false;
			}
			s64 delay_time = show_time - g_server_time;
			if (delay_time <= 0)
			{
				apply();
			}
			else
			{
				gEventMgr.addEvent(this, &TimeShopPromotion::apply, EVENT_SHOP_SALES_PROMOTION_APPLY, delay_time * 1000, 1, 0);
			}
		}
		break;
	case Time_Week:
	case Time_OpenServerOffsetTime:
		{
			DayPass();
		}
		break;
	default:
		break;
	}
	return true;
	
}

void TimeShopPromotion::exit()
{
	ShopSalesPromotion::exit();
	excute(false);
	//const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	//_parent->modifyCheapGold(config_entry->grid_id_, -config_entry->cheap_gold_);
	_type = ShopSalesPromotionType_Destroy;
}


void TimeShopPromotion::excute()
{
	excute(true);
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	const EventTimeBase* cur_time = config_entry->GetTime();
	if (cur_time->type_ == Time_Normal)
	{
		const TimeNormal* normal_time = (const TimeNormal*)cur_time;
		s64 delay_time = normal_time->end_time_ - g_server_time;

		if (delay_time > 0)
		{
			gEventMgr.addEvent(this, &TimeShopPromotion::exit, EVENT_SHOP_SALES_PROMOTION_EXIT, delay_time * 1000, 1, 0);
		}
		else
		{
			exit();
		}
	}
	else if (cur_time->type_ == Time_Week || cur_time->type_ == Time_OpenServerOffsetTime)
	{
		DayPass();
	}
}


void TimeShopPromotion::excute(bool b)
{
	_type = ShopSalesPromotionType_Excute;
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	const EventTimeBase* cur_time = config_entry->GetTime();

	const BaseShopPromotion* promotion = config_entry->GetPromotion();
	switch (promotion->PromotionType_)
	{
	case HeroPromotionType:
	{
		int cheap_gold = 0;

		const HeroPromotion* hero_promotion = (const HeroPromotion*)promotion;

		std::map<int, int>::const_iterator it = hero_promotion->cheap_hero_.begin();
		for (; it != hero_promotion->cheap_hero_.end(); ++ it)
		{
			if (b)
			{
				cheap_gold = it->second;
			}
			else
			{
				cheap_gold = -it->second;
			}
			_parent->modifyCheapGold(it->first, cheap_gold);
		}

	}
	break;
	case RechargePromotionType:
	{
		const RechargeRattingPromotion* recharge_ratting_promotion = (const RechargeRattingPromotion*)promotion;
		int modify = 0;
		if (b)
		{
			modify = recharge_ratting_promotion->recharge_ratting_;
		}
		else
		{
			modify = -recharge_ratting_promotion->recharge_ratting_;
		}

		switch (recharge_ratting_promotion->PromotionType_)
		{
		case RechargeRattingPromotion::ALL:
			_parent->modifyRechargeRatting(modify);
			break;
		case RechargeRattingPromotion::GOLD:
			_parent->modifyGoldRechargeRatting(modify);
			break;
		case RechargeRattingPromotion::JEWEL:
			_parent->modifyJewelRechargeRatting(modify);
			break;
		default:
			break;
		}
	}
		break;
	case OpenHideHeroType:
	{
		const OpenHideHeroPromotion* open_hide_hero_promotion = (const OpenHideHeroPromotion*)promotion;
		std::list<int>::const_iterator it = open_hide_hero_promotion->hide_grid_ids_.begin();
		for (; it != open_hide_hero_promotion->hide_grid_ids_.end(); ++it)
		{
			int grid_id = (*it);
			gGameConfig.setHeroShopConfig(grid_id, b);
		}
	}
	break;
	default:
		break;
	}
	
}

void TimeShopPromotion::apply()
{
	ShopSalesPromotion::apply();
	_type = ShopSalesPromotionType_Apply;
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);	

	const EventTimeBase* cur_time = config_entry->GetTime();

	switch (cur_time->type_)
	{
	case Time_Normal:
	{
		const TimeNormal* normal = (const TimeNormal*)cur_time;
		s64 begin_time = 0;
		s64 end_time = 0;
		s64 show_time = 0;
		begin_time = normal->begin_time_;
		end_time = normal->end_time_;
		show_time = normal->show_time_;

		s64 delay_time = begin_time - g_server_time;
		if (delay_time <= 0)
		{
			excute();
		}
		else
		{
			gEventMgr.addEvent(this, &TimeShopPromotion::excute, EVENT_SHOP_SALES_PROMOTION_APPLY, delay_time * 1000, 1, 0);
		}
	}
	break;
	case Time_Week:
	case Time_OpenServerOffsetTime:
	{
		DayPass();
	}
	break;
	default:
		break;
	}


}