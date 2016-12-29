#include "stdafx.h"
#include "db_quest.h"
#include "game_session.h"
#include <stdlib.h>
#include <stdio.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#define _SAVE_TO_CLOSE_TIME_ (15 * _TIME_SECOND_MSEL_)
const char* newGUID()  
{  
	static std::string stc_string;
	static char buf[64] = {0};
	using namespace std;
	using namespace boost::uuids;
	random_generator gen;
	uuid ranuuid = gen();
	stringstream ss;
	ss << ranuuid;
	ss >> stc_string;
	return stc_string.c_str();  
} 
enum
{
	_QUERY_HERO_INFO_ = _NORMAL_THREAD + 1,
	_QUERY_HERO_MAP_INFO_,
	_SAVE_HERO_INFO_,
	_SAVE_RECORD_,
	_QUERRY_VERIFY_TOY_CDKEY_

};
struct tgHeroData
{
	tgHeroData(account_type acc, tran_id_type tran, u16 gs)
	{
		account = acc;
		tranid = tran;
		gsid = gs;
	}
	tgHeroData(tgHeroData* hero)
	{
		account = hero->account;
		tranid = hero->tranid;
		info = hero->info;
		gsid = hero->gsid;
	}
	account_type account;
	tran_id_type tranid;
	message::MsgHeroDataDB2GS info;
	u16 gsid;
};

struct tgVerify
{
	tgVerify(account_type acc, tran_id_type tran, u16 gs, const char* k)
	{
		account = acc;
		tranid = tran;
		gsid = gs;
		cd_key = k;
	}

	account_type account;
	tran_id_type tranid;
	u16 gsid;
	std::string cd_key;
};


DBQuestManager::DBQuestManager()
{
	_receive_cose_msg = false;
	//saveToClose(3);
}

DBQuestManager::~DBQuestManager()
{

}

void DBQuestManager::saveHeroMap(message::MsgSaveHeroDataGS2DB* msg)
{
	
}

void DBQuestManager::queryHeroInfo(account_type a, tran_id_type t, u16 gs)
{
	char sz_sql[1024];
	sprintf(sz_sql, "select * ,UNIX_TIMESTAMP(`last_lottery_time`),UNIX_TIMESTAMP(`last_recover_spirit_time`),\
	 UNIX_TIMESTAMP(`last_buy_spirit_time`), UNIX_TIMESTAMP(`last_task_advertisement_time`), UNIX_TIMESTAMP(`daily_game_time`),\
	UNIX_TIMESTAMP(`daily_game_prize_time`), UNIX_TIMESTAMP(`last_daily_reset_game_time`) from `character` where `account_id`=%llu;",a);
	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQueryHeroInfo, sz_sql, 0, new tgHeroData(a, t, gs), _QUERY_HERO_INFO_);
}


void DBQuestManager::onSaveToClose()
{
	gDBCharDatabase.setObjState(RunObject::_wait_stop_);
	gDBRecordDatabase.setObjState(RunObject::_wait_stop_);
	Mylog::log_server(LOG_INFO, "close character mysql and record mysql!");
}

//void DBQuestManager::verifyToyCDKey(tran_id_type t, u16 gs, account_type acc, const char* cdkey)
//{
//	char sz_sql[256];
//	sprintf(sz_sql, "select * from `character_toy` where `toy_cd_key`='%s';", cdkey);
//
//	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQuerryToyVerify, sz_sql, 0, new tgVerify(acc, t, gs, cdkey), _QUERRY_VERIFY_TOY_CDKEY_);
//
//}

void DBQuestManager::saveToClose(u16 gsid)
{
	if (_receive_cose_msg == false)
	{
		if (gEventMgr.hasEvent(this, EVENT_SAVE_TO_CLOSE) == false)
		{
			gDBCharDatabase.set_close_gs_id(gsid);
			gEventMgr.addEvent(this, &DBQuestManager::onSaveToClose, EVENT_SAVE_TO_CLOSE, _SAVE_TO_CLOSE_TIME_, 1, 0);
		}
		_receive_cose_msg = true;
		Mylog::log_server(LOG_INFO, "receive save all heroes msg!");
		message::MsgSaveAllHeroesDB2GS msg;
		gDBGameManager.sendMessage(&msg, 0, gsid);
	}
}


void DBQuestManager::saveSqlData(const char* sql)
{
	std::string str_excute = sql;
	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbCallNothing, str_excute.c_str(), 0, NULL, _SAVE_HERO_INFO_);
}

void DBQuestManager::saveSqlRecord(const char* sql)
{
	std::string str_excute = sql;
	gDBRecordDatabase.addSQueryTask(this, &DBQuestManager::dbCallNothing, str_excute.c_str(), 0, NULL, _SAVE_RECORD_);
}


