#ifndef __stdafx_h__
#define __stdafx_h__
#include <time.h>
#include "task_thread_pool.h"
#include "io_service_pool.h"
#include "tcpsession.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "net_type.h"
#include "crypt.h"
#include "memory.h"
#include "database.h"
#include "server_frame.h"
#include "protoc_common.h"
#include "version.h"
#include "event_table_object.h"
#include "event_manager.h"
#include "message/common.pb.h"
#include "message/login.pb.h"
#include "message/server_define.h"
#include "message/msgs2s.pb.h"
#include "message/msg_gate_game.pb.h"
#include "message/msg_game_db.pb.h"
#include "message/dream_heroes.pb.h"
#include "message/msg_game_platform.pb.h"
#include "server.h"
#include "gate_manager.h"
#include "gate_tcp_server.h"
#include "db_client.h"
#include "DreamHeroManager.h"
#include "ShopSalesPromotionManager.h"
#include "GameConfig.h"
#include "RecordManager.h"
#include "GSHttpManager.h"
#include "platform_client.h"
#include "RankManager.h"
/*#include "ObjectManager.h"
#include "FRWorld.h*"
*/

#include "database.h"


std::string Utf8ToGBK(const char* strUtf8);
bool isInToday(u32 time);
std::string GBKToUtf8(const char* strGBK);
enum EventTypes
{
	EVENT_UNK = 0,
	EVENT_SAVE_PLAYER_DATA_,
	EVENT_PALYER_PING_,
	EVENT_DESTROY_PLAYER_,
	EVENT_PER_HOUR,
	EVENT_SAVE_HEROES_STATU,
	EVENT_COLLECT_INFO_,
	EVENT_COLLECT_SESSION_INFO_,
	EVENT_SHOP_SALES_PROMOTION_APPLY,
	EVENT_SHOP_SALES_PROMOTION_EXIT,
	EVENT_RECOVER_SPIRIT_,
	EVENT_PER_MIN,
	EVENT_SAVE_DAILY_GAME_RANK_,
	EVENT_PER_MIN_COLLECT_,
	EVENT_PER_TEN_MIN_COLLECT_,
};
struct FuGameFather
{
	FuGameFather():sSystemTime(time(0))
	{
	}
	time_t          sSystemTime;
	Version         sVersion;
	GameServer      sGameServer;
	GateManager     sGateManager;
	GateTcpServer   sGateTcpServer;
	PlatformClient  sPlatformClient;
	GameDBClient    sGameConnDB;
	Database        sCharacterDB;
	Database		sCenterDB;
	Database		sWorldDB;
	DreamHeroManager sDreamHeroManager;
	GameConfig       sGameConfig;
	ShopSalesPromotionManager sShopSalesPromotionManager;
	RecordManager	sRecordManager;
	HttpManager		sHttpManager;
	RankManager		sRankManager;

};

extern FuGameFather* gFuGameFather;
#define gGSVersion			gFuGameFather->sVersion
#define gGSServerTime		gFuGameFather->sSystemTime
#define gGameServer			gFuGameFather->sGameServer
#define gGSGateManager		gFuGameFather->sGateManager
#define gGSDBClient			gFuGameFather->sGameConnDB
#define gGSGateServer		gFuGameFather->sGateTcpServer 
#define gDBCharDatabase		gFuGameFather->sCharacterDB
#define gCenterDataBase		gFuGameFather->sCenterDB
#define gWorldDatabase		gFuGameFather->sWorldDB
#define gDreamHeroManager   gFuGameFather->sDreamHeroManager
#define gGameConfig			gFuGameFather->sGameConfig
#define gShopSalesPromotionManager gFuGameFather->sShopSalesPromotionManager
#define gRecordManager      gFuGameFather->sRecordManager
#define gHttpManager		gFuGameFather->sHttpManager
#define gPlatformClient     gFuGameFather->sPlatformClient
#define gRankManager		gFuGameFather->sRankManager
enum
{
	_SAVE_GIRL_ = _NORMAL_THREAD + 1,
	_SAVE_GOODS_ 
};

#endif

