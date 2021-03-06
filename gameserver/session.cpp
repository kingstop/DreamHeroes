#include "stdafx.h"
#include "session.h"
#include "DreamHero.h"
//#include "player.h"
//#include "message/girls.pb.h"
//#include "FRWorld.h"

/************************************************************************/

/*                          注册消息实例                                */  

/*  message::MsgDB2GSQueryCharResult 注册回调    prasePBTest             */

/************************************************************************/
typedef void(Session::*pn_msg_cb)(google::protobuf::Message*);
static std::map<std::string, pn_msg_cb > static_session_cb_funs;
static void registerCBFun(std::string str, pn_msg_cb fun)
{
    static_session_cb_funs.insert(std::make_pair(str, fun));
}

void Session::prasePBDefault(google::protobuf::Message* p)
{
     Mylog::log_server(LOG_INFO, "Parse message name [%s]", p->GetTypeName().c_str());
}

//这里负责注册消息
void Session::registerPBCall()
{
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqGameGlobalConfig), &Session::parseReqGameGlobalConfig);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqShopConfig), &Session::parseReqShopConfig);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqEnterGame), &Session::parseReqEnterGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqExitGame), &Session::parseReqExitGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqUnlockChapter), &Session::parseReqUnlockChapter);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqAdvertisementApplyTask), &Session::parseReqAdvertisementApplyTask);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqAdvertisementRefreshTask), &Session::parseReqAdvertisementRefreshTask);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqGoldShopConfigs), &Session::parseReqGoldShopConfigs);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqBuyHero), &Session::parseReqBuyHero);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqModifyCurrentHero), &Session::parseReqModifyCurrentHero);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqCrearteDeal), &Session::parseReqCrearteDeal);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqVerifyDeal), &Session::parseReqVerifyDeal);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqModifyNewTutorial), &Session::parseReqModifyTutorial);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReliveReq), &Session::parseReqRelive);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqBuySpirit), &Session::parseReqBuySpirit);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqConcernWeiXin), &Session::parseReqConcernWeiXin);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqResetConcernWeiXin), &Session::parseCmdReqResetConcernWeiXin);
	
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqModifyGMLevel), &Session::parseCmdReqMdodifyGMLevel);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqEnterGame), &Session::parseCmdReqEnterGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqResetMap), &Session::parseCmdReqResetMap);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqResetGame), &Session::parseCmdReqResetGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqModifyGold), &Session::parseCmdReqModifyGold);
	registerCBFun(PROTOCO_NAME(message::MsgS2CCmdReqReplaceTask), &Session::parseCmdReqReplaceTask);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqModifyTaskCompleteCount), &Session::parseCmdReqModifyTaskCompleteCount);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqSetSpecialCreatureHis), &Session::parseCmdReqSetSpecialCreatureHis);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqRemoveSpecialCreatureListHis), &Session::parseCmdReqRemoveSpecialCreatureListHis);

	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqModifyJewel), &Session::parseCmdReqModifyJewel);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqModifySpirit), &Session::parseCmdReqModifySpirit);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqBuyLotion), &Session::parseReqBuyLotion);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqDayLottery), &Session::parseReqDayLottery);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqEnterDailyGame), &Session::parseReqEnterDailyGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqReceiveDailyGamePrize), &Session::parseReqReceiveDailyGamePrize);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqUpdateDailyGameProgress), &Session::parseReqUpdateDailyGameProgress);

	///
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqResetDailyLottery), &Session::parseCmdReqResetDailyLottery);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdReqResetDailyGame), &Session::parseCmdReqResetResetDailyGame);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqResetDailyGameProgress), &Session::parseReqResetDailyGameProgress);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqDailyGameRankList), &Session::parseReqDailyGameRankList);
	registerCBFun(PROTOCO_NAME(message::MsgC2SCmdClearDailyRankList), &Session::parseCmdReqClearDailyRankList);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqActivityAnnouncement), &Session::parseReqActivityAnnouncement);
	registerCBFun(PROTOCO_NAME(message::MsgC2SReqModifyNotifyGridState), &Session::parseReqModifyNotifyGridState);
	

}

void Session::parseReqActivityAnnouncement(google::protobuf::Message* p)
{
	gShopSalesPromotionManager.ReqActivityAnnouncement(this);
}
void Session::parseReqResetDailyGameProgress(google::protobuf::Message* p)
{
	message::MsgC2SReqResetDailyGameProgress* msg = (message::MsgC2SReqResetDailyGameProgress*)p;
	if (_dream_hero)
	{
		_dream_hero->ReqResetDailyGameProgress(msg);
	}
}

