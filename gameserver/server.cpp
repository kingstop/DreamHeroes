#include "stdafx.h"

GameServer::GameServer()
    :m_EventHold(WORLD_INSTANCE),
	_http_thread(NULL),
	_time_pass(0),
	_last_pass(0)
{
}

GameServer::~GameServer()
{
	if (_http_thread != NULL)
	{
		delete _http_thread;
		_http_thread = NULL;
	}
}
bool GameServer::init()
{
    ServerFrame::init();
    DataBaseConfig dbconfig;

    if (!ServerFrame::loadDBConfig(dbconfig, CENTER_DATABASE_CONFIG))
    {
		//printf("game db server init  failed \n" );
        Mylog::log_server( LOG_ERROR, "load database config failed !");
        return false;
    }

	gEventMgr.addEvent(this, &GameServer::tenMinuteCollect, EVENT_PER_TEN_MIN_COLLECT_, 60 * 1000 * 10, -1, 0);

	gEventMgr.addEvent(this, &GameServer::minuteCollect, EVENT_PER_MIN_COLLECT_, 60 * 1000, -1, 0);
    if (!Database::addBlockTask(dbconfig, this, &GameServer::initDataFromDatabase, NULL))
    {
		//printf("init from Database server init  failed \n" );
        Mylog::log_server( LOG_ERROR, "Init Server from DataBase Error!");
        return false;
    }

	if (gCenterDataBase.initDatabase(dbconfig) == false)
	{
		Mylog::log_server( LOG_ERROR, "init center db server init  failed \n" );
		return false;
	}

    Mylog::log_server( LOG_INFO, "Start Game Server v:[%u] Success!", gGSVersion.toString().c_str());

	gDreamHeroManager.init();
	gShopSalesPromotionManager.init();
	_http_thread = new boost::thread(HttpProcess);
    return true;
}



bool GameServer::onKey()
{

    return true;

}



void GameServer::runOnce(u32 nDiff)
{
    net_global::update_net_service();
	gGSServerTime = time(NULL);
    gGSGateServer.run_no_wait();
    gGSDBClient.run_no_wait();
	gPlatformClient.run_no_wait();
    m_EventHold.update(nDiff);
    static u32 curtime = 0;
    curtime += nDiff;
    if (curtime > 15000)
    {
        curtime = 0;
    }
	gShopSalesPromotionManager.update();
	gHttpManager.logicUpdate();
	gRecordManager.update();
    if (nDiff > 800)
    {
        Mylog::log_server(LOG_WARNING, "server delay [%u]", nDiff);
    }
	_time_pass += nDiff;
}

void GameServer::tenMinuteCollect()
{
	std::string time_unix;
	build_unix_time_to_string(g_server_time, time_unix);
	u64 time_spawn = _time_pass - _last_ten_pass;
	Mylog::log_server(LOG_INFO, " Ten time[%s] time pass[%llu] time spawn[%llu]", time_unix.c_str(), _time_pass, time_spawn);
	_last_ten_pass = _time_pass;
}

void GameServer::minuteCollect()
{
	std::string time_unix;
	build_unix_time_to_string(g_server_time, time_unix);
	u64 time_spawn = _time_pass - _last_pass;
	Mylog::log_server(LOG_INFO, "Time[%s] time pass[%llu] time spawn[%llu]", time_unix.c_str(), _time_pass, time_spawn);
	_last_pass = _time_pass;
}

void GameServer::shutDown()
{
    gGSDBClient.close();
    gGSGateServer.stop();
	gPlatformClient.close();
	gHttpManager.setStop(true);
	_http_thread->join();
}



void GameServer::memoryLog() 
{

}

void GameServer::setStop()
{
	gDreamHeroManager.SaveDreamHeroes();
	Mylog::log_server(LOG_INFO, "prepare to stop game server!");
	
}

void GameServer::checkStop()
{
	if (gDreamHeroManager.is_save_all_heroes_ok())
	{
		gDreamHeroManager.SaveDreamHeroes();
		Mylog::log_server(LOG_INFO, "game server is stopping!");
		shutDown();
		_stop = true;
	}
}

