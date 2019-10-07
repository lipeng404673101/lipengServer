#ifndef _L_MSG_H_
#define _L_MSG_H_

#include "GameDefine.h"
#include "LSocket.h"
#include "LSingleton.h"
#include "LTool.h"
#include "LLog.h"
#include "LVideo.h"


struct LMsg;

class API_LBASESX LMsgFactory : public LSingleton<LMsgFactory>
{
public:
	virtual bool Init();

	virtual bool Final();

	void RegestMsg(LMsg* pMsg);

	LMsg* CreateMsg(int msgId);

private:
	std::map<int,LMsg*> m_msgMap;
};


struct API_LBASESX LMsg
{
	LMsg(int id);
	virtual~LMsg();

	LBuffPtr		GetSendBuff();

	void			WriteMsgId(LBuff& buff);
	void			ReadMsgId(LBuff& buff);

	virtual bool	Write(LBuff& buff);
	virtual bool	Write(msgpack::packer<msgpack::sbuffer>& pack);
	virtual bool	Read(LBuff& buff);
	virtual bool	Read(msgpack::object& obj);
	virtual LMsg*	Clone();
	virtual Lint	preAllocSendMemorySize();

	LBuffPtr			m_send;
	LSocketPtr			m_sp;
	Lint				m_msgId;
	bool				m_isMsgPack;
};

//客户端 服务器交互的消息
struct LMsgSC :public LMsg
{
	LMsgSC(int id) :LMsg(id)
	{
		m_isMsgPack = true;
	}
};
//玩家状态信息定义
enum LGUserMsgState
{
	LGU_STATE_CENTER,		//玩家在大厅，发到Center
	LGU_STATE_DESK,			//玩家在桌子，发到Logic
	LGU_STATE_COIN,			//玩家在金币场，发到CoinsServer
	LGU_STATE_COINDESK,		//玩家在金币场桌子，发到Logic
	LGU_STATE_CLUB,             //玩家在俱乐部
};
//服务器端跟客户端通讯的消息号
enum LMSG_ID
{
	MSG_ERROR_MSG = 0,//错误消息

	MSG_C_2_S_MSG = 1,//客户端发给服务器
	MSG_S_2_C_MSG = 2,//服务器返回结果

    MSG_C_2_S_LOGIN = 11, //客户端请求登录
    MSG_S_2_C_LOGIN = 12, //服务器返回客户登录结果

	MSG_S_2_C_ITEM_INFO = 13,  //返回玩家物品信息
	MSG_S_2_C_HORSE_INFO = 14,  //跑马灯信息

	MSG_C_2_S_HEART = 15, //心跳包
	MSG_S_2_C_HEART = 16, //心跳包

	MSG_C_2_S_NOTICE = 17, //请求服务器通知
	MSG_S_2_C_NOTICE = 18, //服务器返回通知

	//vip场相关消息
	MSG_C_2_S_CREATE_ROOM = 20,//玩家请求创建房间

	MSG_C_2_S_CREATE_ROOM_FOR_OTHER = 29,//玩家请求创建房间 房主建房

	MSG_S_2_C_CREATE_ROOM = 21,//玩家请求创建房间结果

	MSG_C_2_S_ADD_ROOM = 22,//玩家请求加入房间
	MSG_S_2_C_ADD_ROOM = 23,//玩家请求加入房间结果

	MSG_C_2_S_LEAVE_ROOM = 24,//玩家请求退出房间
	MSG_S_2_C_LEAVE_ROOM = 25,//玩家请求退出房间结果

	MSG_C_2_S_RESET_ROOM = 26,//申请解散房间
	MSG_S_2_C_RESET_ROOM = 27,// 服务器广播申请解散房间结果


	MSG_C_2_S_RESET_ROOM_SELECT = 28,//玩家执行申请操作



	MSG_S_2_C_INTO_DESK = 30,//服务器通知玩家进入桌子
	MSG_S_2_C_ADD_USER = 31,//桌子添加玩家
	MSG_S_2_C_DEL_USER = 32,//桌子删除玩家


	MSG_C_2_S_USER_SELECT_SEAT = 33,//桌子玩家选择座位
	MSG_S_2_C_USER_SELECT_SEAT = 34,//桌子玩家选择座位广播桌子玩家

	MSG_S_2_C_DESK_STATE = 35,//发送桌子状态

	MSG_C_2_S_READY = 36,//客户端玩家准备
	MSG_S_2_C_READY = 37,//服务器广播某个玩家准备

	MSG_C_2_S_START_GAME =38,      //客户端发送游戏开始指令

	MSG_S_2_C_USER_ONLINE_INFO = 40,//服务器广播玩家在线状态

	MSG_S_2_C_VIP_INFO = 41,//同步桌子信息

    MSG_C_2_S_COINS_ROOM_LIST = 42, //预留
    MSG_S_2_C_COINS_ROOM_LIST = 43, //预留
	MSG_C_2_S_ENTER_COINS_ROOM = 44, //玩家请求进入金币房间
	MSG_S_2_C_ENTER_COINS_ROOM = 45, //玩家请求创建房间结果

	MSG_S_2_C_HAOZI = 49,//耗子牌
	MSG_S_2_C_START = 50,//服务器发送游戏开始消息
	MSG_S_2_C_GET_CARD = 51,//服务器发送玩家摸牌，准备出牌的消息

	MSG_C_2_S_PLAY_CARD = 52,//客户端请求出牌
	MSG_S_2_C_PLAY_CARD = 53,//服务器广播玩家出牌

	MSG_S_2_C_USER_THINK = 54,//服务器通知玩家思考

	MSG_C_2_S_USER_OPER = 55,//玩家操作结果
	MSG_S_2_C_USER_OPER = 56,//服务器广播玩家操作结果

	MSG_C_2_S_USER_SPEAK = 57,//玩家请求说话
	MSG_S_2_C_USER_SPEAK = 58,//服务器广播玩家说话

	MSG_S_2_C_GAME_OVER = 60,//服务器通知本场游戏结束

	MSG_C_2_S_USER_OUT_ROOM = 61,//玩家请求退出房间
	MSG_S_2_C_USER_OUT_ROOM = 62,//玩家退出房间结果

	MSG_C_2_S_USER_AIOPER = 63,  //客户端请求自动决策
	MSG_S_2_C_USER_AIOPER = 64,  //服务器自动帮决策/出牌

	MSG_S_2_C_START_HU = 65,//玩家起手胡
	MSG_C_2_S_START_HU_SELECT = 66,//玩家选择起手胡
	MSG_S_2_C_START_HU_SELECT = 67,//玩家选择起手胡结果

	MSG_S_2_C_GANG_RESULT = 68,//玩家杠牌结果

	MSG_S_2_C_END_CARD_SELECT = 69,//服务器广播海底牌选择
	MSG_C_2_S_END_SELECT = 70,//玩家选择结果
	MSG_S_2_C_END_GET = 71,//服务器广播玩家摸海底牌
	MSG_S_2_C_REMOVE_QIANGGANG_CARD = 72,//移除抢杠胡的手牌

	MSG_S_2_C_FANPIGU_BUPAI = 73, //服务器发送翻屁股补牌消息
	MSG_C_2_S_FANPIGU_BUPAI_RESULT = 74, //玩家翻屁股补牌结果消息
	MSG_S_2_C_FANPIGU_BROADCAST = 75, //翻屁股新牌广播

	MSG_S_2_C_VIP_END = 80,//游戏结束

	MSG_C_2_S_VIP_LOG = 90,//玩家请求vip房间对账单
	MSG_S_2_C_VIP_LOG = 91,//玩家请求vip房间对账单结果

	MSG_C_2_S_VIDEO = 92,//玩家请求录像
	MSG_S_2_C_VIDEO = 93,//玩家请求录像


	MSG_C_2_S_SHAREVIDEO = 105,//玩家请求查看某shareid的分享录像
	MSG_S_2_C_SHAREVIDEO = 106,//服务器返回某shareid对应的分享录像
	MSG_C_2_S_REQSHAREVIDEO = 107,//玩家请求分享某录像
	MSG_S_2_C_RETSHAREVIDEOID = 108,//服务器返回分享的id
	MSG_C_2_S_GETSHAREVIDEO = 109,//玩家通过精确的videoid查看分享的某录像

	MSG_S_2_C_USER_CHANGE = 112,			//通知玩家换牌
	MSG_C_2_S_USER_CHANGE = 113,			//玩家回复换牌
	MSG_S_2_C_USER_CHANGERESULT = 114,		//玩家换牌结果
	MSG_S_2_C_GAMEREULT = 115,				//单局结算信息

	MSG_S_2_C_USER_DINGQUE_COMPLETE = 116,	//所有玩家定缺完毕

	//MSG_C_2_S_ROOM_LOG = 117,//玩家请求vip房间对账单
	//MSG_S_2_C_ROOM_LOG = 118,//玩家请求vip房间对账单结果

	MSG_C_2_S_ROOM_LOG = 110,//玩家请求vip房间对账单
	MSG_S_2_C_ROOM_LOG = 111,//玩家请求vip房间对账单结果

	//--------------------------------------------------------------
	MSG_C_2_S_GET_USER_INFO = 130,		// 请求获取指定玩家的信息
	MSG_S_2_C_USER_INFO = 131,			// 服务器返回玩家信息

	MSG_C_2_S_GET_INVITING_INFO = 132,	      // 请求获取邀请信息
	MSG_S_2_C_INVITING_INFO = 133,		      // 服务器返回邀请信息

	MSG_C_2_S_BIND_INVITER = 134,		      // 请求绑定邀请人
	MSG_S_2_C_BIND_INVITER = 135,		      // 服务器返回绑定邀请人结果
	MSG_C_2_S_USER_SHARED = 136,		// 玩家进行了分享
	MSG_C_2_S_REQ_LUCKY_DRAW_NUM = 137,	// 请求玩家抽奖次数
	MSG_S_2_C_LUCKY_DRAW_NUM = 138,		// 服务器推送玩家抽奖次数

	MSG_C_2_S_TANGCARD = 139,			// 玩家发起廊起请求消息
	MSG_S_2_C_TANGCARD_RET = 140,		// 服务器发回廊起请求验证结果

	MSG_C_2_S_REQ_CREHIS = 141,         //玩家请求点赞记录
	MSG_S_2_C_RET_CREHIS = 142,         //服务器返回客户端 点赞记录

	MSG_C_2_S_REQ_GTU = 143,           //客户端 点赞
	MSG_S_2_C_RET_GTU = 144,           //服务器返回点赞结果

	MSG_C_2_S_REQ_EXCH = 145,          //换金币
	MSG_S_2_C_RET_EXCH = 146,          //结果

	MSG_C_2_S_DESK_LIST = 147,         //玩家请求创建的房间
	MSG_S_2_C_DESK_LIST = 148,         //返回玩家请求创建的房间

	MSG_S_2_C_USER_PLAY_DATA = 149,    //返回玩家数据

	//MSG_S_2_C_VIP_LOG = 91,//玩家请求vip房间对账单结果

	//////////////////////////////////////////////////////////////////////////
	//client 跟 server 之间消息交互
	//////////////////////////////////////////////////////////////////////////
	MSG_C_2_S_LOGIN_CENTER = 150,//客户端请求登录中心服务器
	MSG_S_2_C_LOGIN_CENTER = 150,//客户端请求登录中心服务器

	MSG_C_2_S_LOGIN_SERVER = 160,//客户端请求登录服务器
	MSG_S_2_C_LOGIN_SERVER = 160,//客户端请求登录服务器

	//client and gate
	MSG_C_2_S_LOGIN_GATE = 170,	//客户端登录Gate
	MSG_S_2_C_LOGIN_GATE = 171,	//Gate回客户端登录消息


	MSG_C_2_S_CREATER_RESET_ROOM = 172,  //房主申请解散房间
	MSG_S_2_C_CREATER_RESET_ROOM = 173,  //服务器返回房主申请解散房间结果


	// 活动 200 - 300
	MSG_S_2_C_ACTIVITY_INFO = 200,			//通用的活动内容 推动给客户端
	MSG_C_2_S_ACTIVITY_WRITE_PHONE = 201,	//填写活动相关的电话号码
	MSG_S_2_C_ACTIVITY_WRITE_PHONE = 202,
	MSG_C_2_S_ACTIVITY_REQUEST_LOG = 203,	//请求中奖纪录
	MSG_S_2_C_ACTIVITY_REPLY_LOG = 204,		//返回中奖纪录

	// 转盘
	MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN = 211,	//请求打开转盘
	MSG_S_2_C_ACTIVITY_REPLY_DRAW_OPEN = 212,	//返回请求打开转盘
	MSG_C_2_S_ACTIVITY_REQUEST_DRAW = 213,	//客户端请求抽卡
	MSG_S_2_C_ACTIVITY_REPLY_DRAW = 214,	//通知客户端抽卡结果

	// 分享
	MSG_C_2_S_ACTIVITY_REQUEST_SHARE = 220,	//客户端请求分享 
	MSG_S_2_C_ACTIVITY_FINISH_SHARE = 221,	//通知客户端分享结果

<<<<<<< HEAD
	///////////////////////////////////////////////////////
	//扑克游戏命令400-490
	MSG_C_2_S_POKER_GAME_MESSAGE =401,      //扑克游戏命令
	MSG_S_2_C_POKER_GAME_MESSAGE=402,  //扑克游戏命令


	////////////////////////////////////////////////////

=======
	//关于俱乐部
	MSG_C_2_S_ENTER_CLUB  = 301,      // 进入俱乐部
	MSG_S_2_C_CLUB_SCENE = 302,      //俱乐部界面内容

	MSG_C_2_S_LEAVE_CLUB = 303,     //离开俱乐部   
	MSG_S_2_C_LEAVE_CLUB = 304,      //离开俱乐部

	MSG_C_2_S_SWITCH_PLAY_SCENE = 305,   //俱乐部界面切换玩法
	MSG_S_2_C_SWITCH_PLAY_SCENE=306,     //返回俱乐部界面信息

	MSG_C_2_S_REQUEST_PERSON_INFO = 307,  //获取个人信息
	MSG_S_2_C_REPLY_PERSON_INFO = 308,   //获取个人信息

	MSG_C_2_S_FRESH_CLUB_DESK_INFO=309,   //刷新俱乐部桌子信息
>>>>>>> MessageProcess

	//视频命令
	//SKYEYE_IS_VIP_ROOM			    =	905, //查看是否为vip防作弊房间
	MSG_C_2_S_SEND_VIDEO_INVITATION	        =	900, //发起视频邀请

	MSG_C_2_S_RECEIVE_VIDEO_INVITATION	    =	901, //收到视频邀请
	MSG_C_2_S_INBUSY_VIDEO_INVITATION	    =	903, //视频邀请忙线中
	MSG_C_2_S_ONLINE_VIDEO_INVITATION		=	904, //视频已连线
	MSG_C_2_S_SHUTDOWN_VIDEO_INVITATION	    =	906, //关闭视频
	MSG_C_2_S_UPLOAD_GPS_INFORMATION		=	907, //上传GPS坐标
	MSG_C_2_S_UPLOAD_VIDEO_PERMISSION		=	908, //上传视频是否允许
	MSG_S_2_C_UPDATE_USER_VIP_INFO		    =   909, //用户的视频是否开启，gps



	MSG_S_2_C_BCAST_MESSAGE = 888,	//广播客户端消息
	//客户端包头验证头8
	MSG_C_2_S_VERIFY_HEAD = 500,

	MSG_S_2_C_MAX = 4096, //服务器客户端之间最大消息号

	//////////////////////////////////////////////////////////////////////////
	//下面这是服务器与服务器之间的消息交互
	//////////////////////////////////////////////////////////////////////////
	//logic 跟 center 之间的交互

	MSG_L_2_CE_LOGIN = 6000, //logic登录center

	MSG_L_2_CE_GATE_INFO = 6002, //logic同步gate信息到center

	MSG_CE_2_L_USER_ID_INFO = 6003, //center同步玩家id信息

	MSG_CE_2_L_USER_LOGIN = 6004, //center同步玩家请求登录的消息

	MSG_L_2_CE_MODIFY_USER_NEW = 6005, //logic同步玩家更新的消息

	MSG_L_2_CE_ROOM_SAVE = 6006, //logic同步更新room的消息s

	MSG_L_2_CE_USER_LOGIN = 6011,			//同步Center玩家当前登陆某服务器

	MSG_L_2_CE_USER_LOGOUT = 6012,			//同步Center玩家当前登出某服务器

	MSG_CE_2_L_REQ_LOG_INFO = 6013, //center发送玩家请求战绩信息			---已废

	MSG_L_2_CE_REQ_VIDEO = 6014, //logic发送玩家请求录像					---已废

	MSG_CE_2_L_REQ_VIDEO = 6015, //center发送玩家请求录像					---已废

	MSG_CE_2_L_GM_CHARGE = 6020, //center发送gm充值请求

	MSG_CE_2_L_GM_HORSE = 6021, //center发送gm跑马灯

	MSG_CE_2_L_GM_BUYINFO = 6022, //center发送gm够买提示

	MSG_CE_2_L_GM_HIDE = 6023, //center发送gmyincang

	MSG_CE_2_L_GM_COINS = 6024, //center发送gm充值金币

	MSG_LMG_2_CE_SAVE_CARD = 6400, //logicmanager发给center来保存房卡		---暂时未增加该功能

