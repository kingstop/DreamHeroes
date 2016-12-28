#ifndef __player_session_h__
#define __player_session_h__
class DreamHero;
class Session :PUBLIC_BASE_OBJECT(Session)
{
	REGISTER_POOL_INFO(Session, 100, 0)
public:
	enum
	{
		_session_online_,
		_session_offline_,
	};
	void prasePBDefault(google::protobuf::Message* p);	
	static void registerPBCall();
public:
	void praseDBQueryHeroInfo(message::MsgHeroDataDB2GS* HeroDataMsg);
	void parseDBQueryNeedCreateHero();
	DreamHero* get_dream_hero();
public:
	Session(tran_id_type t, account_type a, u16 gate);
	~Session();
	void close();
	void setReconnet();
	void setWaitReconnet();
	void setDreamHero(DreamHero* p);
	DreamHero* getDreamHero() { return _dream_hero; }
	u16  getGateId() const {return m_gate ;}
	tran_id_type getTranId() const {return m_tranid ;}
	account_type getAccount() const {return m_account ;}
	void sendPBMessage(google::protobuf::Message* p);
	void parsePBMessage(google::protobuf::Message* p);

	void parseReqShopConfig(google::protobuf::Message* p);
	void parseReqGameGlobalConfig(google::protobuf::Message* p);
	void parseReqEnterGame(google::protobuf::Message* p);
	void parseReqExitGame(google::protobuf::Message* p);
	void parseReqUnlockChapter(google::protobuf::Message* p);
	void parseReqAdvertisementApplyTask(google::protobuf::Message* p);
	void parseReqAdvertisementRefreshTask(google::protobuf::Message* p);
	void parseReqBuyHero(google::protobuf::Message* p);
	void parseReqGoldShopConfigs(google::protobuf::Message* p);
	void parseReqModifyCurrentHero(google::protobuf::Message* p);
	void parseReqCrearteDeal(google::protobuf::Message* p);
	void parseReqVerifyDeal(google::protobuf::Message* p);
	void parseReqModifyTutorial(google::protobuf::Message* p);
	void parseReqRelive(google::protobuf::Message* p);
	void parseReqBuySpirit(google::protobuf::Message* p);
	void parseReqBuyLotion(google::protobuf::Message* p);
	void parseReqDayLottery(google::protobuf::Message* p);
	void parseReqApplyDeal(google::protobuf::Message* p);
	void parseReqEnterDailyGame(google::protobuf::Message* p);
	void parseReqUpdateDailyGameProgress(google::protobuf::Message* p);
	void parseReqReceiveDailyGamePrize(google::protobuf::Message* p);
	void parseReqResetDailyGameProgress(google::protobuf::Message* p);
public:
	void parseCmdReqMdodifyGMLevel(google::protobuf::Message* p);
	void parseCmdReqEnterGame(google::protobuf::Message* p);
	void parseCmdReqResetMap(google::protobuf::Message* p);
	void parseCmdReqResetGame(google::protobuf::Message* p);
	void parseCmdReqModifyGold(google::protobuf::Message* p);
	void parseCmdReqReplaceTask(google::protobuf::Message* p);
	void parseCmdReqModifyTaskCompleteCount(google::protobuf::Message* p);
	void parseCmdReqRemoveSpecialCreatureListHis(google::protobuf::Message* p);
	void parseCmdReqSetSpecialCreatureHis(google::protobuf::Message* p);
	void parseCmdReqModifyJewel(google::protobuf::Message* p);
	void parseCmdReqModifySpirit(google::protobuf::Message* p);
	void parseCmdReqResetDailyLottery(google::protobuf::Message* p);
	void parseCmdReqResetResetDailyGame(google::protobuf::Message* p);

	int getState();

protected:
	tran_id_type m_tranid;
	account_type m_account;
	u16 m_gate;
	u8  m_state;
	DreamHero* _dream_hero;
private:
};
#endif

