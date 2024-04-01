#pragma once
#include "dtype.h"

#define ACCEPT_BUF_SIZE		131072							//128*1024
#define SERVER_BUF_SIZE		1048576							//1024*1024e
#define MAX_MSG_LEN			64000

#define SERVER_INIT_NUM		10
#define PLAYER_INIT_MAX		100

#define SERVER_CON_SEC		1

#define INDEX_MOD_NUM 1000000									//用于区分标记加后缀移位
#define MAKE_MOD_ID(id, group) ((id) * INDEX_MOD_NUM + (group)) //用于分区标记加后缀移位
#define SPLIT_MOD_ID(id, group) (((id) - (group)) / INDEX_MOD_NUM)
#define SPLIT_MOD_GAME(id) ((id) % (INDEX_MOD_NUM / 10))
#define SPLIT_MOD_PLAT(id) ((((id) % INDEX_MOD_NUM) * 10) / INDEX_MOD_NUM)
#define SPLIT_MOD_ACCID(id) ((id) / INDEX_MOD_NUM)

#define DB_THREAD_NUM 10	//游戏服开启数据库处理线程数


#define CONST_CONFIG		"../Config/const_config.xml"		//全部本地配置(服务器、数据库、zk等配置信息)

#define MAX_PALYER_NAME_LEN			21	//最大角色名称长
#define MAX_CHANNEL_LEN				13	//最大渠道字长

#define GLOBAL_MAIL_LIST	"g_mail_list"							//全局邮件表,所有game读取

#define SAFE_DELETE(ptr) if(ptr) {delete (ptr); (ptr) = NULL;}
#define SAFE_FREE(ptr)	if(ptr) { free(ptr); (ptr) = NULL;}

#define MAX_CON_SERVER 50

#define MAX_UDP_BUFF 1200
#define NET_FRAME_TIME  50						//网络帧时间
#define MAX_HEART_ROOM 50						//最大房间数
#define WAIT_DELETE_MILLSEC 2000
#define MAX_WAIT_CREATEROOM 30000				//30秒

#define ROOM_FILTER_NUM 10

#ifndef WIN32
#define SP_THREAD_CALL
#else 
#define SP_THREAD_CALL __stdcall
#endif


enum SERVER_KIND
{
	SERVER_KIND_NONE = 0,
	SERVER_KIND_GATE = 1,					//网关服
	SERVER_KIND_CROSS = 2,					//跨服
	SERVER_KIND_GAME = 3,					//游戏服
	SERVER_KIND_SYS = 4,					//登录、系统服
	SERVER_KIND_LOG = 5,					//日志服
	SERVER_KIND_BATTLE = 6,					//战斗服
	SERVER_KIND_FBATTLE = 7,				//战斗中心服
	SERVER_KIND_DB = 8,						//数据服
};

#define DAY_SECONDS 86400
#define WEEK_SECONDS 604800
#define MONTH_SECONDS 2592000

#define MAX_SIZE_256M 268435456		//256*1024*1024
#define MAX_SIZE_512M 536870912		//512*1024*1024
#define MAX_SIZE_1M 1048575			//1024*1024
#define MAX_SIZE_256K 262144		//1024*256
#define MAX_SIZE_128K 131072		//128*1024
#define MAX_SIZE_64K 65536			//64*1024
#define MAX_SIZE_32K 32768			//32*1024