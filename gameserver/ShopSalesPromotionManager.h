#pragma once
#include <iostream>
#include <map>
#include <vector>

class ShopSalesPromotion;

class ShopSalesPromotionManager : public EventableObject
{
	
public:
	ShopSalesPromotionManager();
	virtual ~ShopSalesPromotionManager();
	void init();
	void modifyCheapGold(int grid, int cheap_gold);
	int getCheapGold(int grid);
	void update();
	void modifyRechargeRatting(int ratting);
	void modifyGoldRechargeRatting(int ratting);
	void modifyJewelRechargeRatting(int ratting);
	int getRechargeRatting();
	int getGoldRechargeRatting();
	int getJewelRechargeRatting();
	void DayPass();
	void ReqActivityAnnouncement(Session* p);

protected:
	std::map<int, int> _cheap_gold;
	std::list<ShopSalesPromotion*> _shopSalesPromotions;
	int _recharge_ratting;
	int _recharge_gold_ratting;
	int _recharge_jewel_ratting;
	
};

enum ShopSalesPromotionType
{
	ShopSalesPromotionType_Wait,
	ShopSalesPromotionType_Apply,
	ShopSalesPromotionType_Excute,
	ShopSalesPromotionType_Destroy
};

class ShopSalesPromotion : public EventableObject
{
public:
	ShopSalesPromotion();
	virtual ~ShopSalesPromotion();
public:
	virtual bool init(int id, ShopSalesPromotionManager* parent);
	virtual void apply();
	virtual void exit();
	virtual void DayPass();
	const char* getName()
	{
		return _name.c_str();
	}
	const char* getTimeDescribe()
	{
		return _time_describe.c_str();
	}
	const char* getDescribe()
	{
		return _describe.c_str();
	}
	int GetID()
	{
		return _id;
	}
	ShopSalesPromotionType getType();
protected:
	ShopSalesPromotionManager* _parent;
	int _id;
	ShopSalesPromotionType _type;
	std::string _name;
	std::string _time_describe;
	std::string _describe;
};


class TimeShopPromotion : public ShopSalesPromotion
{
public:
	TimeShopPromotion();
	~TimeShopPromotion();
public:
	bool init(int id, ShopSalesPromotionManager* parent);
	void apply();
	void exit();	
	void excute();
	void DayPass();
private:
	void excute(bool b);
};