	MSG_CE_2_L_SET_GAME_FREE = 6401,	//设置游戏免费

	MSG_CE_2_L_SET_DRAWACTIVE = 6402,	//设置抽奖活动配置 已作废

	MSG_CE_2_L_SET_PXACTIVE = 6403,		//设置牌型活动配置

	MSG_CE_2_L_SET_OUGCACTIVE = 6404,	//设置老玩家送卡活动配置

	MSG_CE_2_L_SET_EXCHACTIVE = 6405,	//设置老玩家送卡活动配置

	MSG_CE_2_L_SET_ACTIVITY = 6406,		//设置活动

	MSG_LMG_2_CE_UPDATE_COIN = 6407,     //发给center来从数据库更新金币	

	MSG_CE_2_LMG_UPDATE_COIN = 6408,     //center发给logicmanager来从数据库更新金币	

	MSG_LMG_2_CE_UPDATE_COIN_JOIN_DESK = 6409,     //发给center来从数据库更新金币	

	MSG_CE_2_LMG_UPDATE_COIN_JOIN_DESK = 6410,     //center发给logicmanager来从数据库更新金币	

	MSG_CE_2_LMG_CONFIG = 6411,	  //设置配置数据

	MSG_LMG_2_CE_UPDATE_USER_GPS = 6412, //更新玩家GPS信息到数据库

	MH_MSG_CE_2_L_UPDATE_GATE_IP=6413, // Center发送过来的更改gate server的对外IP

	MSG_CE_2_LMG_ADD_CLUB = 6414,	  //添加俱乐部

	MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE = 6415,	  //添加俱乐部玩法数据

	MSG_CE_2_LMG_CLUB_ADD_USER = 6416,	  //添加俱乐部玩家

	MSG_CE_2_LMG_CLUB_DEL_USER = 6417,	  //删除俱乐部玩家

	MSG_CE_2_LMG_CLUB_HIDE_PLAYTYPE = 6418,	  //隐藏俱乐部玩法

	MSG_CE_2_LMG_CLUB_MIDIFY_CLUB_NAME = 6419,	  //修改俱乐部名字

	MSG_CE_2_LMG_CLUB=6420,	  //更新俱乐部数据

	

	//////////////////////////////////////////////////////////////////////////
	//gete 跟 logic 之间的交互
	MSG_G_2_L_LOGIN = 7000, //gate登录logic		由LogicManager转发

	MSG_G_2_L_USER_MSG = 8000, //gate转发玩家消息到logic
	MSG_L_2_G_USER_MSG = 8001, //logic转发玩家消息到gate

	MSG_G_2_L_USER_OUT_MSG = 8002, //gate转发玩家退出消息到logic
	MSG_L_2_G_USER_OUT_MSG = 8003, //logic转发玩家退出消息到gate

	MSG_L_2_G_MODIFY_USER_STATE = 8006,	//修改玩家状态 gate to logic 



//////////////////////////////////////////////////////////////////////////
//LogicManager 与 Logic
	MSG_L_2_LMG_LOGIN = 11001,				//逻辑管理登陆

	MSG_LMG_2_L_CREATE_DESK = 11011,		    //创建桌子	只有玩家不在Logic上才创建
	MSG_LMG_2_L_CREATE_DESK_FOR_OTHER = 11012,	//创建桌子	只有玩家不在Logic上才创建
	MSG_LMG_2_L_ADDTO_DESK = 11013,			    //加入桌子	只有玩家不在Logic上才加入
	MSG_LMG_2_L_CREATE_DESK_FOR_CLUB=11014,	//创建俱乐部桌子, 这个是批量建桌子，暂时不用了
	MSG_LMG_2_L_USER_LOGIN = 11016,			    //玩家重新登陆

	MSG_L_2_LMG_MODIFY_USER_STATE = 11017,			//修改玩家状态  logicmanager to logic
	MSG_L_2_LMG_MODIFY_USER_CARD = 11018,			//修改玩家卡数
	MSG_L_2_LMG_ADD_USER_PLAYCOUNT = 11019,			//修改玩家把数
	MSG_L_2_LMG_RECYLE_DESKID = 11020,				//创建失败，或桌子解散时

	MSG_LMG_2_L_PAIXING_ACTIVE = 11021,				//LMtoL牌型活动信息

	MSG_LMG_2_L_EXCH_CARD = 11022,

	MSG_LMG_2_L_CREATED_DESK_LIST = 11023,			//LMtoL创建的桌子

	MSG_L_2_LMG_MODIFY_USER_CREATED_DESK = 11024,	//修改玩家创建的桌子
	MSG_L_2_LMG_DELETE_USER_CREATED_DESK = 11025,	//删除玩家创建的桌子

	MSG_LMG_2_L_DISSMISS_CREATED_DESK = 11026,			//LMtoL 删除房主创建的桌子

    MSG_L_2_LMG_ADD_USER_CIRCLE_COUNT = 11027,			//修改玩家局数 和赢的局数


	MSG_L_2_LMG_UPDATE_USER_GPS =  11028,//更新玩家GPS信息到数据库


	//俱乐部
	MSG_LMG_2_L_CREATE_CLUB_DESK_AND_ENTER=11029, //创建俱乐部桌子，并加入
	MSG_LMG_2_L_ENTER_CLUB_DESK=11030,                            //加入俱乐部桌子
	MSG_L_2_LMG_USER_ADD_CLUB_DESK=11031,                    //用户加入俱乐部桌子成功
	MSG_L_2_LMG_USER_LEAVE_CLUB_DESK=11032,           //用户离开俱乐部桌子成功
	MSG_L_2_LMG_FRESH_DESK_INFO =11033,                     //发送manager刷新桌子信息,主要局数
 

	MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT = 11040, // 新代理满场活动更新总场次

//LogicManager 与 Gate
	MSG_G_2_LMG_LOGIN = 12001,			//逻辑管理登陆
	MSG_LMG_2_G_SYNC_LOGIC = 12010,		//同步逻辑信息
	MSG_LMG_2_G_HORSE_INFO = 12011,		//跑马灯



    //LogicManager 与 Gate 与 Logic
    MSG_LMG_2_GL_COINS_SERVER_INFO = 13001,     //金币服务器的信息


   //////////////////////////////////////////////////////////////////////////
   //logicmanager 和 logicdb 交互消息
   MSG_LMG_2_LDB_USER_LOGIN = 14901,	//用户登陆
   MSG_LDB_2_LMG_USER_MSG = 14902,	//发送给用户的消息

   MSG_LMG_2_LDB_LOGIN = 14999,	//LogicManager 登入LogicDB


   //logic 和 logicdb 直接的消息交互
   MSG_L_2_LDB_LOGIN = 15000, //logic 登录 logicdb

   MSG_L_2_LDB_VIP_LOG = 15001,//logic 请求 vip战绩
   MSG_LDB_2_L_VIP_LOG = 15002,//logicdb 返回 vip战绩

   MSG_L_2_LDB_VIDEO = 15003,//logic 请求 video
   MSG_LDB_2_L_VIDEO = 15004,//logicdb 返回 video

   MSG_L_2_LDB_ROOM_SAVE = 15005, //logic同步更新room的消息
   MSG_L_2_LDB_VIDEO_SAVE = 15006, //logic同步更新video的消息

   MSG_L_2_LDB_REQ_ACTIVE = 15007,		//Logic请求活动  已作废
   MSG_LDB_2_L_REQ_ACTIVE = 15008,		//Center返回请求活动  已作废
   MSG_L_2_LDB_SYC_DRAW = 15009,			//Center返回请求活动  已作废
   MSG_L_2_LDB_SYC_PHONE = 15010,		//Center返回请求活动  已作废

   MSG_LDB_2_L_LOG_INFO = 15011, //center同步未完成的桌子信息

  //MSG_L_2_LDB_ROOM_LOG = 15014,//logic 请求room vip战绩
  MSG_LDB_2_L_ROOM_LOG = 15015,//logicdb 返回room vip战绩

  MSG_LM_2_LDB_BIND_INVITER = 15016,		      // 请求绑定邀请人
  MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS = 15017,

  MSG_LM_2_LDB_REQ_INFO = 15020,         //
  MSG_LDB_2_LM_RET_INFO = 15021,

  MSG_L_2_LDB_CRE_LOG = 15022,//logic 请求 vip战绩
  //MSG_LDB_2_L_VIP_LOG = 15002,//logicdb 返回 vip战绩

   MSG_L_2_LDB_ENDCRE_LOG = 15024,      //logicdb保存 点赞日志
   MSG_LDB_2_LM_RET_CRELOGHIS = 15025,  //logicdb返回点赞记录

   MSG_L_2_LDB_REQ_DELGTU = 15026,      //logicdb删除点赞记录

   MSG_L_2_LDB_REQ_PLAYER_SCORE = 15027,      //logicdb获取成绩，总局数和赢得局数 

   MSG_L_2_LDB_UPDATE_PLAYER_SCORE = 15028,   //logicdb更新成绩，总局数和赢得局数 

   MSG_LDB_2_LM_BCAST = 15029,             //群发消息发到logicManage	
//////////////////////////////////////////////////////////////////////////

	//LogicManager 与 CoinsServer
	MSG_CN_2_LMG_LOGIN = 16001,			//金币服务登陆Manager
	MSG_LMG_2_CN_LOGIN = 16002,

	MSG_CN_2_LMG_FREE_DESK_REQ = 16003,		//请求桌子号
	MSG_LMG_2_CN_FREE_DESK_REPLY = 16004,	//请求桌子回复
	MSG_CN_2_LMG_RECYCLE_DESK = 16005,		// 释放一些桌子

	MSG_LMG_2_CN_USER_LOGIN = 16007,	//玩家在桌内时的登陆

	MSG_LMG_2_CN_ENTER_COIN_DESK = 16011,	//
	MSG_CN_2_LMG_MODIFY_USER_COINS = 16012,	//修改玩家金币

	MSG_LMG_2_CN_GET_COINS = 16013,		 //玩家领取金币 Manager转发的消息


	// CoinsServer 与 Gate
	MSG_G_2_CN_LOGIN = 16101,			//逻辑管理金币服务器

	// CoinsServer 与 Logic
	MSG_L_2_CN_LOGIN = 16201,			//逻辑管理金币服务器

	MSG_CN_2_L_CREATE_COIN_DESK = 16211,  //创建金币桌子
	MSG_L_2_CN_CREATE_COIN_DESK_RET = 16212,  //创建桌子返回
	MSG_L_2_CN_END_COIN_DESK = 16213,     //结算

	// CoinServer 与 Client
	MSG_C_2_S_GOON_COINS_ROOM = 16401,   //玩家金币场继续游戏
	MSG_S_2_C_GOON_COINS_ROOM = 16402,   //玩家金币场继续游戏 回复
	MSG_S_2_C_KICK_COINS_ROOM = 16403,	 //玩家被踢出房间
	MSG_S_2_C_GIVE_COINS = 16404,		 //给玩家赠送金币 服务器主动
	MSG_C_2_S_GET_COINS = 16405,		 //玩家领取金币
	MSG_S_2_C_GET_COINS = 16406,		 //返回玩家领取金币
	MSG_S_2_C_OTHER_COINS = 16407,		 //返回玩家领取金币

	//LM 2 CENTER
	MSG_LM_2_CEN_ADD_CRE = 16800,    //点赞
	MSG_C_2_C_ADD_CRE = 16801,       //点赞

	/////////////////////////////////////////////////////////////////////////

	//LoginServer传递消息命令
	MSG_FROM_LOGINSERVER = 20000,	//来自login server 的消息
	MSG_TO_LOGINSERVER = 20001,	//回给login server 的消息	

	//公用消息
	MSG_CONVERT_CLIENTMSG = 50000,	//客户端数据包转换

	MSG_HEARDBEAT_REQUEST = 50100,	//心跳发送
	MSG_HEARDBEAT_REPLY = 50101,	//心跳回包	

	MSG_NEWUSER_VERIFY_REQUEST = 50200,	//新用户校验请求
	MSG_NEWUSER_VERIFY_REPLY = 50201,	//新用户校验返回


	MSG_L_2_LDB_GETSHAREVIDEOID = 17020,//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享
	MSG_L_2_LDB_GETSHAREVIDEO = 17021,//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询
	MSG_LDB_2_L_RETSHAREVIDEOID = 17022,//logicdb返回某录像是否有shareid，没有则将其从video中移至sharevideo中
	MSG_LDB_2_L_RETSHAREVIDEO = 17023,//logicdb 返回 通过shareid查询到的可能的录像
	MSG_L_2_LDB_SAVESHAREVIDEOID = 17024,//logic 请求保存某已转移到sharevideo中的某录像的shareid
	MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID = 17025, //loigc 通过精确的videoid来查找sharevideo中的对应录像

	MSG_L_2_LDB_GETUSERPLAYDATA = 17026,  //loigc 通过Userid来查找玩家统计数据
	MSG_LDB_2_L_GETUSERPLAYDATA = 17027,  //logicdb 返回 通过Userid来查找玩家统计数据

	MH_MSG_L_2_LDB_QUERY_SHARE_VIDEO_BY_SHARE_ID = 17028,
	MH_MSG_LDB_2_L_SHARE_VIDEO_REULST = 17029 //logicdb 返回共享录像信息



};

//////////////////////////////////////////////////////////////////////////
//socket断开的消息
#define MSG_CLIENT_KICK 0x7FFFFFFF

struct LMsgKick:public LMsg
{
	LMsgKick():LMsg(MSG_CLIENT_KICK){}

	virtual LMsg* Clone()
	{
		return new LMsgKick();
	}
};

//////////////////////////////////////////////////////////////////////////
//所有socket连上的消息
#define MSG_CLIENT_IN 0x7FFFFFFE

struct LMsgIn:public LMsg
{
	LMsgIn():LMsg(MSG_CLIENT_IN){}

	virtual LMsg* Clone()
	{
		return new LMsgIn();
	}
};

//////////////////////////////////////////////////////////////////////////
//http消息号
#define MSG_HTTP 0x7FFFFFFD

struct LMsgHttp:public LMsg
{
	std::string m_head;//消息头
	std::string m_body;//消息体

	LMsgHttp():LMsg(MSG_HTTP){}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_head);
		buff.Write(m_body);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_head);
		buff.Read(m_body);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHttp();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端实体消息验证头
struct LMsgC2SVerifyHead : public LMsgSC
{
	Lstring m_strUserId;	//用户id
	int		m_iMd5Begin;	//消息实体计算MD5的起始位置
	int		m_iMd5End;		//消息实体计算MD5的结束位置
	Lstring m_strMd5;		//消息实体MD5值
	Lstring m_strVerifyKey;		//服务器给的一串验证码
	int		m_lMsgOrder;	//消息实体序列。每个消息包都需要增值
	LMsgC2SVerifyHead() : LMsgSC(MSG_C_2_S_VERIFY_HEAD)
	{
		m_iMd5Begin = -1;
		m_iMd5End   = -1;
		m_lMsgOrder = 0xFFFFFFFF;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_strUserId", m_strUserId);
		ReadMapData(obj, "m_iMd5Begin", m_iMd5Begin);
		ReadMapData(obj, "m_iMd5End", m_iMd5End);
		ReadMapData(obj, "m_strMd5", m_strMd5);
		ReadMapData(obj, "m_strVerifyKey", m_strVerifyKey);
		ReadMapData(obj, "m_lMsgOrder", m_lMsgOrder);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserId", m_strUserId);
		WriteKeyValue(pack, "m_iMd5Begin", m_iMd5Begin);
		WriteKeyValue(pack, "m_iMd5End", m_iMd5End);
		WriteKeyValue(pack, "m_strMd5", m_strMd5);
		WriteKeyValue(pack, "m_strVerifyKey", m_strVerifyKey);
		WriteKeyValue(pack, "m_lMsgOrder", m_lMsgOrder);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SVerifyHead();
	}
};

//客户端发送消息
struct LMsgC2SMsg :public LMsgSC
{
	Lstring			m_openId;
	Lstring			m_nike;
	Lstring			m_sign;
	Lstring			m_plate;
	Lstring			m_accessToken;
	Lstring			m_refreshToken;
	Lstring			m_md5;
	Lint			m_severID;
	Lstring			m_uuid;
	Lint			m_sex;
	Lstring			m_imageUrl;
	Lstring			m_nikename;

	LMsgC2SMsg() :LMsgSC(MSG_C_2_S_MSG)
	{
		m_severID = 0;
		m_sex = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_openId", m_openId);
		ReadMapData(obj, "m_nike", m_nike);
		ReadMapData(obj, "m_sign", m_sign);
		ReadMapData(obj, "m_plate", m_plate);
		ReadMapData(obj, "m_accessToken", m_accessToken);
		ReadMapData(obj, "m_refreshToken", m_refreshToken);
		ReadMapData(obj, "m_md5", m_md5);
		ReadMapData(obj, "m_severID", m_severID);
		ReadMapData(obj, "m_uuid", m_uuid);
		ReadMapData(obj, "m_sex", m_sex);
		ReadMapData(obj, "m_imageUrl", m_imageUrl);
		ReadMapData(obj, "m_nikename", m_nikename);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,13);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_openId", m_openId);
		WriteKeyValue(pack, "m_nike", m_nike);
		WriteKeyValue(pack, "m_sign", m_sign);
		WriteKeyValue(pack, "m_plate", m_plate);
		WriteKeyValue(pack, "m_accessToken", m_accessToken);
		WriteKeyValue(pack, "m_refreshToken", m_refreshToken);
		WriteKeyValue(pack, "m_md5", m_md5);
		WriteKeyValue(pack, "m_severID", m_severID);
		WriteKeyValue(pack, "m_uuid", m_uuid);
		WriteKeyValue(pack, "m_sex", m_sex);
		WriteKeyValue(pack, "m_imageUrl", m_imageUrl);
		WriteKeyValue(pack, "m_nikename", m_nikename);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgS2CMsg :public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-服务器还没启动成功 2-微信登陆失败 3- 微信返回失败 4-创建角色失败 5- 在原APP未退,6-错误的地区号,7-MD5验证失败,9-账号被封
	Lint			m_seed;
	Lint			m_id;
	Lstring			m_gateIp;
	Lshort			m_gatePort;
	Lstring			m_errorMsg;	//若登陆失败，返回玩家错误原因;
	Lint            m_totalPlayNum;
	Lint            m_TotalCircleNum;
	Lint            m_totalWinNum;