void Session::parseReqEnterDailyGame(google::protobuf::Message* p)
{
	message::MsgC2SReqEnterDailyGame* msg = (message::MsgC2SReqEnterDailyGame*)p;
	if (_dream_hero)
	{
		_dream_hero->ReqReqEnterDailyGame(msg);
	}
}

void Session::parseReqReceiveDailyGamePrize(google::protobuf::Message* p)
{
	message::MsgC2SReqReceiveDailyGamePrize* msg = (message::MsgC2SReqReceiveDailyGamePrize*)p;
	if (_dream_hero)
	{
		_dream_hero->ReqReceiveDailyGamePrize();
	}

}

void Session::parseReqUpdateDailyGameProgress(google::protobuf::Message* p)
{
	message::MsgC2SReqUpdateDailyGameProgress* msg = (message::MsgC2SReqUpdateDailyGameProgress*)p;
	if (_dream_hero)
	{
		_dream_hero->ReqUpdateDailyGameProgress(msg);
	}

}


void Session::parseReqApplyDeal(google::protobuf::Message* p)
{
	message::MsgC2SReqApplyDeal* msg = (message::MsgC2SReqApplyDeal*)p;
	if (_dream_hero)
	{
		_dream_hero->ReqApplyHeroDeal(msg);
	}
}

void Session::parseReqDayLottery(google::protobuf::Message* p)
{
	if (_dream_hero != NULL)
	{
		message::MsgC2SReqDayLottery* msg = (message::MsgC2SReqDayLottery*)p;
		_dream_hero->ReqDayLottery(msg);

	}
}

void Session::parseReqShopConfig(google::protobuf::Message* p)
{
	const MAPSHOPHEROCONFIGS* shopConfigInfos = gGameConfig.getShopConfigs();
	MAPSHOPHEROCONFIGS::const_iterator it = shopConfigInfos->begin();
	message::MsgS2CShopConfigACK msg;
	for (; it != shopConfigInfos->end(); ++ it)
	{
		message::MsgShopConfigInfo* info = msg.add_shop_infos();
		info->CopyFrom(it->second);
		int grid_ = info->grid_id();
		int cheap_gold =  gShopSalesPromotionManager.getCheapGold(grid_);
		info->set_cheape_gold(cheap_gold);
	}
	sendPBMessage(&msg);	
}


void Session::parseReqModifyNotifyGridState(google::protobuf::Message* p)
{
	const message::MsgC2SReqModifyNotifyGridState* msg = (message::MsgC2SReqModifyNotifyGridState*)p;
	if (_dream_hero == NULL)
	{
		return;
	}
	_dream_hero->ReqModifyNotifyGridState(msg);
}

void Session::parseReqEnterGame(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	message::MsgC2SReqEnterGame* msg = (message::MsgC2SReqEnterGame*)p;
	_dream_hero->ReqEnterGame(msg);
}


void Session::parseCmdReqModifyJewel(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	message::MsgS2CCmdModifyJewelACK msgACK;
	message::GameError error = message::Error_NO;
	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgC2SCmdReqModifyJewel* msg = (message::MsgC2SCmdReqModifyJewel*)p;
		int jewel = _dream_hero->getJewel() + msg->jewel();
		_dream_hero->setJewel(jewel);
	}
	else
	{
		error = message::Error_CmdFailedRequiredGMLevel;
	}
	msgACK.set_current_jewel(_dream_hero->getJewel());
	msgACK.set_error(error);
	sendPBMessage(&msgACK);

}

void Session::parseCmdReqResetDailyLottery(google::protobuf::Message* p)
{
	message::GameError error = message::Error_NO;
	if (_dream_hero)
	{
		if (_dream_hero->getGMLevel() > 0)
		{
			_dream_hero->ResetDailyLottery();
		}
		else
		{
			error = message::Error_CmdFailedRequiredGMLevel;
		}
		message::MsgS2CCmdResetDailyLotteryACK msg;
		msg.set_error(error);
		sendPBMessage(&msg);
	}
}

void Session::parseCmdReqClearDailyRankList(google::protobuf::Message* p)
{
	message::MsgC2SCmdClearDailyRankList* msg = (message::MsgC2SCmdClearDailyRankList*)p;
	message::GameError error = message::Error_NO;
	if (_dream_hero->getGMLevel() > 0)
	{
		gRankManager.ClearRankList();
	}
	else
	{
		error = message::Error_CmdFailedRequiredGMLevel;
	}
	message::MsgS2CCmdClearDailyRankListACK msgACK;
	msgACK.set_error(error);
	sendPBMessage(&msgACK);

}

