#pragma once

struct DealWaitToPay
{
	std::string key_code_;
	int status_;
	int price_;
	int order_id_;
	std::string receipt_;
	DealStatusType type_;
};
class DreamHero : public EventableObject
{
public:
	//typedef std::map<u64, message::MsgEquipData> HEROEQUIPS;
	//typedef std::map<std::string, message::MsgToyData> HEROTOYS;
	typedef std::map<int, DealWaitToPay> DEALSWAITTOPAY;
	typedef std::map<std::pair<int, int>, std::vector<message::MsgObjConfig> > SPECIALKILLS;
	typedef std::map<int, int> SPECIALCREATURES;
public:
	DreamHero();
	virtual ~DreamHero();
	message::MsgHeroData get_info();
	u64 get_account();
	void set_account(u64 account);
	void set_info(const message::MsgHeroDataDB2GS* info);
	void set_session(Session* session);
	void set_name(const char* name);
	void set_parent(DreamHeroManager* p);
	const char* get_name();
	Session* get_session();
	void SaveHero();
	void StartSave();
	void StopSave();
	void LoadFromConfig();
	void SendClientInit();
	void set_online(bool online);
	bool is_online();
	void StartDestroyTime();
	void StopDestroyClock();
	void recoverSpirit();
	//void StartPing();
public:
	void dayRefresh(bool need_send_msg = true);
	int getGMLevel();
	void SetGMLevel(int level);
public:
	void ReqEnterGame(const message::MsgC2SReqEnterGame* msg);
	void ReqExitGame(const message::MsgC2SReqExitGame* msg);
	void ReqUnlockChapter(const message::MsgC2SReqUnlockChapter* msg);
	void ReqAdvertisementApplyTask(const message::MsgC2SReqAdvertisementApplyTask* msg);
	void ReqAdvertisementRefreshTask(const message::MsgC2SReqAdvertisementRefreshTask* msg);
	void ReqBuyHero(const message::MsgC2SReqBuyHero* msg);
	void ReqResetMap(const message::MsgC2SCmdReqResetMap* msg);
	void ReqModifyGold(const message::MsgC2SCmdReqModifyGold* msg);
	void ReqReplaceTask(const message::MsgS2CCmdReqReplaceTask* msg);
	void ReqModifyTaskCompleteCount(const message::MsgC2SCmdReqModifyTaskCompleteCount* msg);
	void ReqCrearteDeal(const message::MsgC2SReqCrearteDeal* msg);
	void ReqVerifyDeal(const message::MsgC2SReqVerifyDeal* msg);
	void ReqModifyTutorialFlag(const message::MsgC2SReqModifyNewTutorial* msg);
	void ReqReliveReq(const message::MsgC2SReliveReq* msg);
	void ReqBuySpirit(const message::MsgC2SReqBuySpirit* msg);
	void ReqBuyLotion(const message::MsgC2SReqBuyLotion* msg);
	void ReqDayLottery(const message::MsgC2SReqDayLottery* msg);
	void ReqApplyHeroDeal(const message::MsgC2SReqApplyDeal* msg);
	void ReqReqEnterDailyGame(const message::MsgC2SReqEnterDailyGame* msg);
	void ReqUpdateDailyGameProgress(const message::MsgC2SReqUpdateDailyGameProgress* msg);
	void ReqResetDailyGameProgress(const message::MsgC2SReqResetDailyGameProgress* msg);
	void ReqReceiveDailyGamePrize();


	void ReqRemoveAllSpecialCreatureList();
	void ReqSetSpecialCreatureList(int creature_id, int status);
	void ReqModifyCurrentHero(int grid_id);
	void ReqGoldShopConfigs();
	void EnterGame(int chapter_id, int section_id, bool admin = false);
	void ResetGame();
	void ResetDailyGame();
	void ResetDailyLottery();
	void SendResetGameACK(message::GameError en);
	void DailyGamePrize(int gold);
	void TryToGetGamePrize(bool sendmsg);
public:
	void addDealWaitToPay(std::string key_code, int status, int price, int order_id, const char* notify_url,
		message::GameError error);
	void addDealPay(std::string key_code, int status, int order_id, message::GameError error, bool send_msg = true);
	//void completeDealByOrder(const char* order_id, bool success ,bool needmsg);
	

public:
	int	getSpirit();
	void setSpirit(int spirit);
	int	getJewel();
	void setJewel(int jewel);

protected:
	message::MsgTaskConfigInfo RadnomTaskInfo(int give_up_task = 0);
	void RefreshTask(int give_up_task_id, bool gold);	
	
	void Destroy();
protected:
	void fillSpecialCreatureList(::google::protobuf::RepeatedPtrField< ::message::MsgIntPair >* list);
	void sendPBMessage(google::protobuf::Message* p);
	bool isInToday(u32 time);
	//void pingNotify();
protected:
	SPECIALKILLS _special_kills;
	SPECIALCREATURES _special_creatures;
	message::MsgHeroData _info;
	int _current_chapter;
	int _current_section;
	int _current_task_count;
	u32 _last_task_advertisement_time;
	int _day_offset_time;
	account_type _account;
	Session* _session;
	bool _online;
	DreamHeroManager* _parent;
	DEALSWAITTOPAY _deals_wait_to_pay;
	int _gm_level;
	int _ping_count;
	u32 _last_recover_spirit_time;
	u32 _last_buy_spirit_time;
	u32 _last_day_lottery_time;
	bool _destroy_clock;
	u32 _daily_game_time;
	u32 _daily_game_prize_time;
	u32 _last_daily_reset_game_time;
	//std::map<std::string, message::MsgHeroDealInfo> _hero_deals;
	
};