	LMsgS2CMsg() :LMsgSC(MSG_S_2_C_MSG)
	{
		 m_errorCode = -1;
		 m_seed = 0;
		 m_id = 0;
		 m_gatePort = 0;
		 m_totalPlayNum = 0;
		 m_TotalCircleNum = 0;
		 m_totalWinNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_errorCode", m_errorCode);
		ReadMapData(obj, "m_seed", m_seed);
		ReadMapData(obj, "m_id", m_id);
		ReadMapData(obj, "m_gateIp", m_gateIp);
		ReadMapData(obj, "m_gatePort", m_gatePort);
		ReadMapData(obj, "m_errorMsg", m_errorMsg);
		ReadMapData(obj, "m_totalPlayNum",m_totalPlayNum);
		ReadMapData(obj, "m_TotalCircleNum", m_TotalCircleNum);
		ReadMapData(obj, "m_totalWinNum", m_totalWinNum);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 10);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_seed", m_seed);
		WriteKeyValue(pack, "m_id", m_id);
		WriteKeyValue(pack, "m_gateIp", m_gateIp);
		WriteKeyValue(pack, "m_gatePort", m_gatePort);
		WriteKeyValue(pack, "m_errorMsg", m_errorMsg);
	    WriteKeyValue(pack, "m_totalPlayNum",m_totalPlayNum);
		WriteKeyValue(pack, "m_TotalCircleNum", m_TotalCircleNum);
		WriteKeyValue(pack, "m_totalWinNum", m_totalWinNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CMsg();
	}
};

/////////////////////
//客户端发送登录Gate消息
struct LMsgC2SLoginGate : public LMsgSC
{
	Lstring m_strUserUUID;

	LMsgC2SLoginGate() : LMsgSC(MSG_C_2_S_LOGIN_GATE){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLoginGate();
	}
};

//Gate回客户端登录信息
struct LMsgS2CLoginGate : public LMsgSC
{
	Lstring m_strUserUUID;
	Lstring m_strKey;
	unsigned long m_uMsgOrder;

	LMsgS2CLoginGate() : LMsgSC(MSG_S_2_C_LOGIN_GATE)
	{
		m_uMsgOrder = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_strKey), m_strKey);
		ReadMapData(obj, NAME_TO_STR(m_uMsgOrder), m_uMsgOrder);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, "m_strKey", m_strKey);
		WriteKeyValue(pack, "m_uMsgOrder", m_uMsgOrder);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CLoginGate();
	}
};

/////////////////////
//客户端发送房主解散房间的消息
struct LMsgC2SCreaterResetRoom : public LMsgSC
{
	Lint m_userId;     
	Lint m_deskId;
	Lstring m_strUserUUID;

	LMsgC2SCreaterResetRoom() : LMsgSC(MSG_C_2_S_CREATER_RESET_ROOM) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_userId", m_userId);
		WriteKeyValue(pack, "m_deskId", m_deskId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SCreaterResetRoom();
	}
};

/////////////////////
//客户端发送房主解散房间的消息
struct LMsgS2CCreaterResetRoom : public LMsgSC
{
	Lint m_errorCode;
	Lstring m_strErrorDes;
	Lint m_deskId;

	LMsgS2CCreaterResetRoom() : LMsgSC(MSG_S_2_C_CREATER_RESET_ROOM) 
	{
		m_errorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_strErrorDes), m_strErrorDes);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_deskId", m_deskId);
		WriteKeyValue(pack, "m_strErrorDes", m_strErrorDes);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CCreaterResetRoom();
	}
};


/////////////////////
//客户端发送获取创建的房间的消息
struct LMsgC2SDeskList : public LMsgSC
{
	Lstring m_strUserUUID;

	LMsgC2SDeskList() : LMsgSC(MSG_C_2_S_DESK_LIST) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SDeskList();
	}
};


//返回客户端已经创建的房间的消息
struct LMsgS2CDeskList : public LMsgSC
{
	Lint		                    m_errorCode;//0-成功，1-没有创建的桌子，
	Lint                            m_creatTime;
	Lint                            m_deskId;
	Lint                            m_cheatAgainst; //防作弊
	Lint                            m_feeType; //费用类型 0：房主付费 1：分摊房费
	Lint                            m_flag;  //局数类型
	Lint                            m_state;   //大玩法类型
	Lint                            m_share;    //0-不能分享， 1-能被分享
	Lint                            m_dissmiss; //0-不能解散， 1-能解散
	Lint                            m_cellscore; //底分
	std::vector<Lint>	            m_playtype;
	std::vector<std::string>		m_deskPlayerName;//桌子id
	Lint                           m_Greater2CanStart;        //>=2人开局

	LMsgS2CDeskList() :LMsgSC(MSG_S_2_C_DESK_LIST)
	{
		m_errorCode = 0;
		m_creatTime = 0;
		m_deskId = 0;
		m_share = 1;
		m_dissmiss = 1;
		m_cheatAgainst = 0;
		m_feeType = 0;
		m_flag = 0;
		m_state = 0;
		m_cellscore = 0;
		m_Greater2CanStart=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_creatTime), m_creatTime);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_share), m_share);
		ReadMapData(obj, NAME_TO_STR(m_dissmiss), m_dissmiss);

		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);

		ReadMapData(obj, NAME_TO_STR(m_playtype), m_playtype);
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerName), m_deskPlayerName);
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 14);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_creatTime), m_creatTime);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_share), m_share);
		WriteKeyValue(pack, NAME_TO_STR(m_dissmiss), m_dissmiss);

		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);

		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerName), m_deskPlayerName);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CDeskList(); }
};


//返回客户端已经创建的房间的消息
struct LMsgS2CUserPlayData : public LMsgSC
{
	Lint		                    m_errorCode;//0-成功，1-没有创建的桌子，
	Lint                            m_totalAround;
	Lint                            m_winAround;


	LMsgS2CUserPlayData() :LMsgSC(MSG_S_2_C_USER_PLAY_DATA)
	{
		m_errorCode = 0;
		m_totalAround = 0;
		m_winAround = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_totalAround), m_totalAround);
		ReadMapData(obj, NAME_TO_STR(m_winAround), m_winAround);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_totalAround), m_totalAround);
		WriteKeyValue(pack, NAME_TO_STR(m_winAround), m_winAround);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUserPlayData(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端发送心跳包
struct LMsgC2SHeart:public LMsgSC
{
	//Lint	m_time;//时间戳

	LMsgC2SHeart() :LMsgSC(MSG_C_2_S_HEART){}

	virtual bool Read(msgpack::object& obj)
	{
		//buff.Read(m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SHeart();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送心跳包
struct LMsgS2CHeart :public LMsgSC
{
	Lint	m_time;//时间戳

	LMsgS2CHeart() :LMsgSC(MSG_S_2_C_HEART)
	{
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHeart(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求通知
struct LMsgC2SNotice :public LMsgSC
{
	
	LMsgC2SNotice() :LMsgSC(MSG_C_2_S_NOTICE){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SNotice(); }
};


//////////////////////////////////////////////////////////////////////////
//服务器返回通知内容
struct Notice
{
	Notice()
	{
		m_time = 0;
	}

	Lint	m_time;//时间戳
	Lstring	m_title;//标题
	Lstring m_content;//内容

	MSGPACK_DEFINE(m_time, m_title, m_content); 
};

struct LMsgS2CNotice :public LMsgSC
{
	Lint	m_count;//时间戳
	Notice	m_data[20];

	LMsgS2CNotice() :LMsgSC(MSG_S_2_C_NOTICE)
	{
		m_count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		m_count = 0;

		msgpack::object array;
		ReadMapData(obj, "array", array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object pv = *(array.via.array.ptr+i);
				pv.convert(m_data[i]);
				m_count++;
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKey(pack, "array");
		std::vector<Notice>vce;
		for (Lint i = 0; i < m_count&&i < 20; ++i)
		{
			vce.push_back(m_data[i]);
		}
		WriteKey(pack,vce);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CNotice(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求登录大厅

struct LMsgC2SLogin:public LMsgSC
{
	Lint	m_seed;
	Lint	m_id;
	Lstring	m_md5;

	LMsgC2SLogin() :LMsgSC(MSG_C_2_S_LOGIN)
	{
		m_seed = 0;
		m_id = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_seed), m_seed);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
struct clubOnlineUserSize
{
	Lint m_clubId;
	Lint m_onlineUserSize;

	MSGPACK_DEFINE(m_clubId, m_onlineUserSize);

	clubOnlineUserSize()
	{
		m_clubId=0;
		m_onlineUserSize=0;
	}
	clubOnlineUserSize(Lint clubId, Lint onlineUserSize)
	{
		m_clubId=clubId;
		m_onlineUserSize=onlineUserSize;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_onlineUserSize), m_onlineUserSize);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_onlineUserSize), m_onlineUserSize);

		return true;
	}

};

//返回玩家登陆结果消息
struct LMsgS2CLogin:public LMsgSC
{
	Lint		m_errorCode;//登录错误码，0-登录成功,1-帐号未注册，2-帐号已登录，9-被封
	Lint		m_id;
	Lstring		m_nike;
	Lint		m_exp;
	Lint		m_sex;
	Lstring		m_face;//
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_ip;
	Lint		m_new;//0-不是，1-是
	Lint		m_card1;//新手奖励房卡1
	Lint		m_card2;//新手奖励房卡2     俱乐部id
	Lint		m_card3;//新手奖励房卡3     玩法id
	Lint		m_state;//0-进入大厅，1-进入桌子 4-俱乐部
	Lstring		m_buyInfo;//够买信息
	Lint		m_hide;//购买隐藏0-隐藏 1- 不隐藏
	Lint		m_gm;//0-不是，1-是GM
	Lint        m_oldUser;
	Lint		m_loginInterval;	// 距离上次登录多少天, 老用户登录送房卡活动使用字段
	std::vector<clubOnlineUserSize>  m_clubOnlineUserCount;   //俱乐部编号和在线人数

	LMsgS2CLogin() :LMsgSC(MSG_S_2_C_LOGIN)
	{
		m_errorCode = -1;
		m_id = 0;
		m_exp = 0;
		m_sex = 1;
		m_new = 0;
		m_card1 = 0;
		m_card2 = 0;
		m_card3 = 0;
		m_state = 0;
		m_hide = 0;
		m_gm = 0;
		m_oldUser = 1;
		m_loginInterval = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(m_exp), m_exp);
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_face), m_face);
		ReadMapData(obj, NAME_TO_STR(m_provice), m_provice);
		ReadMapData(obj, NAME_TO_STR(m_city), m_city);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_new), m_new);
		ReadMapData(obj, NAME_TO_STR(m_card1), m_card1);
		ReadMapData(obj, NAME_TO_STR(m_card2), m_card2);
		ReadMapData(obj, NAME_TO_STR(m_card3), m_card3);
		ReadMapData(obj, NAME_TO_STR(m_oldUser),m_oldUser);
		ReadMapData(obj, NAME_TO_STR(m_loginInterval),m_loginInterval);
		ReadMapData(obj, NAME_TO_STR(m_clubOnlineUserCount), m_clubOnlineUserCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,21);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_nike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(m_exp), m_exp);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_face), m_face);
		WriteKeyValue(pack, NAME_TO_STR(m_provice), m_provice);
		WriteKeyValue(pack, NAME_TO_STR(m_city), m_city);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_new), m_new);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_buyInfo), m_buyInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_hide), m_hide);
		WriteKeyValue(pack, NAME_TO_STR(m_gm), m_gm);
		WriteKeyValue(pack, NAME_TO_STR(m_oldUser), m_oldUser);
		WriteKeyValue(pack, NAME_TO_STR(m_loginInterval), m_loginInterval);
		WriteKeyValue(pack, NAME_TO_STR(m_clubOnlineUserCount), m_clubOnlineUserCount);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLogin();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家物品信息
struct LMsgS2CItemInfo:public LMsgSC
{
	Lint	m_card1;
	Lint	m_card2;
	Lint	m_card3;
	Lint	m_coins;
	Lint    m_credit;

	LMsgS2CItemInfo() :LMsgSC(MSG_S_2_C_ITEM_INFO)
	{
		m_card1 = 0;
		m_card2 = 0;
		m_card3 = 0;
		m_coins = 0;
		m_credit = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_card1), m_card1);
		ReadMapData(obj, NAME_TO_STR(m_card2), m_card2);
		ReadMapData(obj, NAME_TO_STR(m_card3), m_card3);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credit), m_credit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credit), m_credit);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CItemInfo();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回跑马灯信息
struct LMsgS2CHorseInfo :public LMsgSC
{
	enum
	{
		TInfo_Horse = 0,
		TInfo_Free = 1,
		TInfo_Exch = 2,
	};

	Lstring	m_str;
	Lint    m_type;

	LMsgS2CHorseInfo() :LMsgSC(MSG_S_2_C_HORSE_INFO)
	{
		m_type = 0;   //默认0为跑马灯消息推送，1为限时免费
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_str), m_str);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_str), m_str);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHorseInfo(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家进入桌子
struct LMsgS2CIntoDesk:public LMsgSC
{
	Lint			m_deskId;
	Lint			m_pos;	//我的位置，0-3 分别代表东，南，西，北
	Lint			m_ready;//我是否准备，0-未准备，1-已经准备
	Lint			m_score;//我的积分
	Lint			m_coins;//我的金币
	Lint            m_credits; //信用值
	Lint			m_state;//玩法  0 转转 3 长沙 101 血流  102 血战
	Lint			m_maxCircle;//最大局数
	Lint			m_changeOutTime;// 换牌的超时时间 <=0 不限制
	Lint			m_opOutTime; // 桌内操作超时时间 <=0 不限制
	Lint			m_baseScore; // 基础分
	Lint            m_cellscore; //底分
	Lint            m_createUserId;
	Lint            m_deskCreatedType;
	Lint            m_flag;
	Lint            m_feeType;
	Lint            m_cheatAgainst;
	Lint            m_deskType;
	Lstring         m_userIp;
	Lstring         m_userGps;
	std::vector<Lint>	m_playtype;
	Lint              m_Greater2CanStart;
	Lint             m_StartGameButtonPos;
	Lint             m_gamePlayerCount;
	Lint            m_startButtonAppear;
	Lint            m_clubId;
	Lint            m_playTypeId;
	

	LMsgS2CIntoDesk():LMsgSC(MSG_S_2_C_INTO_DESK)
	{
		m_deskId = 0;
		m_pos = INVAILD_POS;
		m_ready = 0;
		m_score = 0;
		m_coins = 0;
		m_state = 0;
		m_maxCircle = 0;
		m_changeOutTime = 0;
		m_opOutTime = 0;
		m_baseScore = 1;
		m_cellscore = 0;
		m_credits = 0;
		m_createUserId = 0;
		m_deskCreatedType = 0;
		m_flag = 0;
		m_feeType = 0;
		m_cheatAgainst = 1;
		m_deskType = 0;
		m_userIp = "";
		m_userGps = "";
		m_Greater2CanStart=0;
		m_StartGameButtonPos=INVAILD_POS;
		m_gamePlayerCount=4;
		m_startButtonAppear=0;
		m_clubId=0;
		m_playTypeId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_maxCircle), m_maxCircle);
		ReadMapData(obj, NAME_TO_STR(m_playtype), m_playtype);
		ReadMapData(obj, NAME_TO_STR(m_changeOutTime), m_changeOutTime);
		ReadMapData(obj, NAME_TO_STR(m_opOutTime), m_opOutTime);
		ReadMapData(obj, NAME_TO_STR(m_baseScore), m_baseScore);
		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(m_createUserId), m_createUserId);
		ReadMapData(obj, NAME_TO_STR(m_deskCreatedType), m_deskCreatedType);

		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_deskType), m_deskType);

		ReadMapData(obj, NAME_TO_STR(m_userIp), m_userIp);
		ReadMapData(obj, NAME_TO_STR(m_userGps), m_userGps);
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		ReadMapData(obj, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		ReadMapData(obj, NAME_TO_STR(m_gamePlayerCount), m_gamePlayerCount);
		ReadMapData(obj, NAME_TO_STR(m_startButtonAppear), m_startButtonAppear);
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_playTypeId), m_playTypeId);


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 28);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(m_changeOutTime), m_changeOutTime);
		WriteKeyValue(pack, NAME_TO_STR(m_opOutTime), m_opOutTime);  
		WriteKeyValue(pack, NAME_TO_STR(m_baseScore), m_baseScore);
		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(m_createUserId), m_createUserId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskCreatedType), m_deskCreatedType);

		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_deskType), m_deskType);


		WriteKeyValue(pack, NAME_TO_STR(m_userIp), m_userIp);
		WriteKeyValue(pack, NAME_TO_STR(m_userGps), m_userGps);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		WriteKeyValue(pack, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		WriteKeyValue(pack, NAME_TO_STR(m_gamePlayerCount), m_gamePlayerCount);
		WriteKeyValue(pack, NAME_TO_STR(m_startButtonAppear), m_startButtonAppear);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeId), m_playTypeId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CIntoDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回桌子添加一个玩家