void Session::set_channel(int channel)
{
	_channel = channel;
}

void Session::parseCmdReqResetResetDailyGame(google::protobuf::Message* p)
{
	message::GameError error = message::Error_NO;
	if (_dream_hero->getGMLevel() > 0)
	{
		_dream_hero->ResetDailyGame();
	}
	else
	{
		error = message::Error_CmdFailedRequiredGMLevel;
	}
	message::MsgS2CCmdResetDailyGameACK msg;
	msg.set_error(error);
	sendPBMessage(&msg);

}

void Session::parseCmdReqModifySpirit(google::protobuf::Message* p)
{
	message::MsgS2CCmdModifySpiritACK msgACK;
	message::GameError error = message::Error_NO;
	
	if (_dream_hero->getGMLevel() > 0)
	{
			message::MsgC2SCmdReqModifySpirit* msg = (message::MsgC2SCmdReqModifySpirit*)p;
			int spirit = _dream_hero->getSpirit() + msg->spirit();
			_dream_hero->setSpirit(spirit);
	}
	else
	{
		error = message::Error_CmdFailedRequiredGMLevel;
	}
	msgACK.set_current_spirit(_dream_hero->getSpirit());
	msgACK.set_error(error);
	sendPBMessage(&msgACK);
}


void Session::parseReqExitGame(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SReqExitGame* msg = (message::MsgC2SReqExitGame*)p;
	_dream_hero->ReqExitGame(msg);
}

void Session::parseReqUnlockChapter(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SReqUnlockChapter* msg = (message::MsgC2SReqUnlockChapter*)p;
	_dream_hero->ReqUnlockChapter(msg);
}

void Session::parseReqRelive(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SReliveReq* msg = (message::MsgC2SReliveReq*)p;
	_dream_hero->ReqReliveReq(msg);
}

void Session::parseReqModifyTutorial(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}	
	message::MsgC2SReqModifyNewTutorial* msg = (message::MsgC2SReqModifyNewTutorial*) p;
	_dream_hero->ReqModifyTutorialFlag(msg);
}


void Session::parseReqAdvertisementApplyTask(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	message::MsgC2SReqAdvertisementApplyTask* msg = (message::MsgC2SReqAdvertisementApplyTask*)p;
	_dream_hero->ReqAdvertisementApplyTask(msg);

}

void Session::parseReqAdvertisementRefreshTask(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SReqAdvertisementRefreshTask* msg = (message::MsgC2SReqAdvertisementRefreshTask*)p;
	_dream_hero->ReqAdvertisementRefreshTask(msg);
}


void Session::parseReqBuyHero(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	message::MsgC2SReqBuyHero* msg = (message::MsgC2SReqBuyHero*)p;
	_dream_hero->ReqBuyHero(msg);

}

void Session::parseReqGoldShopConfigs(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	_dream_hero->ReqGoldShopConfigs();
}

void Session::parseReqModifyCurrentHero(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SReqModifyCurrentHero* msg = (message::MsgC2SReqModifyCurrentHero*)p;
	_dream_hero->ReqModifyCurrentHero(msg->current_grid());

}

void Session::parseReqGameGlobalConfig(google::protobuf::Message* p)
{
	const MAPCHAPTERCONFIGINFOS* chapterConfigInfos = gGameConfig.getChapterConfigInfos();
	message::MsgS2CSGameGlobalConfigACK msg;
	MAPCHAPTERCONFIGINFOS::const_iterator it = chapterConfigInfos->begin();
	for (; it != chapterConfigInfos->end(); ++it)
	{
		message::MsgChapterConfigInfo* ChapterInfo = msg.add_chapter_infos();
		ChapterInfo->CopyFrom(it->second);
	}
	sendPBMessage(&msg);
}

