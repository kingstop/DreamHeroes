#pragma once
class DreamHero : public EventableObject
{
public:
	//typedef std::map<u64, message::MsgEquipData> HEROEQUIPS;
	//typedef std::map<std::string, message::MsgToyData> HEROTOYS;
public:
	DreamHero();
	virtual ~DreamHero();
	message::MsgHeroData get_info();
	u64 get_account();
	void set_account(u64 account);
	void set_info(const message::MsgHeroDataDB2GS* info);
	void set_session(Session* session);
	void set_name(const char* name);
	const char* get_name();
	Session* get_session();
	void SaveHero();
	void StartSave();
	void StopSave();
	void LoadFromConfig();
	void SendClientInit();
	void set_online(bool online);
	bool is_online();
public:
	void dayRefresh(bool need_send_msg = true);

public:
	void ReqEnterGame(const message::MsgC2SReqEnterGame* msg);
	void ReqExitGame(const message::MsgC2SReqExitGame* msg);
	void ReqUnlockChapter(const message::MsgC2SReqUnlockChapter* msg);
	void ReqAdvertisementApplyTask(const message::MsgC2SReqAdvertisementApplyTask* msg);
	void ReqAdvertisementRefreshTask(const message::MsgC2SReqAdvertisementRefreshTask* msg);
	void ReqBuyHero(const message::MsgC2SReqBuyHero* msg);
protected:
	message::MsgTaskConfigInfo RadnomTaskInfo();
	void RefreshTask(int give_up_task_id = 0);
protected:

	void sendPBMessage(google::protobuf::Message* p);
protected:
	message::MsgHeroData _info;
	int _current_chapter;
	int _current_section;
	int _current_gold;
	int _current_free_task_count;
	u32 _last_task_advertisement_time;
	int _day_offset_time;
	account_type _account;
	Session* _session;
	bool _online;
};