struct LMsgS2CDeskAddUser:public LMsgSC
{
	Lint			m_userId;
	Lstring			m_nike;//玩家昵称
	Lstring			m_face;//玩家图形，为空就为默认图形
	Lint			m_sex;//玩家性别
	Lint			m_pos;	//位置，0-3 分别代表东，南，西，北
	Lint			m_ready;//0-未准备，1-已准备
	Lint			m_score;//玩家积分
	Lstring			m_ip;
	bool			m_online;//是否在线，true在线，false不在线
	Lint			m_coins;//玩家金币
	Lint            m_credits; //信用值
	Lstring         m_userGps; //
	Lint            m_cheatAgainst;
	Lint            m_videoPermission;

	LMsgS2CDeskAddUser() :LMsgSC(MSG_S_2_C_ADD_USER)
	{
		m_userId = 0;
		m_sex = 1;
		m_pos = INVAILD_POS;
		m_ready = 0;
		m_score = 0;
		m_online = false;
		m_coins = 0;
		m_credits = 0;
		m_userGps = "0,0";
		m_cheatAgainst = 1;
		m_videoPermission = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(m_face), m_face);
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_online), m_online);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		ReadMapData(obj, NAME_TO_STR(m_userGps), m_userGps);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_videoPermission), m_videoPermission);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 15);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_nike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(m_face), m_face);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_online), m_online);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_userGps), m_userGps);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_videoPermission), m_videoPermission);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskAddUser();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回删除玩家
struct LMsgS2CDeskDelUser:public LMsgSC
{
	Lint			m_pos;//删除玩家位置 如果pos == 我的位置，代表我自己被踢出桌子，把桌子其他人清掉
	Lint            m_userId;
	Lstring         m_DismissName;

	//新加
	Lint         m_StartGameButtonPos;

	LMsgS2CDeskDelUser() :LMsgSC(MSG_S_2_C_DEL_USER)
	{
		m_pos = INVAILD_POS;
		m_DismissName = "";
		m_StartGameButtonPos=INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_DismissName), m_DismissName);
		WriteKeyValue(pack, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskDelUser();}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求推出房间
struct LMsgC2SLeaveDesk:public LMsgSC
{
	Lint			m_pos;//我的位置

	LMsgC2SLeaveDesk() :LMsgSC(MSG_C_2_S_LEAVE_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回结果
struct LMsgS2CLeaveDesk:public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-失败

	LMsgS2CLeaveDesk() :LMsgSC(MSG_S_2_C_LEAVE_ROOM)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端申请解算房间
struct LMsgC2SResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	LMsgC2SResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgS2CResetDesk:public LMsgSC
{
	Lint					m_errorCode;//0-等待操作中，1-未开始直接解算，2-三个人同意，解算成功，3-时间到，解算成功，4-有一个人拒绝，解算失败
	Lint					m_time;//倒计时
	Lstring					m_applay;//申请的玩家
	Lint                    m_userId;//申请解散玩家ID
	Lstring					m_refluse;//拒绝的玩家,有一个玩家拒绝，则解算房间失败
	Lint					m_flag;//0-等待我同意或者拒绝，1-我已经同意或者拒绝    
	std::vector<Lint>	    m_agreeUserId;//同意的玩家ID
	std::vector<Lint>	    m_waitUserId;//等待操作的玩家ID
	std::vector<Lstring>	m_agree;//同意的玩家
	std::vector<Lstring>	m_wait;//等待操作的玩家

	std::vector<Lstring>	m_agreeHeadUrl;//同意的玩家头像地址
	std::vector<Lstring>	m_waitHeadUrl;//等待操作的玩家头像地址
	

	LMsgS2CResetDesk() :LMsgSC(MSG_S_2_C_RESET_ROOM)
	{
		m_errorCode = -1;
		m_time = 0;
		m_flag = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_applay), m_applay);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_refluse), m_refluse);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 13);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_apply), m_applay);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		
		WriteKey(pack, "m_agreeUserId");
		WriteKey(pack, m_agreeUserId);

		WriteKey(pack, "m_waitUserId");
		WriteKey(pack, m_waitUserId);

		WriteKey(pack,"m_agree");
		WriteKey(pack, m_agree);

		WriteKey(pack,"m_refuse");
		WriteKey(pack,m_refluse);

		WriteKey(pack, "m_wait");
		WriteKey(pack, m_wait);

		WriteKey(pack, "m_agreeHeadUrl");
		WriteKey(pack, m_agreeHeadUrl);

		WriteKey(pack, "m_waitHeadUrl");
		WriteKey(pack, m_waitHeadUrl);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端同意或者拒绝操作
struct LMsgC2SSelectResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	Lint	m_flag;//1-同意，2-拒绝
	LMsgC2SSelectResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM_SELECT)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SSelectResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送桌子状态，这个消息主要用于重连用
struct LMsgS2CDeskState:public LMsgSC
{
#define   MAX_CARD_NUM   64
	Lint		m_state;  //当前桌子状态 1-等待，2-游戏中，会有后面的数据
	Lint		m_time;   //倒计时
	Lint		m_flag;   //0-等待别人，1-该我出牌，2-该我思考
	Lint		m_zhuang; //庄的位置
	Lint		m_dCount; //桌面剩余的牌
	Lint		m_pos;    //我的位置
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint		m_myPos;
	Lint        m_hType[4];
	Lint        m_deskCreatType;
	//我手上的牌
	CardValue	m_cardValue[MAX_CARD_NUM];
	//手牌中索牌
	CardValue	m_cardSuoValue[4];
	Lint		m_user_count;
	//碰的牌
	Lint		m_pCount[4];
	CardValue	m_pCard[4][MAX_CARD_NUM];
	Lint        m_pCardFirePos[4][MAX_CARD_NUM];

	CardValue	m_phCard[4][MAX_CARD_NUM];	// 带混的牌 给客户端填充四张 填充m_pCard就不能填充m_phCard
	//Lint        m_FirPos[4][MAX_CARD_NUM];  //

	//暗杠的牌
	Lint		m_aCount[4];
	CardValue	m_aCard[4][MAX_CARD_NUM];
	CardValue	m_ahCard[4][MAX_CARD_NUM];	// 带混的牌 给客户端填充四张 填充m_aCard就不能填充m_ahCard

	//明杠的牌
	Lint		m_mCount[4];
	CardValue	m_mCard[4][MAX_CARD_NUM];
	Lint        m_mCardFirePos[4][MAX_CARD_NUM];

	CardValue	m_mhCard[4][MAX_CARD_NUM];	// 带混的牌 给客户端填充四张 填充m_mCard就不能填充m_mhCard

	//出出去的牌
	Lint		m_oCount[4];
	CardValue	m_oCard[4][MAX_CARD_NUM];
	Lint		m_kouCount[4];

	//吃的牌
	Lint		m_eCount[4];
	CardValue	m_eCard[4][MAX_CARD_NUM];

	Lint		m_fhCount[4];
	CardValue	m_fhCard[4][MAX_CARD_NUM];

	CardValue	m_winCard[4];	// 玩家的胡牌
	bool		m_bChange[4];	// 玩家换牌
	CardValue	m_changeCard[3];	// 玩家换的牌

	Lint        m_hCount[4];
	CardValue   m_hCard[4][MAX_CARD_NUM];

	Lint        m_winList[4];

	std::vector<Lint>	m_playtype;	//玩法

	//躺牌
	Lint        m_tCount[4];    //躺牌数量
	CardValue   m_tCard[4][MAX_CARD_NUM];   //躺牌

	Lint		m_changeOutTime;// 换牌的超时时间 <=0 不限制
	Lint		m_opOutTime; // 桌内操作超时时间 <=0 不限制
	Lint		m_baseScore; // 基础分

	CardValue   m_hongTongShuangWang[4];  //洪洞王牌双王大将报牌，需要传递每个人的王牌给客户端（**Ren 2017-12-13）
	std::vector<CardValue> m_hunCard;
	std::vector<Lint>      m_TingState;
	std::vector<Lint>      m_iTing;
	std::vector<CardValue>   m_iTingHuCard;

	std::vector<int> m_score;

	LMsgS2CDeskState() :LMsgSC(MSG_S_2_C_DESK_STATE)
	{
		m_state = 0;
		m_time = 0;
		m_flag = 0;
		m_zhuang = 0;
		m_dCount = 0;
		m_pos = INVAILD_POS;
		m_myPos = INVAILD_POS;
		m_deskCreatType = 0;

		memset( m_cardCount, 0, sizeof(m_cardCount) );
		memset( m_hType, 0, sizeof(m_hType) );
		memset( m_pCount, 0, sizeof(m_pCount) );
		memset( m_aCount, 0, sizeof(m_aCount) );
		memset( m_mCount, 0, sizeof(m_mCount) );
		memset( m_oCount, 0, sizeof(m_oCount) );
		memset( m_eCount, 0, sizeof(m_eCount) );
		memset( m_bChange, 0, sizeof(m_bChange) );
		memset( m_hCount, 0, sizeof(m_hCount) );
		memset( m_winList,0, sizeof(m_winList));
		memset( m_tCount, 0, sizeof(m_tCount));
		memset( m_fhCount, 0, sizeof(m_fhCount));
		memset( m_kouCount, 0, sizeof(m_kouCount));
		m_changeOutTime = 0;
		m_opOutTime = 0;
		m_baseScore = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);
		//ReadMapData(obj, NAME_TO_STR(m_myPos), m_myPos);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			if ( m_pos >= 0 && m_pos < 4 )
			{
				m_cardCount[m_pos] = array.via.array.size;
			}
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		// 加了m_score，这里多加1，按map算的
		WriteMap(pack, 41+12+1+1+8+1+1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);

		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_TingState), m_TingState);
		WriteKeyValue(pack, NAME_TO_STR(m_iTing), m_iTing);  
		WriteKeyValue(pack, NAME_TO_STR(m_deskCreatType), m_deskCreatType);
		std::vector<CardValue> vec;
		WriteKey(pack, "m_CardCount");
		std::vector<Lint> veci;
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_cardCount[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_winList");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_winList[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_hType");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_hType[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_myCard");
		vec.clear();
		if ( m_myPos >= 0 && m_myPos < 4 )
		{
			for (Lint i = 0; i < m_cardCount[m_myPos]; ++i)
			{
				vec.push_back(m_cardValue[i]);
			}
		}
		WriteKey(pack,vec);
		//手牌中锁牌
		WriteKey(pack, "m_mySuoCard");
		vec.clear();
		if ( m_myPos >= 0 && m_myPos < 4 )
		{
			for (Lint i = 0; i < 4; ++i)
			{
				vec.push_back(m_cardSuoValue[i]);
			}
		}
		WriteKey(pack,vec);

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			vec.clear();

			if ( m_pCard[i][0].m_color != 0 || m_pCount[i] == 0 )
			{
				sprintf(name,"m_pCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_pCount[i]; j+=3)
				{
					vec.push_back(m_pCard[i][j]);
				}

			}
			else
			{
				sprintf(name,"m_phCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_pCount[i]; j++)
				{
					vec.push_back(m_phCard[i][j]);
				}
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			veci.clear();

			//if (m_pCard[i][0].m_color != 0 || m_pCount[i] == 0)
			{

				//////////////////////////////////////////
				sprintf(name, "m_pCardFirePos%d", i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_pCount[i]; j += 3)
				{
				veci.push_back(m_pCardFirePos[i][j]);
				}

			}
			WriteKey(pack, veci);
		}


		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			vec.clear();

			sprintf(name,"m_fhCard%d",i);
			WriteKey(pack, name);
			for (Lint j = 0; j < m_fhCount[i]; j++)
			{
				vec.push_back(m_fhCard[i][j]);
			}

			WriteKey(pack,vec);
		}



		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			vec.clear();
			if ( m_aCard[i][0].m_color != 0 || m_aCount[i] == 0 )
			{
				sprintf(name,"m_aCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_aCount[i]; j+=4)
				{
					vec.push_back(m_aCard[i][j]);
				}
			}
			else
			{
				sprintf(name,"m_ahCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_aCount[i]; j++)
				{
					vec.push_back(m_ahCard[i][j]);
				}
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			vec.clear();
			if ( m_mCard[i][0].m_color != 0 || m_mCount[i] == 0 )
			{
				sprintf(name,"m_mCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_mCount[i]; j+=4)
				{
					vec.push_back(m_mCard[i][j]);
				}
		
			}
			else
			{
				sprintf(name,"m_mhCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_mCount[i]; j++)
				{
					vec.push_back(m_mhCard[i][j]);
				}
			}
			WriteKey(pack,vec);
		}


		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			veci.clear();
			//if (m_mCard[i][0].m_color != 0 || m_mCount[i] == 0)
			{
				///////////////////////////////////////////////////
				sprintf(name, "m_mCardFirePos%d", i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_mCount[i]; j += 4)
				{
				veci.push_back(m_mCardFirePos[i][j]);
				}
			}

			WriteKey(pack, veci);

		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_oCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_oCount[i]; ++j)
			{
				vec.push_back(m_oCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "m_kouCount");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_kouCount[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_score");
		WriteKey(pack, m_score);

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_eCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_eCount[i]; ++j)
			{
				vec.push_back(m_eCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "m_winCard");
		vec.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_winCard[i]);
		}
		WriteKey(pack,vec);

		WriteKey(pack, "m_bchange");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bChange[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_changeCard");
		vec.clear();
		for (Lint i = 0; i < 3; ++i)
		{
			vec.push_back(m_changeCard[i]);
		}
		WriteKey(pack,vec);

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_hCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_hCount[i] && (j < 23); ++j)
			{
				vec.push_back(m_hCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_tCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_tCount[i] && (j < 23); ++j)
			{
				vec.push_back(m_tCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "m_hunCard");
		WriteKey(pack, m_hunCard);

		WriteKey(pack, "m_iTingHuCard");
		WriteKey(pack, m_iTingHuCard);

		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(m_changeOutTime), m_changeOutTime);
		WriteKeyValue(pack, NAME_TO_STR(m_opOutTime), m_opOutTime);
		WriteKeyValue(pack, NAME_TO_STR(m_baseScore), m_baseScore);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CDeskState(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端选择座位
struct LMsgC2SUserSelectSeat :public LMsgSC
{
	Lint m_pos;

	LMsgC2SUserSelectSeat() :LMsgSC(MSG_C_2_S_USER_SELECT_SEAT)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUserSelectSeat(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端选择座位  反馈到客户端
struct LMsgS2CUserSelectSeat :public LMsgSC
{
	Lint m_id;
	Lint m_pos;

	//新加开始按钮位置
	Lint  m_StartGameButtonPos;

	LMsgS2CUserSelectSeat() :LMsgSC(MSG_S_2_C_USER_SELECT_SEAT)
	{
		m_id  = 0;
		m_pos = INVAILD_POS;

		m_StartGameButtonPos=INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUserSelectSeat(); }
};



//////////////////////////////////////////////////////////////////////////
//客户端请求准备
struct LMsgC2SUserReady:public LMsgSC
{
	Lint m_pos;

	LMsgC2SUserReady() :LMsgSC(MSG_C_2_S_READY)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserReady();}
};


//////////////////////////////////////////////////////////////////////////
//客户端发送游戏开始指令
struct LMsgC2SStartGame :public LMsgSC
{
	Lint m_pos;

	LMsgC2SStartGame() :LMsgSC(MSG_C_2_S_START_GAME)
	{
		m_pos=INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SStartGame(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserReady:public LMsgSC
{
	Lint			m_pos;//准备玩家的位置

	LMsgS2CUserReady() :LMsgSC(MSG_S_2_C_READY)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserReady();}
};


//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserOnlineInfo:public LMsgSC
{
	Lint			m_flag;//0-掉线了，1-回来了
	Lint			m_pos;//
	Lint            m_userid; //

	LMsgS2CUserOnlineInfo() :LMsgSC(MSG_S_2_C_USER_ONLINE_INFO)
	{
		m_flag = 0;
		m_pos = INVAILD_POS;
		m_userid = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_userid), m_userid);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserOnlineInfo(); }
};
//////////////////////////////////////////////////////////////////////////
//服务器广播游戏开始

struct LMsgS2CPlayStart:public LMsgSC
{
	Lint		m_zhuang;
	Lint		m_dCount;//桌面剩余的牌	
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint		m_pos;
	std::vector<Lint> m_score;
	//我手上的牌
	CardValue	m_cardValue[14];
	CardValue	m_cardSuoValue[4];
	//新添加,游戏开始人数和方位图
	Lint    m_gamePlayerCount;
	Lint    m_posUserid[4];      //东南西北方位依次userid;

	LMsgS2CPlayStart() :LMsgSC(MSG_S_2_C_START)
	{
		for (int i = 0; i < 4; i++) 
		{
			m_cardCount[i] = 0;
			m_posUserid[i]=0;
		}
		m_gamePlayerCount=4;

	}

	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
 		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
 		if (array.type == MSGPACK_OBJECT_ARRAY)
 		{
 			//m_cardCount[0] = array.via.array.size;
 			for (Lsize i = 0; i < array.via.array.size; ++i)
 			{
 				msgpack::object& pv = *(array.via.array.ptr + i);
 				pv.convert(m_cardValue[i]);
 			}
 		}

		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_CardCount), array1);
		if (array1.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(m_cardCount[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);
		ReadMapData(obj, NAME_TO_STR(m_gamePlayerCount), m_gamePlayerCount);

		msgpack::object array2;
		ReadMapData(obj, NAME_TO_STR(m_posUserid), array2);
		if (array2.type==MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i=0; i < array2.via.array.size; ++i)
			{
				msgpack::object& pv=*(array2.via.array.ptr+i);
				pv.convert(m_posUserid[i]);
			}
		}

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);

		WriteKey(pack, "m_myCard");
		std::vector<CardValue>vce;
		for (Lint i = 0; i < m_cardCount[m_pos]; ++i)
		{
			vce.push_back(m_cardValue[i]);
		}
 		WriteKey(pack,vce);

		WriteKey(pack, "m_mySuoCard");
		std::vector<CardValue>vcesuo;
		for (Lint i = 0; i < 4; ++i)
		{
			vcesuo.push_back(m_cardSuoValue[i]);
		}
 		WriteKey(pack,vcesuo);

		WriteKey(pack, "m_CardCount");
		std::vector<Lint>vcei;
		for(Lint i = 0 ; i < 4;++i)
		{
			vcei.push_back(m_cardCount[i]);
		}
		WriteKey(pack,vcei);

		WriteKeyValue(pack, NAME_TO_STR(m_dCount),m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_gamePlayerCount), m_gamePlayerCount);

		WriteKey(pack, "m_posUserid");
		std::vector<Lint>userid;
		for (Lint i=0; i < 4; ++i)
		{
			userid.push_back(m_posUserid[i]);
		}
		WriteKey(pack, userid);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CPlayStart();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸牌

struct ThinkData
{
	Lint	m_type;                 //选择的思考操作：出牌、胡、杠、明杠、暗杠、补杠...
	std::vector<CardValue> m_card;  //选择操作对应的牌

	ThinkData()
	{
		m_type = 0;
	}

	MSGPACK_DEFINE(m_type, m_card); 
};

struct TingData
{
	struct TingCard
	{
		CardValue m_card;
		Lint m_hufan;

		MSGPACK_DEFINE(m_card, m_hufan);
	};

	CardValue	m_card;
	std::vector<TingCard> m_tingcard;

	MSGPACK_DEFINE(m_card, m_tingcard); 
};


//////////////////////////////////////////////////////////////////////////
//客户端摸牌后的操作
struct LMsgC2SUserPlay:public LMsgSC
{
	ThinkData m_thinkInfo;

	LMsgC2SUserPlay() :LMsgSC(MSG_C_2_S_PLAY_CARD)
	{

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_thinkInfo.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_thinkInfo.m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_thinkInfo.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_thinkInfo.m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserPlay();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回操作结果
struct LMsgS2CUserPlay:public LMsgSC
{
	enum
	{
		OutSuccessed = 0,    //0:无错误
		OutNotYourTurn = 1,  //1:不应该由你出牌(当前出牌人由m_pos指定)
		OutOtherTing = 2,    //2:在卡心五中使用,出的牌是别人廊起听的牌,不允许出（除非玩家也廊起了）
		OutTingError = 3,    //3:在卡心五中使用,已经廊起的玩家，打出的牌不是摸到的牌
	};

	Lint		m_errorCode;     // 0: success	
	Lint        m_type;
	Lint		m_flag;          // 0-自摸，1-出牌,2-暗牌,3-明杠(自己碰过之后),4-提
	Lint		m_qiangGangflag; // 标示是否该杠\补操作被抢 0-未被抢/无需等待胡牌 1-被抢 2-等待胡牌
	Lint		m_pos;           // 出牌玩家的位置
	ThinkData	m_card;
	std::vector<Lint>	m_hu;
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	CardValue   m_huCard;				//自摸胡的那张牌
	Lint		m_rewardCardNum;		//胡牌奖励房卡数
	Lint		m_rewardHu;				//胡牌被奖励的番型
	LMsgS2CUserPlay() :LMsgSC(MSG_S_2_C_PLAY_CARD)
	{
		m_errorCode = -1;
		m_type = 0;
		m_flag = 0;
		m_qiangGangflag=2;
		m_pos = INVAILD_POS;
		m_cardCount = 0;
		m_rewardCardNum = 0;
		m_rewardHu = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		ReadMapData(obj, NAME_TO_STR(m_hu), m_hu);
		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), m_cardValue);
		m_huCard.Read(obj);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 14);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_card.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_qianggangflag), m_qiangGangflag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_card.m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardCardNum), m_rewardCardNum);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardHu), m_rewardHu);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		m_huCard.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserPlay();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送玩家思考信息

struct LMsgS2CThink:public LMsgSC
{
	Lint				m_time;//思考倒计时
	Lint				m_flag;//0-等待其他玩家思考中，1-当前我正在思考
	CardValue			m_card;
	bool				m_bOnlyHu;		// 最后四张有胡必胡
	std::vector<ThinkData>	m_think;


	LMsgS2CThink() :LMsgSC(MSG_S_2_C_USER_THINK)
	{
		m_time = 0;
		m_flag = 0;
		m_bOnlyHu = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		m_card.Read(obj);
		//
		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_think), array1);
		if (array1.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				ThinkData v;
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(v);
				m_think.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		WriteKeyValue(pack, NAME_TO_STR(m_bOnlyHu), m_bOnlyHu);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CThink();}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送思考操作结果
struct LMsgC2SUserOper:public LMsgSC
{
	ThinkData m_think; 
	//Lint m_think;//思考结果0-过，1-胡，2-杠，3-碰,4-补

	LMsgC2SUserOper() :LMsgSC(MSG_C_2_S_USER_OPER){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_think.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_think.m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_think.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think.m_card);

		return true;
	}


	virtual LMsg* Clone(){return new LMsgC2SUserOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserOper:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败
	Lint		m_pos;//pos=我自己的时候，需要根据类型响应不同的操作
	ThinkData	m_think; 
	CardValue	m_card; //玩家碰，杠，胡的张牌
	std::vector<Lint>	m_hu;
	Lint        m_hType;                //1代表抢杠胡，否则为其他
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	Lint		m_rewardCardNum;		//胡牌奖励房卡数
	Lint		m_rewardHu;				//胡牌被奖励的牌型
	
	LMsgS2CUserOper() :LMsgSC(MSG_S_2_C_USER_OPER)
	{
		m_errorCode = -1;
		m_pos = INVAILD_POS;
		m_hType = 0;
		m_cardCount = 0;
		m_rewardCardNum = 0;
		m_rewardHu = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		m_card.Read(obj);
		ReadMapData(obj, NAME_TO_STR(m_hu), m_hu);
		ReadMapData(obj, NAME_TO_STR(m_hType), m_hType);
		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), m_cardValue);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		m_card.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(m_hType), m_hType);
		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardCardNum), m_rewardCardNum);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardHu), m_rewardHu);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOper();}
};

