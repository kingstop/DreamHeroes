#pragma once
enum RecordType
{
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
	RecotdTypeGiveUpDeal,
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
		GoldModify_BuyGold
	};
public:
	RecordManager();
	virtual ~RecordManager();
public:
	void enterGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int current_gold);
	void leaveGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, bool success, int gold, int current_gold);
	void taskCompleteRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int task_id, int gold, int current_gold);
	void taskAccepteRecordRecord(account_type acc, const char* nick_name, int task_id);
	void taskGiveUpRecord(account_type acc, const char* nick_name, int task_id);
	void chapterUnlockRecord(account_type acc, const char* nick_name, int chapter_id, int gold, int current_gold);
	void buyHeroRecord(account_type acc, const char* nick_name, int grid, int gold, int current_gold);
	void goldModifyRecord(account_type acc, const char* nick_name, int gold, int current_gold, GoldModifyType en);

	void dealWaitToPayRecord(account_type acc, const char* key_code, int status, int price, int order_id);
	void dealPayRecord(account_type acc, const char* key_code, int status, int order_id, int modify_gold, int current_gold);
	void dealWaitToVerifyRecord(account_type acc, const char* name, int order_id, const char* recepit);
	void giveUpDealRecord(account_type acc, const char* key_code, int status, int price, int order_id);
	const char* getCurTime();
	void update();
public:
	RECORDS _record[RecordTypeMax];
	std::string _sql_head[RecordTypeMax];
	char _szTemp[10240];
	std::string _cur_time;
};