void DBQuestManager::dbDoQueryHeroDealInfos(const SDBResult* r, const void* d, bool s)
{
	//tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
	//if (!pkParm)
	//{
	//	return;
	//}
	//if (r != NULL)
	//{
	//	const SDBResult& result = *r;
	//	int row_count = result.num_rows();
	//	for (int i = 0; i < row_count; i++)
	//	{
	//		message::MsgHeroDealInfo* entry = pkParm->info.add_hero_deal_infos();
	//		const mysqlpp::Row row = result[i];
	//		entry->set_order_id(row["order_id"].c_str());
	//		entry->set_product_id(row["product_key"].c_str());
	//		int status = row["status"];
	//		message::HeroDealType type = (message::HeroDealType) status;
	//		entry->set_type(type);
	//		u32 create_time = row["UNIX_TIMESTAMP[`create_time`]"];
	//		entry->set_createtime(create_time);
	//	}
	//}
	//gDBGameManager.sendMessage(&pkParm->info, pkParm->tranid, pkParm->gsid);
}

void DBQuestManager::dbDoQueryHeroDeals(const SDBResult* r, const void* d, bool s)
{
	tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
	if (!pkParm)
	{
		return;
	}

	if (r != NULL)
	{
		const SDBResult& result = *r;
		int row_count = result.num_rows();
		for (int i = 0; i < row_count; i ++)
		{
			const mysqlpp::Row row = result[i];
			message::MsgDealInfo msgDeal;
			message::MsgDealInfo* deal_info = pkParm->info.add_deals();
			account_type acc = row["account_id"];
			deal_info->set_order(row["order_id"]);
			deal_info->set_product_id(row["key_code"].c_str());
			deal_info->set_status(row["status"]);
			deal_info->set_complete_status(row["complete_status"]);	
			std::string receipt = row["receipt"].c_str();
			deal_info->set_cur_receipt(receipt.c_str());
			
		}

	}
	char sz_sql[256];
	gDBGameManager.sendMessage(&pkParm->info, pkParm->tranid, pkParm->gsid);

	//sprintf(sz_sql, "select *,UNIX_TIMESTAMP(`create_time`) from `heroes_deal` where `account_id`=%llu and status!=2", pkParm->account);
	//gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQueryHeroDealInfos, sz_sql, 0, new tgHeroData(pkParm), _QUERY_HERO_INFO_);

	
}