//////////////////////////////////////////////////////////////////////////
//玩家耗子操作
struct LMsgS2CUserHaozi:public LMsgSC
{
	Lint		m_type;
	Lint		m_pos;
	std::vector<Lint> m_randNum; //随机数
	std::vector<CardValue> m_HaoZiCards;
	std::vector<ThinkData> m_think;

	LMsgS2CUserHaozi():LMsgSC(MSG_S_2_C_HAOZI), m_type(0),m_pos(4){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_randNum), m_randNum);
		WriteKeyValue(pack, NAME_TO_STR(m_HaoZiCards), m_HaoZiCards);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserHaozi();}
};


//////////////////////////////////////////////////////////////////////////
//客户端请求退出房间
struct LMsgC2SUserOutRoom:public LMsgSC
{
	Lint m_pos;//准备玩家的位置

	LMsgC2SUserOutRoom():LMsgSC(MSG_C_2_S_USER_OUT_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserOutRoom();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果
struct LMsgS2CUserOutRoom:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败
	Lint		m_pos;
	
	LMsgS2CUserOutRoom():LMsgSC(MSG_S_2_C_USER_OUT_ROOM)
	{
		m_errorCode = -1;
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOutRoom();}
};

struct LMsgC2SUserAIOper:public LMsgSC
{
	LMsgC2SUserAIOper() :LMsgSC(MSG_C_2_S_USER_AIOPER)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserAIOper();}
};

//客户端摸牌后的操作
struct LMsgS2CUserAIOper:public LMsgSC
{
	ThinkData m_thinkInfo;

	LMsgS2CUserAIOper() :LMsgSC(MSG_S_2_C_USER_AIOPER)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_thinkInfo.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_thinkInfo.m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserAIOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送起手胡消息

struct StartHuInfo
{
	Lint	m_type;
	std::vector<CardValue> m_card;

	StartHuInfo()
	{
		m_type = 0;
	}

	MSGPACK_DEFINE(m_type, m_card); 
};

struct LMsgS2CUserStartHu:public LMsgSC
{
	std::vector<StartHuInfo> m_card;

	LMsgS2CUserStartHu():LMsgSC(MSG_S_2_C_START_HU){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHu();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果
struct LMsgC2SUserStartHuSelect:public LMsgSC
{
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgC2SUserStartHuSelect():LMsgSC(MSG_C_2_S_START_HU_SELECT)
	{
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		//ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_card), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播玩家操作
struct LMsgS2CUserStartHuSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgS2CUserStartHuSelect():LMsgSC(MSG_S_2_C_START_HU_SELECT)
	{
		m_pos = INVAILD_POS;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//玩家杠牌
struct LMsgS2CUserGangCard:public LMsgSC
{
	Lint		m_pos;
	Lint		m_flag;//0-没有自摸，1-自摸
	std::vector<CardValue> m_card;

	LMsgS2CUserGangCard():LMsgSC(MSG_S_2_C_GANG_RESULT)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserGangCard();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家选择海底牌
struct LMsgS2CUserEndCardSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_time;

	LMsgS2CUserEndCardSelect():LMsgSC(MSG_S_2_C_END_CARD_SELECT)
	{
		m_pos = INVAILD_POS;
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardSelect();}
};

//////////////////////////////////////////////////////////////////////////
//客户的选择海底
struct LMsgC2SUserEndCardSelect:public LMsgSC
{
	Lint		m_flag;//0-不要，1-要

	LMsgC2SUserEndCardSelect():LMsgSC(MSG_C_2_S_END_SELECT)
	{
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserEndCardSelect();}
};


//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸海底
struct LMsgS2CUserEndCardGet:public LMsgSC
{
	Lint		m_pos;	//位置
	Lint		m_flag;//是否自摸 0-没有，1-自摸
	CardValue	m_card;

	LMsgS2CUserEndCardGet():LMsgSC(MSG_S_2_C_END_GET)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardGet();}
};

//玩家杠牌
struct LMsgS2CRemoveQiangGangCard:public LMsgSC
{
	Lint		m_pos;
	Lint		m_flag;		//0-手牌， 1未用
	CardValue	m_card;

	LMsgS2CRemoveQiangGangCard():LMsgSC(MSG_S_2_C_REMOVE_QIANGGANG_CARD)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CRemoveQiangGangCard();}
};
//////////////////////////////////////////////////////////////////////////
//客户端请求说话
struct LMsgC2SUserSpeak:public LMsgSC
{
	Lint		m_type;//说话类型，0-表情，1-语音,2-文字
	Lint		m_id;	//表情id
	Lstring		m_musicUrl; //语音地址链接
	Lstring		m_msg;//聊天文字

	LMsgC2SUserSpeak() :LMsgSC(MSG_C_2_S_USER_SPEAK)
	{
		m_type = -1;
		m_id = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_musicUrl), m_musicUrl);
		ReadMapData(obj, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SUserSpeak(); }
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserSpeak:public LMsgSC
{
	Lint		m_userId;//说话人id
	Lint		m_pos;//说话人的位置
	Lint		m_type;//说话类型
	Lint		m_id;//类型id
	Lstring		m_musicUrl;//语音url链接
	Lstring		m_msg;//聊天文字

	LMsgS2CUserSpeak() :LMsgSC(MSG_S_2_C_USER_SPEAK)
	{
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_type = -1;
		m_id = 0;
	}

	
	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserSpeak(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求创建桌子
struct LMsgC2SCreateDesk :public LMsgSC
{
	Lint		m_flag;					//房间类型，1-2圈，2-四圈，3-8圈
	Lstring		m_secret;				//房间密码，如果为空，服务器会随机一个密码
	Lint		m_gold;					//房间抵住，单位元，最低为1元，最多不能超过100
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	Lint		m_robotNum;				// 0,不加机器人，1，2，3加机器人数量
	Lint        m_credits;              //最大信用值

	Lint        m_cellscore;            // 底分
	Lint        m_feeType;              // 费用类型 ，0:房主付费 1:玩家分摊
	Lint        m_cheatAgainst;         // 是否防作弊，0:不防作弊 1：防作弊
	Lint        m_deskType;             //房间类型，0 创建的房间创建者必须自己加入  1 创建的房间可以自己不加入 
										//	CardValue	m_cardValue[CARD_COUNT];
//	CardValue	m_cardValue[CARD_COUNT];
	CardValue	m_Player0CardValue[13];
	CardValue	m_Player1CardValue[13];
	CardValue	m_Player2CardValue[13];
	CardValue	m_Player3CardValue[13];
	CardValue	m_SendCardValue[84];
	CardValue	m_HaoZiCardValue[2];
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	Lint       m_Greater2CanStart;          //>2可以开局
	
	LMsgC2SCreateDesk() :LMsgSC(MSG_C_2_S_CREATE_ROOM)
	{
		m_flag = 0;	
		m_gold = 0;		
		m_state = 0;	
		m_credits = 0;
		m_robotNum = 0;
		m_Greater2CanStart=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_secret), m_secret);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);

		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_deskType), m_deskType);
		

		msgpack::object array;
		/*ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}*/
		ReadMapData(obj, NAME_TO_STR(m_Player0CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player0CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player1CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player1CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player2CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player2CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player3CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player3CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_SendCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 84; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_SendCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_HaoZiCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 2; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_HaoZiCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 10);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_secret), m_secret);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_robotNum), m_robotNum);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);

		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_deskType), m_deskType);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCreateDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求创建桌子
struct LMsgC2SCreateDeskForOther :public LMsgSC
{
	Lint		m_flag;					//房间类型，1-2圈，2-四圈，3-8圈
	Lstring		m_secret;				//房间密码，如果为空，服务器会随机一个密码
	Lint		m_gold;					//房间抵住，单位元，最低为1元，最多不能超过100
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	Lint		m_robotNum;				// 0,不加机器人，1，2，3加机器人数量
	Lint        m_credits;              //最大信用值
	Lint        m_cellscore;            // 底分
	Lint        m_feeType;              // 费用类型 ，0:房主付费 1:玩家分摊
	Lint        m_cheatAgainst;         // 是否防作弊，0:不防作弊 1：防作弊
										//	CardValue	m_cardValue[CARD_COUNT];
	CardValue	m_Player0CardValue[13];
	CardValue	m_Player1CardValue[13];
	CardValue	m_Player2CardValue[13];
	CardValue	m_Player3CardValue[13];
	CardValue	m_SendCardValue[84];
	CardValue	m_HaoZiCardValue[2];
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	Lint        m_deskType;             //房间类型，0 创建的房间创建者必须自己加入  1 创建的房间可以自己不加入 
	Lint  m_Greater2CanStart;
	LMsgC2SCreateDeskForOther() :LMsgSC(MSG_C_2_S_CREATE_ROOM_FOR_OTHER)
	{
		m_flag = 0;
		m_gold = 0;
		m_state = 0;
		m_credits = 0;
		m_robotNum = 0;
		m_deskType = 0;
		m_Greater2CanStart=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_secret), m_secret);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		ReadMapData(obj, NAME_TO_STR(m_deskType), m_deskType);

		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);

		msgpack::object array;
		/*ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
		for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
		{
		msgpack::object& pv = *(array.via.array.ptr + i);
		pv.convert(m_cardValue[i]);
		}
		}*/
		ReadMapData(obj, NAME_TO_STR(m_Player0CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player0CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player1CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player1CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player2CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player2CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Player3CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player3CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_SendCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 84; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_SendCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_HaoZiCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 2; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_HaoZiCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_secret), m_secret);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_robotNum), m_robotNum);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		WriteKeyValue(pack, NAME_TO_STR(m_deskType), m_deskType);

		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SCreateDeskForOther(); }
};