void Session::parseReqDailyGameRankList(google::protobuf::Message* p)
{
	message::MsgC2SReqDailyGameRankList* msg = (message::MsgC2SReqDailyGameRankList*)p;
	RankManager::HEROESDAILYRANK*  ranks = gRankManager.getHeroesRank();
	RankManager::HEROESDAILYRANK::const_iterator it = ranks->begin();
	message::MsgS2CDailyGameRankListACK msgACK;
	for (; it != ranks->end(); ++it)
	{
		const DailyGameRankTg& entry = (*it);
		message::MsgDailyGameHeroInfo* info = msgACK.add_infos();
		info->set_name(entry.name_.c_str());
		info->set_rank(entry.rank_);
		info->set_score(entry.score_);
	}
	sendPBMessage(&msgACK);

}
void Session::parseReqCrearteDeal(google::protobuf::Message* p)
{
	message::MsgC2SReqCrearteDeal* msg = (message::MsgC2SReqCrearteDeal*)p;
	if (_dream_hero != NULL)
	{
		_dream_hero->ReqCrearteDeal(msg);
	}	
}

void Session::parseReqVerifyDeal(google::protobuf::Message* p)
{
	message::MsgC2SReqVerifyDeal* msg = (message::MsgC2SReqVerifyDeal*)p;
	if (_dream_hero != NULL)
	{
		_dream_hero->ReqVerifyDeal(msg);
	}
	
}


int Session::getState()
{
	return m_state;
}



void Session::parsePBMessage(google::protobuf::Message* p)
{
    std::map<std::string, pn_msg_cb >::iterator it = static_session_cb_funs.find(p->GetTypeName());
    if (it != static_session_cb_funs.end())
    {
        pn_msg_cb fun = boost::ref( it->second);
        if ( NULL != fun )
        {
            (this->*fun)(p);
            return ;
        }
    }
    prasePBDefault(p);
}

//////////////////////////////////////////////////////////////////////////

Session::Session(tran_id_type t, account_type a, u16 gate)
    :m_tranid(t), m_account(a), m_gate(gate),m_state(_session_online_), _dream_hero(NULL)
{
	DreamHero* hero = gDreamHeroManager.GetHero(m_account);
	if (hero)
	{
		hero->StopDestroyClock();
	}
	
	if (hero != NULL)
	{
		_dream_hero = hero;
		_dream_hero->set_session(this);
		_dream_hero->SendClientInit();
		
	}
	else
	{
		message::MsgReqHeroDataGS2DB msg;
		msg.set_account(m_account);
		gGSDBClient.sendPBMessage(&msg, m_tranid);
	}

}

Session::~Session()
{
	if (_dream_hero != NULL)
	{
		_dream_hero->StartDestroyTime();
		_dream_hero->set_session(NULL);
	}	
	_dream_hero = NULL;
}

int Session::get_channel()
{
	return _channel;
}

void Session::close()
{
	if (_dream_hero != NULL)
	{
		_dream_hero->StartDestroyTime();
	}
	
}

void Session::setReconnet()
{
    m_state = _session_online_;
	if (_dream_hero)
	{
		_dream_hero->set_online(true);
	}
}

void Session::setDreamHero(DreamHero* p)
{
	_dream_hero = p;
}

void Session::setWaitReconnet()
{
    m_state = _session_offline_;
	if (_dream_hero)
	{
		_dream_hero->set_online(false);
	}	
}

void Session::sendPBMessage(google::protobuf::Message* p)
{
    if (m_state == _session_online_)
    {
	    gGSGateManager.sendMessage(p, m_tranid, m_gate);
    }
	else
    {

    }
}



void Session::parseDBQueryNeedCreateHero()
{
	_dream_hero = gDreamHeroManager.CreateHero(m_account, this);
	if (_dream_hero)
	{
		_dream_hero->SendClientInit();
	}
}

/*
void ReqReliveReq(const message::MsgC2SReliveReq* msg);
void ReqBuySpirit(const message::MsgC2SReqBuySpirit* msg);
*/



void Session::parseReqBuySpirit(google::protobuf::Message* p)
{
	if (_dream_hero)
	{
		message::MsgC2SReqBuySpirit* msg = (message::MsgC2SReqBuySpirit*)p;
		_dream_hero->ReqBuySpirit(msg);
	}
}

void Session::parseReqConcernWeiXin(google::protobuf::Message* p)
{
	if (_dream_hero)
	{
		message::MsgC2SReqConcernWeiXin* msg = (message::MsgC2SReqConcernWeiXin*)p;
		_dream_hero->ReqConcernWeiXin(msg);

	}
}


void Session::praseDBQueryHeroInfo(message::MsgHeroDataDB2GS* HeroDataMsg)
{

	if (HeroDataMsg)
	{
		if (_dream_hero == NULL)
		{
			_dream_hero = gDreamHeroManager.CreateHero(HeroDataMsg, m_account, this);
		}

		if (_dream_hero)
		{
			_dream_hero->SendClientInit();
		}		
	}
}