void DBQuestManager::dbDoQueryHeroInfo(const SDBResult* r, const void* d, bool s)
{
	tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
	if (!pkParm)
	{
		return;
	}

	bool need_create = true;
	if (r != NULL)
	{
		const SDBResult& result = *r;
		if (result.num_rows() > 0)
		{
			const mysqlpp::Row row = result[0];
			account_type acc = row["account_id"];
			pkParm->info.set_account(acc);
			message::MsgHeroData* data = pkParm->info.mutable_data();
			std::string name_temp = row["name"].c_str();
			data->set_name(name_temp.c_str());	
			data->set_gold(row["gold"]);
			data->set_current_hero(row["current_hero"]);
			data->set_complete_task_count(row["complete_task_count"]);
			data->set_new_tutorial(row["tutorial_flag"]);
			data->set_jewel(row["jewel"]);
			data->set_spirit(row["spirit"]);
			data->set_day_buy_spirit(row["day_buy_spirit"]);
			data->set_daily_game_gold(row["daily_game_gold"]);
			data->set_daily_game_progress(row["daily_game_progress"]);
			data->set_daily_game_score(row["daily_game_score"]);
			data->set_daily_game_hp_pct(row["daily_game_hp_pct"]);
			data->set_daily_game_record_progress(row["daily_game_record_progress"]);
			data->set_daily_reset_game_count(row["daily_reset_game_count"]);
			data->set_daily_top_grogress(row["daily_top_grogress"]);
			pkParm->info.set_current_section(row["current_section"]);
			pkParm->info.set_current_chapter(row["current_chapter"]);
			pkParm->info.set_free_task_count(row["free_task_count"]);
			pkParm->info.set_last_task_advertisement_time(row["UNIX_TIMESTAMP(`last_task_advertisement_time`)"]);
			pkParm->info.set_last_buy_spirit_time(row["UNIX_TIMESTAMP(`last_buy_spirit_time`)"]);
			pkParm->info.set_last_recover_spirit_time(row["UNIX_TIMESTAMP(`last_recover_spirit_time`)"]);
			pkParm->info.set_gm_level(row["gm_level"]);	
			pkParm->info.set_last_lottery_time(row["UNIX_TIMESTAMP(`last_lottery_time`)"]);//last_lottery_time
			pkParm->info.set_daily_game_time(row["UNIX_TIMESTAMP(`daily_game_time`)"]);
			pkParm->info.set_daily_game_prize_time(row["UNIX_TIMESTAMP(`daily_game_prize_time`)"]);
			
			pkParm->info.set_last_daily_reset_game_time(row["UNIX_TIMESTAMP(`last_daily_reset_game_time`)"]);
			std::string records_str;
			records_str = row["record_his"].c_str();
			std::vector<std::string> outVC;
			std::vector<std::string> outVC1;
			std::vector<std::string> outVC2;
			SplitStringA(records_str, ";", outVC);
			int length_vc = outVC.size();
			std::string strTemp;
			for (int i = 0; i < length_vc; i ++)
			{
				strTemp = outVC[i];
				outVC1.clear();
				SplitStringA(strTemp, ",", outVC1);
				int length_temp = outVC1.size();
				if (length_temp == 2)
				{
					message::MsgIntPair* intPair = data->mutable_records()->Add();
					int number_1 = atoi(outVC1[0].c_str());
					int number_2 = atoi(outVC1[1].c_str());
					intPair->set_number_1(number_1);
					intPair->set_number_2(number_2);
				}
			}
			std::string heroesstate;
			heroesstate = row["heroes_state"].c_str();
			outVC.clear();
			SplitStringA(heroesstate, ";", outVC);
			length_vc = outVC.size();
			for (int i = 0; i < length_vc; i ++)
			{
				strTemp = outVC[i];
				int number_1 = atoi(strTemp.c_str());
				bool b_grid = false;
				if (number_1 != 0)
				{
					b_grid = true;
				}
				data->add_heroes(b_grid);
			}

			std::string heroestasks;
			heroestasks = row["tasks"].c_str();
			outVC.clear();
			SplitStringA(heroestasks, ";", outVC);
			length_vc = outVC.size();
			for (int i = 0; i < length_vc; i ++)
			{
				strTemp = outVC[i];
				outVC1.clear();
				SplitStringA(strTemp, ",", outVC1);
				if (outVC1.size() == 3)
				{
					message::MsgTaskInfo* msg = data->mutable_tasks()->Add();
					msg->set_taskid(atoi(outVC1[0].c_str()));					
					msg->set_argument_1(atoi(outVC1[1].c_str()));
					msg->set_usetime(atoi(outVC1[2].c_str()));					
				}
			}
			
			std::string herospecialkills;
			herospecialkills = row["special_kill"].c_str();
			outVC.clear();
			SplitStringA(herospecialkills, ";", outVC);
			length_vc = outVC.size();
			for (int i = 0; i < length_vc; i++)
			{
				strTemp = outVC[i];
				outVC1.clear();
				SplitStringA(strTemp, ":", outVC1);
				if (outVC1.size() >= 2)
				{
					SplitStringA(outVC1[0], ",", outVC2);
					if (outVC2.size() == 2)
					{
						
						int chapter_id = atoi(outVC2[0].c_str());
						int section_id = atoi(outVC2[1].c_str());
						message::MsgMapSpecialKill* map_kill_entry = pkParm->info.add_special_kills();
						map_kill_entry->set_chapter_id(chapter_id);
						map_kill_entry->set_section_id(section_id);
						for (int i = 1; i < outVC1.size(); i++)
						{
							outVC2.clear();
							SplitStringA(outVC1[i], ",", outVC2);
							if (outVC2.size() == 2)
							{
								message::MsgObjConfig* obj_config_entry = map_kill_entry->add_kills();
								int id_temp = atoi(outVC2[0].c_str());
								int type_temp = atoi(outVC2[1].c_str());
								obj_config_entry->set_id(id_temp);
								obj_config_entry->set_type((message::SubType)type_temp);
							}							
						}
					}
				}
			}
			
			std::string lotions_str = row["lotions"].c_str();
			outVC.clear();
			SplitStringA(lotions_str, ",", outVC);
			length_vc = outVC.size();
			for (size_t i = 0; i < length_vc; i++)
			{
				data->add_lotions(atoi(outVC[i].c_str()));
			}


			std::string special_creatures = row["special_creatures"].c_str();
			outVC.clear();
			SplitStringA(special_creatures, ";", outVC);
			length_vc = outVC.size();
			for (int i = 0; i < length_vc; i++)
			{
				strTemp = outVC[i];
				outVC1.clear();
				SplitStringA(strTemp, ",", outVC1);
				if (outVC1.size() >= 2)
				{
					int creatures_id = atoi(outVC1[0].c_str());
					int creature_status = atoi(outVC1[1].c_str());
					message::MsgIntPair* entry = pkParm->info.add_special_creatures();
					entry->set_number_1(creatures_id);
					entry->set_number_2(creature_status);		
				}
			}
		
			pkParm->info.set_account(acc);
			need_create = false;
			char sz_sql[256];
			sprintf(sz_sql, "select * from `deal_wait_to_pay` where `account_id`=%llu and complete_status!=2", acc);
			gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQueryHeroDeals, sz_sql, 0, new tgHeroData(pkParm), _QUERY_HERO_INFO_);

		}
	}

	if (need_create)
	{
		message::MsgNeedCreateHeroDB2GS msg;
		gDBGameManager.sendMessage(&msg, pkParm->tranid, pkParm->gsid);
	}
}