//////////////////////////////////////////////////////////////////////////
//玩家请求创建桌子结果
struct LMsgS2CCreateDeskRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
	};
	Lint		m_errorCode;//0-成功，1-房卡不够，2-玩家已有存在的桌子，3-桌子号已被使用,4-创建桌子失败,5-创建参数有问题,6-人品不够,信用有误 8-超过数量限制
	Lint		m_deskId;//桌子id

	LMsgS2CCreateDeskRet() :LMsgSC(MSG_S_2_C_CREATE_ROOM)
	{
		m_errorCode = 0;
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCreateDeskRet(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端加入桌子
struct LMsgC2SAddDesk :public LMsgSC
{
	Lint		m_deskId;//桌子id

	LMsgC2SAddDesk() :LMsgSC(MSG_C_2_S_ADD_ROOM)
	{
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SAddDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端进入俱乐部
struct LMsgC2SEnterClub :public LMsgSC
{
	Lint		m_clubId;;//俱乐部id

	LMsgC2SEnterClub() :LMsgSC(MSG_C_2_S_ENTER_CLUB)
	{
		m_clubId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SEnterClub(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端离开俱乐部回复
struct LMsgS2CLeaveClub :public LMsgSC
{
	Lint     m_errorCode;
	Lint		m_clubId;;//俱乐部id

	LMsgS2CLeaveClub() :LMsgSC(MSG_S_2_C_LEAVE_CLUB)
	{
		m_errorCode=0;
		m_clubId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLeaveClub(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端离开俱乐部
struct LMsgC2SLeaveClub :public LMsgSC
{
	Lint		m_clubId;//俱乐部id
	Lint    m_playTypeId;   //玩法id

	LMsgC2SLeaveClub() :LMsgSC(MSG_C_2_S_LEAVE_CLUB)
	{
		m_clubId=0;
		m_playTypeId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_playTypeId), m_playTypeId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeId), m_playTypeId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SLeaveClub(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端俱乐部界面切换玩法
struct LMsgC2SSwitchPlayScene :public LMsgSC
{
	Lint		m_clubId;;                    //俱乐部id
	Lint    m_switchToType;       //切换到其他玩法界面  
	Lint    m_currPlayType;        //现在的playTypeId

	LMsgC2SSwitchPlayScene() :LMsgSC(MSG_C_2_S_SWITCH_PLAY_SCENE)
	{
		m_clubId=0;
		m_switchToType=0;
		m_currPlayType=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_switchToType), m_switchToType);
		ReadMapData(obj, NAME_TO_STR(m_currPlayType), m_currPlayType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_switchToType), m_switchToType);
		WriteKeyValue(pack, NAME_TO_STR(m_currPlayType), m_currPlayType);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SSwitchPlayScene(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端获取个人信息
struct LMsgC2SRequestPersonInfo :public LMsgSC
{
	Lint     m_userId;                       //用户userid
	//Lint		m_clubId;;                    //俱乐部id
	//Lint    m_custom;                  //自定义  

	LMsgC2SRequestPersonInfo() :LMsgSC(MSG_C_2_S_REQUEST_PERSON_INFO)
	{
		m_userId=0;
		//m_clubId=0;
		//m_custom=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		//ReadMapData(obj, NAME_TO_STR(m_custom), m_custom);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		//WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		//WriteKeyValue(pack, NAME_TO_STR(m_custom), m_custom);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequestPersonInfo(); }
};

//////////////////////////////////////////////////////////////////////////
//服务端返回个人信息
struct LMsgS2CReplyPersonInfo :public LMsgSC
{
	Lint                           m_userId;                                       //用户id
	Lstring                     m_name;                                         //用户昵称
	Lstring                     m_headImageUrl;                       //用户头像
	Lstring                     m_ip;                                               //用户ip
	Lint                           m_topScore;                                 //用户一段时间最高分
	Lint                           m_rateOfWin;                             //用户一段时间内胜率
											 
										
	LMsgS2CReplyPersonInfo() :LMsgSC(MSG_S_2_C_REPLY_PERSON_INFO)
	{
		m_userId=0;
		m_topScore=0;
		m_rateOfWin=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		ReadMapData(obj, NAME_TO_STR(m_headImageUrl), m_headImageUrl);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_topScore), m_topScore);
		ReadMapData(obj, NAME_TO_STR(m_rateOfWin), m_rateOfWin);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);
		WriteKeyValue(pack, NAME_TO_STR(m_headImageUrl), m_headImageUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_topScore), m_topScore);
		WriteKeyValue(pack, NAME_TO_STR(m_rateOfWin), m_rateOfWin);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CReplyPersonInfo(); }
};

//////////////////////////////////////////////////////////////////////////
//刷新俱乐部桌子信息
struct LMsgS2CFreshClubDeskInfo :public LMsgSC
{
	Lint m_clubDeskId;       //7位房间号
	Lint m_showDeskId;       //展示房间号
	Lint m_roomFull;         //满员标志  0--房间未满  1--房间已满  （在满员后，有人退出房间，会发送0）
	Lint m_currCircle;       //当前局数  0--未开始游戏，不用展示；开始游戏后，实际局数
	Lint m_totalCircle;      //总局数，8 or 16


	LMsgS2CFreshClubDeskInfo() :LMsgSC(MSG_C_2_S_FRESH_CLUB_DESK_INFO)
	{
		m_clubDeskId=0;
		m_showDeskId=0;
		m_roomFull=0;
		m_currCircle=0;
		m_totalCircle=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_clubDeskId), m_clubDeskId);
		ReadMapData(obj, NAME_TO_STR(m_showDeskId), m_showDeskId);
		ReadMapData(obj, NAME_TO_STR(m_roomFull), m_roomFull);
		ReadMapData(obj, NAME_TO_STR(m_currCircle), m_currCircle);
		ReadMapData(obj, NAME_TO_STR(m_totalCircle), m_totalCircle);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubDeskId), m_clubDeskId);
		WriteKeyValue(pack, NAME_TO_STR(m_showDeskId), m_showDeskId);
		WriteKeyValue(pack, NAME_TO_STR(m_roomFull), m_roomFull);
		WriteKeyValue(pack, NAME_TO_STR(m_currCircle), m_currCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_totalCircle), m_totalCircle);
	
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CFreshClubDeskInfo(); }
};

//////////////////////////////////////////////////////////////////////////
struct deskUserInfo
{

	Lstring   m_deskPlayerName; //用户名字
	Lstring   m_deskPlayerUrl;  //用户头像
	Lint         m_userId;                 //用户id
	Lint         m_pos;                    // 用户座位号 0-3 东南西北
	
	MSGPACK_DEFINE(m_deskPlayerName, m_deskPlayerUrl, m_userId, m_pos);
	deskUserInfo() 
	{
		m_userId=0;
		m_pos=INVAILD_POS;
	}
	deskUserInfo(Lstring  deskPlayerName, Lstring  deskPlayerUrl)
	{
		m_deskPlayerName=deskPlayerName;
		m_deskPlayerUrl=deskPlayerUrl;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerName), m_deskPlayerName);
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}
	
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		//WriteKey(pack, "m_deskPlayerName");
		//WriteKey(pack, m_deskPlayerName);
		//WriteKey(pack, "m_deskPlayerUrl");
		//WriteKey(pack, m_deskPlayerUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}
};

struct desksInfo {
	Lint            m_deskId;             //桌子号
	std::vector<deskUserInfo>    m_deskUserInfo;	 //用户名字,头像
	Lint            m_showDeskId;         //展示的桌子号

	MSGPACK_DEFINE(m_deskId, m_deskUserInfo, m_showDeskId);

	desksInfo()
	{
		m_deskId=0;
		m_showDeskId=0;
	}
	desksInfo(Lint clubDeskId)
	{
		m_deskId=clubDeskId;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_deskUserInfo), m_deskUserInfo);
		ReadMapData(obj, NAME_TO_STR(m_showDeskId), m_showDeskId);
		return true;
	}
	
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskUserInfo), m_deskUserInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_showDeskId), m_showDeskId);	
		return true;
	}
};

//玩法
struct playTypeInfo
{
	Lint     m_playTypeId;           //玩法id
	Lstring  m_playTypeName;         //玩法名称

	MSGPACK_DEFINE(m_playTypeId, m_playTypeName);

	playTypeInfo()
	{
		m_playTypeId=0;
	}
	playTypeInfo(Lint playTypeId, Lstring playTypeName)
	{
		m_playTypeId=playTypeId;
		m_playTypeName=playTypeName;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_playTypeId), m_playTypeId);
		ReadMapData(obj, NAME_TO_STR(m_playTypeName), m_playTypeName);
		return true;
	}
	
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeId), m_playTypeId);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeName), m_playTypeName);
	
		return true;
	}


};

//服务器返回俱乐部游戏玩法界面信息
struct LMsgS2CClubScene :public LMsgSC
{
	//俱乐部信息
	Lint                               m_errorCode;             //结果，成功失败    0--成功，1--用户不是俱乐部会员
	Lint                               m_clubId;                     //俱乐部号
	Lstring                         m_clubName;             //俱乐部名字

	std::vector<playTypeInfo> m_playTypeInfo;     //玩法信息
	//玩法信息
	Lint                              m_PlayTypeid;            //玩法id
	Lstring                        m_name;                  //玩法名字
	Lint        					      m_cellscore;             // 底分
	Lint                              m_flag;                  // 局数
	Lint        					      m_feeType;               // 费用类型 ，0:房主付费 1:玩家分摊
	Lint        					      m_cheatAgainst;          // 是否防作弊，0:不防作弊 1：防作弊
	Lint                              m_state;                 //  大玩法
	Lint                             m_playTypeCount;         //  小选项个数
	std::vector<Lint>	 m_playType;              //  小选项玩法
	Lint                             m_Greater2CanStart;      //  大于2人开局

															 //桌子信息
	Lint               m_deskCount;           //桌子数目
	std::vector<desksInfo>  m_desksInfo;

	LMsgS2CClubScene() :LMsgSC(MSG_S_2_C_CLUB_SCENE)
	{
		m_errorCode=-1;
		m_PlayTypeid=0;
		m_cellscore=0;
		m_flag=0;
		m_feeType=0;
		m_cheatAgainst=0;
		m_state=0;
		m_playTypeCount=0;
		m_Greater2CanStart=0;
		m_deskCount=0;
		m_clubId=0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_clubName), m_clubName);
		ReadMapData(obj, NAME_TO_STR(m_playTypeInfo), m_playTypeInfo);
		ReadMapData(obj, NAME_TO_STR(m_PlayTypeid), m_PlayTypeid);
		ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_playTypeCount), m_playTypeCount);
		ReadMapData(obj, NAME_TO_STR(m_playType), m_playType);
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		ReadMapData(obj, NAME_TO_STR(m_deskCount), m_deskCount);
		ReadMapData(obj, NAME_TO_STR(m_desksInfo), m_desksInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_clubName), m_clubName);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeInfo), m_playTypeInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_PlayTypeid), m_PlayTypeid);
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);
		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeCount), m_playTypeCount);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		WriteKeyValue(pack, NAME_TO_STR(m_deskCount), m_deskCount);
		WriteKeyValue(pack, NAME_TO_STR(m_desksInfo), m_desksInfo);
		
		//WriteKey(pack, "m_desksInfo");
		//WriteKey(pack, m_desksInfo);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CClubScene(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回俱乐部游戏切换玩法界面信息
struct LMsgS2CSwitchClubScene :public LMsgSC
{
	//俱乐部信息
	Lint                               m_errorCode;             //结果，成功失败
	Lint                               m_clubId;                     //俱乐部号
	Lstring                         m_clubName;             //俱乐部名字

	std::vector<playTypeInfo> m_playTypeInfo;     //玩法信息
												  //玩法信息
	Lint                              m_PlayTypeid;            //玩法id
	Lstring                        m_name;                  //玩法名字
	Lint        					      m_cellscore;             // 底分
	Lint                              m_flag;                  // 局数
	Lint        					      m_feeType;               // 费用类型 ，0:房主付费 1:玩家分摊
	Lint        					      m_cheatAgainst;          // 是否防作弊，0:不防作弊 1：防作弊
	Lint                              m_state;                 //  大玩法
	Lint                             m_playTypeCount;         //  小选项个数
	std::vector<Lint>	 m_playType;              //  小选项玩法
	Lint                             m_Greater2CanStart;      //  大于2人开局

															  //桌子信息
	Lint               m_deskCount;           //桌子数目
	std::vector<desksInfo>  m_desksInfo;

	LMsgS2CSwitchClubScene() :LMsgSC(MSG_S_2_C_SWITCH_PLAY_SCENE)
	{
		m_errorCode=-1;
		m_PlayTypeid=0;
		m_cellscore=0;
		m_flag=0;
		m_feeType=0;
		m_cheatAgainst=0;
		m_state=0;
		m_playTypeCount=0;
		m_Greater2CanStart=0;
		m_deskCount=0;
		m_clubId=0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_clubName), m_clubName);
		ReadMapData(obj, NAME_TO_STR(m_playTypeInfo), m_playTypeInfo);
		ReadMapData(obj, NAME_TO_STR(m_PlayTypeid), m_PlayTypeid);
		ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		ReadMapData(obj, NAME_TO_STR(m_cellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_feeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_playTypeCount), m_playTypeCount);
		ReadMapData(obj, NAME_TO_STR(m_playType), m_playType);
		ReadMapData(obj, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		ReadMapData(obj, NAME_TO_STR(m_deskCount), m_deskCount);
		ReadMapData(obj, NAME_TO_STR(m_desksInfo), m_desksInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_clubName), m_clubName);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeInfo), m_playTypeInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_PlayTypeid), m_PlayTypeid);
		WriteKeyValue(pack, NAME_TO_STR(m_name), m_name);
		WriteKeyValue(pack, NAME_TO_STR(m_cellscore), m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_feeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(m_cheatAgainst), m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeCount), m_playTypeCount);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		WriteKeyValue(pack, NAME_TO_STR(m_Greater2CanStart), m_Greater2CanStart);
		WriteKeyValue(pack, NAME_TO_STR(m_deskCount), m_deskCount);
		WriteKeyValue(pack, NAME_TO_STR(m_desksInfo), m_desksInfo);

		//WriteKey(pack, "m_desksInfo");
		//WriteKey(pack, m_desksInfo);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSwitchClubScene(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求加入桌子结果
struct LMsgS2CAddDeskRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
	};

	Lint		m_errorCode;//0-成功，1-人已满,2-房间不存在,3-未知错误 6-信用不够 4- 金币不足 5- 用户有自己建房的桌子，暂时进不去俱乐部桌子（只有俱乐部桌子才会返回这个值）
	Lint		m_deskId;//桌子号码

	std::vector<Lstring>  m_deskSeatPlayerName;
	std::vector<Lstring>  m_deskNoSeatPlayerName;
	
	LMsgS2CAddDeskRet() :LMsgSC(MSG_S_2_C_ADD_ROOM)
	{
		m_errorCode = -1;
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_deskSeatPlayerName), m_deskSeatPlayerName);
		ReadMapData(obj, NAME_TO_STR(m_deskNoSeatPlayerName), m_deskNoSeatPlayerName);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskSeatPlayerName), m_deskSeatPlayerName);
		WriteKeyValue(pack, NAME_TO_STR(m_deskNoSeatPlayerName), m_deskNoSeatPlayerName);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CAddDeskRet(); }
};


struct LMsgC2SEnterCoinsDesk :public LMsgSC
{
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	Lint		m_robotNum;				// 0,不加机器人，1，2，3加机器人数量
	CardValue	m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	Lint		m_coins;				// 玩家的金币
	LMsgC2SEnterCoinsDesk() :LMsgSC(MSG_C_2_S_ENTER_COINS_ROOM)
	{
		m_state = 0;
		m_robotNum = 0;
		m_coins = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v = 0;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SEnterCoinsDesk(); }
};

struct LMsgS2CEnterCoinsDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-金币不够，2-玩家已在金币场，3-未知错误 4-创建桌子失败,5-创建参数有问题 6-人数太多
	Lint		m_remainCount; //m_errorCode=1时有效 表示玩家可以领取的金币的次数

	LMsgS2CEnterCoinsDeskRet() :LMsgSC(MSG_S_2_C_ENTER_COINS_ROOM)
	{
		m_errorCode = 0;
		m_remainCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CEnterCoinsDeskRet(); }
};

//////////////////////////////////////////////////////////////////////////
//vip比赛结束
struct LMsgS2CVipEnd :public LMsgSC
{
	Lint	m_id[4];//玩家id
	Lstring	m_nike[4];//游戏玩家昵称
	Lint	m_gold[4];//输赢积分总数
	Lint	m_zimo[4];//自摸次数
	Lint	m_bomb[4];//放炮次数
	Lint	m_win[4];//收炮次数
	Lint	m_mgang[4];//明杠次数
	Lint	m_agang[4];//暗杠的次数

	Lint	m_dzimo[4];//大胡自摸次数
	Lint	m_dbomb[4];//大胡放炮次数
	Lint	m_dwin[4];//大胡收炮次数
	Lint	m_CheckTing[4];	// 被查叫次数
	Llong   m_time;
	Lint    m_creatorId;
	Lstring m_creatorNike;
	Lstring m_creatorHeadUrl;

