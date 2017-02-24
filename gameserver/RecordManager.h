#pragma once
enum RecordType
{
	RecordTypeLogin,
	RecordTypeLoginOut,
	RecordTypeEnterGame,
	RecordTypeLeaveGame,
	RecordTypeTaskComplete,
	RecordTypeTaskAccepte,
	RecordTypeTaskGiveUp,
	RecordTypeChapterUnlock,
	RecordTypeGoldModify,
	RecordTypeBuyHero,
	RecordTypeDealWaitToPay,
	RecordTypeDealToPay,
	RecordTypeWaitToVerify,
	RecordTypeGiveUpDeal,
	RecordTypeRelive,
	RecordTypeOnlineCount,
	RecordTypeBuySpirit,
	RecordDailyGame,
	RecordDailyGameReward,
	RecordDailyLotteryRecord,
	RecordCreateName,
	RecordTypeMax
};


class RecordManager
{
public:
	typedef std::vector<std::string> RECORDS;
	enum GoldModifyType
	{
		GoldModify_LeaveGame,
		GoldModify_KillMonster,
		GoldModify_CompleteTask,
		GoldModify_UnlockChapter,
		GoldModify_BuyHero,
		GoldModify_BuyGold,
		GoldModify_AcceptTask,
		GoldModify_ReliveRecord,
		GoldModify_DailyGameReward,
		GoldModify_DailyLottery,
		GoldModify_DailyConcernWeiXin
	};
public:
	RecordManager();
	virtual ~RecordManager();
public:
	void loginRecord(account_type acc, const char* nick_name);
	void loginOutRecord(account_type acc, const char* nick_name);
	void reliveRecord(account_type acc, const char* nick_name, int gold, int current_gold);
	void enterGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int current_gold);
	void leaveGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, bool success, int gold, int current_gold);
	void taskCompleteRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int task_id, int gold, int current_gold);
	void taskAccepteRecordRecord(account_type acc, const char* nick_name,int gold,int current_gold, int task_id);
	void taskGiveUpRecord(account_type acc, const char* nick_name, int task_id);
	void chapterUnlockRecord(account_type acc, const char* nick_name, int chapter_id, int gold, int current_gold);
	void buyHeroRecord(account_type acc, const char* nick_name, int grid, int gold, int current_gold);
	void goldModifyRecord(account_type acc, const char* nick_name, int gold, int current_gold, GoldModifyType en);
	void gameOnlineRecord(int online_count, int offline_count);
	void dealWaitToPayRecord(account_type acc, const char* key_code, int status, int price, int order_id);
	void dealPayRecord(account_type acc, const char* key_code, int status, int order_id, int modify_gold, int current_gold, int modify_jewel, int current_jewel);
	void dealWaitToVerifyRecord(account_type acc, const char* name, int order_id, const char* recepit);
	void giveUpDealRecord(account_type acc, const char* key_code, int status, int price, int order_id);
	void buySpiritRecord(account_type acc, const char* nick_name, int spirit, int day_buy_count, int current_spirit, int use_jewel, int current_jewel);
	void dailyGameRecord(account_type acc, const char* nick_name, int daily_game_rank, int daily_game_score);
	void dailyGameRewardRecord(account_type acc, const char* nick_name, int daily_game_rank, int daily_gold, int current_gold);
	void dailyLotteryRecord(account_type acc, const char* nick_name, int jewel, int current_jewel, int gold, int current_gold, int lotion_id);
	void generateNameRecord(const char* nick_name, int day_create_count, const char* day_title);
	const char* getCurTime();
	void update();
public:
	RECORDS _record[RecordTypeMax];
	std::string _sql_head[RecordTypeMax];
	char _szTemp[10240];
	std::string _cur_time;
};