bool GameServer::initDataFromDatabase(DBQuery* p, const void* data)
{
	if (!p)
	{ 
		return false;
	}

 	net_info user_net;
 	DBQuery& query = *p;
 	DBQParms parms;
 	parms << _GAME_LISTEN_GATE_;
	parms << m_service_conf.id;
 	query << "SELECT * FROM `net_config` WHERE `Category` = %0 AND `ServerId` = %1";
 	query.parse();

 	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
 	{ 
 		Mylog::log_server( LOG_ERROR, "Init game server net config failed !");
 		return false;
 	}

 	if(!gGSGateServer.create( user_net._port, user_net._max_connect, user_net._thread_count))
 	{
 		Mylog::log_server( LOG_ERROR, "game Init NetConfig failed!");
 		return false;
 	}

 	Mylog::log_server( LOG_INFO, "ListenGate begin lister port[%d], MaxConnect[%d], ThreadCount[%d]!", user_net._port, user_net._max_connect, user_net._thread_count);
	parms.clear();
	query.reset();
	parms <<  m_service_conf.id;
 	parms.clear();
 	query.reset();
 	parms << _DB_LISNEN_GAME_;
 	query << "SELECT * FROM `net_config` WHERE `Category` = %0";
 	query.parse();
 	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
 	{ 
 		Mylog::log_server( LOG_ERROR, "Init gate server net config failed !");
 		return false;
 	}
 	gGSDBClient.connect(user_net._ip.c_str(), user_net._port);
 	Mylog::log_server( LOG_INFO, "connect db  port[%d], ip[%s]!", user_net._port, user_net._ip.c_str());
	parms.clear();
	query.reset();
	parms << _FU_WORLD_DATABASE_;
	query << "SELECT * FROM `database_config` WHERE `Category` = %0";
	query.parse();
	if (!ServerFrame::getDatabaseConfig(m_world_db_conf, query.store(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "load world db config failed !");
		return false;
	}
	if (gWorldDatabase.initDatabase(m_world_db_conf) == false)
	{
		Mylog::log_server( LOG_ERROR, "init world db failed !");
		return false;
	}

	if (!Database::addBlockTask(m_world_db_conf, this, &GameServer::initDataFromWorldDB, NULL))
	{
		Mylog::log_server( LOG_ERROR, "Init world server from database error!");
		return false;
	}

	parms.clear();
	query.reset();
	parms << _FU_CHARACTER_DATABASE_;
	query << "SELECT * FROM `database_config` WHERE `Category` = %0";
	query.parse();
	if (!ServerFrame::getDatabaseConfig(m_character_conf, query.store(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "load character db config failed !");
		return false;
	}

	if (!Database::addBlockTask(m_character_conf, this, &GameServer::initDataFromCharacterDB, NULL))
	{
		Mylog::log_server(LOG_ERROR, "Init character server from database error");
		return false;
	}

	if (!gDBCharDatabase.initDatabase(m_character_conf))
	{
		Mylog::log_server(LOG_ERROR, "start character db service failed!");
		return false;
	}    


	parms.clear();
	query.reset();
	
	query << "SELECT * , UNIX_TIMESTAMP(`server_open_time`) FROM `server_config`";
	query.parse();
	const SDBResult& r = query.store();
	if (r.num_rows() > 0)
	{
		DBRow row = r[0];
		std::string server_temp = row["server_id"].c_str();
		char s_temp = 'a';
		if (server_temp.empty() == false)
		{
			s_temp = server_temp[0];
		}
		gGameConfig.setServerID(s_temp);
		u64 open_time = row["UNIX_TIMESTAMP(`server_open_time`)"];
		std::string paltformHttpUrl = row["http_platform_url"].c_str();
		gGameConfig.setPlatformHttpUrl(paltformHttpUrl.c_str());
		gGameConfig.setServerOpenTime(open_time);

		std::string platformIp = row["platform_ip"].c_str();
		int platform_port = row["platform_port"];
		int need_connect_platform = row["need_connect_platform"];
		gGameConfig.setNeedConnectPlatform((bool)need_connect_platform);
		gGameConfig.setPlatformServerIp(platformIp.c_str());
		gGameConfig.setPlatformServerPort(platform_port);
		int game_id = row["game_id"];
		int server_type = row["server_type"];
		gGameConfig.setGameID(game_id);
		gGameConfig.setServerType(server_type);
		gGameConfig.generateTitle();

	}

	if (gGameConfig.isNeedConnectPlatform())
	{
		gPlatformClient.connect(gGameConfig.getPlatformServerIp(), gGameConfig.getPlatformServerPort());		
	}
    return true;
}

bool GameServer::initDataFromCharacterDB(DBQuery* p, const void* data)
{
	if (!p)
	{
		return false;
	}
	gDreamHeroManager.Load(p);
	gRankManager.Load(p);

	return true;
}



bool GameServer::initDataFromWorldDB(DBQuery* p, const void* data)
{
	if (!p)
	{ 
		return false;
	}
	gGameConfig.Load(p);
	return true;
}