	LMsgS2CVipEnd() :LMsgSC(MSG_S_2_C_VIP_END)
	{
		for ( int i = 0; i < 4; ++i )
		{
			m_id[i] = 0;
			m_gold[i] = 0;
			m_zimo[i] = 0;
			m_bomb[i] = 0;
			m_win[i] = 0;
			m_mgang[i] = 0;
			m_agang[i] = 0;
			m_dzimo[i] = 0;
			m_dbomb[i] = 0;
			m_dwin[i] = 0;
			m_CheckTing[i] = 0;
		}
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		
		WriteKey(pack,"m_nike");
		std::vector<Lstring>vecs;
		for(Lint i = 0; i < 4; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack,"m_id");
		std::vector<Lint>veci;
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_id[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_gold");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_gold[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_zimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_zimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_bomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_win");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_win[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_mgang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_agang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dzimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dzimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dbomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dbomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dwin");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dwin[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_ting");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_CheckTing[i]);
		}
		WriteKey(pack,veci);
		WriteKeyValue(pack, "m_time", m_time);
		WriteKeyValue(pack, "m_creatorId", m_creatorId);
		WriteKeyValue(pack, "m_creatorNike", m_creatorNike);
		WriteKeyValue(pack, "m_creatorHeadUrl", m_creatorHeadUrl);
		return true;
	}


	virtual LMsg* Clone(){ return new LMsgS2CVipEnd(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求对账单
struct LMsgC2SVipLog :public LMsgSC
{
	Lint		m_time;//id
	Lint		m_userId;//请求某个玩家的战绩
	
	LMsgC2SVipLog() :LMsgSC(MSG_C_2_S_VIP_LOG)
	{
		m_time = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVipLog(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求对账单2
//Client 2 LogicManager 2 LogicDB
struct LMsgC2SRoomLog :public LMsgSC
{
	Lstring		m_strUUID; //客户端没有传递，由服务器填写
	Lint		m_userId; //请求某个玩家的战绩
	Lint        m_pos;    //pos1
	Lint        m_time;   //time
	Lint        m_deskId; //桌子id

	LMsgC2SRoomLog() :LMsgSC(MSG_C_2_S_ROOM_LOG)
	{
		m_strUUID = "";
		m_userId = 0;
		m_pos = 0;
		m_deskId = 0;
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID),m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId),m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SRoomLog(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果

struct VipLog
{
	Lint	m_time;
	Lstring	m_videoId;
	Lint	m_score[4];

	VipLog()
	{
		m_time = 0;
		memset( m_score, 0, sizeof(m_score) );
	}
};

struct VipLogItemInfo
{
	Lstring m_id;
	Lint m_time;
	Lint m_flag;		//房间类型
	Lint m_userid[4];
	Lstring m_nike[4];
	Lint m_score[4];
	Lint m_reset;//是否解散
	Lint m_deskId;//是否解散
	Lint m_size;
	VipLog m_item[32];

	VipLogItemInfo()
	{
		m_time = 0;
		m_flag = 0;
		memset( m_userid, 0, sizeof(m_userid) );
		memset( m_score, 0, sizeof(m_score) );
		m_reset = 0;
		m_deskId = 0;
		m_size = 0;
	}
};

struct LMsgS2CVipLog:public LMsgSC
{
	Lint				m_size;//长度
	VipLogItemInfo		m_info[20];//log信息
	
	LMsgS2CVipLog() :LMsgSC(MSG_S_2_C_VIP_LOG)
	{
		m_size = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack,"m_data");
		WriteArray(pack,m_size);
		for(Lint i = 0 ; i < m_size; ++i)
		{
			WriteMap(pack,9);
			WriteKeyValue(pack,"m_id",m_info[i].m_id);
			WriteKeyValue(pack,"m_time",m_info[i].m_time);
			WriteKeyValue(pack,"m_flag",m_info[i].m_flag);
			WriteKeyValue(pack,"m_reset",m_info[i].m_reset);
			WriteKeyValue(pack,"m_deskId",m_info[i].m_deskId);

			std::vector<Lstring> vecs;
			std::vector<Lint>veci,vec;
			for(Lint j = 0 ; j < 4; ++j)
			{
				vecs.push_back(m_info[i].m_nike[j]);
				veci.push_back(m_info[i].m_score[j]);
				vec.push_back(m_info[i].m_userid[j]);
			}
			WriteKey(pack,"m_userid");
			WriteKey(pack,vec);

			WriteKey(pack,"m_nike");
			WriteKey(pack,vecs);

			WriteKey(pack,"m_score");
			WriteKey(pack,veci);

			WriteKey(pack,"m_match");
			WriteArray(pack,m_info[i].m_size);
		}
	
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipLog(); }
};

//LogicManager 2 Client
struct LMsgS2CRoomLog:public LMsgSC
{
	Lint m_size;
	Lint m_flag;		//房间类型
	VipLog m_item[32];

	LMsgS2CRoomLog() :LMsgSC(MSG_S_2_C_ROOM_LOG)
	{
		m_flag = 0;
		m_size = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack,"m_flag",m_flag);
		WriteKey(pack,"m_data");
		WriteArray(pack,m_size);
		for(Lint k = 0 ; k < m_size && k < 32; ++k)
		{
			WriteMap(pack,3);
			WriteKeyValue(pack,"m_time",m_item[k].m_time);
			WriteKeyValue(pack,"m_videoId",m_item[k].m_videoId);

			std::vector<Lint> vec;
			for(Lint n = 0 ; n < 4; ++n)
			{
				vec.push_back(m_item[k].m_score[n]);
			}
			WriteKey(pack,"m_score");
			WriteKey(pack,vec);
		}
		
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CRoomLog(); }
};

//inviter-----------------------------------------------
struct UserBasicInfo
{
	Lint		m_id;
	Lstring		m_unionId;
	Lstring		m_nickName;
	Lstring		m_headImgUrl;
	UserBasicInfo()
	{
		m_id = 0;
		m_unionId = "";
		m_nickName = "";
		m_headImgUrl = "";
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_headImgUrl);
		return true;
	}
};

struct InviteeInfo
{
	UserBasicInfo		m_info;
	Lint				m_taskFinished;		// 被邀请者任务是否完成，0：没有完成，非0：完成
	InviteeInfo():m_info()
	{
		m_taskFinished = 0;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_info.m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_info.m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_info.m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_info.m_headImgUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_taskFinished), m_taskFinished);
		return true;
	}
};


struct DeskUserSeatInfo
{
	UserBasicInfo		m_info;
	Lint				m_SeatId;		// 被，0：没有完成，非0：完成
	DeskUserSeatInfo() :m_info()
	{
		m_SeatId = 0;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_info.m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_info.m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_info.m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_info.m_headImgUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_SeatId), m_SeatId);
		return true;
	}
};

struct LMsgS2CInvitingInfo : public LMsgSC
{
	struct Reward
	{
		Lint	m_type;
		Lint	m_count;
		Reward():m_type(0),m_count(0){}
	};

	Reward						m_reward;
	UserBasicInfo				m_inviter;
	std::vector<InviteeInfo>	m_invitees;

	LMsgS2CInvitingInfo() : LMsgSC(MSG_S_2_C_INVITING_INFO) ,m_inviter()
	{

	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack, NAME_TO_STR(m_reward));
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_reward.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_count), m_reward.m_count);

		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);

		WriteKey(pack, NAME_TO_STR(m_invitees));
		WriteArray(pack, m_invitees.size());

		for (Lsize i = 0; i < m_invitees.size(); i++)
			m_invitees[i].Write(pack);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CInvitingInfo; }
};

struct LMsgC2SBindInviter : public LMsgSC
{
	Lint				m_inviterId;
	LMsgC2SBindInviter() : LMsgSC(MSG_C_2_S_BIND_INVITER) 
	{
		m_inviterId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SBindInviter; }
};

struct LMsgS2CBindInviter : public LMsgSC
{
	ErrorCode			m_error;
	UserBasicInfo		m_inviter;

	LMsgS2CBindInviter() : LMsgSC(MSG_S_2_C_BIND_INVITER),m_inviter()
	{
		m_error = Unknown;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBindInviter; }
};
////-end

struct LMsgC2SGetUserInfo : public LMsgSC
{
	Lint		m_userId;

	LMsgC2SGetUserInfo() : LMsgSC(MSG_C_2_S_GET_USER_INFO), m_userId(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetUserInfo; }
};

struct LMsgS2CUserInfo : public LMsgSC
{
	ErrorCode			m_error;
	UserBasicInfo		m_user;

	LMsgS2CUserInfo() : LMsgSC(MSG_S_2_C_USER_INFO) 
	{
		m_error = Unknown;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_user));
		m_user.Write(pack);

		return true;
	}

	virtual LMsg* Clone() {return new LMsgS2CUserInfo;}
};

struct LMsgC2SGetInvitingInfo : public LMsgSC
{
	Lstring  m_strUUID;		//由服务器填写
	Lint     m_userId;
	LMsgC2SGetInvitingInfo() : LMsgSC(MSG_C_2_S_GET_INVITING_INFO) 
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetInvitingInfo; }
};

//------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//客户端请求录像
struct LMsgC2SVideo :public LMsgSC
{
	Lstring		m_videoId;//id

	LMsgC2SVideo() :LMsgSC(MSG_C_2_S_VIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVideo(); }
};

//玩家通过精确的videoid查看分享的某录像
struct LMsgC2SGetShareVideo :public LMsgSC
{
	Lstring		m_videoId;//id

	LMsgC2SGetShareVideo() :LMsgSC(MSG_C_2_S_GETSHAREVIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGetShareVideo(); }
};

//玩家请求查看某shareid的分享录像
struct LMsgC2SShareVideo :public LMsgSC
{
	Lint		m_shareID;//id

	LMsgC2SShareVideo() :LMsgSC(MSG_C_2_S_SHAREVIDEO),m_shareID(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_shareID), m_shareID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_shareID), m_shareID);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SShareVideo(); }
};

//玩家请求分享某录像
struct LMsgC2SReqShareVideo :public LMsgSC
{
	Lstring		m_videoid;//id

	LMsgC2SReqShareVideo() :LMsgSC(MSG_C_2_S_REQSHAREVIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoid);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoid), m_videoid);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SReqShareVideo(); }
};

//服务器返回分享的id
struct LMsgS2CRetShareVideoId :public LMsgSC
{
	Lint		m_errorId;//0成功 1失败
	Lint		m_shareId;//id

	LMsgS2CRetShareVideoId() :LMsgSC(MSG_S_2_C_RETSHAREVIDEOID),m_errorId(0),m_shareId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorId), m_errorId);
		ReadMapData(obj, NAME_TO_STR(m_shareId), m_shareId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);
		WriteKeyValue(pack, NAME_TO_STR(m_shareId), m_shareId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CRetShareVideoId(); }
};


struct S2CShareVideo{
	Lstring	m_videoId;
	Lint	m_time;
	Lstring	m_userNike[4];
	Lint	m_score[4];
	Lint    m_user_count;
	S2CShareVideo():m_time(0),m_user_count(0){
		memset(m_score,0,sizeof(m_score));
	}
};

//服务器返回某shareid对应的分享录像
struct LMsgS2CShareVideo:public LMsgSC
{
	Lint		m_count;
	Lint		m_errorId;//0失败 1成功
	std::vector<S2CShareVideo> m_vec_share_video;

	LMsgS2CShareVideo() :LMsgSC(MSG_S_2_C_SHAREVIDEO),m_errorId(0),m_count(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);

		WriteKey(pack,"m_data");
		WriteArray(pack,m_count);

		for(Lint i = 0 ; i < m_count; ++i)
		{
			WriteMap(pack,4);
			WriteKeyValue(pack, NAME_TO_STR(m_videoId),m_vec_share_video[i].m_videoId);
			WriteKeyValue(pack, NAME_TO_STR(m_time), m_vec_share_video[i].m_time);
			std::vector<Lint> vecscore;
			std::vector<Lstring> vecnike;
			for(Lint n = 0 ; n < m_vec_share_video[i].m_user_count; ++n)
			{
				vecnike.push_back(m_vec_share_video[i].m_userNike[n]);
				vecscore.push_back(m_vec_share_video[i].m_score[n]);
			}

			WriteKey(pack,"m_score");
			WriteKey(pack,vecscore);

			WriteKey(pack,"m_nike");
			WriteKey(pack, vecnike);
		}

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CShareVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果

struct LMsgS2CVideo:public LMsgSC
{
	VideoLog		m_video;//log信息
	Lint			m_sex[4];
	Lstring			m_nike[4];
	Lstring			m_imageUrl[4];
	Lint			m_zhuang;
	Lint			m_deskId;
	Lint			m_curCircle;
	Lint			m_maxCircle;
	Lint			m_flag;
	Lint			m_score[4];

	LMsgS2CVideo() :LMsgSC(MSG_S_2_C_VIDEO)
	{
		memset( m_sex, 0, sizeof(m_sex) );
		m_zhuang = 0;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		m_flag = 0;
		memset( m_score, 0, sizeof(m_score) );
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_flag);

		std::vector<Lint>vecid;
		std::vector<Lint>vecsex;
		std::vector<Lint>vecscore;
		std::vector<Lstring>vecnike;
		std::vector<Lstring>vecurl;

		for(Lint i = 0 ; i < 4; ++i)
		{
			if ( m_video.m_userId[i] == 0 )
			{
				continue;
			}
			vecid.push_back(m_video.m_userId[i]);
			vecnike.push_back(m_nike[i]);
			vecsex.push_back(m_sex[i]);
			vecurl.push_back(m_imageUrl[i]);
			vecscore.push_back(m_score[i]);
		}

		WriteKey(pack,"m_score");
		WriteKey(pack,vecscore);

		WriteKey(pack,"m_userid");
		WriteKey(pack,vecid);

		WriteKey(pack,"m_nike");
		WriteKey(pack, vecnike);

		WriteKey(pack, "m_sex");
		WriteKey(pack, vecsex);

		WriteKey(pack, "m_imageUrl");
		WriteKey(pack, vecurl);

		WriteKey(pack,"m_time");
		WriteKey(pack,m_video.m_time);

		WriteKey(pack,"m_oper");
		WriteKey(pack,m_video.m_oper);

		WriteKey(pack,"m_playtype");
		WriteKey(pack,m_video.m_playType);

		for(Lint i = 0 ; i < 4; ++i)
		{
			std::stringstream ss ;
			ss << "m_card" << i;
			WriteKey(pack,ss.str());
			WriteKey(pack,m_video.m_handCard[i]);
		}

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//同步vip房间信息
struct LMsgS2CVipInfo :public LMsgSC
{
	Lint			m_curCircle;//当前局数
	Lint			m_curMaxCircle;//最大局数
	std::vector<Lint>	m_playtype;

	LMsgS2CVipInfo() :LMsgSC(MSG_S_2_C_VIP_INFO)
	{
		m_curCircle = 0;
		m_curMaxCircle = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		//ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_curMaxCircle), m_curMaxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipInfo(); }
};


// 服务器通知玩家换牌
struct LMsgS2CUserChange:public LMsgSC
{
	Lint		m_time;		// 10:请求玩家 0:表示玩家换牌成功
	Lint		m_pos;		//换牌玩家位置
	CardValue	m_card[3];	// m_time=0时有效  目前只有金币场填写因为有倒计时

	LMsgS2CUserChange() :LMsgSC(MSG_S_2_C_USER_CHANGE)
	{
		m_time = 0;
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		std::vector<CardValue> vec;
		WriteKey(pack, "m_card");
		for (Lint i = 0; i < 3; ++i)
		{
			vec.push_back(m_card[i]);
		}
		WriteKey(pack,vec);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserChange(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端换牌
struct LMsgC2SUserChange:public LMsgSC
{
	Lint		m_pos;		//玩家的位置
	CardValue	m_card[3];	//玩家换的牌

	LMsgC2SUserChange() :LMsgSC(MSG_C_2_S_USER_CHANGE)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_card), array);
		if (array.type == msgpack::v1::type::ARRAY && array.via.array.size == 3)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_card[i] = v;
			}
		}

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserChange();}
};

// 服务器通知玩家换牌结果
struct LMsgS2CUserChangeResult:public LMsgSC
{
	Lint		m_flag;		//换牌规则
	CardValue	m_card[3];	//玩家换的牌

	LMsgS2CUserChangeResult() :LMsgSC(MSG_S_2_C_USER_CHANGERESULT)
	{
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);

		std::vector<CardValue> vec;
		WriteKey(pack, "m_card");
		for (Lint i = 0; i < 3; ++i)
		{
			vec.push_back(m_card[i]);
		}
		WriteKey(pack,vec);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserChangeResult(); }
};

// 单局结算
struct SingleResult
{
	Lint m_winPos;
	Lint m_dianPos;
	std::vector<Lint> m_fanType;
	Lint m_genCount;
	CardValue m_winCard;

	SingleResult()
	{
		m_winPos = INVAILD_POS;
		m_dianPos = INVAILD_POS;
		m_fanType.clear();
		m_genCount = 0;
		m_winCard.m_color = 0;
		m_winCard.m_number = 0;
	}

	MSGPACK_DEFINE(m_winPos, m_dianPos, m_fanType,m_genCount, m_winCard); 

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_winPos), m_winPos);
		WriteKeyValue(pack, NAME_TO_STR(m_dianPos), m_dianPos);
		WriteKeyValue(pack, NAME_TO_STR(m_fanType), m_fanType);
		WriteKeyValue(pack, NAME_TO_STR(m_genCount), m_genCount);
		WriteKeyValue(pack, NAME_TO_STR(m_winCard), m_winCard);
		return true;
	}
};

// 服务器通知玩家单次胡牌信息
struct LMsgS2CGameResult:public LMsgSC
{
	std::vector<SingleResult>	m_result;

	LMsgS2CGameResult() :LMsgSC(MSG_S_2_C_GAMEREULT){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);