DreamHero* Session::get_dream_hero()
{
	return _dream_hero;
}

void Session::parseCmdReqMdodifyGMLevel(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SCmdReqModifyGMLevel* msg = (message::MsgC2SCmdReqModifyGMLevel*)p;
	message::MsgS2CCmdModifyGMLevelACK msgACK;
	msgACK.set_level(msg->level());
	msgACK.set_name(msg->name().c_str());
	message::GameError error = message::Error_NO;
	
	if (_dream_hero->getGMLevel() < 3)
	{
		error = message::Error_CmdFailedRequiredGMLevel;
	}
	else
	{
		int level_temp = msg->level();
		std::string name = msg->name();
		DreamHero* hero =gDreamHeroManager.GetHeroByName(name.c_str());
		if (hero)
		{
			hero->SetGMLevel(level_temp);
		}
		else
		{
			char szTemp[512];
			sprintf(szTemp, "update `character` set `gm_level` = %d where `name` = '%s'", level_temp, name.c_str());
			message::MsgSaveDataGS2DB msg_db;
			msg_db.set_sql(szTemp);
			gGSDBClient.sendPBMessage(&msg_db, getTranId());
		}
	}
	msgACK.set_error(error);
	sendPBMessage(&msgACK);
}

void Session::parseCmdReqEnterGame(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	message::MsgC2SCmdReqEnterGame* msg = (message::MsgC2SCmdReqEnterGame*)p;
	if (_dream_hero->getGMLevel() > 0)
	{
		_dream_hero->EnterGame(msg->chapter_id(), msg->section_id(), true);
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}

void Session::parseCmdReqResetMap(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgC2SCmdReqResetMap* msg = (message::MsgC2SCmdReqResetMap*)p;
		_dream_hero->ReqResetMap(msg);
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}


void Session::parseCmdReqResetConcernWeiXin(google::protobuf::Message* p)
{

	if (_dream_hero == NULL)
	{
		return;
	}
	message::GameError error = message::Error_NO;
	message::MsgS2CCmdResetConcernWeiXinACK msg;
	if (_dream_hero->getGMLevel() > 0)
	{
		_dream_hero->ResetWeiXin();
	}
	else
	{
		error = message::Error_CmdFailedRequiredGMLevel;		
	}
	msg.set_error(error);
	sendPBMessage(&msg);
}

void Session::parseCmdReqResetGame(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	if (_dream_hero->getGMLevel() > 0)
	{
		_dream_hero->ResetGame();
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}


void Session::parseCmdReqModifyGold(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}

	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgC2SCmdReqModifyGold* msg = (message::MsgC2SCmdReqModifyGold*) p;
		_dream_hero->ReqModifyGold(msg);
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}	
}

void Session::parseCmdReqReplaceTask(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgS2CCmdReqReplaceTask* msg = (message::MsgS2CCmdReqReplaceTask*) p;
		_dream_hero->ReqReplaceTask(msg);

	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}

void Session::parseCmdReqModifyTaskCompleteCount(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgC2SCmdReqModifyTaskCompleteCount* msg = (message::MsgC2SCmdReqModifyTaskCompleteCount*) p;
		_dream_hero->ReqModifyTaskCompleteCount(msg);

	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}

void Session::parseCmdReqRemoveSpecialCreatureListHis(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	if (_dream_hero->getGMLevel() > 0)
	{
		_dream_hero->ReqRemoveAllSpecialCreatureList();
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
}

void Session::parseReqBuyLotion(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	message::MsgC2SReqBuyLotion* msg = (message::MsgC2SReqBuyLotion*)p;
	_dream_hero->ReqBuyLotion(msg);
}

void Session::parseCmdReqSetSpecialCreatureHis(google::protobuf::Message* p)
{
	if (_dream_hero == NULL)
	{
		return;
	}
	if (_dream_hero->getGMLevel() > 0)
	{
		message::MsgC2SCmdReqSetSpecialCreatureHis* msg = (message::MsgC2SCmdReqSetSpecialCreatureHis*)p;
		_dream_hero->ReqSetSpecialCreatureList(msg->creature_id(), msg->status());
	}
	else
	{
		message::MsgS2CNotifyError msgError;
		msgError.set_error(message::Error_CmdFailedRequiredGMLevel);
		sendPBMessage(&msgError);
	}
	

}