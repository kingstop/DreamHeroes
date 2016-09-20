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

void Session::parseReqEnterGame(google::protobuf::Message* p)
{
	message::MsgC2SReqEnterGame* msg = (message::MsgC2SReqEnterGame*)p;
	_dream_hero->ReqEnterGame(msg);
}


void Session::parseReqExitGame(google::protobuf::Message* p)
{
	message::MsgC2SReqExitGame* msg = (message::MsgC2SReqExitGame*)p;
	_dream_hero->ReqExitGame(msg);
}

void Session::parseReqUnlockChapter(google::protobuf::Message* p)
{
	message::MsgC2SReqUnlockChapter* msg = (message::MsgC2SReqUnlockChapter*)p;
	_dream_hero->ReqUnlockChapter(msg);
}

void Session::parseReqAdvertisementApplyTask(google::protobuf::Message* p)
{
	message::MsgC2SReqAdvertisementApplyTask* msg = (message::MsgC2SReqAdvertisementApplyTask*)p;
	_dream_hero->ReqAdvertisementApplyTask(msg);

}

void Session::parseReqAdvertisementRefreshTask(google::protobuf::Message* p)
{
	message::MsgC2SReqAdvertisementRefreshTask* msg = (message::MsgC2SReqAdvertisementRefreshTask*)p;
	_dream_hero->ReqAdvertisementRefreshTask(msg);
}


void Session::parseReqBuyHero(google::protobuf::Message* p)
{
	message::MsgC2SReqBuyHero* msg = (message::MsgC2SReqBuyHero*)p;
	_dream_hero->ReqBuyHero(msg);

}

void Session::parseReqGoldShopConfigs(google::protobuf::Message* p)
{
	_dream_hero->ReqGoldShopConfigs();
}

void Session::parseReqModifyCurrentHero(google::protobuf::Message* p)
{
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
	DreamHero* heros = gDreamHeroManager.GetHero(m_account);
	if (heros != NULL)
	{
		_dream_hero = heros;
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

}

void Session::close()
{

}

void Session::setReconnet()
{
    m_state = _session_online_;
	if (_dream_hero)
	{
		_dream_hero->set_online(true);
	}
}

void Session::setWaitReconnet()
{
    m_state = _session_offline_;
	_dream_hero->set_online(false);
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

void Session::praseDBQueryHeroInfo(message::MsgHeroDataDB2GS* HeroDataMsg)
{
	if (HeroDataMsg)
	{
		_dream_hero = gDreamHeroManager.CreateHero(HeroDataMsg, m_account, this);
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