		std::vector<SingleResult> vec;
		WriteKey(pack, "m_result");
		for (Lsize i = 0; i < m_result.size(); ++i)
		{
			vec.push_back(m_result[i]);
		}
		WriteKey(pack,vec);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGameResult(); }
};

struct LMsgC2STangCard : public LMsgSC
{
	CardValue			m_cardValue[14];  //躺牌
	CardValue           m_outCard;

	LMsgC2STangCard() :LMsgSC(MSG_C_2_S_TANGCARD)
	{
	}
	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv =  *(array.via.array.ptr+i);
				pv.convert(m_cardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_outCard), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			msgpack::object& pv =  *(array.via.array.ptr);
			pv.convert(m_outCard);
		}
		return true;
	}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2STangCard();
	}
};

struct LMsgS2CTangCardRet : public LMsgSC
{
	enum
	{
		TSuccessed   = 0,    //躺成功
		TPosError    = 1,    //pos错误
		TStatusError = 2,    //状态错误
		TCardError   = 3,    //躺牌错误,躺牌不在手牌中 ，或为空
		TFailed      = 4,    //躺不下去
	};

	Lint				m_result;   //0=success other=failed
	Lint                m_pos;      // 躺牌玩家的pos
	Lint                m_tangCount;
	CardValue	        m_cardValue[14];	//玩家 躺下的牌
	CardValue			m_outCard;			//玩家躺牌出去的牌
	Lint                m_tingCount;
	CardValue	        m_tingCards[CARD_COUNT/4];	//玩家听的牌

	LMsgS2CTangCardRet() :LMsgSC(MSG_S_2_C_TANGCARD_RET)
	{	
		m_result = -1;	
		m_pos = INVAILD_POS;
		m_tangCount = 0;
		m_tingCount = 0;
	}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		std::vector<CardValue> vec;
		WriteKey(pack, "m_cardValue");
		for (Lint i = 0; i < m_tangCount && i < 14; ++i)
		{
			vec.push_back(m_cardValue[i]);
		}
		WriteKey(pack,vec);

		WriteKeyValue(pack, NAME_TO_STR(m_outCard), m_outCard);

		vec.clear();
		WriteKey(pack, "m_tingCards");
		for (Lint i = 0; i < m_tingCount && i < 14; ++i)
		{
			vec.push_back(m_tingCards[i]);
		}
		WriteKey(pack,vec);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CTangCardRet();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求 可点赞记录
struct LMsgC2SCRELog :public LMsgSC
{
	Lint		m_time;   //暂未用
	Lint		m_userId; //用户id

	LMsgC2SCRELog() :LMsgSC(MSG_C_2_S_REQ_CREHIS)
	{
		m_time = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCRELog(); }
};

//用户基本信息for 点赞客户端显示头像 等信息
struct UserBaseInfo_s     
{
	Lint    m_userId;
	Lstring m_strNike;
	Lstring m_strHeadUrl;
	Lint    m_sex;

	UserBaseInfo_s()
	{
		m_sex = 0;
		m_userId = 0;
	}

	MSGPACK_DEFINE(m_userId, m_strNike, m_strHeadUrl, m_sex); 

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strNike), m_strNike);
		WriteKeyValue(pack, NAME_TO_STR(m_strHeadUrl), m_strHeadUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		return true;
	}
};

//服务器 返回 可点赞记录
struct LMsgS2CCRELogRet :public LMsgSC
{
	std::vector<std::string> m_record;   //点赞记录 
	std::vector<UserBaseInfo_s> m_info;  //用户基本信息


	LMsgS2CCRELogRet() :LMsgSC(MSG_S_2_C_RET_CREHIS)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_record), m_record);

		std::vector<UserBaseInfo_s> vec;
		WriteKey(pack, "m_info");
		for (Lsize i = 0; i < m_info.size(); ++i)
		{
			vec.push_back(m_info[i]);
		}
		WriteKey(pack,vec);
	
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCRELogRet(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求 评价点赞
struct LMsgC2SREQ_GTU :public LMsgSC
{
	Lint        m_index;                   //索引
	Lint		m_userId;                  //当前请求玩家id
	Lstring     m_onelog;                  //可点赞记录
	std::vector<Lint>        m_userList;   //用户列表

	LMsgC2SREQ_GTU() :LMsgSC(MSG_C_2_S_REQ_GTU)
	{
		m_userId = 0;
		m_index = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_index), m_index);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_onelog), m_onelog);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_userList), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v = 0;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_userList.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SREQ_GTU(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器 回复 客户端请求点赞成功
struct LMsgS2CREQ_GTU_RET :public LMsgSC
{
	Lint        m_index;   //索引

	LMsgS2CREQ_GTU_RET() :LMsgSC(MSG_S_2_C_RET_GTU)
	{
		m_index = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_index), m_index);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CREQ_GTU_RET(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求 评价点赞
struct LMsgC2SREQ_Exch :public LMsgSC
{
	Lint        m_card;                   //索引

	LMsgC2SREQ_Exch() :LMsgSC(MSG_C_2_S_REQ_EXCH)
	{
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SREQ_Exch(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器 回复 客户端请求点赞成功
struct LMsgS2CREQ_EXCH_RET :public LMsgSC
{
	Lint        m_card;   //索引

	LMsgS2CREQ_EXCH_RET() :LMsgSC(MSG_S_2_C_RET_EXCH)
	{
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CREQ_EXCH_RET(); }
};

// 和后的ID对应
enum ActivityID
{
	ActivityID_Draw	= 1002,	// 新装盘
	ActivityID_Share	= 1003,	// 分享
};

struct LMsgS2CActivityInfo:public LMsgSC
{
	typedef struct Value
	{
		Lstring m_key;
		Lstring m_value;
		MSGPACK_DEFINE(m_key,m_value);
	}ATTRI;

	struct Info
	{
		Lint m_id;		   // ActivityID类型
		Lstring m_info;	   // 活动内容 根据m_id来填充和解析
		std::vector<struct Value> m_attri;  //根据m_id来填充解析

		Info()
		{
			m_id = 0;
		}

		MSGPACK_DEFINE(m_id, m_info,m_attri);
	};

	std::vector<Info> m_activities;

	LMsgS2CActivityInfo() : LMsgSC(MSG_S_2_C_ACTIVITY_INFO)
	{
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		Lint m_num = (Lint)m_activities.size();
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_activities), m_activities);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityInfo(); }
};

struct LMsgC2SActivityPhone : public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	Lint		m_AutoId;		// 服务器的给客户端的ID
	Lstring		m_PhoneNum;		// 电话号码

	LMsgC2SActivityPhone() :LMsgSC(MSG_C_2_S_ACTIVITY_WRITE_PHONE)
	{
		m_userId = 0;
		m_AutoId = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_AutoId), m_AutoId);
		ReadMapData(obj, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_AutoId), m_AutoId);
		WriteKeyValue(pack, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityPhone(); }
};

struct LMsgS2CActivityPhone:public LMsgSC
{
	Lint		m_errorCode;			//结果 0成功 其他失败
	LMsgS2CActivityPhone() :LMsgSC(MSG_S_2_C_ACTIVITY_WRITE_PHONE)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityPhone(); }
};

struct LMsgC2SActivityRequestLog : public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	Lint		m_activeId;	// 活动ID ActivityID类型 若为0表示查询玩家所有的活动纪录

	LMsgC2SActivityRequestLog() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_LOG)
	{
		m_userId = 0;
		m_activeId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_activeId), m_activeId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeId), m_activeId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestLog(); }
};

struct ActivityLog
{
	ActivityLog()
	{
		AutoId = 0;
		Id = 0;
		SpendType = 0;
		SpendCount = 0;
		GiftType = 0;
		GiftCount = 0;
		Date = 0;
	}

	Lint AutoId;
	Lint Id;
	Lint SpendType;
	Lint SpendCount;
	Lstring GiftName;
	Lint GiftType;
	Lint GiftCount;
	Lint Date;
	Lstring Phone;

	// 下面这种写法 客户端看不到变量名 直接安装顺序来取值 所以顺序很重要
	MSGPACK_DEFINE(AutoId, Id, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date, Phone);
};

struct LMsgS2CActivityReplyLog:public LMsgSC
{
	Lint		m_errorCode;			//结果 0成功 其他失败
	std::vector<ActivityLog> m_logs;

	LMsgS2CActivityReplyLog() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_LOG)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_logs), m_logs);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_logs), m_logs);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyLog(); }
};

struct LMsgC2SActivityRequestDrawOpen:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestDrawOpen() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestDrawOpen(); }
};

// 装盘
struct ActivityDrawWinUser
{
	Lstring m_userId;	// 玩家id
	Lstring m_giftName;	// 中奖的名称
	MSGPACK_DEFINE(m_userId,m_giftName);
};

struct LMsgS2CActivityReplyDrawOpen:public LMsgSC
{
	Lint		m_errorCode;	// 转盘结果 0成功 -1未知错误 1当前没有这个转盘活动
	// m_errorCode=0 时 下面的值有效
	std::vector<ActivityDrawWinUser> m_winUsers;
	Lint		m_SpendType;	// 消耗的类型 0免费 1房卡（单位个）
	Lint		m_SpendCount;	// 消耗的数量

	LMsgS2CActivityReplyDrawOpen() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_DRAW_OPEN)
	{
		m_errorCode = -1;
		m_SpendType = 0;
		m_SpendCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_winUsers), m_winUsers);
		ReadMapData(obj, NAME_TO_STR(m_SpendType), m_SpendType);
		ReadMapData(obj, NAME_TO_STR(m_SpendCount), m_SpendCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_winUsers), m_winUsers);
		WriteKeyValue(pack, NAME_TO_STR(m_SpendType), m_SpendType);
		WriteKeyValue(pack, NAME_TO_STR(m_SpendCount), m_SpendCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyDrawOpen(); }
};

struct LMsgC2SActivityRequestDraw:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_cardNum;	// 玩家的卡 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestDraw() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_DRAW)
	{
		m_userId = 0;
		m_cardNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_cardNum), m_cardNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_cardNum), m_cardNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestDraw(); }
};

struct LMsgS2CActivityReplyDraw:public LMsgSC
{
	Lint		m_errorCode;	// 转盘结果 0成功 -1未知错误 1消耗的物品不足（一般卡不足） 2 抽奖活动已经关闭或者未开启
	// 结果为0时 下面的值有效
	Lint		m_GiftIndex;	// 奖励索引
	Lstring		m_GiftName;		// 奖励名称
	Lint		m_GiftType;		// 奖励类型 参照ActivityDrawGift 1房卡（单位个） 2话费（单位元） 3 乐视电视（单位个） 4 IPad min4（单位个） 5 iPhone7 plus（单位个）
	Lint		m_GiftCount;	// 奖励数量
	Lint		m_NeedPhoneNum;	// 是否需要电话
	Lint		m_AutoId;		// 填写电话号码时回执使用

	LMsgS2CActivityReplyDraw() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_DRAW)
	{
		m_errorCode = -1;
		m_GiftIndex = 0;
		m_GiftType = 0;
		m_GiftCount = 0;
		m_NeedPhoneNum = false;
		m_AutoId = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_GiftIndex), m_GiftIndex);
		ReadMapData(obj, NAME_TO_STR(m_GiftName), m_GiftName);
		ReadMapData(obj, NAME_TO_STR(m_GiftType), m_GiftType);
		ReadMapData(obj, NAME_TO_STR(m_GiftCount), m_GiftCount);
		ReadMapData(obj, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		ReadMapData(obj, NAME_TO_STR(m_AutoId), m_AutoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftIndex), m_GiftIndex);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftName), m_GiftName);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftType), m_GiftType);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftCount), m_GiftCount);
		WriteKeyValue(pack, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		WriteKeyValue(pack, NAME_TO_STR(m_AutoId), m_AutoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyDraw(); }
};

//客户端 分享朋友圈 成功
struct LMsgC2SActivityRequestShare:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestShare() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_SHARE)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
 		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestShare(); }
};

struct LMsgS2CActivityFinishShare:public LMsgSC
{
	Lint		m_ErrorCode;	// 转盘结果 0成功 -1未知错误
	Lint		m_GiftCount;	// 奖励房卡张数0=不奖励
	Lint		m_GiftType;		// 奖励类型 参照ActivityDrawGift

	LMsgS2CActivityFinishShare() :LMsgSC(MSG_S_2_C_ACTIVITY_FINISH_SHARE)
	{
		m_ErrorCode = -1;
		m_GiftCount = 0;
		m_GiftType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		ReadMapData(obj, NAME_TO_STR(m_GiftCount), m_GiftCount);
		ReadMapData(obj, NAME_TO_STR(m_GiftType), m_GiftType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftType), m_GiftType);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftCount), m_GiftCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityFinishShare(); }
};
/*
MSG_C_2_S_SEND_VIDEO_INVITATION = 900, //发起视频邀请

MSG_C_2_S_RECEIVE_VIDEO_INVITATION = 901, //收到视频邀请
MSG_C_2_S_INBUSY_VIDEO_INVITATION = 903, //视频邀请忙线中
MSG_C_2_S_ONLINE_VIDEO_INVITATION = 904, //视频已连线
MSG_C_2_S_SHUTDOWN_VIDEO_INVITATION = 906, //关闭视频
MSG_C_2_S_UPLOAD_GPS_INFORMATION = 907, //上传GPS坐标
MSG_C_2_S_UPLOAD_VIDEO_PERMISSION = 908, //上传视频是否允许
MSG_S_2_C_UPDATE_USER_VIP_INFO = 909, //用户的视频是否开启，gps
*/
struct LMsgC2SSendVideoInvitation :public LMsgSC
{
	Lint m_userId;     //请求视频响应方
	Lstring m_strUUID;
	Lint m_reqUserId;  //请求视频发起方

	LMsgC2SSendVideoInvitation() :LMsgSC(MSG_C_2_S_SEND_VIDEO_INVITATION)
	{
		m_userId = 0;
		m_strUUID = "";
		m_reqUserId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SSendVideoInvitation(); }

};


struct LMsgC2SReceiveVideoInvitation :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lint m_reqUserId;

	LMsgC2SReceiveVideoInvitation() :LMsgSC(MSG_C_2_S_RECEIVE_VIDEO_INVITATION)
	{
		m_userId = 0;
		m_strUUID = "";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SReceiveVideoInvitation(); }

};

struct LMsgC2SInbusyVideoInvitation :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lint m_reqUserId;

	LMsgC2SInbusyVideoInvitation() :LMsgSC(MSG_C_2_S_INBUSY_VIDEO_INVITATION)
	{
		m_userId = 0;
		m_strUUID = "";
		m_reqUserId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SInbusyVideoInvitation(); }

};

struct LMsgC2SOnlineVideoInvitation :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lint m_reqUserId;
	LMsgC2SOnlineVideoInvitation() :LMsgSC(MSG_C_2_S_ONLINE_VIDEO_INVITATION)
	{
		m_userId = 0;
		m_strUUID = "";
		m_reqUserId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SOnlineVideoInvitation(); }

};


struct LMsgC2SShutDownVideoInvitation :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lint m_reqUserId;

	LMsgC2SShutDownVideoInvitation() :LMsgSC(MSG_C_2_S_SHUTDOWN_VIDEO_INVITATION)
	{
		m_userId = 0;
		m_strUUID = "";
		m_reqUserId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_reqUserId), m_reqUserId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SShutDownVideoInvitation(); }

};

struct LMsgC2SUploadGPSInformation :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lstring m_strGPS;

	LMsgC2SUploadGPSInformation() :LMsgSC(MSG_C_2_S_UPLOAD_GPS_INFORMATION)
	{
		m_userId = 0;
		m_strUUID = "";
		m_strGPS = "";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_strGPS), m_strGPS);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strGPS), m_strGPS);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUploadGPSInformation(); }

};


struct LMsgC2SUploadVideoPermission :public LMsgSC
{
	Lint m_userId;
	Lint m_VideoPermit;

	LMsgC2SUploadVideoPermission() :LMsgSC(MSG_C_2_S_UPLOAD_VIDEO_PERMISSION)
	{
		m_userId = 0;
		m_VideoPermit = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_VideoPermit), m_VideoPermit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_VideoPermit), m_VideoPermit);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUploadVideoPermission(); }

};


struct LMsgS2CUpdateUserVipInfo :public LMsgSC
{
	Lint m_userId;
	Lstring m_strUUID;
	Lstring m_strGPS;
	Lint    m_VideoPermit;

	LMsgS2CUpdateUserVipInfo() :LMsgSC(MSG_S_2_C_UPDATE_USER_VIP_INFO)
	{
		m_userId = 0;
		m_strUUID = "";
		m_strGPS = "";
		m_VideoPermit = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_strGPS), m_strGPS);
		ReadMapData(obj, NAME_TO_STR(m_VideoPermit), m_VideoPermit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strGPS), m_strGPS);
		WriteKeyValue(pack, NAME_TO_STR(m_VideoPermit), m_VideoPermit);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUpdateUserVipInfo(); }
};

struct LMsgS2CBcastMessage :public LMsgSC
{
	std::vector<std::string>		m_MessageList;//消息列表

	LMsgS2CBcastMessage() :LMsgSC(MSG_S_2_C_BCAST_MESSAGE)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_MessageList), m_MessageList);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_MessageList), m_MessageList);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBcastMessage(); }
};



#endif