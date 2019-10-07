#ifndef _L_MSG_H_
#define _L_MSG_H_

#include "LMsgBase.h"
#include "GameDefine.h"
#include "LTool.h"
#include "LLog.h"
#include "LVideo.h"

//新增俱乐部消息
#include "LMsgClub.h"

//poker 棋牌游戏
#include "..\..\MJShanxi2\LogicQipai\LMsgPoker.h"


//服务器端跟客户端通讯的消息号
enum LMSG_ID
{
	MSG_ERROR_MSG = 0,//错误消息

	MSG_C_2_S_PHONE_LOGIN = 60008,//客户端发给服务器
	MSG_S_2_C_PHONE_LOGIN_1 = 60009, //服务器返回结果  验证返回
	MSG_S_2_C_PHONE_LOGIN_2 = 60010, //服务器返回结果  登录返回

	MSG_C_2_S_MSG = 61001,// 1,//客户端发给服务器
	MSG_S_2_C_MSG = 61002, //2,//服务器返回结果

	MSG_S_2_C_KILL_CLIENT = 61003,  //客户端被同一账号挤掉后，通知被挤掉的客户端结束自己

    MSG_C_2_S_LOGIN = 61004,//11, //客户端请求登录
    MSG_S_2_C_LOGIN = 61005, //12, //服务器返回客户登录结果

	MSG_S_2_C_ITEM_INFO = 61006,// 13,  //返回玩家物品信息
	MSG_S_2_C_HORSE_INFO = 61007,// 14,  //跑马灯信息

	MSG_C_2_S_HEART = 61008,// 15, //心跳包
	MSG_S_2_C_HEART = 61009,// 16, //心跳包

	MSG_C_2_S_NOTICE = 61010,// 17, //请求服务器通知
	MSG_S_2_C_NOTICE = 61011,//  18, //服务器返回通知

	//vip场相关消息
	MSG_C_2_S_CREATE_ROOM = 61012, //20,//玩家请求创建房间

	MSG_C_2_S_CREATE_ROOM_FOR_OTHER = 61021,//29,//玩家请求创建房间 房主建房

	MSG_S_2_C_CREATE_ROOM = 61013, //21,//玩家请求创建房间结果

	MSG_C_2_S_ADD_ROOM = 61014, //22,//玩家请求加入房间
	MSG_S_2_C_ADD_ROOM = 61015, //23,//玩家请求加入房间结果

	MSG_C_2_S_LEAVE_ROOM = 61016,// 24,//玩家请求退出房间
	MSG_S_2_C_LEAVE_ROOM = 61017,// 25,//玩家请求退出房间结果

	MSG_C_2_S_RESET_ROOM = 61018,// 26,//申请解散房间
	MSG_S_2_C_RESET_ROOM = 61019,// 27,// 服务器广播申请解散房间结果


	MSG_C_2_S_RESET_ROOM_SELECT = 61020,// 28,//玩家执行申请操作



	MSG_S_2_C_INTO_DESK = 61022,// 30,//服务器通知玩家进入桌子
	MSG_S_2_C_ADD_USER = 61023,// 31,//桌子添加玩家
	MSG_S_2_C_DEL_USER = 61024,// 32,//桌子删除玩家


	MSG_C_2_S_USER_SELECT_SEAT = 61025,// 33,//桌子玩家选择座位
	MSG_S_2_C_USER_SELECT_SEAT = 61026,// 34,//桌子玩家选择座位广播桌子玩家

	MSG_S_2_C_DESK_STATE = 61027,// 35,//发送桌子状态

	MSG_C_2_S_READY = 61028,// 36,//客户端玩家准备
	MSG_S_2_C_READY = 61029,// 37,//服务器广播某个玩家准备

	MSG_C_2_S_START_GAME = 61030,// 38,      //客户端发送游戏开始指令

	MSG_S_2_C_USER_ONLINE_INFO = 61031,//40,//服务器广播玩家在线状态

	MSG_S_2_C_VIP_INFO = 61032,//41,//同步桌子信息

    MSG_C_2_S_COINS_ROOM_LIST = 42, //预留
    MSG_S_2_C_COINS_ROOM_LIST = 43, //预留
	MSG_C_2_S_ENTER_COINS_ROOM = 44, //玩家请求进入金币房间
	MSG_S_2_C_ENTER_COINS_ROOM = 45, //玩家请求创建房间结果

	MSG_S_2_C_HAOZI = 61043,// 49,//耗子牌
	MSG_S_2_C_START = 61033,//50,//服务器发送游戏开始消息
	MSG_S_2_C_GET_CARD = 61034,//51,//服务器发送玩家摸牌，准备出牌的消息

	MSG_C_2_S_PLAY_CARD = 61035,// 52,//客户端请求出牌
	MSG_S_2_C_PLAY_CARD = 61036,// 53,//服务器广播玩家出牌

	MSG_S_2_C_USER_THINK = 61037, //54,//服务器通知玩家思考

	MSG_C_2_S_USER_OPER = 61038, //55,//玩家操作结果
	MSG_S_2_C_USER_OPER = 61039,// 56,//服务器广播玩家操作结果

	MSG_C_2_S_USER_SPEAK = 61040, //57,//玩家请求说话
	MSG_S_2_C_USER_SPEAK = 61041, //58,//服务器广播玩家说话

	MSG_S_2_C_GAME_OVER = 61042, // 60,//服务器通知本场游戏结束

	MSG_C_2_S_USER_OUT_ROOM = 61,//玩家请求退出房间
	MSG_S_2_C_USER_OUT_ROOM = 62,//玩家退出房间结果

	MSG_C_2_S_USER_AIOPER = 63,  //客户端请求自动决策
	MSG_S_2_C_USER_AIOPER = 64,  //服务器自动帮决策/出牌

	MSG_S_2_C_START_HU = 65,//玩家起手胡
	MSG_C_2_S_START_HU_SELECT = 61044,//66,//玩家选择起手胡
	MSG_S_2_C_START_HU_SELECT = 61045,//67,//玩家选择起手胡结果

	MSG_S_2_C_GANG_RESULT = 61046, //68,//玩家杠牌结果

	MSG_S_2_C_END_CARD_SELECT = 61047,// 69,//服务器广播海底牌选择
	MSG_C_2_S_END_SELECT = 61048, //70,//玩家选择结果
	MSG_S_2_C_END_GET = 61049, //71,//服务器广播玩家摸海底牌
	MSG_S_2_C_REMOVE_QIANGGANG_CARD = 61085,// 72,//移除抢杠胡的手牌

	MSG_S_2_C_FANPIGU_BUPAI = 73, //服务器发送翻屁股补牌消息
	MSG_C_2_S_FANPIGU_BUPAI_RESULT = 74, //玩家翻屁股补牌结果消息
	MSG_S_2_C_FANPIGU_BROADCAST = 75, //翻屁股新牌广播

	MSG_S_2_C_VIP_END = 61050,//80,//游戏结束

	MSG_C_2_S_VIP_LOG = 61051,// 90,//玩家请求vip房间对账单
	MSG_S_2_C_VIP_LOG = 61052, //91,//玩家请求vip房间对账单结果

	MSG_C_2_S_VIDEO = 61053,// 92,//玩家请求录像
	MSG_S_2_C_VIDEO = 61054,// 93,//玩家请求录像


	MSG_C_2_S_SHAREVIDEO = 61055, //105,//玩家请求查看某shareid的分享录像
	MSG_S_2_C_SHAREVIDEO = 61056, // 106,//服务器返回某shareid对应的分享录像
	MSG_C_2_S_REQSHAREVIDEO = 61057, //107,//玩家请求分享某录像
	MSG_S_2_C_RETSHAREVIDEOID = 61058,// 108,//服务器返回分享的id
	MSG_C_2_S_GETSHAREVIDEO = 61059, //109,//玩家通过精确的videoid查看分享的某录像

	MSG_S_2_C_USER_CHANGE = 61060,//112,			//通知玩家换牌
	MSG_C_2_S_USER_CHANGE = 61061,// 113,			//玩家回复换牌
	MSG_S_2_C_USER_CHANGERESULT = 61062,// 114,		//玩家换牌结果
	MSG_S_2_C_GAMEREULT = 61097, //115,				//单局结算信息

	MSG_S_2_C_USER_DINGQUE_COMPLETE = 61098, //116,	//所有玩家定缺完毕

	//MSG_C_2_S_ROOM_LOG = 117,//玩家请求vip房间对账单
	//MSG_S_2_C_ROOM_LOG = 118,//玩家请求vip房间对账单结果

	MSG_C_2_S_ROOM_LOG = 61095, //110,//玩家请求vip房间对账单
	MSG_S_2_C_ROOM_LOG = 61096, //111,//玩家请求vip房间对账单结果

	//--------------------------------------------------------------
	MSG_C_2_S_GET_USER_INFO = 61075,//130,		// 请求获取指定玩家的信息
	MSG_S_2_C_USER_INFO = 61076,//131,			// 服务器返回玩家信息

	MSG_C_2_S_GET_INVITING_INFO = 61077,//132,	      // 请求获取邀请信息
	MSG_S_2_C_INVITING_INFO = 61078, //133,		      // 服务器返回邀请信息

	MSG_C_2_S_BIND_INVITER = 61079,// 134,		      // 请求绑定邀请人
	MSG_S_2_C_BIND_INVITER = 61080,// 135,		      // 服务器返回绑定邀请人结果
	MSG_C_2_S_USER_SHARED = 61081,// 136,		// 玩家进行了分享
	MSG_C_2_S_REQ_LUCKY_DRAW_NUM = 61082,// 137,	// 请求玩家抽奖次数
	MSG_S_2_C_LUCKY_DRAW_NUM = 61083, //138,		// 服务器推送玩家抽奖次数

	MSG_C_2_S_TANGCARD = 61084, //139,			// 玩家发起廊起请求消息
	MSG_S_2_C_TANGCARD_RET = 140,		// 服务器发回廊起请求验证结果

	MSG_C_2_S_REQ_CREHIS = 61099,//141,         //玩家请求点赞记录
	MSG_S_2_C_RET_CREHIS = 61100,//142,         //服务器返回客户端 点赞记录

	MSG_C_2_S_REQ_GTU = 143,           //客户端 点赞
	MSG_S_2_C_RET_GTU = 144,           //服务器返回点赞结果

	MSG_C_2_S_REQ_EXCH = 145,          //换金币
	MSG_S_2_C_RET_EXCH = 146,          //结果

	MSG_C_2_S_DESK_LIST = 61101, //147,         //玩家请求创建的房间
	MSG_S_2_C_DESK_LIST = 61102, //148,         //返回玩家请求创建的房间

											

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
	MSG_C_2_S_LOGIN_GATE = 61114,// 170,	//客户端登录Gate
	MSG_S_2_C_LOGIN_GATE = 61115,// 171,	//Gate回客户端登录消息


	MSG_C_2_S_CREATER_RESET_ROOM = 61103, //172,  //房主申请解散房间
	MSG_S_2_C_CREATER_RESET_ROOM = 61104, //173,  //服务器返回房主申请解散房间结果


	MSG_C_2_S_GPS_INFO = 61105,                //客户端请求GPS 图形内容
	MSG_S_2_C_GPS_INFO = 62106,             //返回gps图形内容


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


											//视频命令
	//SKYEYE_IS_VIP_ROOM			    = 61118	905,  //查看是否为vip防作弊房间
	MSG_C_2_S_SEND_VIDEO_INVITATION	        = 61119, // 900, //发起视频邀请

	MSG_C_2_S_RECEIVE_VIDEO_INVITATION	    = 61120,// 901, //收到视频邀请
	MSG_C_2_S_INBUSY_VIDEO_INVITATION	    = 61121,// 903, //视频邀请忙线中
	MSG_C_2_S_ONLINE_VIDEO_INVITATION		= 61122,//	904, //视频已连线
	MSG_C_2_S_SHUTDOWN_VIDEO_INVITATION	    = 61123,//	906, //关闭视频
	MSG_C_2_S_UPLOAD_GPS_INFORMATION		= 61124,//	907, //上传GPS坐标
	MSG_C_2_S_UPLOAD_VIDEO_PERMISSION		= 61125,//	908, //上传视频是否允许
	MSG_S_2_C_UPDATE_USER_VIP_INFO		    = 61126,//   909, //用户的视频是否开启，gps    

	MSG_C_2_S_REQUEST_DESK_STATE=61127,        //新添加消息，客户端请求断线重连数据

	MSG_S_2_C_WARN_POINT_LOW = 61185,		//提醒用户能量值低
	MSG_S_2_C_STAND_PLAYER_SHOW_SEAT = 61186,   //能量不足玩家充上能量后显示入座按钮
	MSG_C_2_S_STAND_PLAYER_RESEAT = 61187,	//能量不足玩家充上能量后从新点击入座

	MSG_S_2_C_BCAST_MESSAGE = 61117,// 888,	//广播客户端消息
	//客户端包头验证头8
	MSG_C_2_S_VERIFY_HEAD = 61116,// 500,

	//////////////////////begin  61130 --61200关于俱乐部, client--><--server   
	//俱乐部的消息转移到  LMsgClub.h 定义
	//MSG_C_2_S_ENTER_CLUB=61130, //301,      // 进入俱乐部
	//MSG_S_2_C_CLUB_SCENE= 61131, // 302,      //俱乐部界面内容
	//
	//MSG_C_2_S_LEAVE_CLUB= 61132, // 303,     //离开俱乐部   
	//MSG_S_2_C_LEAVE_CLUB= 61133, // 304,      //离开俱乐部
	//
	//MSG_C_2_S_SWITCH_PLAY_SCENE= 61134,//  305,   //俱乐部界面切换玩法
	//MSG_S_2_C_SWITCH_PLAY_SCENE= 61135, // 306,     //返回俱乐部界面信息
	//
	//MSG_C_2_S_REQUEST_PERSON_INFO= 61136, // 307,  //获取个人信息
	//MSG_S_2_C_REPLY_PERSON_INFO= 61137, // 308,   //获取个人信息
	//
	//MSG_C_2_S_FRESH_CLUB_DESK_INFO= 61138, //309,   //刷新俱乐部桌子信息
	//
	//MSG_S_2_C_CLUB_DESK_INFO=61139,    //俱乐部桌子信息
	//
	////////////////////////////////////
	//MSG_C_2_S_REQUEST_CLUB_MEMBER = 61140,   //客户端请求俱乐部成员
	//MSG_S_2_C_CLUB_MEMBER_INFO   = 61141,        //服务器返回俱乐部成员信息
	//
	//MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME=61142,  //用户在房间中请求亲友圈可以游戏的用户列表
	//MSG_S_2_C_CLUB_MEMBER_ONLINE_NOGAME = 61143,                   //服务器返回内容
	//
    //MSG_C_2_S_INVITE_CLUB_MENBER=61144,    //邀请俱乐部成员参加游戏
	//MSG_S_2_C_INVITE_CLUB_MEMBER_REPLY=61145,   //返回邀请结果，只有错误状态才返回
	//
	//MSG_S_2_C_TRANSMIT_INVITE_CLUB_MENBER=61146,  //转发用户邀请
	//MSG_C_2_S_REPLY_CLUB_MEMBER_INVITE=61147,        //回复俱乐部成员的邀请
	//MSG_S_2_C_REPLY_CLUB_MEMBER_INVITE_REPLY = 61148,        //回复俱乐部成员的邀请的回复，只有在接收邀请的但失败才会有
	//
	//MSG_S_2_C_NOTIFY_USER_JOIN_CLUB=61149,     //服务器通知用户申请加入俱乐部
	//
	////////////////////////////////////
	//
	//MH_MSG_C_2_S_QUERY_ROOM_GPS_LIMIT=61150, //查询房间的GPS距离限制配置信息
	//MH_MSG_S_2_C_QUERY_ROOM_GPS_LIMIT_RET=61151,   //返回房间的GPS距离限制配置信息
	//
	//MSG_C_2_S_CLUB_MASTER_RESET_ROOM = 61152,   //俱乐部会长申请解散房间
	//MSG_S_2_C_CLUB_MASTER_RESET_ROOM = 61153, //服务器返回申请解散房间结果
	//
	//MSG_C_2_S_CLUB_CREATE_ROOM = 61154,   //用户创建俱乐部房间
	//
	//MSG_C_2_S_QUICK_JOIN_CLUB_DESK = 61155,  //用户快速加入俱乐部房间
	//
	//MSG_S_2_C_CLUB_USER_BCAST_MESSAGE = 61156,  //服务器向俱乐部用户广播消息
	//
	//MSG_C_2_S_REQUEST_CLUB_ONLINE_COUNT=61157,         //客户端请求俱乐部在线人数
	//MSG_S_2_C_CLUB_ONLINE_COUNT = 61158,                           //服务器返回俱乐部在线人数
	//
	//MSG_S_2_C_NOTIFY_CLUBOWNER_MEMBER=61159,       //通知会长俱乐部成员情况
	//
	//MSG_S_2_C_NOTIFY_USER_LEAVE_CLUB=61160,           //通知用户他被退出俱乐部
	//
	//
	//
	//MH_MSG_C_2_S_GAME_RUNTIME_CONFIG = 61161, //玩家请求游戏参数
	//MH_MSG_S_2_C_GAME_RUNTIME_CONFIG = 61162,  // 服务器发送参数结果
	//
	//MSG_C_2_S_CLUB_SET_ADMINISTRTOR = 61163,    //设置俱乐部管理员
	//MSG_S_2_C_CLUB_SET_ADMINSTRTOR=61164,       //设置俱乐部管理员返回
	//
	//MSG_C_2_S_POWER_POINT_LIST=61165,                //能量列表请求
	//MSG_S_2_C_POWER_POINT_LIST=61166,               //能量列表返回
	//
	//MSG_C_2_S_REQUEST_POINT_RECORD = 61167,                //能量查询
	//MSG_S_2_C_REPLY_POINT_RECORD = 61168,               //能量查询返回
	//
	//MSG_C_2_S_CHANGE_POINT = 61169,                //能量改变
	//MSG_S_2_C_CHANGE_POINT = 61170,               //能量改变返回
	//
	//
	//MSG_C_2_S_REQUEST_ADD_CLUB=61171,    //用户申请加入俱乐部
	//MSG_S_2_C_REQUEST_ADD_CLUB=61172,    
	//
	//MSG_C_2_S_REPLY_ADD_CLUB=61173,       //申请加入俱乐部响应
	//MSG_S_2_C_REPLY_ADD_CLUB=61174,      //
	//
	//MSG_C_2_S_DEL_CLUB_USER=61175,      //退出俱乐部
	//MSG_S_2_C_EXIT_CLUB=61176,
	//
	//MSG_C_2_S_CREATE_CLUB=61177,      //创建俱乐部
	//MSG_S_2_C_CREATE_CLUB_61178,
	//
	//MSG_C_2_S_ADD_PLAY_TYPE=61179,  //创建玩法
	//MSG_S_2_C_ADD_PLAY_TYPE=61180,   

		//////////////////////end   61130 --61200 关于俱乐部, client--><--server   

	/////////////////////begin 62001-63000  关于扑克,client--><--server 
	//棋牌新添加的消息号
	MH_MSG_S_2_C_USER_DESK_COMMAND=61200, // 桌子级别总命令码 

	//三张牌  62001-62050
	//MSG_C_2_S_POKER_GAME_MESSAGE=62001,   //扑克游戏  -可以根据子命令区分
	//MSG_S_2_C_POKER_GAME_MESSAGE=62010,  //扑克游戏  -可以根据子命令区分

	//MSG_S_2_C_POKER_GAME_END_RESULT=62002,      //服务器发送扑克游戏大结算命令

	//MSG_C_2_S_POKER_ROOM_LOG=62003,                     //请求战绩记录-match
	//MSG_S_2_C_POKER_ROOM_LOG=62004,                     //服务端返回战绩记录-match
	//
	//MSG_C_2_S_POKER_MATCH_LOG=62005,                  //请求一场比赛每局战绩
	//MSG_S_2_C_POKER_MATCH_LOG=62006,
	//
	//MSG_C_2_S_POKER_RECONNECT=62007,               //断线重连
	//
	//MSG_C_2_S_YINGSANZHANG_ADD_SCORE=62012,                //用户加注
	//
	//MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL = 62013,  //客户端发给服务器玩家买大买小操作结果
	//MSG_YINGSANZHANG_S_2_C_USER_SELECT_BIGSMALL = 62014,  //服务器广播玩家买大买小的操作结果
	//
	//MSG_YINGSANZHANG_S_2_C_CHANGE_BIGSMALL_MODE = 62015,  //服务器改变买大买小模式
	//
	//MSG_YINGSANZHANG_S_2_C_START_ADD_SCORE = 62016,       //服务器广播买大买小结束，通知客户端开始下注
	//
	//
	////推筒子 62050-62070
    //MSG_C_2_S_TUITONGZI_SELECT_ZHUANG=62051,          //选庄
	//MSG_S_2_C_TUITONGZI_SELECT_ZHUANG=62052,       //选庄
	//
	//MSG_C_2_S_TUITONGZI_DO_SHAIZI=62053,       //庄家点色子
	//MSG_S_2_C_TUITONGZI_DO_SHAIZI=62054, 
	//
	//MSG_C_2_S_TUITONGZI_ADD_SCROE=62055,                  //用户选分
	//MSG_S_2_C_TUITONGZI_ADD_SCORE=62056,
	//
	//MSG_S_2_C_TUITONGZI_RECON=62057,   //断线重连
	//
	//MSG_S_2_C_TUITONGZI_DRAW_RESULT=62058,  //小结算
	//
	//MSG_C_2_S_TUITONGZI_OPEN_CARD=62059,  //亮牌
	//MSG_S_2_C_TUITONGZI_OPEN_CARD=62060,
	//
	//MSG_S_2_C_TUITONGZI_START_GAME=62061,
	//
	//MSG_S_2_C_READY_TIME_REMAIN = 62062,  //桌子wait状态倒计时
	//
	//
	///////////////////////////////
	////牛牛  62071-62100
	////MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071,          //选庄
	////MSG_S_2_C_NIUNIU_SELECT_ZHUANG = 62072,
	////
	////MSG_C_2_S_NIUNIU_ADD_SCORE = 62073,           //下注
	////MSG_S_2_C_NIUNIU_ADD_SCORE = 62074,
	////
	////MSG_C_2_S_NIUNIU_OPEN_CARD = 62075,           //亮牌
	////MSG_S_2_C_NIUNIU_OPEN_CARD = 62076,
	////
	////MSG_S_2_C_NIUNIU_START_GAME = 62077,         //游戏开始
	////
	////MSG_S_2_C_NIUNIU_DRAW_RESULT = 62078,        //小结算
	////
	////MSG_S_2_C_NIUNIU_RECON = 62079,              //断线重连
    ////MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG = 62080,              //抢庄通知
	////
	////MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE = 62081,  //明牌下注
	////
	////MSG_C_2_S_NIUNIU_MAI_MA = 62082,			//玩家买码
	////MSG_S_2_C_NIUNIU_MAI_MA_BC = 62083,			//服务器广播玩家买码
	//// 
	////MSG_S_2_C_LOOKON_PLAYER_FULL = 62065,   //服务器广播观战玩家，参与游戏玩家人数已满
	////MSG_C_2_S_ADD_ROOM_SEAT_DOWN = 62066,    //客户端发送观战状态下入座
	////MSG_S_2_C_ADD_ROOM_SEAT_DOWN = 62067,    	//服务器返回观战玩家入座结果
	//
	//
	///////////////////////////////
	////斗地主  62101-62150
	//
	//MSG_C_2_S_JOIN_ROOM_CHECK=62063,    //客户端请求加入的房间
	//MSG_S_2_C_JOIN_ROOM_CHECK =62064,  //if游戏开始，询问是否加入
	//
	//
	//MSG_S_2_C_DOUDIZHU_GAME_MESSAGE_BASE = 62101,  // 斗地主S->C
	//MSG_C_2_S_DOUDIZHU_GAME_MESSAGE_BASE = 62102,  // 斗地主C->S
	//MSG_S_2_C_DOUDIZHU_RECONNECT = 62103,  // 斗地主断线重连
	//
	//MSG_S_2_C_DOUDIZHU_JOIN_CLUB_ROOM_ANONYMOUS=62104,  //俱乐部匿名加入房间
	//
	////回放信息
	//MSG_C_2_S_POKER_REQUEST_VIDEO_ID=62105,          //客户端请求回放
	//MSG_S_2_C_POKER_RESPOND_VIDEO_ID=62106,         //服务器返回回放内容
	//
	//MSG_C_2_S_REQUEST_USER_GPS = 62107,                     //客户端申请房间其他用户gps
	//MSG_S_2_C_REQUEST_USER_GPS = 62108,                     //服务器返回房间其他用户gps
	//
	////俱乐部匹配  3人匹配
	//MSG_S_2_C_POKER_WAIT_JOIN_ROOM = 62222,     //3人斗地主
	//MSG_C_2_S_POKER_EXIT_WAIT = 62223,              //退出
	//MSG_S_2_C_POKER_EXIT_WAIT = 62224,             //返回退出结果
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////
	////十点半消息号  62151-6199
	//
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////
	////双升消息号  62200-62249
	//
	////S->C 服务器给每个玩家发牌
	//MSG_S_2_C_SHUANGSHENG_SEND_CARDS = 62200,
	//
	////S->C 服务器通知玩家报主
	//MSG_S_2_C_SHUANGSHENG_NOTICE_SELECT_ZHU = 62201,
	//
	////C->S 玩家报主操作
	//MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202,
	//
	////S->C 服务器通知玩家反主
	//MSG_S_2_C_SHUANGSHENG_NOTICE_FAN_ZHU = 62203,
	//
	////C->S 玩家反主操作
	//MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204,
	//
	////S->C 服务器通知玩家拿底牌,当前庄家，主花色等信息
	//MSG_S_2_C_SHUANGSHENG_NOTICE_BASE_INFO = 62205,
	//
	////C->S 玩家盖底牌操作
	//MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206,
	//
	////S->C 服务器广播游戏开始，开始出牌
	//MSG_S_2_C_SHUANGSHENG_BC_GAME_STARE = 62207,
	//
	////C->S 玩家出牌操作
	//MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208,
	//
	////S->C 服务器广播玩家出牌结果
	//MSG_S_2_C_SHUANGSHENG_BC_OUT_CARDS_RESULT = 62209,
	//
	////S->C 服务器广播小结算
	//MSG_S_2_C_SHUANGSHENG_BC_DRAW_RESULT = 62210,
	//
	////S->C 服务器发送断线重连消息
	//MSG_S_2_C_SHUANGSHENG_RECON = 62211,
	//
	////S->C 服务器发送第一局的开局消息（设置防作弊的消息等）
	//MSG_S_2_C_SHUANGSHENG_FIRST_ROUND = 62212,
	//
	////S->C 服务器广播玩家叫主叫主结果（玩家每次叫主广播一下）
	//MSG_S_2_C_SHUANGSHENG_BC_SELECT_ZHU = 62213,
	//
	////S->C 服务器广播玩家埋底结束
	//MSG_S_2_C_SHUANGSHENG_BC_BASE_OVER = 62216,
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////
	////3打2消息号  62250-62299
	//
	////S->C 服务器给每个玩家发牌
	//MSG_S_2_C_SANDAER_SEND_CARDS = 62250,
	//
	////S->C 服务器依次次给玩家推送叫分消息(待定)
	//MSG_S_2_C_SANDAER_SELECT_SCORE = 62251,
	//
	////S->C 服务器广播玩家叫分结果
	//MSG_S_2_C_SANDAER_SELECT_SCORE_R = 62252,
	//
	////C->S 玩家选择叫分
	//MSG_C_2_S_SANDAER_SELECT_SCORE = 62253,
	//
	////S->C 服务器通知玩家拿底牌，并选主花色
	//MSG_S_2_C_SANDAER_BASE_CARD_AND_SELECT_MAIN_N = 62254,
	//
	////C->S 玩家选主牌
	//MSG_C_2_S_SANDAER_SELECT_MAIN = 62255,
	//
	////S->C 服务器返回玩家选主牌结果
	//MSG_S_2_C_SANDAER_SELECT_MAIN_R = 62256,
	//
	////C->S 玩家埋底
	//MSG_C_2_S_SANDAER_BASE_CARD = 62257,
	//
	////S->C 服务器返回玩家埋底结果
	//MSG_S_2_C_SANDAER_BASE_CARD_R = 62258,
	//
	////C->S 玩家选对家牌
	//MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259,
	//
	////S->C 服务器返回庄家选对家的牌
	//MSG_S_2_C_SANDAER_SELECT_FRIEND_BC = 62260,
	//
	////C->S 玩家出牌
	//MSG_C_2_S_SANDAER_OUT_CARD = 62261,
	//
	////S->C 服务器广播出牌结果
	//MSG_S_2_C_SANDAER_OUT_CARD_BC = 62262,
	//
	////S->C 服务器发送断线重连
	//MSG_S_2_C_SANDAER_RECON = 62263,
	//
	////S->C 服务发送小结算
	//MSG_S_2_C_SANDAER_DRAW_RESULT_BC = 62264,
	//
	////S->C 服务发送闲家得分满105分,是否停止游戏
	//MSG_S_2_C_SANDAER_SCORE_105 = 62265,
	//
	////C->S 玩家回复闲家得分满105分,是否停止游戏
	//MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	//
	////C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	//MSG_C_2_S_SANDAER_SCORE_105_RESULT = 62267,
	//
	////S->C 服务器提示庄家可以摊牌
	//MSG_S_2_C_SANDAER_ZHUANG_CAN_TAN_PAI = 62270,
	//
	////C-S 庄家选择是否摊牌
	//MSG_C_2_S_SANDAER_ZHUANG_SELECT_TAN_PAI = 62271,
	//
	////C-S 庄家选择摊牌结果
	//MSG_S_2_C_SANDAER_TAN_PAI_RET = 62272,
	//
	//MSG_C_2_S_USER_GET_BASECARDS = 62268,		// 庄家请求底牌 原始：61027
	//MSG_S_2_C_USER_GET_BASECARDS_RET = 62269,	// 返回底牌     原始：61028
	//
	////C->S 客户端选择交牌
	//MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272,
	//
	////S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
	//MSG_S_2_C_SANDAER_ZHUANG_JIAO_PAI_BC = 62273,
	//
	////C->S 非庄家客户端选择庄家交牌的申请
	//MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274,
	//
	////S->C 庄家交牌的最终结果
	//MSG_S_2_C_SANDAER_JIAO_PAI_RESULT = 62275,
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////
	////3打1消息号  62300-62349
	//
	////S->C 服务器给每个玩家发牌
	//MSG_S_2_C_SANDAYI_SEND_CARDS = 62300,
	//
	////S->C 服务器依次次给玩家推送叫分消息(待定)
	//MSG_S_2_C_SANDAYI_SELECT_SCORE = 62301,
	//
	////S->C 服务器广播玩家叫分结果
	//MSG_S_2_C_SANDAYI_SELECT_SCORE_R = 62302,
	//
	////C->S 玩家选择叫分
	//MSG_C_2_S_SANDAYI_SELECT_SCORE = 62303,
	//
	////S->C 服务器通知玩家拿底牌，并选主花色
	//MSG_S_2_C_SANDAYI_BASE_CARD_AND_SELECT_MAIN_N = 62304,
	//
	////C->S 玩家选主牌
	//MSG_C_2_S_SANDAYI_SELECT_MAIN = 62305,
	//
	////S->C 服务器返回玩家选主牌结果
	//MSG_S_2_C_SANDAYI_SELECT_MAIN_R = 62306,
	//
	////C->S 玩家埋底
	//MSG_C_2_S_SANDAYI_BASE_CARD = 62307,
	//
	////S->C 服务器返回玩家埋底结果
	//MSG_S_2_C_SANDAYI_BASE_CARD_R = 62308,
	//
	////C->S 玩家出牌
	//MSG_C_2_S_SANDAYI_OUT_CARD = 62309,
	//
	////S->C 服务器广播出牌结果
	//MSG_S_2_C_SANDAYI_OUT_CARD_BC = 62310,
	//
	////S->C 服务器发送断线重连
	//MSG_S_2_C_SANDAYI_RECON = 62311,
	//
	////S->C 服务发送小结算
	//MSG_S_2_C_SANDAYI_DRAW_RESULT_BC = 62312,
	//
	////S->C 服务发送闲家得分满105分,是否停止游戏
	//MSG_S_2_C_SANDAYI_SCORE_105 = 62313,
	//
	////C->S 玩家回复闲家得分满105分,是否停止游戏
	//MSG_C_2_S_SANDAYI_SCORE_105_RET = 62314,
	//
	////C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	//MSG_C_2_S_SANDAYI_SCORE_105_RESULT = 62315,
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////
	////五人百分消息号  62350-62399
	//
	////S->C 服务器给每个玩家发牌
	//MSG_S_2_C_WURENBAIFEN_SEND_CARDS = 62350,
	//
	////S->C 服务器依次次给玩家推送叫分消息(待定)
	//MSG_S_2_C_WURENBAIFEN_SELECT_SCORE = 62351,
	//
	////S->C 服务器广播玩家叫分结果
	//MSG_S_2_C_WURENBAIFEN_SELECT_SCORE_R = 62352,
	//
	////C->S 玩家选择叫分
	//MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353,
	//
	////S->C 服务器通知玩家拿底牌，并选主花色
	//MSG_S_2_C_WURENBAIFEN_BASE_CARD_AND_SELECT_MAIN_N = 62354,
	//
	////C->S 玩家选主牌
	//MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355,
	//
	////S->C 服务器返回玩家选主牌结果
	//MSG_S_2_C_WURENBAIFEN_SELECT_MAIN_R = 62356,
	//
	////C->S 玩家埋底
	//MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357,
	//
	////S->C 服务器返回玩家埋底结果
	//MSG_S_2_C_WURENBAIFEN_BASE_CARD_R = 62358,
	//
	////C->S 玩家选对家牌
	//MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359,
	//
	////S->C 服务器返回庄家选对家的牌
	//MSG_S_2_C_WURENBAIFEN_SELECT_FRIEND_BC = 62360,
	//
	////C->S 玩家出牌
	//MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361,
	//
	////S->C 服务器广播出牌结果
	//MSG_S_2_C_WURENBAIFEN_OUT_CARD_BC = 62362,
	//
	////S->C 服务器发送断线重连
	//MSG_S_2_C_WURENBAIFEN_RECON = 62363,
	//
	////S->C 服务发送小结算
	//MSG_S_2_C_WURENBAIFEN_DRAW_RESULT_BC = 62364,
	//
	////S->C 服务发送闲家得分满105分,是否停止游戏
	//MSG_S_2_C_WURENBAIFEN_SCORE_105 = 62365,
	//
	////C->S 玩家回复闲家得分满105分,是否停止游戏
	//MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
	//
	////C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	//MSG_C_2_S_WURENBAIFEN_SCORE_105_RESULT = 62367,
	//
	////C->S 客户端选择交牌
	//MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368,
	//
	////S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
	//MSG_S_2_C_WURENBAIFEN_ZHUANG_JIAO_PAI_BC = 62369,
	//
	////C->S 非庄家客户端选择庄家交牌的申请
	//MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370,
	//
	////S->C 庄家交牌的最终结果
	//MSG_S_2_C_WURENBAIFEN_JIAO_PAI_RESULT = 62371,



	/////////////////////end 62000-63000  关于扑克,client--><--server 


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

	MH_MSG_CE_2_L_UPDATE_GATE_IP = 6413, // Center发送过来的更改gate server的对外IP

	//俱乐部 center->manager
	MSG_CE_2_LMG_ADD_CLUB=6414,	  //添加俱乐部

	MSG_CE_2_LMG_CLUB_ADD_PLAYTYPE=6415,	  //添加俱乐部玩法数据

	MSG_CE_2_LMG_CLUB_USER_INFO=6416,	  //添加俱乐部玩家

	MSG_CE_2_LMG_CLUB_DEL_USER=6417,	  //删除俱乐部玩家

	MSG_CE_2_LMG_CLUB_HIDE_PLAYTYPE=6418,	  //隐藏俱乐部玩法

	MSG_CE_2_LMG_CLUB_MIDIFY_CLUB_NAME=6419,	  //修改俱乐部名字

	MSG_CE_2_LMG_CLUB=6420,	  //更新俱乐部数据

	MSG_LMG_2_CE_UPDATE_DATA=6421,    //manager 发送信息到center更新

	MSG_CE_2_LMG_CLUB_MODIFY_CLUB_FEE = 6422,   //更改俱乐部支付方式


	MSG_CE_2_LMG_DISMISS_DESK=6423,        //url命令解散房间

	MSG_CE_2_LMG_DEL_USER=6424,             //从manager删除用户记录

	MSG_L_2_CE_CLUB_FIRST_RED_PACKET=6425,  //俱乐部开局红包 logic-manager-center

	MSG_CE_2_LMG_CONTROL_MSG = 6426,                  //center 发生控制信息到manager

	MSG_CE_2_LMG_SQL_INFO=6427,                      //manager 发送sql信息到center

	MSG_CE_2_LMG_UPDATE_POINT_RECORD=6428,   //center->manager 能量记录

	MSG_LMG_2_CE_POINT_RECORD_LOG=6429,    //查询能量值
	MSG_CE_2_LMG_POINT_RECORD_LOG=6430,

	MSG_CE_2_LMG_UPDATE_CLUB=6431,      //更新俱乐部
	MSG_CE_2_LMG_UPDATE_PLAY=6432,      //更新玩法
	



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
	MSG_LMG_2_L_CREATE_DESK_FOR_CLUB=11014,	//创建俱乐部桌子, 这个是批量建桌子，暂时不用了，俱乐部
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
	MSG_L_2_LMG_FRESH_DESK_INFO=11033,                     //发送manager刷新桌子信息,主要局数

	MSG_L_2_LMG_SYNCHRO_DATA=11034,    //logic 和manager断线重连后，向manager同步用户和桌子信息
	MSG_LMG_2_L_G_REQUEST_SYNCHRO_DATA=11035,   //manager 向gate 和logic 请求同步信息
	
	MSG_LMG_2_L_DISSMISS_CLUB_DESK=11036,        //manager 向logic请求解散桌子

	MSG_LMG_2_L_API_DISMISS_ROOM=11037,          //API命令解散房间

	MH_MSG_L_2_LMG_NEW_AGENCY_ACTIVITY_UPDATE_PLAY_COUNT = 11040, // 新代理满场活动更新总场次
	MH_MSG_LMG_2_L_QUERY_ROOM_GPS_INFO = 11041,                   // 查询房间的GPS配置信息

	MSG_L_2_LMG_USER_LIST_LEAVE_CLUB_DESK = 11042,           //用户离开俱乐部桌子成功-多个用户

	MSG_LMG_2_L_SPEC_ACTIVE = 11043,                           //特殊活动

	MSG_LMG_2_L_UPDATE_POINT_COINTS = 11044,			//manager更新logic上玩家的能量值

	MSG_LMG_2_L_UPDATE_MANAGER_LIST = 11045,			//manager更新desk上的m_managerList列表

//LogicManager 与 Gate
	MSG_G_2_LMG_LOGIN = 12001,			//逻辑管理登陆
	MSG_LMG_2_G_SYNC_LOGIC = 12010,		//同步逻辑信息
	MSG_LMG_2_G_HORSE_INFO = 12011,		//跑马灯

	MSG_G_2_LMG_SYNCHRO_DATA = 12012,   //gate 和manager断线重连后，向manager同步用户信息
	


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

  MSG_L_2_LDB_ROOM_LOG = 15014,//logic 请求room vip战绩
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

   MSG_LDB_2_LM_POKER_LOG=10530,       //logic db 发送poker查询战绩结果

    MSG_LDB_2_L_POKER_VIDEO= 10531,                    //db -  manager 扑克战绩
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

//客户端因为端口 或 注册ID冲突，结束客户端消息
#define MSG_KILL_CLIENT 0x7FFFFFFC
struct LMsgKillClient :public LMsg
{
	LMsgKillClient() : LMsg(MSG_KILL_CLIENT) 
	{}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgKillClient();
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
		m_iMd5End = -1;
		m_lMsgOrder = 0xFFFFFFFF;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kUserId", m_strUserId);
		ReadMapData(obj, "kStart", m_iMd5Begin);
		ReadMapData(obj, "kEnd", m_iMd5End);
		ReadMapData(obj, "kPwd", m_strMd5);
		ReadMapData(obj, "kAuthKey", m_strVerifyKey);
		ReadMapData(obj, "kSequence", m_lMsgOrder);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kUserId", m_strUserId);
		WriteKeyValue(pack, "kStart", m_iMd5Begin);
		WriteKeyValue(pack, "kEnd", m_iMd5End);
		WriteKeyValue(pack, "kPwd", m_strMd5);
		WriteKeyValue(pack, "kAuthKey", m_strVerifyKey);
		WriteKeyValue(pack, "kSequence", m_lMsgOrder);

		return true;
	}
	/*
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
	WriteKeyValue(pack, "kMId", m_msgId);
	WriteKeyValue(pack, "m_strUserId", m_strUserId);
	WriteKeyValue(pack, "m_iMd5Begin", m_iMd5Begin);
	WriteKeyValue(pack, "m_iMd5End", m_iMd5End);
	WriteKeyValue(pack, "m_strMd5", m_strMd5);
	WriteKeyValue(pack, "m_strVerifyKey", m_strVerifyKey);
	WriteKeyValue(pack, "m_lMsgOrder", m_lMsgOrder);

	return true;
	}
	*/
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
	Lint       m_UserId;
	Lstring   m_phoneUUID;
	Lint         m_appVersion;
	Lint       m_UserIdSys;
	Lstring  m_device;
	Lstring			m_uuidOfzy;
	Lstring			m_openIdOfzy;


	LMsgC2SMsg() :LMsgSC(MSG_C_2_S_MSG)
	{
		m_severID = 0;
		m_sex = 1;
		m_UserId = 0;
		m_appVersion = 0;
		m_UserIdSys = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kOpenId", m_openId);
		ReadMapData(obj, "kNike", m_nike);
		ReadMapData(obj, "kSign", m_sign);
		ReadMapData(obj, "kPlate", m_plate);
		ReadMapData(obj, "kAccessToken", m_accessToken);
		ReadMapData(obj, "kRefreshToken", m_refreshToken);
		ReadMapData(obj, "kMd5", m_md5);
		ReadMapData(obj, "kSeverID", m_severID);
		ReadMapData(obj, "kUuid", m_uuid);
		ReadMapData(obj, "kSex", m_sex);
		ReadMapData(obj, "kImageUrl", m_imageUrl);
		ReadMapData(obj, "kNikename", m_nikename);
		ReadMapData(obj, "kUserId", m_UserId);
		ReadMapData(obj, "kUserIdSys", m_UserIdSys);
		ReadMapData(obj, "kPhoneUUID", m_phoneUUID);
		ReadMapData(obj, "kAppVersion", m_appVersion);
		ReadMapData(obj, "kDevice", m_device);
		ReadMapData(obj, "kUuidOfzy", m_uuidOfzy);
		ReadMapData(obj, "kOpenIdOfzy", m_openIdOfzy);
		/*
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
		*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,20);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kOpenId", m_openId);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kSign", m_sign);
		WriteKeyValue(pack, "kPlate", m_plate);
		WriteKeyValue(pack, "kAccessToken", m_accessToken);
		WriteKeyValue(pack, "kRefreshToken", m_refreshToken);
		WriteKeyValue(pack, "kMd5", m_md5);
		WriteKeyValue(pack, "kSeverID", m_severID);
		WriteKeyValue(pack, "kUuid", m_uuid);
		WriteKeyValue(pack, "kSex", m_sex);
		WriteKeyValue(pack, "kImageUrl", m_imageUrl);
		WriteKeyValue(pack, "kNikename", m_nikename);
		WriteKeyValue(pack, "kUserId", m_UserId);
		WriteKeyValue(pack, "kUserIdSys", m_UserIdSys);
		WriteKeyValue(pack, "kPhoneUUID", m_phoneUUID);
		WriteKeyValue(pack, "kAppVersion", m_appVersion);
		WriteKeyValue(pack, "kDevice", m_device);
		WriteKeyValue(pack, "kUuidOfzy", m_uuidOfzy);
		WriteKeyValue(pack, "kOpenIdOfzy", m_openIdOfzy);
		/*
		WriteKeyValue(pack, "kMId", m_msgId);
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
		*/
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
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kSeed", m_seed);
		WriteKeyValue(pack, "kId", m_id);
		WriteKeyValue(pack, "kGateIp", m_gateIp);
		WriteKeyValue(pack, "kGatePort", m_gatePort);
		WriteKeyValue(pack, "kErrorMsg", m_errorMsg);
		WriteKeyValue(pack, "kTotalPlayNum", m_totalPlayNum);
		WriteKeyValue(pack, "kTotalCircleNum", m_TotalCircleNum);
		WriteKeyValue(pack, "kTotalWinNum", m_totalWinNum);
		/*
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_seed", m_seed);
		WriteKeyValue(pack, "m_id", m_id);
		WriteKeyValue(pack, "m_gateIp", m_gateIp);
		WriteKeyValue(pack, "m_gatePort", m_gatePort);
		WriteKeyValue(pack, "m_errorMsg", m_errorMsg);
		WriteKeyValue(pack, "m_totalPlayNum",m_totalPlayNum);
		WriteKeyValue(pack, "m_TotalCircleNum", m_TotalCircleNum);
		WriteKeyValue(pack, "m_totalWinNum", m_totalWinNum);
		*/
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CMsg();
	}
};

//客户端被同一个账号挤掉后，通知远来客户端结束自己
struct LMsgS2CKillClient : public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-服务器还没启动成功 2-微信登陆失败 3- 微信返回失败 4-创建角色失败 5- 在原APP未退,6-错误的地区号,7-MD5验证失败,9-账号被封

	LMsgS2CKillClient() : LMsgSC(MSG_S_2_C_KILL_CLIENT)
	{
		m_errorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CKillClient();
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
		ReadMapData(obj, "kStrUserUUID", m_strUserUUID);

		/*ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kStrUserUUID", m_strUserUUID);
		/*
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);*/
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
		ReadMapData(obj, "kStrUserUUID", m_strUserUUID);
		ReadMapData(obj, "kStrKey", m_strKey);
		ReadMapData(obj, "kUMsgOrder", m_uMsgOrder);
		/*
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_strKey), m_strKey);
		ReadMapData(obj, NAME_TO_STR(m_uMsgOrder), m_uMsgOrder);*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kStrUserUUID", m_strUserUUID);
		WriteKeyValue(pack, "kStrKey", m_strKey);
		WriteKeyValue(pack, "kUMsgOrder", m_uMsgOrder);
		/*
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, "m_strKey", m_strKey);
		WriteKeyValue(pack, "m_uMsgOrder", m_uMsgOrder);
		*/
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
		ReadMapData(obj, "kUserId", m_userId);
		ReadMapData(obj, "kDeskId", m_deskId);
		ReadMapData(obj, "kStrUserUUID", m_strUserUUID);
		/*
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
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
	Lint m_errorCode;              //  0--成功    1--房间号不存在   2--房间已经开始游戏  10--服务器2小时解散房间
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
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kDeskId", m_deskId);
		WriteKeyValue(pack, "kStrErrorDes", m_strErrorDes);
		/*
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_deskId", m_deskId);
		WriteKeyValue(pack, "m_strErrorDes", m_strErrorDes);*/
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
		ReadMapData(obj, "kStrUserUUID", m_strUserUUID);
		/*
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);*/
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
	Lint                            m_GpsLimit;  // 0 or 1
	Lint                            m_tuoguan_mode;   //托管

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
		m_GpsLimit = 0;
		m_tuoguan_mode = 0;
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
		ReadMapData(obj, NAME_TO_STR(m_GpsLimit), m_GpsLimit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 16);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kCreatTime", m_creatTime);
		WriteKeyValue(pack, "kDeskId", m_deskId);
		WriteKeyValue(pack, "kShare", m_share);
		WriteKeyValue(pack, "kDissmiss", m_dissmiss);

		WriteKeyValue(pack, "kCheatAgainst", m_cheatAgainst);
		WriteKeyValue(pack, "kFeeType", m_feeType);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kCellscore", m_cellscore);

		WriteKeyValue(pack, "kPlaytype", m_playtype);
		WriteKeyValue(pack, "kDeskPlayerName", m_deskPlayerName);
		WriteKeyValue(pack, "kGreater2CanStart", m_Greater2CanStart);
		WriteKeyValue(pack, "kGpsLimit", m_GpsLimit);
		WriteKeyValue(pack, "kTuoGuanMode", m_tuoguan_mode);
		/*
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
		*/
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
		WriteKeyValue(pack,"kMId", m_msgId);
		WriteKeyValue(pack,"kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kTotalAround", m_totalAround);
		WriteKeyValue(pack, "kWinAround", m_winAround);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_totalAround), m_totalAround);
		WriteKeyValue(pack, NAME_TO_STR(m_winAround), m_winAround);*/
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
		WriteKeyValue(pack,"kMId", m_msgId);
		WriteKeyValue(pack, "kTime", m_time);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		*/
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

		ReadMapData(obj, "kSeed", m_seed);
		ReadMapData(obj, "kId", m_id);
		ReadMapData(obj, "kMd5", m_md5);
		/*
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);*/
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
	Lint		m_card2;//新手奖励房卡2
	Lint		m_card3;//新手奖励房卡3
	Lint		m_state;//0-进入大厅，1-进入桌子
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
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kId", m_id);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kExp", m_exp);
		WriteKeyValue(pack, "kSex", m_sex);
		WriteKeyValue(pack, "kFace", m_face);
		WriteKeyValue(pack, "kProvice", m_provice);
		WriteKeyValue(pack, "kCity", m_city);
		WriteKeyValue(pack, "kIp", m_ip);
		WriteKeyValue(pack, "kNew", m_new);
		WriteKeyValue(pack, "kCard1", m_card1);
		WriteKeyValue(pack, "kCard2", m_card2);
		WriteKeyValue(pack, "kCard3", m_card3);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kBuyInfo", m_buyInfo);
		WriteKeyValue(pack, "kHide", m_hide);
		WriteKeyValue(pack, "kGm", m_gm);
		WriteKeyValue(pack, "kOldUser", m_oldUser);
		WriteKeyValue(pack, "kLoginInterval", m_loginInterval);
		WriteKeyValue(pack, "kClubOnlineUserCount", m_clubOnlineUserCount);
		/*
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
		WriteKeyValue(pack, NAME_TO_STR(m_loginInterval), m_loginInterval); */
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
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kCard1", m_card1);
		WriteKeyValue(pack, "kCard2", m_card2);
		WriteKeyValue(pack, "kCard3", m_card3);
		WriteKeyValue(pack, "kCoins", m_coins);
		WriteKeyValue(pack, "kCredit", m_credit);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credit), m_credit);
		*/
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
struct LMsgS2CIntoDesk :public LMsgSC
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
	Lint            m_Greater2CanStart;
	Lint            m_StartGameButtonPos;
	Lint            m_gamePlayerCount;
	Lint            m_startButtonAppear;
	Lint            m_clubId;
	Lint            m_playTypeId;
	Lint            m_clubDeskId;
	Lstring			m_clubName;
	Lint            m_GpsLimit;
	Lint            m_dynamicJoin;					// 动态加入
	Lstring         m_nike;							//玩家昵称
	Lint			m_isLookOn;						//玩家是否处于旁观状态 0：非旁观状态，1：旁观状态
	Lint			m_deskUserStatus[10];			//桌子上座位的是否入座状态， 0：为入座，1：已入座
	Lint			m_nextDrawLimitTime;			//下一局倒计时时间
	Lint			m_allowLookOn;					//是否允许旁观
	Lint			m_clubOwerLookOn;				// 俱乐部会长明牌观战
	Lint			m_isClubOwerLooking;			//俱乐部会长是否在明牌观战中  0:不在明牌观战中  1:会长在明牌观战中	
	Lint			m_curBoBoScore;					//扯炫当前总钵钵分

	Lint			m_isCoinsGame;					// 是否为金币场  0：普通场  1：金币场
	Lint			m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	Lint			m_qiangZhuangMinCoins;			// 金币场参与抢庄的最小能量值
	Lint			m_isAllowCoinsNegative;			// 每局结算时是否允许负数能量值出现
	Lint         m_myRole;                                    // 0: 普通玩家 1:俱乐部桌子管理员

	Lint			m_warnScore;                    //预警分数
	Lint			m_isMedal;						//玩家是否有勇士勋章

	LMsgS2CIntoDesk() :LMsgSC(MSG_S_2_C_INTO_DESK)
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
		m_Greater2CanStart = 0;
		m_StartGameButtonPos = INVAILD_POS;
		m_gamePlayerCount = 4;
		m_startButtonAppear = 0;
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		m_GpsLimit = 0;
		m_dynamicJoin = 0;
		m_nike = "";
		m_isLookOn = 0;  //默认出去非旁观状态
		memset(m_deskUserStatus, 0, sizeof(m_deskUserStatus));
		m_nextDrawLimitTime = 0;
		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_isClubOwerLooking = 0;
		m_curBoBoScore = 0;

		m_isCoinsGame = 0;					// 是否为金币场  0：普通场  1：金币场
		m_inDeskMinCoins = 0;				// 金币场入座所需的最小能量值
		m_qiangZhuangMinCoins = 0;			// 金币场参与抢庄的最小能量值
		m_isAllowCoinsNegative = 0;			// 每局结算时是否允许负数能量值出现
		m_warnScore = 0;
		m_myRole = 0;
		m_isMedal = 0;
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
		ReadMapData(obj, NAME_TO_STR(m_clubName), m_clubName);
		ReadMapData(obj, NAME_TO_STR(m_playTypeId), m_playTypeId);
		ReadMapData(obj, NAME_TO_STR(m_GpsLimit), m_GpsLimit);
		ReadMapData(obj, NAME_TO_STR(m_dynamicJoin), m_dynamicJoin);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(kAllowLookOn), m_allowLookOn);
		ReadMapData(obj, NAME_TO_STR(kClubOwerLookOn), m_clubOwerLookOn);
		ReadMapData(obj, NAME_TO_STR(kIsClubOwerLooking), m_isClubOwerLooking);

		ReadMapData(obj, NAME_TO_STR(kIsCoinsGame), m_isCoinsGame);
		ReadMapData(obj, NAME_TO_STR(kInDeskMinCoins), m_inDeskMinCoins);
		ReadMapData(obj, NAME_TO_STR(kQiangZhuangMinCoins), m_qiangZhuangMinCoins);
		ReadMapData(obj, NAME_TO_STR(kIsAllowCoinsNegative), m_isAllowCoinsNegative);
		ReadMapData(obj, NAME_TO_STR(kWarn), m_warnScore);
		ReadMapData(obj, NAME_TO_STR(kMyRole), m_myRole);
		ReadMapData(obj, NAME_TO_STR(kIsMedal), m_isMedal);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, 47);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kDeskId", m_deskId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kReady", m_ready);
		WriteKeyValue(pack, "kScore", m_score);
		WriteKeyValue(pack, "kCoins", m_coins);
		WriteKeyValue(pack, "kCredits", m_credits);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kMaxCircle", m_maxCircle);
		WriteKeyValue(pack, "kPlaytype", m_playtype);
		WriteKeyValue(pack, "kChangeOutTime", m_changeOutTime);
		WriteKeyValue(pack, "kOpOutTime", m_opOutTime);
		WriteKeyValue(pack, "kBaseScore", m_baseScore);
		WriteKeyValue(pack, "kCellscore", m_cellscore);
		WriteKeyValue(pack, "kCreateUserId", m_createUserId);
		WriteKeyValue(pack, "kDeskCreatedType", m_deskCreatedType);

		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kFeeType", m_feeType);
		WriteKeyValue(pack, "kCheatAgainst", m_cheatAgainst);
		WriteKeyValue(pack, "kDeskType", m_deskType);


		WriteKeyValue(pack, "kUserIp", m_userIp);
		WriteKeyValue(pack, "kUserGps", m_userGps);
		WriteKeyValue(pack, "kGreater2CanStart", m_Greater2CanStart);
		WriteKeyValue(pack, "kStartGameButtonPos", m_StartGameButtonPos);
		WriteKeyValue(pack, "kGamePlayerCount", m_gamePlayerCount);
		WriteKeyValue(pack, "kStartButtonAppear", m_startButtonAppear);
		WriteKeyValue(pack, "kClubId", m_clubId);
		WriteKeyValue(pack, "kPlayTypeId", m_playTypeId);
		WriteKeyValue(pack, "kClubDeskId", m_clubDeskId);
		WriteKeyValue(pack, "kClubName", m_clubName);
		WriteKeyValue(pack, "kGpsLimit", m_GpsLimit);
		WriteKeyValue(pack, "kDynamicJoin", m_dynamicJoin);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kIsLookOn", m_isLookOn);
		WriteKeyValue(pack, "kClock", m_nextDrawLimitTime);
		WriteKeyValue(pack, "kAllowLookOn", m_allowLookOn);
		WriteKeyValue(pack, "kClubOwerLookOn", m_clubOwerLookOn);
		WriteKeyValue(pack, "kIsClubOwerLooking", m_isClubOwerLooking);
		
		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kDeskUserStatus");
		for (int i = 0; i < 10; ++i)
		{
			veci.push_back(m_deskUserStatus[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, "kCurBoBoScore", m_curBoBoScore);
		WriteKeyValue(pack, NAME_TO_STR(kIsCoinsGame), m_isCoinsGame);
		WriteKeyValue(pack, NAME_TO_STR(kInDeskMinCoins), m_inDeskMinCoins);
		WriteKeyValue(pack, NAME_TO_STR(kQiangZhuangMinCoins), m_qiangZhuangMinCoins);
		WriteKeyValue(pack, NAME_TO_STR(kIsAllowCoinsNegative), m_isAllowCoinsNegative);
		WriteKeyValue(pack, NAME_TO_STR(kWarn), m_warnScore);
		WriteKeyValue(pack, NAME_TO_STR(kMyRole), m_myRole);
		WriteKeyValue(pack, NAME_TO_STR(kIsMedal), m_isMedal);

		/*
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
		WriteKeyValue(pack, NAME_TO_STR(m_startButtonAppear), m_startButtonAppear);*/

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CIntoDesk(); }
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
	Lint            m_dynamicJoin;
	Lint			m_allowLookOn;		//是否允许观战
	Lint			m_clubOwerLookOn;	//俱乐部会长允许明牌观战
	Lint			m_isClubOwerLooking;	//俱乐部会长是否在明牌观战中  0:不在明牌观战中  1:会长在明牌观战中
	Lint			m_curBoBoScore;			//侧旋当前钵钵分
	Lint			m_isCoinsGame;					// 是否为金币场  0：普通场  1：金币场
	Lint			m_inDeskMinCoins;				// 金币场入座所需的最小能量值
	Lint			m_qiangZhuangMinCoins;			// 金币场参与抢庄的最小能量值
	Lint			m_isAllowCoinsNegative;			// 每局结算时是否允许负数能量值出现
	Lint			m_isMedal;						// 玩家是否有勇士勋章


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
		m_dynamicJoin=0;

		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_isClubOwerLooking = 0;
		m_curBoBoScore = 0;
		m_isCoinsGame = 0;					// 是否为金币场  0：普通场  1：金币场
		m_inDeskMinCoins = 0;				// 金币场入座所需的最小能量值
		m_qiangZhuangMinCoins = 0;			// 金币场参与抢庄的最小能量值
		m_isAllowCoinsNegative = 0;			// 每局结算时是否允许负数能量值出现
		m_isMedal = 0;
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
		ReadMapData(obj, NAME_TO_STR(m_dynamicJoin), m_dynamicJoin);
		ReadMapData(obj, NAME_TO_STR(m_clubOwerLookOn), m_clubOwerLookOn);
		ReadMapData(obj, NAME_TO_STR(m_isClubOwerLooking), m_isClubOwerLooking);
		ReadMapData(obj, NAME_TO_STR(m_curBoBoScore), m_curBoBoScore);
		ReadMapData(obj, NAME_TO_STR(kIsCoinsGame), m_isCoinsGame);
		ReadMapData(obj, NAME_TO_STR(kInDeskMinCoins), m_inDeskMinCoins);
		ReadMapData(obj, NAME_TO_STR(kQiangZhuangMinCoins), m_qiangZhuangMinCoins);
		ReadMapData(obj, NAME_TO_STR(kIsAllowCoinsNegative), m_isAllowCoinsNegative);
		ReadMapData(obj, NAME_TO_STR(kIsMedal), m_isMedal);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, 25);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kUserId", m_userId);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kFace", m_face);
		WriteKeyValue(pack, "kSex", m_sex);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kReady", m_ready);
		WriteKeyValue(pack, "kScore", m_score);
		WriteKeyValue(pack, "kIp", m_ip);
		WriteKeyValue(pack, "kOnline", m_online);
		WriteKeyValue(pack, "kCoins", m_coins);
		WriteKeyValue(pack, "kCredits", m_credits);
		WriteKeyValue(pack, "kUserGps", m_userGps);
		WriteKeyValue(pack, "kCheatAgainst", m_cheatAgainst);
		WriteKeyValue(pack, "kVideoPermission", m_videoPermission);
		WriteKeyValue(pack, "kDynamicJoin", m_dynamicJoin);
		WriteKeyValue(pack, "kAllowLookOn", m_allowLookOn);
		WriteKeyValue(pack, "kClubOwerLookOn", m_clubOwerLookOn);
		WriteKeyValue(pack, "kIsClubOwerLooking", m_isClubOwerLooking);
		WriteKeyValue(pack, "kCurBoBoScore", m_curBoBoScore);
		WriteKeyValue(pack, NAME_TO_STR(kIsCoinsGame), m_isCoinsGame);
		WriteKeyValue(pack, NAME_TO_STR(kInDeskMinCoins), m_inDeskMinCoins);
		WriteKeyValue(pack, NAME_TO_STR(kQiangZhuangMinCoins), m_qiangZhuangMinCoins);
		WriteKeyValue(pack, NAME_TO_STR(kIsAllowCoinsNegative), m_isAllowCoinsNegative);
		WriteKeyValue(pack, NAME_TO_STR(kIsMedal), m_isMedal);

		/*
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
		WriteKeyValue(pack, NAME_TO_STR(m_videoPermission), m_videoPermission);*/
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskAddUser();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回删除玩家
struct LMsgS2CDeskDelUser:public LMsgSC
{
	Lint			m_pos;					//删除玩家位置 如果pos == 我的位置，代表我自己被踢出桌子，把桌子其他人清掉
	Lint            m_userId;				//玩家ID
	Lstring         m_DismissName;			//
	Lint			m_clubOwerLookingLeave;	//用于会长明牌观战离开提示给其他玩家， 0：不提示在座玩家  1：会长明牌观战离开提示再做玩家。
	Lint			m_outType;				//outType离开类型 0：默认类型，兼容以前，1：房间解散，2：玩家离开房间，3：被踢出房间
	//新加
	Lint            m_StartGameButtonPos;

	LMsgS2CDeskDelUser() :LMsgSC(MSG_S_2_C_DEL_USER)
	{
		m_pos = INVAILD_POS;
		m_DismissName = "";
		m_StartGameButtonPos=INVAILD_POS;
		m_clubOwerLookingLeave = 0;
		m_outType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		ReadMapData(obj, NAME_TO_STR(m_clubOwerLookingLeave), m_clubOwerLookingLeave);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kUserId", m_userId);
		WriteKeyValue(pack, "kDismissName", m_DismissName);
		WriteKeyValue(pack, "kStartGameButtonPos", m_StartGameButtonPos);
		WriteKeyValue(pack, "kClubOwerLookingLeave", m_clubOwerLookingLeave);
		WriteKeyValue(pack, "kOutType", m_outType)
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_DismissName), m_DismissName);
		WriteKeyValue(pack, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		*/
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskDelUser();}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求退出房间
struct LMsgC2SLeaveDesk:public LMsgSC
{
	Lint			m_pos;//我的位置

	LMsgC2SLeaveDesk() :LMsgSC(MSG_C_2_S_LEAVE_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
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
	Lint        m_roomType;   //房间类型

	LMsgS2CLeaveDesk() :LMsgSC(MSG_S_2_C_LEAVE_ROOM)
	{
		m_errorCode = -1;
		m_roomType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(KRoomType), m_roomType);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);*/
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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
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
	Lint					m_errorCode;//0-等待操作中，1-未开始直接解算，2-三个人同意，解算成功，3-时间到，解算成功，4-有一个人拒绝，解算失败, 5-俱乐部会长强制解散,  6-竞技场只有一个人能量值可以玩强制解散
	Lint					m_time;//倒计时
	Lstring					m_applay;//申请的玩家
	Lint                    m_userId;//申请解散玩家ID
	Lstring					m_refluse;//拒绝的玩家,有一个玩家拒绝，则解算房间失败
	Lint					m_flag; //0-等待我同意或者拒绝，1-我已经同意或者拒绝  
	Lint					m_isShowFee;    //是否提示解散不消耗房卡的提示框   0：不提示   1：提示
	Lint					m_isLookOn;		//是否是观战玩家
	Lint   m_userLevel;
	std::vector<Lint>	    m_agreeUserId;//同意的玩家ID
	std::vector<Lint>	    m_waitUserId;//等待操作的玩家ID
	std::vector<Lstring>	m_agree;//同意的玩家
	std::vector<Lstring>	m_wait;//等待操作的玩家

	std::vector<Lstring>	m_agreeHeadUrl;//同意的玩家头像地址
	std::vector<Lstring>	m_waitHeadUrl;//等待操作的玩家头像地址

	std::vector<Lint> m_agreeUserLevel;
	std::vector<Lint> m_waitUserLevel;
	

	LMsgS2CResetDesk() :LMsgSC(MSG_S_2_C_RESET_ROOM)
	{
		m_errorCode = -1;
		m_time = 0;
		m_flag = 0;
		m_userId = 0;
		m_userLevel = 0;
		m_isShowFee = 0;
		m_isLookOn = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_applay), m_applay);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_refluse), m_refluse);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_userLevel), m_userLevel);
		ReadMapData(obj, NAME_TO_STR(m_isLookOn), m_isLookOn)
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_time);
		WriteKeyValue(pack, NAME_TO_STR(kApply), m_applay);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kLevel), m_userLevel);
		WriteKeyValue(pack, NAME_TO_STR(kIsShowFee), m_isShowFee);
		WriteKeyValue(pack, NAME_TO_STR(kIsLookOn), m_isLookOn);

		WriteKey(pack, "kAgreeUserId");
		WriteKey(pack, m_agreeUserId);

		WriteKey(pack, "kWaitUserId");
		WriteKey(pack, m_waitUserId);

		WriteKey(pack, "kAgree");
		WriteKey(pack, m_agree);

		WriteKey(pack, "kRefuse");
		WriteKey(pack, m_refluse);

		WriteKey(pack, "kWait");
		WriteKey(pack, m_wait);

		WriteKey(pack, "kAgreeHeadUrl");
		WriteKey(pack, m_agreeHeadUrl);

		WriteKey(pack, "kWaitHeadUrl");
		WriteKey(pack, m_waitHeadUrl);

		WriteKeyValue(pack, NAME_TO_STR(kAgreeUserLevel), m_agreeUserLevel);
		WriteKeyValue(pack, NAME_TO_STR(kWaitUserLevel), m_waitUserLevel);
		/*
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
		WriteKey(pack, m_waitHeadUrl);*/

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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kFlag), m_flag);
		/*
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);*/
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
	Lint   m_2d3D;     //客户端2d和3D转换过程会请求这个消息

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

	// 抢的牌
	Lint m_qiangCount[4];
	CardValue m_qiangCard[4][MAX_CARD_NUM];
	// 拾的牌
	Lint m_shiCount[4];
	CardValue m_shiCard[4][MAX_CARD_NUM];

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
	//CardValue   m_hongTongShuangWang[4];  //洪洞王牌双王玩法每个人选的王（**Ren 2017-12-15）
	std::vector<CardValue> m_hunCard;
	std::vector<Lint>      m_TingState;
	std::vector<Lint>      m_iTing;
	std::vector<CardValue>   m_iTingHuCard;
	std::vector<Lint> m_iTingHuCardFanScore;  //听牌的胡分

	std::vector<int> m_score;

	LMsgS2CDeskState() :LMsgSC(MSG_S_2_C_DESK_STATE)
	{
		m_2d3D = 0;
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
		memset(m_qiangCount, 0, sizeof(m_qiangCount));
		memset(m_shiCount, 0, sizeof(m_shiCount));
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

	//修改客户端消息包
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		// 加了m_score，这里多加1，按map算的
		//WriteMap(pack, 41+12+1+1+8+1+1);
		WriteMap(pack, 41 + 12 + 1 + 1 + 8 + 1 + 1 + 8 + 1); // 添加抢拾的牌
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(k2d3D), m_2d3D);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_state);

		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kZhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(kDCount), m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kTingState), m_TingState);
		WriteKeyValue(pack, NAME_TO_STR(kITing), m_iTing);  
		WriteKeyValue(pack, NAME_TO_STR(kDeskCreatType), m_deskCreatType);
		std::vector<CardValue> vec;
		WriteKey(pack, "kCardCount");
		std::vector<Lint> veci;
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_cardCount[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "kWinList");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_winList[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "kHType");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_hType[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "kMyCard");
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
		WriteKey(pack, "kMySuoCard");
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

			if ( m_pCard[i][0].kColor != 0 || m_pCount[i] == 0 )
			{
				sprintf(name,"kPCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_pCount[i]; j+=3)
				{
					vec.push_back(m_pCard[i][j]);
				}

			}
			else
			{
				sprintf(name,"kPhCard%d",i);
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
				sprintf(name, "kPCardFirePos%d", i);
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

			sprintf(name,"kFhCard%d",i);
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
			if ( m_aCard[i][0].kColor != 0 || m_aCount[i] == 0 )
			{
				sprintf(name,"kACard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_aCount[i]; j+=4)
				{
					vec.push_back(m_aCard[i][j]);
				}
			}
			else
			{
				sprintf(name,"kAhCard%d",i);
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
			if ( m_mCard[i][0].kColor != 0 || m_mCount[i] == 0 )
			{
				sprintf(name,"kMCard%d",i);
				WriteKey(pack, name);
				for (Lint j = 0; j < m_mCount[i]; j+=4)
				{
					vec.push_back(m_mCard[i][j]);
				}
		
			}
			else
			{
				sprintf(name,"kMhCard%d",i);
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
				sprintf(name, "kMCardFirePos%d", i);
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
			sprintf(name, "kOCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_oCount[i]; ++j)
			{
				vec.push_back(m_oCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "kKouCount");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_kouCount[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "kScore");
		WriteKey(pack, m_score);

		//吃的牌
		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "kECard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_eCount[i]; ++j)
			{
				vec.push_back(m_eCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		//抢的牌 4个位置
		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "kQiangCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_qiangCount[i]; ++j)
			{
				vec.push_back(m_qiangCard[i][j]);
			}
			WriteKey(pack, vec);
		}

		//拾到的牌 4个位置
		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "kShiCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_shiCount[i]; ++j)
			{
				vec.push_back(m_shiCard[i][j]);
			}
			WriteKey(pack, vec);
		}

		WriteKey(pack, "kWinCard");
		vec.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_winCard[i]);
		}
		WriteKey(pack,vec);

		//洪洞王牌每个人的双王牌序列化（**Ren 2017-12-15）
		/*WriteKey(pack, "kHongTongShuangWang");
		vec.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_hongTongShuangWang[i]);
		}
		WriteKey(pack, vec);*/

		WriteKey(pack, "kBchange");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bChange[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "kChangeCard");
		vec.clear();
		for (Lint i = 0; i < 3; ++i)
		{
			vec.push_back(m_changeCard[i]);
		}
		WriteKey(pack,vec);

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "kHCard%d", i);
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
			sprintf(name, "kTCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_tCount[i] && (j < 23); ++j)
			{
				vec.push_back(m_tCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "kHunCard");
		WriteKey(pack, m_hunCard);

		WriteKey(pack, "kITingHuCard");
		WriteKey(pack, m_iTingHuCard);

		WriteKey(pack, "kITingHuCardFanScore");
		WriteValue(pack, m_iTingHuCardFanScore);

		WriteKeyValue(pack, NAME_TO_STR(kPlaytype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(kChangeOutTime), m_changeOutTime);
		WriteKeyValue(pack, NAME_TO_STR(kOpOutTime), m_opOutTime);
		WriteKeyValue(pack, NAME_TO_STR(kBaseScore), m_baseScore);
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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		/*ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);*/
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

	Lint  m_redPacketPos;

	LMsgS2CUserSelectSeat() :LMsgSC(MSG_S_2_C_USER_SELECT_SEAT)
	{
		m_id  = 0;
		m_pos = INVAILD_POS;

		m_StartGameButtonPos=INVAILD_POS;

		m_redPacketPos = INVAILD_POS;
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
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kId), m_id);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kStartGameButtonPos), m_StartGameButtonPos);
		WriteKeyValue(pack, NAME_TO_STR(kRedPos), m_redPacketPos);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_StartGameButtonPos), m_StartGameButtonPos);
		*/
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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
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
//添加开始按钮玩家的位置
struct LMsgS2CUserReady:public LMsgSC
{
	Lint			m_pos;//准备玩家的位置
	Lint        m_readyUserCount; 


	LMsgS2CUserReady() :LMsgSC(MSG_S_2_C_READY)
	{
		m_pos = INVAILD_POS;	
		m_readyUserCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);		
		ReadMapData(obj, NAME_TO_STR(kNum), m_readyUserCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos); 
		WriteKeyValue(pack, NAME_TO_STR(kNum), m_readyUserCount);

		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);*/
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserReady();}
};


//服务器返回玩家数据
//61200:添加开始按钮玩家的位置
struct LMsgS2CUserDeskCommand : public LMsgSC
{
	Lint            m_commandType;				// 命令类型 1:第一个入座玩家显示开始按钮  2:管理员显示开始按钮  3：消失开始按钮
	Lint            m_StartButtonPos;			// 开局命令按钮玩家位置, 
	Lint			m_StartButtonEnable;		// 0：灰色（不可用）  1：正常（可用）
	Lint			m_StartButtonUserIdCount;   // 数量
	std::vector<Lint> m_StartButtonUserId;		// 开始按钮玩家ID（用于管理员开始游戏）
	Lstring			m_Nike;                     // 用户昵称

	LMsgS2CUserDeskCommand() :LMsgSC(MH_MSG_S_2_C_USER_DESK_COMMAND)
	{		 
		m_StartButtonPos = INVAILD_POS_QIPAI;
		m_StartButtonUserIdCount = 0;
		m_StartButtonEnable = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kCommandType), m_commandType);
		ReadMapData(obj, NAME_TO_STR(kStartButtonPos), m_StartButtonPos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kCommandType), m_commandType);
		WriteKeyValue(pack, NAME_TO_STR(kStartButtonPos), m_StartButtonPos);
		WriteKeyValue(pack, NAME_TO_STR(kStartButtonUserId), m_StartButtonUserId);
		WriteKeyValue(pack, NAME_TO_STR(kStartButtonUserIdCount), m_StartButtonUserIdCount);
		WriteKeyValue(pack, NAME_TO_STR(kStartButtonEnable), m_StartButtonEnable);
		WriteKeyValue(pack, NAME_TO_STR(kNike), m_Nike);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUserDeskCommand(); }
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kUserid), m_userid);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_userid), m_userid);*/
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

	//开局红包提示
	Lint    m_redPacketPos;
	Lint    m_redPacketValue;
	Lint    m_userOnlineStatus[4];

	LMsgS2CPlayStart() :LMsgSC(MSG_S_2_C_START)
	{
		for (int i = 0; i < 4; i++) 
		{
			m_cardCount[i] = 0;
			m_posUserid[i]=0;
			m_userOnlineStatus[i] = 0;
		}
		m_gamePlayerCount=4;

		m_redPacketPos = INVAILD_POS;
		m_redPacketValue = 0;

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
		ReadMapData(obj, NAME_TO_STR(m_gamePlayerCount), m_redPacketPos);
		ReadMapData(obj, NAME_TO_STR(m_gamePlayerCount), m_redPacketValue);

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

	//修改客户端消息内容
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kScore), m_score);

		WriteKey(pack, "kMyCard");
		std::vector<CardValue>vce;
		for (Lint i = 0; i < m_cardCount[m_pos]; ++i)
		{
			vce.push_back(m_cardValue[i]);
		}
 		WriteKey(pack,vce);

		WriteKey(pack, "kMySuoCard");
		std::vector<CardValue>vcesuo;
		for (Lint i = 0; i < 4; ++i)
		{
			vcesuo.push_back(m_cardSuoValue[i]);
		}
 		WriteKey(pack,vcesuo);

		WriteKey(pack, "kCardCount");
		std::vector<Lint>vcei;
		for(Lint i = 0 ; i < 4;++i)
		{
			vcei.push_back(m_cardCount[i]);
		}
		WriteKey(pack,vcei);

		WriteKeyValue(pack, NAME_TO_STR(kDCount),m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(kZhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(kGamePlayerCount), m_gamePlayerCount);
		WriteKeyValue(pack, NAME_TO_STR(kRedPos), m_redPacketPos);
		WriteKeyValue(pack, NAME_TO_STR(kRedValue), m_redPacketValue);

		WriteKey(pack, "kPosUserid");
		std::vector<Lint>userid;
		for (Lint i=0; i < 4; ++i)
		{
			userid.push_back(m_posUserid[i]);
		}
		WriteKey(pack, userid);

		WriteKey(pack, "kOnlineStatus");
		std::vector<Lint> userOnlineStatus;
		for (Lint i = 0; i < 4; ++i)
		{
			userOnlineStatus.push_back(m_userOnlineStatus[i]);
		}
		WriteKey(pack, userOnlineStatus);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CPlayStart();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸牌

struct ThinkData
{
	Lint	kType;
	std::vector<CardValue> kCard;

	ThinkData()
	{
		kType = 0;
	}

	MSGPACK_DEFINE(kType, kCard); 
};

struct TingData
{
	struct TingCard
	{
		CardValue kCard;
		Lint kHufan;

		MSGPACK_DEFINE(kCard, kHufan);
	};

	CardValue	kCard;
	std::vector<TingCard> kTingcard;

	MSGPACK_DEFINE(kCard, kTingcard); 
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
		ReadMapData(obj, NAME_TO_STR(kType), m_thinkInfo.kType);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kThink), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_thinkInfo.kCard.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_thinkInfo.kType);
		WriteKeyValue(pack, NAME_TO_STR(kThink), m_thinkInfo.kCard);
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

	Lint		m_errorCode;   //0: success	
	Lint        m_type;
	Lint		m_flag;////0-自摸，1-出牌,2-暗牌,3-明杠(自己碰过之后),4-提
	Lint		m_qiangGangflag; //标示是否该杠\补操作被抢 0-未被抢/无需等待胡牌 1-被抢 2-等待胡牌
	Lint		m_pos;//出牌玩家的位置
	ThinkData	m_card;
	std::vector<Lint>	m_hu;
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	CardValue m_huCard;					//自摸胡的那张牌
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
		ReadMapData(obj, NAME_TO_STR(kErrorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kFlag), m_flag);
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kCard), m_card);
		ReadMapData(obj, NAME_TO_STR(kHu), m_hu);
		ReadMapData(obj, NAME_TO_STR(kCardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(kCardValue), m_cardValue);
		m_huCard.Read(obj);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 14);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_card.kType);
		WriteKeyValue(pack, NAME_TO_STR(kQianggangflag), m_qiangGangflag);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kThink), m_card.kCard);
		WriteKeyValue(pack, NAME_TO_STR(kHu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(kCardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(kCardValue), m_cardValue);
		WriteKeyValue(pack, NAME_TO_STR(kRewardCardNum), m_rewardCardNum);
		WriteKeyValue(pack, NAME_TO_STR(kRewardHu), m_rewardHu);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_time);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kThink), m_think);
		WriteKeyValue(pack, NAME_TO_STR(kBOnlyHu), m_bOnlyHu);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CThink();}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送思考操作结果
struct LMsgC2SUserOper:public LMsgSC
{
	ThinkData kThink; 
	//Lint m_think;//思考结果0-过，1-胡，2-杠，3-碰,4-补

	LMsgC2SUserOper() :LMsgSC(MSG_C_2_S_USER_OPER){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kType), kThink.kType);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kThink), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				kThink.kCard.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kType), kThink.kType);
		WriteKeyValue(pack, NAME_TO_STR(kThink), kThink.kCard);

		return true;
	}


	virtual LMsg* Clone(){return new LMsgC2SUserOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserOper:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败
	Lint		m_pos;            //pos=我自己的时候，需要根据类型响应不同的操作
	ThinkData	m_think; 
	CardValue	m_card; //玩家碰，杠，胡的张牌
	std::vector<Lint>	m_hu;
	Lint        m_hType;                //1代表抢杠胡，否则为其他
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	Lint		m_rewardCardNum;		//胡牌奖励房卡数
	Lint		m_rewardHu;				//胡牌被奖励的牌型
	Lint        m_xuanWang;             //是否为选王操作，0非选王操作，1选王操作 （**Ren 2017-12-16）
	CardValue   m_hongTongShuangWang[4];//每个玩家选的王牌，没有王的选王0  （**Ren 2017-12-16）
	Lint    m_beforeOperPos;
	
	LMsgS2CUserOper() :LMsgSC(MSG_S_2_C_USER_OPER)
	{
		m_errorCode = -1;
		m_pos = INVAILD_POS;
		m_beforeOperPos = INVAILD_POS;
		m_hType = 0;
		m_cardCount = 0;
		m_rewardCardNum = 0;
		m_rewardHu = 0;
		m_xuanWang = 0;
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
		WriteMap(pack, 13);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kThink), m_think);
		m_card.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(kHu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(kHType), m_hType);
		WriteKeyValue(pack, NAME_TO_STR(kCardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(kCardValue), m_cardValue);
		WriteKeyValue(pack, NAME_TO_STR(kRewardCardNum), m_rewardCardNum);
		WriteKeyValue(pack, NAME_TO_STR(kRewardHu), m_rewardHu);
		WriteKeyValue(pack, NAME_TO_STR(kBeforePos), m_beforeOperPos);
		//WriteKeyValue(pack, NAME_TO_STR(kXuanWang), m_xuanWang);  //洪洞王牌双王大将选王标志（**Ren 2017-12-16）
		//std::vector<CardValue> vec;								  //洪洞王牌每个人的双王牌序列化（**Ren 2017-12-15）
		//WriteKey(pack, "kHongTongShuangWang");
		//vec.clear();
		//for (Lint i = 0; i < 4; ++i)
		//{
		//	vec.push_back(m_hongTongShuangWang[i]);
		//}
		//WriteKey(pack, vec);

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
	//修改客户端信息
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kRandNum), m_randNum);
		WriteKeyValue(pack, NAME_TO_STR(kHaoZiCards), m_HaoZiCards);
		WriteKeyValue(pack, NAME_TO_STR(kThink), m_think);
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
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		// ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);*/
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
	ThinkData kThinkInfo;

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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kType), kThinkInfo.kType);
		WriteKeyValue(pack, NAME_TO_STR(kCard), kThinkInfo.kCard);
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
	Lint           m_time; //语音时长

	LMsgC2SUserSpeak() :LMsgSC(MSG_C_2_S_USER_SPEAK)
	{
		m_type = -1;
		m_id = 0;
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kId), m_id);
		ReadMapData(obj, NAME_TO_STR(kMusicUrl), m_musicUrl);
		ReadMapData(obj, NAME_TO_STR(kMsg), m_msg);
		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		return true;
	}

	/*
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_musicUrl), m_musicUrl);
		ReadMapData(obj, NAME_TO_STR(m_msg), m_msg);
		return true;
	}
	*/

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
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
	Lint            m_time; //语音时长

	LMsgS2CUserSpeak() :LMsgSC(MSG_S_2_C_USER_SPEAK)
	{
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_type = -1;
		m_id = 0;
		m_time = 0;
	}

	
	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kId), m_id);
		WriteKeyValue(pack, NAME_TO_STR(kMusicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(kMsg), m_msg);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_time);
		return true;
	}
	/*

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
	*/
	virtual LMsg* Clone(){ return new LMsgS2CUserSpeak(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求创建桌子
struct LMsgC2SCreateDesk :public LMsgSC
{
	Lint   m_clubId;                //俱乐部，如果不为0，则为俱乐部创建玩法

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
	Lint       m_GpsLimit;                  // GPS限制:0,1
	Lint       m_allowLookOn;				// 是否允许观战
	Lint	   m_clubOwerLookOn;			// 允许会长名牌观战
	Lstring     m_GpsLng;                   // 经度
	Lstring     m_GpsLat;                   // 纬度 
	Lint m_tuoguan_mode;             // 托管设置

	LMsgC2SCreateDesk() :LMsgSC(MSG_C_2_S_CREATE_ROOM)
	{
		m_clubId = 0;
		m_flag = 0;	
		m_gold = 0;		
		m_state = 0;	
		m_credits = 0;
		m_robotNum = 0;
		m_Greater2CanStart=0;
		m_GpsLimit = 0;
		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_GpsLng = "0";
		m_GpsLat = "0";
		m_tuoguan_mode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);

		ReadMapData(obj, NAME_TO_STR(kFlag), m_flag);
		ReadMapData(obj, NAME_TO_STR(kSecret), m_secret);
		ReadMapData(obj, NAME_TO_STR(kGold), m_gold);
		ReadMapData(obj, NAME_TO_STR(kState), m_state);
		ReadMapData(obj, NAME_TO_STR(kRobotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(kCredits), m_credits);

		ReadMapData(obj, NAME_TO_STR(kCellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(kFeeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(kCheatAgainst), m_cheatAgainst);
		ReadMapData(obj, NAME_TO_STR(kDeskType), m_deskType);

		ReadMapData(obj, NAME_TO_STR(kGreater2CanStart), m_Greater2CanStart);
		ReadMapData(obj, NAME_TO_STR(kGpsLimit), m_GpsLimit);
		ReadMapData(obj, NAME_TO_STR(kAllowLookOn), m_allowLookOn);
		ReadMapData(obj, NAME_TO_STR(kClubOwerLookOn), m_clubOwerLookOn)
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_GpsLng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_GpsLat);
		ReadMapData(obj, NAME_TO_STR(kTuoGuanMode), m_tuoguan_mode);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kPlayType), array);
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

		//扑克的玩法都小于1000
		if (m_state > 1000)
		{
			ReadMapData(obj, NAME_TO_STR(kPlayer0CardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_Player0CardValue[i]);
				}
			}
			ReadMapData(obj, NAME_TO_STR(kPlayer1CardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_Player1CardValue[i]);
				}
			}
			ReadMapData(obj, NAME_TO_STR(kPlayer2CardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_Player2CardValue[i]);
				}
			}
			ReadMapData(obj, NAME_TO_STR(kPlayer3CardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_Player3CardValue[i]);
				}
			}
			ReadMapData(obj, NAME_TO_STR(kSendCardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 84; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_SendCardValue[i]);
				}
			}
			ReadMapData(obj, NAME_TO_STR(kHaoZiCardValue), array);
			if (array.type == MSGPACK_OBJECT_ARRAY)
			{
				for (Lsize i = 0; i < array.via.array.size && i < 2; ++i)
				{
					msgpack::object& pv = *(array.via.array.ptr + i);
					pv.convert(m_HaoZiCardValue[i]);
				}
			}

		}

		
	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

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
	Lint       m_GpsLimit;                  // GPS限制:0,1
	Lstring     m_GpsLng;                   // 经度
	Lstring     m_GpsLat;                   // 纬度 
	Lint m_tuoguan_mode;             // 托管设置

	LMsgC2SCreateDeskForOther() :LMsgSC(MSG_C_2_S_CREATE_ROOM_FOR_OTHER)
	{
		m_flag = 0;
		m_gold = 0;
		m_state = 0;
		m_credits = 0;
		m_robotNum = 0;
		m_deskType = 0;
		m_Greater2CanStart=0;
		m_GpsLimit = 0;
		m_tuoguan_mode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kFlag), m_flag);
		ReadMapData(obj, NAME_TO_STR(kSecret), m_secret);
		ReadMapData(obj, NAME_TO_STR(kGold), m_gold);
		ReadMapData(obj, NAME_TO_STR(kState), m_state);
		ReadMapData(obj, NAME_TO_STR(kRobotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(kCredits), m_credits);
		ReadMapData(obj, NAME_TO_STR(kDeskType), m_deskType);

		ReadMapData(obj, NAME_TO_STR(kCellscore), m_cellscore);
		ReadMapData(obj, NAME_TO_STR(kFeeType), m_feeType);
		ReadMapData(obj, NAME_TO_STR(kCheatAgainst), m_cheatAgainst);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kPlayer0CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player0CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(kPlayer1CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player1CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(kPlayer2CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player2CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(kPlayer3CardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 13; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_Player3CardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(kSendCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 84; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_SendCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(kHaoZiCardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < 2; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_HaoZiCardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(kPlayType), array);
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
		ReadMapData(obj, NAME_TO_STR(kGreater2CanStart), m_Greater2CanStart);
		ReadMapData(obj, NAME_TO_STR(kGpsLimit), m_GpsLimit);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_GpsLng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_GpsLat);
		ReadMapData(obj, NAME_TO_STR(kTuoGuanMode), m_tuoguan_mode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 13);
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
		WriteKeyValue(pack, NAME_TO_STR(kGpsLimit), m_GpsLimit);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLng), m_GpsLng);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLat), m_GpsLat);
		WriteKeyValue(pack, NAME_TO_STR(kTuoGuanMode), m_tuoguan_mode);
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
		Err_InvalidParameter = 5,
		Err_CreditNotEnough = 6,
		Err_GPS_INVALID = 9,
		Err_CLUB_DESK_FULL = 10,
		Err_CLUB_MASTER_FEE_NOT_ENOUGH = 11
	};
	Lint		m_errorCode;//0-成功，1-房卡不够，2-玩家已有存在的桌子，3-桌子号已被使用,4-创建桌子失败,5-创建参数有问题,6-人品不够,信用有误 8-超过数量限制 9-GPS数据无效
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);*/
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CCreateDeskRet(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端加入桌子
struct LMsgC2SAddDesk :public LMsgSC
{
	Lint		m_deskId;//桌子id
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgC2SAddDesk() :LMsgSC(MSG_C_2_S_ADD_ROOM)
	{
		m_deskId = 0;
		m_Gps_Lng = "0";
		m_Gps_Lat = "0";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kDeskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		// ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SAddDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求加入桌子结果
struct LMsgS2CAddDeskRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
		Err_GPS_INVALID = 9,
		Err_GPS_Limit = 10, //GPS房间，距离受限
		Err_ROOM_GAME_BEGIN=11,   //游戏已经开始
		Err_Player_Coins_Not_Enough = 12,   //玩家能量值不够
		Err_DESK_LOCK=13,       //桌子锁定
	};

	Lint		m_errorCode;//0-成功，1-人已满,2-房间不存在,3-未知错误 6-信用不够 4- 金币不足 5-进入俱乐部桌子（已创建过桌子，先解散）7-会长券不足 8-加入房间失败，请重试
	Lint		m_deskId;//桌子号码
	Lstring     m_userGPSList; 
	Lstring   m_min_distance_user_info;  //保存最小GPS距离用户信息，[id,nike,distance]
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
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(kUserGPSList), m_userGPSList);
		WriteKeyValue(pack, NAME_TO_STR(kMinDistanceUserInfo), m_min_distance_user_info);
		WriteKeyValue(pack, NAME_TO_STR(kDeskSeatPlayerName), m_deskSeatPlayerName);
		WriteKeyValue(pack, NAME_TO_STR(kDeskNoSeatPlayerName), m_deskNoSeatPlayerName);
		/*WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);*/
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

	//修改客户端消息内容
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		
		WriteKey(pack,"kNike");
		std::vector<Lstring>vecs;
		for(Lint i = 0; i < 4; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack,"kId");
		std::vector<Lint>veci;
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_id[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kGold");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_gold[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kZimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_zimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kBomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kWin");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_win[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kMgang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kAgang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kDzimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dzimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kDbomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dbomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kDwin");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dwin[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"kTing");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_CheckTing[i]);
		}
		WriteKey(pack,veci);
		WriteKeyValue(pack, "kTime", m_time);
		WriteKeyValue(pack, "kCreatorId", m_creatorId);
		WriteKeyValue(pack, "kCreatorNike", m_creatorNike);
		WriteKeyValue(pack, "kCreatorHeadUrl", m_creatorHeadUrl);
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

	Lint   m_clubId;
	Lint   m_limit;
	Lint   m_searchId;
	
	LMsgC2SVipLog() :LMsgSC(MSG_C_2_S_VIP_LOG)
	{
		m_time = 0;
		m_userId = 0;
		m_clubId = 0;
		m_limit = 0;
		m_searchId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kPage), m_limit);
		ReadMapData(obj, NAME_TO_STR(kSearch), m_searchId);
		return true;
	}

	/*
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}
    */
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
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kDeskId), m_deskId);

		return true;
	}
	/*
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);

		return true;
	}
	*/

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID),m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId),m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_time);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		return true;
	}

	/*
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}*/

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
	Lint m_clubId;         //俱乐部
	Lint m_showDeskId;  //展示的桌子号
	Lint m_cellScore;       // 桌子低分
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
		m_clubId = 0;
		m_showDeskId = 0;
		m_cellScore = 1;
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);	

		WriteKey(pack, "kData");
		WriteArray(pack, m_size);
		for (Lint i = 0; i < m_size; ++i)
		{
			WriteMap(pack,12);
			WriteKeyValue(pack, "kId", m_info[i].m_id);
			WriteKeyValue(pack, "kTime", m_info[i].m_time);
			WriteKeyValue(pack, "kFlag", m_info[i].m_flag);
			WriteKeyValue(pack, "kReset", m_info[i].m_reset);
			WriteKeyValue(pack, "kDeskId", m_info[i].m_deskId);
			WriteKeyValue(pack, "kClubId", m_info[i].m_clubId);
			WriteKeyValue(pack, "kShowDeskId", m_info[i].m_showDeskId);
			WriteKeyValue(pack, "kCellScore", m_info[i].m_cellScore);

			std::vector<Lstring> vecs;
			std::vector<Lint>veci, vec;
			for (Lint j = 0; j < 4; ++j)
			{
				vecs.push_back(m_info[i].m_nike[j]);
				veci.push_back(m_info[i].m_score[j]);
				vec.push_back(m_info[i].m_userid[j]);
			}
			WriteKey(pack, "kUserid");
			WriteKey(pack, vec);

			WriteKey(pack, "kNike");
			WriteKey(pack, vecs);

			WriteKey(pack, "kScore");
			WriteKey(pack, veci);

			WriteKey(pack, "kMatch");
			WriteArray(pack, m_info[i].m_size);
		}

		return true;
	}
	/*
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
	*/
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);		
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKey(pack, "kData");
		WriteArray(pack, m_size);
		for (Lint k = 0; k < m_size && k < 32; ++k)
		{
			WriteMap(pack, 3);
			WriteKeyValue(pack, "kTime", m_item[k].m_time);
			WriteKeyValue(pack, "kVideoId", m_item[k].m_videoId);

			std::vector<Lint> vec;
			for (Lint n = 0; n < 4; ++n)
			{
				vec.push_back(m_item[k].m_score[n]);
			}
			WriteKey(pack, "kScore");
			WriteKey(pack, vec);
		}

		return true;
	}
	/*
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
	*/
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		//ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kError), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(kUser));
		m_user.Write(pack);

		return true;
	}

	/*
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_user));
		m_user.Write(pack);

		return true;
	}*/

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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		return true;
	}
	/*
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		return true;
	}
	*/

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
		ReadMapData(obj, NAME_TO_STR(kVideoId), m_videoId);
		//ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
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
		ReadMapData(obj, NAME_TO_STR(kVideoId), m_videoId);
		//ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
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
		ReadMapData(obj, NAME_TO_STR(kShareID), m_shareID);
		//ReadMapData(obj, NAME_TO_STR(m_shareID), m_shareID);
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
		ReadMapData(obj, NAME_TO_STR(kVideoId), m_videoid);
		//ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoid);
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorId), m_errorId);
		WriteKeyValue(pack, NAME_TO_STR(kShareId), m_shareId);
		return true;
	}

	/*
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);
		WriteKeyValue(pack, NAME_TO_STR(m_shareId), m_shareId);
		return true;
	}
	*/
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorId), m_errorId);

		WriteKey(pack, "kData");
		WriteArray(pack, m_count);

		for (Lint i = 0; i < m_count; ++i)
		{
			WriteMap(pack, 4);
			WriteKeyValue(pack, NAME_TO_STR(kVideoId), m_vec_share_video[i].m_videoId);
			WriteKeyValue(pack, NAME_TO_STR(kTime), m_vec_share_video[i].m_time);
			std::vector<Lint> vecscore;
			std::vector<Lstring> vecnike;
			for (Lint n = 0; n < m_vec_share_video[i].m_user_count; ++n)
			{
				vecnike.push_back(m_vec_share_video[i].m_userNike[n]);
				vecscore.push_back(m_vec_share_video[i].m_score[n]);
			}

			WriteKey(pack, "kScore");
			WriteKey(pack, vecscore);

			WriteKey(pack, "kNike");
			WriteKey(pack, vecnike);
		}

		return true;
	}

	/*
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
	} */

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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(kZhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(kCurCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(kMaxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_flag);

		std::vector<Lint>vecid;
		std::vector<Lint>vecsex;
		std::vector<Lint>vecscore;
		std::vector<Lstring>vecnike;
		std::vector<Lstring>vecurl;

		for (Lint i = 0; i < 4; ++i)
		{
			if (m_video.m_userId[i] == 0)
			{
				continue;
			}
			vecid.push_back(m_video.m_userId[i]);
			vecnike.push_back(m_nike[i]);
			vecsex.push_back(m_sex[i]);
			vecurl.push_back(m_imageUrl[i]);
			vecscore.push_back(m_score[i]);
		}

		WriteKey(pack, "kScore");
		WriteKey(pack, vecscore);

		WriteKey(pack, "kUserid");
		WriteKey(pack, vecid);

		WriteKey(pack, "kNike");
		WriteKey(pack, vecnike);

		WriteKey(pack, "kSex");
		WriteKey(pack, vecsex);

		WriteKey(pack, "kImageUrl");
		WriteKey(pack, vecurl);

		WriteKey(pack, "kTime");
		WriteKey(pack, m_video.m_time);

		WriteKey(pack, "kOper");
		WriteKey(pack, m_video.m_oper);

		WriteKey(pack, "kPlaytype");
		WriteKey(pack, m_video.m_playType);

		for (Lint i = 0; i < 4; ++i)
		{
			std::stringstream ss;
			ss << "kCard" << i;
			WriteKey(pack, ss.str());
			WriteKey(pack, m_video.m_handCard[i]);
		}

		return true;
	}

	/*
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
	*/

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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kCurCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(kCurMaxCircle), m_curMaxCircle);
		WriteKeyValue(pack, NAME_TO_STR(kPlaytype), m_playtype);
		return true;
	}

	/*
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_curMaxCircle), m_curMaxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		return true;
	}*/

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
		m_winCard.kColor = 0;
		m_winCard.kNumber = 0;
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}
	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}
    /*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}
	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kReqUserId), m_reqUserId);
		return true;
	}
	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kStrGPS), m_strGPS);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kStrGPS), m_strGPS);
		return true;
	}

	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kVideoPermit), m_VideoPermit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kVideoPermit), m_VideoPermit);
		return true;
	}
	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kStrGPS), m_strGPS);
		ReadMapData(obj, NAME_TO_STR(kVideoPermit), m_VideoPermit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kStrGPS), m_strGPS);
		WriteKeyValue(pack, NAME_TO_STR(kVideoPermit), m_VideoPermit);
		return true;
	}
	/*
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
	*/
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
		ReadMapData(obj, NAME_TO_STR(kMessageList), m_MessageList);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kMessageList), m_MessageList);
		return true;
	}

	/*

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
	*/
	virtual LMsg* Clone() { return new LMsgS2CBcastMessage(); }
};

///////////////////////////////////////////////////////club--begin/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
  MSG_C_2_S_ENTER_CLUB = 61130
  客户端进入俱乐部
*/
struct LMsgC2SEnterClub :public LMsgSC
{
	Lint		m_clubId;		//俱乐部id

	LMsgC2SEnterClub() :LMsgSC(MSG_C_2_S_ENTER_CLUB)
	{
		m_clubId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
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
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kPlayTypeId), m_playTypeId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_clubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeId), m_playTypeId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SLeaveClub(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端离开俱乐部回复
struct LMsgS2CLeaveClub :public LMsgSC
{
	Lint     m_errorCode;            // 0--成功， 1-未找到俱乐部
	Lint		m_clubId;;//俱乐部id
	Lint     m_PlayTypeId;

	std::vector<clubOnlineUserSize>  m_clubOnlineUserCount;    //所在俱乐部和俱乐部在线人数

	LMsgS2CLeaveClub() :LMsgSC(MSG_S_2_C_LEAVE_CLUB)
	{
		m_errorCode=0;
		m_clubId=0;
		m_PlayTypeId=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_clubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(m_PlayTypeId), m_PlayTypeId);
		ReadMapData(obj, NAME_TO_STR(m_clubOnlineUserCount), m_clubOnlineUserCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeId), m_PlayTypeId);
		WriteKeyValue(pack, NAME_TO_STR(kClubOnlineUserCount), m_clubOnlineUserCount);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLeaveClub(); }
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
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kSwitchToType), m_switchToType);
		ReadMapData(obj, NAME_TO_STR(kCurrPlayType), m_currPlayType);
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

	LMsgC2SRequestPersonInfo() :LMsgSC(MSG_C_2_S_REQUEST_PERSON_INFO)
	{
		m_userId=0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
	
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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kName), m_name);
		WriteKeyValue(pack, NAME_TO_STR(kHeadImageUrl), m_headImageUrl);
		WriteKeyValue(pack, NAME_TO_STR(kIp), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(kTopScore), m_topScore);
		WriteKeyValue(pack, NAME_TO_STR(kRateOfWin), m_rateOfWin);

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
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubDeskId), m_clubDeskId);
		WriteKeyValue(pack, NAME_TO_STR(kShowDeskId), m_showDeskId);
		WriteKeyValue(pack, NAME_TO_STR(kRoomFull), m_roomFull);
		WriteKeyValue(pack, NAME_TO_STR(kCurrCircle), m_currCircle);
		WriteKeyValue(pack, NAME_TO_STR(kTotalCircle), m_totalCircle);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CFreshClubDeskInfo(); }
};

//////////////////////////////////////////////////////////////////////////
/* 俱乐部入座玩家的详细信息 */
struct deskUserInfo
{
	Lstring	     m_deskPlayerName;         // 用户名字
	Lstring		 m_deskPlayerUrl;          // 用户头像
	Lint         m_userId;                 // 用户id
	Lint         m_pos;                    // 用户座位号 0-3 东南西北
	Lint           m_userLevel;                 // 等级

	MSGPACK_DEFINE(m_deskPlayerName, m_deskPlayerUrl, m_userId, m_pos);

	deskUserInfo()
	{
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_userLevel = 0;
	}

	deskUserInfo(Lstring  deskPlayerName, Lstring  deskPlayerUrl)
	{
		m_deskPlayerName = deskPlayerName;
		m_deskPlayerUrl = deskPlayerUrl;
	}

	void copy(deskUserInfo & info)
	{
		m_userId = info.m_userId;
		m_pos = info.m_pos;
		m_deskPlayerName = info.m_deskPlayerName;
		m_deskPlayerUrl = info.m_deskPlayerUrl;
		m_userLevel = info.m_userLevel;
	}

	deskUserInfo & operator=(deskUserInfo & info)
	{
		m_userId = info.m_userId;
		m_pos = info.m_pos;
		m_deskPlayerName = info.m_deskPlayerName;
		m_deskPlayerUrl = info.m_deskPlayerUrl;
		m_userLevel = info.m_userLevel;
		return *this;
	}
	

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerName), m_deskPlayerName);
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_userLevel), m_userLevel);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerName), m_deskPlayerName);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		return true;
	}
};

//俱乐部大厅桌子总览消息(桌号、入座玩家信息、是否已满、圈数、会长是否在明牌观战)
struct desksInfo 
{
	Lint						m_deskId;             //桌子号
	std::vector<deskUserInfo>   m_deskUserInfo;		  //用户名字,头像
	Lint						m_showDeskId;         //展示的桌子号
	Lint						m_roomFull;           //房间是否满
	Lint						m_currCircle;         //当前局数
	Lint						m_totalCircle;        //房间总局数
	Lint						m_isClubOwerLooking;  //俱乐部会长是否正在明牌观战   0：不在    1：正在观战
	Lint                    m_redPacketFlag;   //红包标志  0--不显示  1--显示
	Lint                    m_userCount;         //人数
	Lint                    m_flag;                      //玩法类型
	Lint                    m_lock;                   // 0-正常  1-锁定

	MSGPACK_DEFINE(m_deskId, m_deskUserInfo, m_showDeskId, m_roomFull, m_currCircle, m_totalCircle, m_isClubOwerLooking, m_redPacketFlag, m_userCount, m_lock);

	desksInfo()
	{
		m_deskId=0;
		m_showDeskId=0;
		m_roomFull=0;
		m_currCircle=0;
		m_totalCircle=0;

		m_isClubOwerLooking = 0;

		m_redPacketFlag = 0;
		m_userCount = 0;
		m_flag = 0;
		m_lock = 0;
	}

	desksInfo(Lint clubDeskId)
	{
		m_deskId=clubDeskId;
	}

	desksInfo & operator= (desksInfo & info)
	{
		m_deskId = info.m_deskId;
		m_deskUserInfo = info.m_deskUserInfo;
		m_showDeskId = info.m_showDeskId;
		m_roomFull = info.m_roomFull;
		m_currCircle = info.m_currCircle;
		m_totalCircle = info.m_totalCircle;
		m_isClubOwerLooking = info.m_isClubOwerLooking;
		m_redPacketFlag = info.m_redPacketFlag;
		m_userCount = info.m_userCount;
		m_flag = info.m_flag;
		m_lock = info.m_lock;
		return *this;
	}

	bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
};

/* 俱乐部玩法信息,俱乐部大厅玩法选项卡 (结构体) 玩法信息概述*/
struct playTypeInfo
{
	Lint       m_playTypeId;           //玩法id
	Lstring    m_playTypeName;         //玩法名称
	Lint       m_showDeskBegin;        //开始桌号
	Lint       m_showDeskEnd;          //结束桌号

	MSGPACK_DEFINE(m_playTypeId, m_playTypeName, m_showDeskBegin, m_showDeskEnd);

	playTypeInfo()
	{
		m_playTypeId=0;
		m_showDeskBegin=0;
		m_showDeskEnd=0;

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
		ReadMapData(obj, NAME_TO_STR(m_showDeskBegin), m_showDeskBegin);
		ReadMapData(obj, NAME_TO_STR(m_showDeskEnd), m_showDeskEnd);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeId), m_playTypeId);
		WriteKeyValue(pack, NAME_TO_STR(m_playTypeName), m_playTypeName);
		WriteKeyValue(pack, NAME_TO_STR(m_showDeskBegin), m_showDeskBegin);
		WriteKeyValue(pack, NAME_TO_STR(m_showDeskEnd), m_showDeskEnd);

		return true;
	}
};

//  玩法信息详述
struct playTypeDetail
{
	Lint	              m_id;                       // 玩法id
	Lstring               m_name;                     // 玩法名字
	Lint        		  m_cellscore;                // 底分
	Lint                  m_flag;                     // 局数
	Lint        		  m_feeType;                  // 费用类型 ，0:房主付费 1:玩家分摊  和俱乐部的类型保持一致
	Lint        		  m_cheatAgainst;             // 是否防作弊，0:不防作弊 1：防作弊
	Lint				  m_state;                    // 大玩法
	std::vector<Lint>	  m_playtype;                 // 小选项玩法
	Lint                  m_Greater2CanStart;         // 大于2人开局
	Lint                  m_GpsLimit;                 // GPS距离限制
	Lint				  m_allowLookOn;			  // 是否允许观战   0:不允许   1:允许
	Lint				  m_clubOwerLookOn;		      // 是否允许俱乐部会长明牌观战   0:不允许   1:允许
	
	Lint				  m_minPointInRoom;           //加入游戏最小值
	Lint				  m_minPointXiaZhu;           //最小下注值
	Lint				  m_smallZero;                //是否可负分   0 不可以  1  可以
	Lint				  m_warnScore;                //预警提醒分数

	playTypeDetail()
	{
		m_id = 0;
		m_cellscore = 0;
		m_flag = 0;
		m_feeType = 0;
		m_cheatAgainst = 0;
		m_state = 0;
		m_Greater2CanStart = 0;
		m_GpsLimit = 0;
		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;

		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
		m_warnScore = 0;

	}
	~playTypeDetail() {  }

	//void copy(playTypeDetail & playDetail)
	//{
	//	m_id = playDetail.m_id;
	//	m_name = playDetail.m_name;
	//	m_cellscore = playDetail.m_cellscore;
	//	m_flag = playDetail.m_flag;
	//	m_feeType = playDetail.m_feeType;
	//	m_cheatAgainst = playDetail.m_cheatAgainst;
	//	m_state = playDetail.m_state;
	//	m_playtype = playDetail.m_playtype;
	//	m_Greater2CanStart = playDetail.m_Greater2CanStart;
	//	m_GpsLimit = playDetail.m_GpsLimit;
	//	m_allowLookOn = playDetail.m_allowLookOn;
	//	m_clubOwerLookOn = playDetail.m_clubOwerLookOn;
	//
	//	m_minPointInRoom = playDetail.m_minPointInRoom;
	//	m_minPointXiaZhu = playDetail.m_minPointXiaZhu;
	//	m_smallZero = playDetail.m_smallZero ;
	//}

	void clear()
	{
		m_id = 0;
		m_cellscore = 0;
		m_flag = 0;
		m_feeType = 0;
		m_cheatAgainst = 0;
		m_state = 0;
		m_Greater2CanStart = 0;
		m_GpsLimit = 0;
		m_allowLookOn = 0;
		m_clubOwerLookOn = 0;
		m_playtype.clear();

		m_minPointInRoom = 0;
		m_minPointXiaZhu = 0;
		m_smallZero = 0;
		m_warnScore = 0;
	}

	/* 重载=操作符，作为该结构体赋值操作 */
	playTypeDetail& operator=(playTypeDetail & playDetail)
	{
		m_id = playDetail.m_id;
		m_name = playDetail.m_name;
		m_cellscore = playDetail.m_cellscore;
		m_flag = playDetail.m_flag;
		m_feeType = playDetail.m_feeType;
		m_cheatAgainst = playDetail.m_cheatAgainst;
		m_state = playDetail.m_state;
		m_playtype = playDetail.m_playtype;
		m_Greater2CanStart = playDetail.m_Greater2CanStart;
		m_GpsLimit = playDetail.m_GpsLimit;
		m_allowLookOn = playDetail.m_allowLookOn;
		m_clubOwerLookOn = playDetail.m_clubOwerLookOn;

		m_minPointInRoom = playDetail.m_minPointInRoom;
		m_minPointXiaZhu = playDetail.m_minPointXiaZhu;
		m_smallZero = playDetail.m_smallZero;
		m_warnScore = playDetail.m_warnScore;

		return *this;
	}

};

struct clubInfo
{
	Lint    m_clubId;
	Lstring  m_name;
	Lint      m_clubType;

	MSGPACK_DEFINE(m_clubId, m_name, m_clubType);

	clubInfo(Lint clubId)
	{
		m_clubId = clubId;
		m_clubType = 0;
	}

	~clubInfo(){}

	clubInfo & operator= (const clubInfo& info)
	{
		m_clubId = info.m_clubId;
		m_name = info.m_name;
		m_clubType = info.m_clubType;
		return *this;
	}

};


/*
  MSG_S_2_C_CLUB_SCENE = 61131
  服务器返回俱乐部游戏玩法界面信息
*/
struct LMsgS2CClubScene :public LMsgSC
{

	Lint                            m_errorCode;            //结果，成功失败    0--成功，1--用户不是俱乐部会员 2--该俱乐部还没有建玩法  3-未找到俱乐部  4-还没有加入俱乐部
	
	//俱乐部信息
	std::vector<clubInfo>  m_clubinfo;

	Lint                            m_clubId;               //当前俱乐部号
	Lint                            m_userState;            //用户在当前俱乐部身份 
	Lint                            m_myClubNum;      //用户有的俱乐部个数
	Lint                            m_joinClubNum;   //用户加入的俱乐部个数

	Lstring                      m_clubName;             //当前俱乐部名字
	Lint                            m_presidentId;          //当前会长id
	Lint                            m_card;                        //当前俱乐部房卡
	 

	//玩法信息
	std::vector<playTypeInfo>		m_playTypeInfo;			//该俱乐部所有可见的玩法信息
	playTypeDetail                           m_playTypeDetail;      //当前玩法的具体信息

	//桌子信息
	Lint							m_deskCount;          
	std::vector<desksInfo>			m_desksInfo;

	//在线人数
	Lint  m_OnlineUserCount;
	Lint  m_UserCount;

	LMsgS2CClubScene() :LMsgSC(MSG_S_2_C_CLUB_SCENE)
	{
		m_errorCode=0;
		m_deskCount=0;

		m_clubId=0;
		m_presidentId = 0;
		m_card = 0;
		m_userState = 0;
		m_OnlineUserCount = 0;
		m_UserCount = 0;
		m_myClubNum = 0;
		m_joinClubNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 32);

		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);

		WriteKeyValue(pack, NAME_TO_STR(kClubList), m_clubinfo);

		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kClubName), m_clubName);
		WriteKeyValue(pack, NAME_TO_STR(kPresidentid), m_presidentId);
		WriteKeyValue(pack, NAME_TO_STR(kClubCard), m_card);
		WriteKeyValue(pack, NAME_TO_STR(kUserStaus), m_userState);

		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeInfo), m_playTypeInfo);

		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeid), m_playTypeDetail.m_id);
		WriteKeyValue(pack, NAME_TO_STR(kName), m_playTypeDetail.m_name);
		WriteKeyValue(pack, NAME_TO_STR(kCellscore), m_playTypeDetail.m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_playTypeDetail.m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kFeeType), m_playTypeDetail.m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(kCheatAgainst), m_playTypeDetail.m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_playTypeDetail.m_state);
		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeCount), m_playTypeDetail.m_playtype.size());
		WriteKeyValue(pack, NAME_TO_STR(kPlayType), m_playTypeDetail.m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(kGreater2CanStart), m_playTypeDetail.m_Greater2CanStart);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLimit), m_playTypeDetail.m_GpsLimit);
		WriteKeyValue(pack, NAME_TO_STR(kAllowLookOn), m_playTypeDetail.m_allowLookOn);
		WriteKeyValue(pack, NAME_TO_STR(kClubOwerLookOn), m_playTypeDetail.m_clubOwerLookOn);
		WriteKeyValue(pack, NAME_TO_STR(kWarn), m_playTypeDetail.m_warnScore);
		WriteKeyValue(pack, NAME_TO_STR(kMinPointInRoom), m_playTypeDetail.m_minPointInRoom);
		WriteKeyValue(pack, NAME_TO_STR(kMinPointXiaZhu), m_playTypeDetail.m_minPointXiaZhu);
		WriteKeyValue(pack, NAME_TO_STR(kFree), m_playTypeDetail.m_smallZero);

		WriteKeyValue(pack, NAME_TO_STR(kDeskCount), m_desksInfo.size());
		WriteKeyValue(pack, NAME_TO_STR(kDesksInfo), m_desksInfo);
		WriteKeyValue(pack, NAME_TO_STR(kOnlineCount), m_OnlineUserCount);
		WriteKeyValue(pack, NAME_TO_STR(kUserCount), m_UserCount);

		WriteKeyValue(pack, NAME_TO_STR(kMyClubNum), m_myClubNum);
		WriteKeyValue(pack, NAME_TO_STR(kJoinClubNum), m_joinClubNum);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CClubScene(); }
};

///////////////
//服务器返回俱乐部游戏切换玩法界面信息
struct LMsgS2CSwitchClubScene : public LMsgSC
{
	Lint                            m_errorCode;            //结果，成功失败    0--成功，1--用户不是俱乐部会员 2--该俱乐部还没有建玩法  3-未找到俱乐部
	//俱乐部信息
	std::vector<clubInfo>  m_clubinfo;

	Lint                            m_clubId;               //当前俱乐部号
	Lint                            m_userState;            //用户在当前俱乐部身份 

	Lstring                      m_clubName;             //当前俱乐部名字
	Lint                            m_presidentId;          //当前会长id
	Lint                            m_card;                        //当前俱乐部房卡
	//玩法信息
	std::vector<playTypeInfo>		m_playTypeInfo;			//该俱乐部所有可见的玩法信息
	playTypeDetail                           m_playTypeDetail;      //当前玩法的具体信息

																	//桌子信息
	Lint							m_deskCount;
	std::vector<desksInfo>			m_desksInfo;

	//在线人数
	Lint  m_OnlineUserCount;
	Lint  m_UserCount;

	LMsgS2CSwitchClubScene() :LMsgSC(MSG_S_2_C_SWITCH_PLAY_SCENE)
	{
		m_errorCode = -1;
		m_deskCount = 0;

		m_clubId = 0;
		m_presidentId = 0;
		m_card = 0;
		m_userState = 0;
		m_OnlineUserCount = 0;
		m_UserCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 26);

		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);

		WriteKeyValue(pack, NAME_TO_STR(kClubList), m_clubinfo);

		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kClubName), m_clubName);
		WriteKeyValue(pack, NAME_TO_STR(kPresidentid), m_presidentId);
		WriteKeyValue(pack, NAME_TO_STR(kClubCard), m_card);
		WriteKeyValue(pack, NAME_TO_STR(kUserStaus), m_userState);

		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeInfo), m_playTypeInfo);

		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeid), m_playTypeDetail.m_id);
		WriteKeyValue(pack, NAME_TO_STR(kName), m_playTypeDetail.m_name);
		WriteKeyValue(pack, NAME_TO_STR(kCellscore), m_playTypeDetail.m_cellscore);
		WriteKeyValue(pack, NAME_TO_STR(kFlag), m_playTypeDetail.m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kFeeType), m_playTypeDetail.m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(kCheatAgainst), m_playTypeDetail.m_cheatAgainst);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_playTypeDetail.m_state);
		WriteKeyValue(pack, NAME_TO_STR(kPlayTypeCount), m_playTypeDetail.m_playtype.size());
		WriteKeyValue(pack, NAME_TO_STR(kPlayType), m_playTypeDetail.m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(kGreater2CanStart), m_playTypeDetail.m_Greater2CanStart);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLimit), m_playTypeDetail.m_GpsLimit);
		WriteKeyValue(pack, NAME_TO_STR(kAllowLookOn), m_playTypeDetail.m_allowLookOn);
		WriteKeyValue(pack, NAME_TO_STR(kClubOwerLookOn), m_playTypeDetail.m_clubOwerLookOn);

		WriteKeyValue(pack, NAME_TO_STR(kDeskCount), m_desksInfo.size());
		WriteKeyValue(pack, NAME_TO_STR(kDesksInfo), m_desksInfo);
		WriteKeyValue(pack, NAME_TO_STR(kOnlineCount), m_OnlineUserCount);
		WriteKeyValue(pack, NAME_TO_STR(kUserCount), m_UserCount);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSwitchClubScene(); }
};

///////////////
//服务器返回俱乐部玩法界面桌子信息
struct deskUserBaseInfo
{
	Lstring     m_deskPlayerUrl;          //用户头像
	Lint        m_userId;                 //用户id
	Lint        m_pos;                    // 用户座位号 0-3 东南西北

	MSGPACK_DEFINE(m_deskPlayerUrl, m_userId, m_pos);
	deskUserBaseInfo()
	{
		m_userId=0;
		m_pos=INVAILD_POS;
	}
	deskUserBaseInfo(Lstring  deskPlayerUrl)
	{
		m_deskPlayerUrl=deskPlayerUrl;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskPlayerUrl), m_deskPlayerUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
	
		return true;
	}
};

struct desksBaseInfo
{
	Lint                             m_deskId;           //桌子号
	std::vector<deskUserBaseInfo>    m_deskUserInfo;	 //用户名字,头像

	MSGPACK_DEFINE(m_deskId, m_deskUserInfo);

	desksBaseInfo()
	{
		m_deskId=0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_deskUserInfo), m_deskUserInfo);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskUserInfo), m_deskUserInfo);
		return true;
	}

};

struct LMsgS2CClubDeskInfo :public LMsgSC
{
	Lint                                                m_deskNum;
	std::vector<desksBaseInfo>  m_deskInfo;


	MSGPACK_DEFINE(m_deskNum, m_deskInfo);

	LMsgS2CClubDeskInfo() :LMsgSC(MSG_S_2_C_CLUB_DESK_INFO)
	{
		m_deskNum=0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskNum), m_deskNum);
		ReadMapData(obj, NAME_TO_STR(m_deskInfo), m_deskInfo);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kDeskNum), m_deskNum);
		WriteKeyValue(pack, NAME_TO_STR(kDeskInfo), m_deskInfo);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgS2CClubDeskInfo(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端发送俱乐部会长解散房间的消息
struct LMsgC2SClubMasterResetRoom : public LMsgSC
{
	Lint m_clubId;
	Lint m_playTypeId;
	Lint m_clubDeskId;
	Lint m_showDeskId;

	LMsgC2SClubMasterResetRoom() : LMsgSC(MSG_C_2_S_CLUB_MASTER_RESET_ROOM) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kClubId", m_clubId);
		ReadMapData(obj, "kPlayTypeId", m_playTypeId);
		ReadMapData(obj, "kClubDeskId", m_clubDeskId);
		ReadMapData(obj, "kShowDeskId", m_showDeskId);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SClubMasterResetRoom();
	}
};

//////////////////////////////////////////////////////////////////////////
/*
  MSG_S_2_C_CLUB_MASTER_RESET_ROOM = 61153
  服务器返回解散房间的消息
*/
struct LMsgS2CClubMasterResetRoom : public LMsgSC
{
	Lint m_errorCode;                                // 0--解散成功   1--房间号错误 2--解散失败 3-该用户没有权限
	Lstring m_strErrorDes;
	Lint m_clubId;
	Lint m_playTypeId;
	Lint m_clubDeskId;
	Lint m_showDeskId;

	LMsgS2CClubMasterResetRoom() : LMsgSC(MSG_S_2_C_CLUB_MASTER_RESET_ROOM)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		m_showDeskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kErrorCode", m_errorCode);
		WriteKeyValue(pack, "kStrErrorDes", m_strErrorDes);
		WriteKeyValue(pack, "kClubId", m_clubId);
		WriteKeyValue(pack, "kPlayTypeId", m_playTypeId);
		WriteKeyValue(pack, "kClubDeskId", m_clubDeskId);
		WriteKeyValue(pack, "kShowDeskId", m_showDeskId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CClubMasterResetRoom();
	}
};



///////////////////////////////////////////////////////club--end/////////////////////////////////////////////////////////


//////查询房间GPS限制信息/////////////////////////////////////////////////////////////////////
//客户端查询房间信息
struct MHLMsgC2SQueryRoomGPSLimitInfo :public LMsgSC
{
	Lint m_deskId;        //RoomId
	MHLMsgC2SQueryRoomGPSLimitInfo() :LMsgSC(MH_MSG_C_2_S_QUERY_ROOM_GPS_LIMIT)
	{
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kDeskId), m_deskId);		 
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgC2SQueryRoomGPSLimitInfo(); }
};

///////////////
//服务器返回房间GPS信息
struct MHLMsgS2CQueryRoomGPSLimitInfoRet :public LMsgSC
{
	Lint  m_error_code; //错误代码:0成功，1失败
	Lint  m_gps_limit; // GPS限制信息 0:不限制 1:限制 
	MHLMsgS2CQueryRoomGPSLimitInfoRet() : LMsgSC(MH_MSG_S_2_C_QUERY_ROOM_GPS_LIMIT_RET)
	{
		m_error_code = 0;
		m_gps_limit = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kError), m_error_code);
		ReadMapData(obj, NAME_TO_STR(kGpsLimit), m_gps_limit);	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error_code);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLimit), m_gps_limit); 
		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgS2CQueryRoomGPSLimitInfoRet(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端询问加入房间
struct LMsgC2SJoinRoomCheck :public LMsgSC
{
	Lint m_deskId;        //RoomId
	Lint m_clubId;
	Lint m_playTypeId;
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgC2SJoinRoomCheck() :LMsgSC(MSG_C_2_S_JOIN_ROOM_CHECK)
	{
		m_deskId = 0;
		m_clubId = 0;
		m_playTypeId = 0;
		m_Gps_Lng = "0";
		m_Gps_Lat = "0";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kDeskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kPlayTypeId), m_playTypeId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SJoinRoomCheck(); }
};

//////////////////////////////////////////////////////////////////////////
struct  UserNikeAndPhoto
{
	Lstring    m_nike;
	Lstring    m_headUrl;
	Lint          m_id;
	Lint          m_userLevel;

	UserNikeAndPhoto()
	{
		m_id = 0;
		m_userLevel = 0;
	}

};

//服务端返回询问加入房间
struct LMsgS2CJoinRoomCheck :public LMsgSC
{
	Lint  m_errorCode;  //错误代码:0成功
	Lint  m_feeType;      //付费类型
	Lint  m_costFee;        //消耗费用
	Lint  m_curCircle;     //当前局数
	Lint  m_maxCircle;    //最大局数
	Lint  m_roomUserCount;    //
	Lint  m_state;        //大玩法編號
   std::vector<UserNikeAndPhoto>   m_roomUser;   //房间用户

   LMsgS2CJoinRoomCheck() : LMsgSC(MSG_S_2_C_JOIN_ROOM_CHECK)
	{
	   m_errorCode = 0;
	   m_feeType = 0;
	   m_costFee = 0;
	   m_curCircle = 0;
	   m_maxCircle = 0;
	   m_roomUserCount = 0;
	   m_state = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kFeeType), m_feeType);
		WriteKeyValue(pack, NAME_TO_STR(kCostFee), m_costFee);
		WriteKeyValue(pack, NAME_TO_STR(kCurCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(kMaxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_state);

		WriteKey(pack, "kRoomUser");
		WriteArray(pack, m_roomUserCount);

		for (Lint i = 0; i < m_roomUserCount; ++i)
		{
			WriteMap(pack, 4);
			WriteKeyValue(pack, NAME_TO_STR(kid), m_roomUser[i].m_id);
			WriteKeyValue(pack, NAME_TO_STR(kNike), m_roomUser[i].m_nike);
			WriteKeyValue(pack, NAME_TO_STR(kHeadUrl), m_roomUser[i].m_headUrl);
			WriteKeyValue(pack, NAME_TO_STR(kLevel), m_roomUser[i].m_userLevel);
		}

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CJoinRoomCheck(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端 用户创建俱乐部房间
struct LMsgC2SClubUserCreateRoom :public LMsgSC
{
	Lint m_clubId;
	Lint m_playTypeId;
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgC2SClubUserCreateRoom() :LMsgSC(MSG_C_2_S_CLUB_CREATE_ROOM)
	{
		m_clubId = 0;
		m_playTypeId = 0;
		m_Gps_Lng = "0";
		m_Gps_Lat = "0";
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kPlayTypeId), m_playTypeId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SClubUserCreateRoom(); }


};


/* 玩家请求游戏参数
type=1 抢拾开关                           parameter: 0不抢拾，1
type=2 玩家牌局中发送托管，parameter: 1 进入托管 0 不托管
type=3 玩家牌局中发送胡牌，parameter: 0-不自动胡，1 自动胡牌
**************************************/
// 玩家请求游戏参数
struct MHLMsgC2SGameConfig : public LMsgSC
{
	Lint m_pos;
	Lint m_type;                  //  类型  0:  None   1 抢拾
	Lint m_parameter;	    //  参数 type=0 invalid 1: 0 不允许 1:允许

	MHLMsgC2SGameConfig() :LMsgSC(MH_MSG_C_2_S_GAME_RUNTIME_CONFIG)
	{
		m_pos = INVAILD_POS;
		m_type = 0;
		m_parameter = 0;	  
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgC2SGameConfig(); }


}; 

//  服务器广播游戏参数
struct MHLMsgS2CGameConfig : public LMsgSC
{
	Lint m_pos;                    // 位置
	Lint m_type;                  // 类型  0:None 1 抢拾
	Lint m_parameter;	    //  参数 type=0 invalid 1: 0 不允许 1:允许
	Lint m_error;                // 1: 类型错误, 2: 位置错误 3: 参数错误, 10:未知错误	 

	MHLMsgS2CGameConfig() : LMsgSC(MH_MSG_S_2_C_GAME_RUNTIME_CONFIG)
	{
		m_pos = INVAILD_POS;
		m_type = 0;
		m_parameter = 0;
		m_error = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgS2CGameConfig(); }

};


//////////////////////////////////////////////////////////////////////////
//客户端请求俱乐部成员
struct LMsgC2SRequestClubMemberList :public LMsgSC
{
	Lint m_ClubId;            //俱乐部id
	Lint m_Page;	              //当前请求数据是第几页，1代表第一页数据（0-10），2 代表第二页数据（11-20）
	Lstring m_Search;   //搜索的用户, 如果为空，则搜索club全部用户
	Lstring  m_Nike;           //用户昵称，模糊搜索
	Lint   m_count;              //客户端收到用户id的个数
	std::vector<Lint>  m_oldUser;    //客户端已经收到的用户id

	LMsgC2SRequestClubMemberList() :LMsgSC(MSG_C_2_S_REQUEST_CLUB_MEMBER)
	{
		m_ClubId = 0;
		m_Page = 0;
		//m_SearchUserId = 0;
		m_count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		ReadMapData(obj, NAME_TO_STR(kPage), m_Page);
		ReadMapData(obj, NAME_TO_STR(kSearch), m_Search);
		ReadMapData(obj, NAME_TO_STR(kNike), m_Nike);
		ReadMapData(obj, NAME_TO_STR(kCount), m_count);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kOldUser), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_oldUser.push_back(v);
			}
		}


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
	

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequestClubMemberList(); }
};

//////////////////////////////////////////////////////////////////////////
struct ClubMember
{
	Lint                        m_Id;        //用户id
	Lstring                  m_Nike;      //昵称
	Lstring                  m_photo;     //头像
	Lint                        m_State;     //状态  0-不在线（读取时间）  1-在线（未参加游戏） 2-在线（游戏中）
	Lint                        m_LastTime;  //最后在线时间（unixtime)  如果在线用户，就是当前时间
	Lint                        m_status;           //1-普通用户  2-管理员  3--会长

	ClubMember()
	{
		m_Id = 0;
		m_State = 0;
		m_LastTime = 0;
		m_status = 1;
	}

	MSGPACK_DEFINE(m_Id, m_Nike, m_photo, m_State, m_LastTime, m_status);
};


//服务端返回个人信息
struct LMsgS2CClubMemberList :public LMsgSC
{
	Lint                          m_ErrorCode;	                     //错误码   1--没有数据  2-俱乐部id不存在  3-userId 不为0，但该userId不是该俱乐部成员
	Lint                          m_ClubId;                               //俱乐部id
	Lint                          m_Time;                                   //时间  统计数据的时间
	Lint                          m_ClubOwnerId;                    //会长id
	Lstring	                  m_ClubOwnerNike;              //会长昵称
	Lstring	                  m_ClubHeadUrl;                  //会长头像

	Lstring                    m_OtherData;         //暂时不用

	Lint                          m_ClubTotal;         //俱乐部总用户数
	Lint                       	 m_HasSendAll;        //已经发送所有俱乐部用户 0：没有 1：已经发送所有

	Lint                       	 m_Count;       //这个消息包含的用户数
	std::vector<ClubMember>    	m_MemberList;    //用户列表

	Lint                         m_OnlineUserCount;        //在线人数
	Lint                         m_UserCount;                    //总人数



	LMsgS2CClubMemberList() :LMsgSC(MSG_S_2_C_CLUB_MEMBER_INFO)
	{
		m_ErrorCode = 0;
		m_ClubId = 0;
		m_Time = 0;
		m_ClubOwnerId = 0;
		m_ClubTotal = 0;
		m_HasSendAll = 0;
		m_Count = 0;
		m_OnlineUserCount = 0;
		m_UserCount=0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		//ReadMapData(obj, NAME_TO_STR(m_name), m_name);
		//ReadMapData(obj, NAME_TO_STR(m_headImageUrl), m_headImageUrl);
		//ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		//ReadMapData(obj, NAME_TO_STR(m_topScore), m_topScore);
		//ReadMapData(obj, NAME_TO_STR(m_rateOfWin), m_rateOfWin);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 14);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_Time);
		WriteKeyValue(pack, NAME_TO_STR(kClubOwnerId), m_ClubOwnerId);
		WriteKeyValue(pack, NAME_TO_STR(kClubOwnerNike), m_ClubOwnerNike);
		WriteKeyValue(pack, NAME_TO_STR(kClubHeadUrl), m_ClubHeadUrl);
		WriteKeyValue(pack, NAME_TO_STR(kOtherData), m_OtherData);
		WriteKeyValue(pack, NAME_TO_STR(kClubTotal), m_ClubTotal);
		WriteKeyValue(pack, NAME_TO_STR(kHasSendAll), m_HasSendAll);
		WriteKeyValue(pack, NAME_TO_STR(kCount), m_Count);
		WriteKeyValue(pack, NAME_TO_STR(kMemberList), m_MemberList);
		WriteKeyValue(pack, NAME_TO_STR(kOnlineCount), m_OnlineUserCount);
		WriteKeyValue(pack, NAME_TO_STR(kUserCount), m_UserCount);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CClubMemberList(); }
};


//////////////////////////////////////////////////////////////////////////
struct  ClubMessageList
{
	Lint  m_Type;         //消息类型
	Lint m_Time;         //时间
	Lstring m_Message;      //消息内容

	ClubMessageList() 
	{
		m_Type = 0;
		m_Time = 0;
	}
	MSGPACK_DEFINE(m_Type, m_Time, m_Message);

};
//服务器广播俱乐部用户信息
struct LMsgS2CBcast2ClubUser : public LMsgSC
{
	std::vector<ClubMessageList>  m_messageList;

	LMsgS2CBcast2ClubUser() : LMsgSC(MSG_S_2_C_CLUB_USER_BCAST_MESSAGE)
	{
		
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(kType), m_type);
		//ReadMapData(obj, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kMessageList), m_messageList);
		//WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error);
		//WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		//WriteKeyValue(pack, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBcast2ClubUser(); }

};


//////////////////////////////////////////////////////////////////////////
//客户端请求人数
struct LMsgC2SRequestClubOnLineCount :public LMsgSC
{
	Lint m_ClubId;            //俱乐部id
	

	LMsgC2SRequestClubOnLineCount() :LMsgSC(MSG_C_2_S_REQUEST_CLUB_ONLINE_COUNT)
	{
		m_ClubId = 0;
	
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//WriteMap(pack, 2);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequestClubOnLineCount(); }
};


//////////////////////////////////////////////////////////////////////////
//服务器返回人数
struct LMsgS2CClubOnlineCount : public LMsgSC
{
	Lint m_clubId;
	Lint m_OnlineCount;          //在线人数
	Lint m_totalCount;            //总人数

	LMsgS2CClubOnlineCount() : LMsgSC(MSG_S_2_C_CLUB_ONLINE_COUNT)
	{
		m_clubId = 0;
		m_OnlineCount = 0;
		m_totalCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kPos), m_clubId);
		//ReadMapData(obj, NAME_TO_STR(kType), m_type);
		//ReadMapData(obj, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kOnlineCount), m_OnlineCount);
		WriteKeyValue(pack, NAME_TO_STR(kTotalCount), m_totalCount);
		//WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		//WriteKeyValue(pack, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CClubOnlineCount(); }

};


//////////////////////////////////////////////////////////////////////////
//服务器通知用户申请加入俱乐部
struct LMsgS2CNotifyUserJoinClub : public LMsgSC
{
	Lint m_clubId;
	Lint m_userId;          //在线人数

	LMsgS2CNotifyUserJoinClub() : LMsgSC(MSG_S_2_C_NOTIFY_USER_JOIN_CLUB)
	{
		m_clubId = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kPos), m_clubId);
		//ReadMapData(obj, NAME_TO_STR(kType), m_type);
		//ReadMapData(obj, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		//WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		//WriteKeyValue(pack, NAME_TO_STR(kParameter), m_parameter);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CNotifyUserJoinClub(); }

};

//////////////////////////////////////////////////////////////////////////
//客户端请求亲友圈好友列表
struct LMsgC2SRequestClubMemberOnlineNogame:public LMsgSC
{
	Lint m_ClubId;            //俱乐部id


	LMsgC2SRequestClubMemberOnlineNogame() :LMsgSC(MSG_C_2_S_REQUEST_CLUB_MEMBER_ONLINE_NOGAME)
	{
		m_ClubId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		

		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequestClubMemberOnlineNogame(); }
};

//////////////////////////////////////////////////////////////////////////
struct ClubMemberOnLine
{
	Lint                        m_Id;        //用户id
	Lstring                  m_Nike;      //昵称
	Lstring                  m_photo;     //头像
	Lint                        m_state;      //用户状态
	

	ClubMemberOnLine()
	{
		m_Id = 0;
		m_state = 1;
	}

	MSGPACK_DEFINE(m_Id, m_Nike, m_photo, m_state);
};


//服务端返回个人信息
struct LMsgS2CClubOnLineMemberList :public LMsgSC
{
	Lint                          m_ErrorCode;	                     //错误码   0--成功   1--俱乐部没有找到（clubId错误）2-用户没有参加任何俱乐部
	Lint                          m_ClubId;                               //俱乐部id

	Lint                       	 m_Count;       //这个消息包含的用户数
	std::vector<ClubMemberOnLine>    	m_MemberList;    //用户列表

	LMsgS2CClubOnLineMemberList() :LMsgSC(MSG_S_2_C_CLUB_MEMBER_ONLINE_NOGAME)
	{
		m_ErrorCode = 0;
		m_ClubId = 0;
		m_Count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		m_Count = m_MemberList.size();
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kCount), m_Count);
		WriteKeyValue(pack, NAME_TO_STR(kMemberList), m_MemberList);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CClubOnLineMemberList(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端邀请用户
struct LMsgC2SInviteClubMember :public LMsgSC
{
	Lint m_ClubId;                                     //俱乐部id
	Lint m_InviteeId;                     //邀请的用户Id
	Lint  m_ClubShowDeskId;             //桌子id eg  101
	Lint  m_clubDeskId;               //7位桌子

	//房间规则
	Lint m_state;
	std::vector<Lint>  m_playType;


	LMsgC2SInviteClubMember() :LMsgSC(MSG_C_2_S_INVITE_CLUB_MENBER)
	{
		m_ClubId = 0;
		m_InviteeId = 0;
		m_clubDeskId = 0;
		m_ClubShowDeskId = 0;
		m_state = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		ReadMapData(obj, NAME_TO_STR(kInviteeId), m_InviteeId);
		ReadMapData(obj, NAME_TO_STR(kClubDeskId), m_clubDeskId);
		ReadMapData(obj, NAME_TO_STR(kClubShowDeskId), m_ClubShowDeskId);
		ReadMapData(obj, NAME_TO_STR(kState), m_state);
		
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kPlayType), array);
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

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{


		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SInviteClubMember(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端邀请用户,服务器返回
struct LMsgS2CInviteClubMemberReply :public LMsgSC
{
	Lint                          m_ErrorCode;	                     //错误码  1--用户已下线  2--用户游戏中  3-未知错误，没有邀请成功
	Lint                          m_ClubId;                               //俱乐部id
	Lint                          m_InviteeId;              //邀请的用户Id

	LMsgS2CInviteClubMemberReply() :LMsgSC(MSG_S_2_C_INVITE_CLUB_MEMBER_REPLY)
	{
		m_ErrorCode = 0;
		m_ClubId = 0;
		m_InviteeId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kInviteeId), m_InviteeId);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CInviteClubMemberReply(); }
};


//////////////////////////////////////////////////////////////////////////
////转发用户邀请
struct LMsgS2CTransmitInviteClubMember :public LMsgSC
{
	Lint m_ClubId;                     //俱乐部id
	Lint m_InviterId;         //邀请用户Id
	Lstring  m_InviterNike;      //昵称
	Lstring m_InviterPhoto;     //头像
	Lint m_ClubShowDeskId;    //桌子id eg  101
	Lint m_clubDeskId;

	 //房间规则
	Lint m_state;
	std::vector<Lint>  m_playType;



	LMsgS2CTransmitInviteClubMember() :LMsgSC(MSG_S_2_C_TRANSMIT_INVITE_CLUB_MENBER)
	{
		m_ClubId = 0;
		m_InviterId = 0;
		m_ClubShowDeskId = 0;
		m_state = 0;
		m_clubDeskId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kInviterId), m_InviterId);
		WriteKeyValue(pack, NAME_TO_STR(kInviterNike), m_InviterNike);
		WriteKeyValue(pack, NAME_TO_STR(kInviterPhoto), m_InviterPhoto);
		WriteKeyValue(pack, NAME_TO_STR(kClubShowDeskId), m_ClubShowDeskId);
		WriteKeyValue(pack, NAME_TO_STR(kClubDeskId), m_clubDeskId);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_state);
		WriteKeyValue(pack, NAME_TO_STR(kPlayType), m_playType);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CTransmitInviteClubMember(); }
};


//////////////////////////////////////////////////////////////////////////
////回复俱乐部成员的邀请
struct LMsgC2SReplyClubMemberInvite :public LMsgSC
{
	Lint m_ClubId;                                     //俱乐部id
	Lint m_InviterId;                     //邀请的用户Id
	Lint  m_ClubShowDeskId;             //桌子id eg  101
	Lint  m_clubDeskId;
	Lint  m_reply;                                  //0--no  1-yes
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;


	LMsgC2SReplyClubMemberInvite() :LMsgSC(MSG_C_2_S_REPLY_CLUB_MEMBER_INVITE)
	{
		m_ClubId = 0;
		m_InviterId = 0;
		m_ClubShowDeskId = 0;
		m_reply = 0;
		m_clubDeskId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		ReadMapData(obj, NAME_TO_STR(kInviterId), m_InviterId);
		ReadMapData(obj, NAME_TO_STR(kClubShowDeskId), m_ClubShowDeskId);
		ReadMapData(obj, NAME_TO_STR(kClubDeskId), m_clubDeskId);
		ReadMapData(obj, NAME_TO_STR(kReply), m_reply);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{


		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SReplyClubMemberInvite(); }
};

//////////////////////////////////////////////////////////////////////////
//////回复俱乐部成员的邀请的回复，只有在接收邀请但失败才会有
struct LMsgS2CReplyClubMemberInviteReply :public LMsgSC
{
	Lint m_ErrorCode;             //  1-游戏已经开始  2-未知错误，接收邀请失败
	Lint m_ClubId;                     //俱乐部id
	Lint m_InviterId;         //邀请用户Id
	Lint m_ClubShowDeskId;    //桌子id eg  101



	LMsgS2CReplyClubMemberInviteReply() :LMsgSC(MSG_S_2_C_REPLY_CLUB_MEMBER_INVITE_REPLY)
	{
		m_ErrorCode = 0;
		m_ClubId = 0;
		m_InviterId = 0;
		m_ClubShowDeskId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kInviterId), m_InviterId);
		WriteKeyValue(pack, NAME_TO_STR(kClubShowDeskId), m_ClubShowDeskId);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CReplyClubMemberInviteReply(); }
};

//////////////////////////////////////////////////////////////////////////
////通知会长俱乐部成员情况
struct LMsgS2CNotifyClubOwnerMember : public LMsgSC
{
	Lint m_ClubOwerId;
	Lint m_LeaveCount;          //离开的用户
	Lint m_ApplyCount;         //加入的用户

	LMsgS2CNotifyClubOwnerMember() : LMsgSC(MSG_S_2_C_NOTIFY_CLUBOWNER_MEMBER)
	{
		m_ClubOwerId = 0;
		m_LeaveCount = 0;
		m_ApplyCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubOwnerId), m_ClubOwerId);
		WriteKeyValue(pack, NAME_TO_STR(kLeave), m_LeaveCount);
		WriteKeyValue(pack, NAME_TO_STR(kApply), m_ApplyCount);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CNotifyClubOwnerMember(); }

};


//////////////////////////////////////////////////////////////////////////
////通知用户他被踢出亲友圈
struct LMsgS2CNotifyUserLeaveClub: public LMsgSC
{
	Lint m_clubId;
	Lstring m_clubName;          //离开的用户
	

	LMsgS2CNotifyUserLeaveClub() : LMsgSC(MSG_S_2_C_NOTIFY_USER_LEAVE_CLUB)
	{
		m_clubId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kClubName), m_clubName);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CNotifyUserLeaveClub(); }

};
 
//////////////////////////////////////////////////////////////////////////
////设置俱乐部管理员
struct LMsgC2SSetClubAdministrtor :public LMsgSC
{
	Lint m_ClubId;                                     //俱乐部id
	Lint  m_adminUserId;                     //管理员Id
	Lint  m_type;                                     //1-设置管理员，2-取消管理员


	LMsgC2SSetClubAdministrtor() :LMsgSC(MSG_C_2_S_CLUB_SET_ADMINISTRTOR)
	{
		m_ClubId = 0;
		m_adminUserId = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		ReadMapData(obj, NAME_TO_STR(kPlayerId), m_adminUserId);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{


		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SSetClubAdministrtor(); }
};

//////////////////////////////////////////////////////////////////////////
////设置俱乐部管理员返回
////通知会长俱乐部成员情况
struct LMsgS2CSetClubAdministrtor : public LMsgSC
{
	Lint m_error_code;                             // 0--成功， 1--你没有权限设置管理员  2--设置管理员失败  3--俱乐部id不存在 4--管理员用户不是俱乐部成员 5--已经是管理员（设置） 6--该用户是会长 7-该用户不是管理员（取消的）
	Lint m_ClubId;                                     //俱乐部id
	Lint m_type;                                        //  //1-设置管理员，2-取消管理员
	

	LMsgS2CSetClubAdministrtor() : LMsgSC(MSG_S_2_C_CLUB_SET_ADMINSTRTOR)
	{
		m_error_code = 0;
		m_ClubId = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error_code);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_ClubId);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSetClubAdministrtor(); }

};


//////////////////////////////////////////////////////////////////////////
////客户端请求GPS 图形内容
struct LMsgC2SGPSInfo :public LMsgSC
{
	
	LMsgC2SGPSInfo() :LMsgSC(MSG_C_2_S_GPS_INFO)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGPSInfo(); }
};


//////////////////////////////////////////////////////////////////////////
////GPS 图形内容
struct LMsgS2CGPSInfo : public LMsgSC
{
	Lint                               m_error_code;                             // 0--成功，1--获取失败
	Lint                              m_type;

	std::vector<Lint>     m_IdList;          //用户Id列表
	std::vector<Lstring>  m_PhotoList;       //用户头像列表
	std::vector<Lstring>  m_NikeList;         //用户昵称列表
	std::vector<Lstring>  m_gpsInfo;          //gps数据

	LMsgS2CGPSInfo() : LMsgSC(MSG_S_2_C_GPS_INFO)
	{
		m_error_code = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error_code);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kIdList), m_IdList);
		WriteKeyValue(pack, NAME_TO_STR(kPhotoList), m_PhotoList);
		WriteKeyValue(pack, NAME_TO_STR(kNikeList), m_NikeList);
		WriteKeyValue(pack, NAME_TO_STR(kGpsList), m_gpsInfo);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CGPSInfo(); }

};

//////////////////////////////////////////////////////////////////////////
////客户端请求断线重连消息
struct LMsgC2SRequestDeskState :public LMsgSC
{

	LMsgC2SRequestDeskState() :LMsgSC(MSG_C_2_S_REQUEST_DESK_STATE)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kClubId), m_ClubId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequestDeskState(); }
};


//61185:用户能量值预警
struct LMsgS2CWarnPointLow :public LMsgSC
{
	Lint		    m_msgType;            //0== 提醒   1== 取消提醒   
	Lint			m_userId;
	Lint			m_pos;
	Lstring			m_nike;
	Lint			m_point;              //用户当前能力值
	Lint			m_warnPoint;		  // 预警的能力值

	LMsgS2CWarnPointLow() :LMsgSC(MSG_S_2_C_WARN_POINT_LOW)
	{
		m_msgType = 0;
		m_userId = 0;
		m_pos = 0;
		m_point = 0;
		m_warnPoint = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kMsgType), m_msgType);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kNike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(kPoint), m_point);
		WriteKeyValue(pack, NAME_TO_STR(kWarnPoint), m_warnPoint);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CWarnPointLow(); }
};

/*
  能量不足玩家充上能量后显示入座按钮
  MSG_S_2_C_STAND_PLAYER_SHOW_SEAT = 61186
*/
struct LMsgS2CStandPlayerShowSeat : public LMsgSC
{
	Lint m_pos;				//玩家位置
	Lint m_addCoins;		//曾加的能量值
	Lint m_isShowSeat;		//是否显示入座按钮  0：加能量提示    1：显示入座   2：隐藏入座

	LMsgS2CStandPlayerShowSeat() : LMsgSC(MSG_S_2_C_STAND_PLAYER_SHOW_SEAT)
	{
		m_pos = INVAILD_POS_QIPAI;
		m_addCoins = 0;
		m_isShowSeat = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kAddCoins), m_addCoins);
		WriteKeyValue(pack, NAME_TO_STR(kIsShowSeat), m_isShowSeat);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CStandPlayerShowSeat(); }
};

/*
  能量不足玩家充上能量后从新点击入座
  MSG_C_2_S_STAND_PLAYER_RESEAT = 61187	
*/
struct LMsgC2SStandPlayerReSeat : public LMsgSC
{
	Lint m_userId;
	Lint m_pos;

	LMsgC2SStandPlayerReSeat() : LMsgSC(MSG_C_2_S_STAND_PLAYER_RESEAT)
	{
		m_userId = 0;
		m_pos = INVAILD_POS_QIPAI;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SStandPlayerReSeat(); }
};



//////////////////////////////////////////////////////////////////////////
#define QIPAI_VIDEO_USER_MAX   10

struct UserHandCards
{
	std::vector<Lint> m_handcards[QIPAI_VIDEO_USER_MAX];
	//std::vector<Lint> m_handcards1;
	//std::vector<Lint> m_handcards2;

	MSGPACK_DEFINE(m_handcards[0], m_handcards[1], m_handcards[2], m_handcards[3], m_handcards[4]);
	
	UserHandCards()
	{

	}


};
/* 
  双升出牌操作
*/
struct QiPaiVideoOper_ShengJi
{
	Lint m_pos;				//操作人位置
	Lint m_oper;			//操作类型：1拿底牌，2埋底牌，3普通出牌，4甩牌
	Lint m_firstOutPos;		//本轮第一个出牌玩家位置
	Lint m_totleScore;		//闲家抓分

	std::vector<Lint> m_operCards;		//客户端操作的牌
	std::vector<Lint> m_handCards;		//手牌
	//std::vector<Lint> m_resultCards;	//甩牌失败的出牌结果

	QiPaiVideoOper_ShengJi()
	{
		m_pos = 0;
		m_oper = 0;
		m_firstOutPos = 0;
		m_totleScore = 0;
		m_handCards.clear();
		m_operCards.clear();
	}

	Lstring ToString()
	{
		std::stringstream ss;
		ss << m_pos << ";";
		ss << m_oper << ";";
		ss << m_firstOutPos << ";";
		ss << m_totleScore << ";";
		int t_operCardsSize = m_operCards.size();
		for (int i = 0; i < t_operCardsSize; ++i)
		{
			ss << m_operCards[i];
			if (i < t_operCardsSize - 1)
			{
				ss << ",";
			}
		}
		if (!m_handCards.empty())
		{
			ss << ";";
		}
		int t_handCardsSize = m_handCards.size();
		for (int i = 0; i < t_handCardsSize; ++i)
		{
			ss << m_handCards[i];
			if (i < t_handCardsSize - 1)
			{
				ss << ",";
			}
		}
		return ss.str();
	}
};

/*
  扑克类录像结功能结构体
  每一局写入数据库Vedio表中的一项
*/
struct QiPaiVideoLog_Base
{
	//一下是video表中公共的字段
	Lstring									m_Id;			//回放ID
	Lint									m_time;			//时间
	Lint									m_userId[QIPAI_VIDEO_USER_MAX];	//玩家ID列表
	Lint									m_deskId;		//桌子ID
	Lint									m_flag;			//牌局玩法：升级:106
	Lint									m_curCircle;	//当前局数
	Lint									m_maxCircle;	//玩家规定的最大局数
	Lint									m_score[QIPAI_VIDEO_USER_MAX];	//玩家总分
	Lint									m_zhuang;		//庄家本局庄家位置
	std::vector<Lint>						m_playType;		//玩法小选项
};

/*
  升级录像结功能结构体
  每一局中Video表中Data字段内容
*/
struct QiPaiVideoLog_ShengJi : QiPaiVideoLog_Base
{
	//以下是Video表中Data字段的内容
	Lint									m_currGrade;				//当前打几 A-K
	Lint                                    m_bankerSelectCard;			//庄家庄家亮主的牌（无人亮主时，默认为64）
	Lint									m_banderSelectCardCount;	//庄家亮主牌张数（无人亮主时，默认为2）
	Lint									m_playerGreade[2];			//两对级牌
	
	std::vector<Lint>						m_handCard[QIPAI_VIDEO_USER_MAX];  //游戏开始玩家手牌
	std::vector<Lint>                       m_baseCard;		//升级底牌
	std::vector<QiPaiVideoOper_ShengJi>		m_oper;			//操作记录

	/********************************************************************************
	* 函数名：	addOper()
	* 描述：	将玩家出牌或者盖底牌操作记录到录像的vector中
	* 参数：
	*  @ pos 操作玩家的位置
	*  @ oper 操作类型：1拿底牌，2埋底牌，3普通出牌，4甩牌
	*  @ firstOutPos 每轮第一个出牌玩家位置
	*  @ totleScore 闲家总抓分
	*  @ operCards 操作的牌
	*  @ handCards 操作玩家的手牌
	* 返回：	void
	*********************************************************************************/
	void addOper(Lint pos, Lint oper, Lint firstOutPos, Lint totleScore, std::vector<Lint>& operCards, std::vector<Lint>& handCards)
	{
		QiPaiVideoOper_ShengJi  tempQPVideoOper;
		tempQPVideoOper.m_pos = pos;
		tempQPVideoOper.m_oper = oper;
		tempQPVideoOper.m_firstOutPos = firstOutPos;
		tempQPVideoOper.m_totleScore = totleScore;
		tempQPVideoOper.m_operCards = operCards;
		tempQPVideoOper.m_handCards = handCards;

		m_oper.push_back(tempQPVideoOper);
	}

	/*******************************************************************************
	* 函数名：	getInsertSql()
	* 描述：	获取写入数据库Vedio表中记录的SQL语句
	* 参数：
	* 返回：	(String)
	********************************************************************************/
	Lstring getInsertSql()
	{
		std::stringstream ss;
		ss << "INSERT INTO";
		ss << " video ";
		ss << "(Id, UserId1, Time, UserId2, UserId3, UserId4, Data, DeskId, Flag, CurCircle, MaxCircle, Score1, Score2, Score3, Score4, Zhuang, PlayType) VALUES (";
		ss << "'" << m_Id << "',";
		ss << "'" << m_userId[0] << "',";
		ss << "'" << m_time << "',";
		for (int i = 1; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_userId[i] << "',";
		}
		ss << "'" << vedioDataToString() << " ',";
		ss << "'" << m_deskId << "',";
		ss << "'" << m_flag << "',";
		ss << "'" << m_curCircle << "',";
		ss << "'" << m_maxCircle << "',";
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_score[i] << "',";
		}
		ss << "'" << m_zhuang << "',";
		ss << "'" << playTypeToStrint() << "')";

		return ss.str();
	}

	/* 开局信息 */
	Lstring gameStartInfoToString()
	{
		std::stringstream ss;
		ss << m_currGrade << ";";
		ss << m_bankerSelectCard << ";";
		ss << m_banderSelectCardCount << ";";
		ss << m_playerGreade[0] << "," <<m_playerGreade[1];
		return ss.str();
	}

	/* 开局手牌 */
	Lstring handCardToString()
	{
		std::stringstream ss;
		for (int i = 0; i< QIPAI_VIDEO_USER_MAX; i++)
		{
			for (int j = 0; j < m_handCard[i].size(); j++)
			{
				ss << m_handCard[i][j];
				if (j< m_handCard[i].size() - 1)
				{
					ss << ",";
				}
			}
			if (i<QIPAI_VIDEO_USER_MAX - 1)
				ss << ";";
		}
		return ss.str();
	}

	/* 开局底牌 */
	Lstring baseCardToString()
	{
		std::stringstream ss;
		for (size_t i = 0; i < m_baseCard.size(); ++i)
		{
			ss << m_baseCard[i];
			if (i < m_baseCard.size() - 1)
			{
				ss << ",";
			}
		}
		return ss.str();
	}

	/* 牌局中操作数据 */
	Lstring operDataToString()
	{
		std::stringstream ss;
		for (int i = 0; i < m_oper.size(); i++)
		{
			ss << m_oper[i].ToString();
			if (i<m_oper.size() - 1)
			{ 
				ss << "|";
			}
		}
	}

	/* 玩法小选项 */
	Lstring playTypeToStrint()
	{
		std::stringstream ss;
		for (Lsize i = 0; i < m_playType.size(); ++i)
		{
			ss << m_playType[i];
			if (i <m_playType.size() - 1)
			{ 
				ss << ";";
			}
		}
		return ss.str();
	}

	/* Vedio表中Data字段内容*/
	Lstring vedioDataToString()
	{
		std::stringstream ss;
		ss << gameStartInfoToString() << "|";
		ss << handCardToString() << ";" << baseCardToString() << "|";
		ss << operDataToString();
		return ss.str();
	}	
};

//录像操作结构
struct QiPaiVideoOper
{
	Lint		m_pos;
	Lint		m_oper;             //操作，1-出牌  0-不出
	std::vector<Lint> m_cards;
	std::vector<Lint> m_handcards;
	Lint    m_score;
	struct UserHandCards  m_userHandcards;
	Lint	m_sound;				//声音
	MSGPACK_DEFINE(m_pos, m_oper, m_score,m_cards, m_userHandcards, m_sound);
	QiPaiVideoOper()
	{
		m_pos = 0;
		m_oper = 0;
		m_score = 0;
		m_cards.clear();
		m_handcards.clear();
		m_sound = -1;
	}

	Lstring		ToString()
	{
		std::stringstream ss;
		ss << m_pos << ";";
		ss << m_oper << ";";
		ss << m_score << ";";
		
		for (size_t i = 0; i< m_cards.size(); i++)
		{
			ss << m_cards[i];
			if (i < m_cards.size() - 1)
			{
				ss << ",";
			}
		}
		if(!m_handcards.empty())
			ss << ";";
		for (size_t i = 0; i< m_handcards.size(); i++)
		{
			ss << m_handcards[i];
			if (i < m_handcards.size() - 1)
			{
				ss << ",";
			}
		}
		ss << ";";
		ss << m_sound;

		return ss.str();
	}

};

//三打二录像操作结构107
struct QiPaiVideoOper_SanDaEr
{
	Lint m_pos;						//位置
	Lint m_oper;					//操作，1-出牌  0-不出
	std::vector<Lint> m_cards;
	std::vector<Lint> m_handcards;
	Lint m_score;
	struct UserHandCards m_userHandcards;
	MSGPACK_DEFINE(m_pos, m_oper, m_score, m_cards, m_userHandcards);

	QiPaiVideoOper_SanDaEr()
	{
		m_pos = 0;
		m_oper = 0;
		m_score = 0;
		m_cards.clear();
		m_handcards.clear();
	}
	Lstring ToString()
	{
		std::stringstream ss;
		ss << m_pos << ";";
		ss << m_oper << ";";
		ss << m_score << ";";
		for (size_t i = 0; i < m_cards.size(); i++)
		{
			ss << m_cards[i];
			if (i < m_cards.size() - 1)
			{
				ss << ",";
			}
		}
		if (!m_handcards.empty())
			ss << ";";
		for (size_t i = 0; i < m_handcards.size(); i++)
		{
			ss << m_handcards[i];
			if (i < m_handcards.size() - 1)
			{
				ss << ",";
			}
		}
		return ss.str();
	}
};

//棋牌录像结构基类
struct  QiPaiVideoLogBase
{
	Lint							m_time;
	Lstring							m_Id;
	Lint							m_zhuang;
	Lint							m_fuZhuang;							//副庄家（三打二添加）
	Lint							m_deskId;
	Lint							m_curCircle;
	Lint							m_maxCircle;
	Lint							m_score[QIPAI_VIDEO_USER_MAX];		//玩家得分
	Lint							m_userId[QIPAI_VIDEO_USER_MAX];		//玩家ID
	Lint							m_flag;								//扑克玩法 101,102...111
	Lstring							m_str;								//数据库中Data字段内容
	Lstring							m_str_playtype;						//玩家小选项书序列化后的字段

	std::vector<Lint>			    m_handCard[QIPAI_VIDEO_USER_MAX];	//玩家手牌
	std::vector<QiPaiVideoOper>		m_oper;								//玩家操作
	std::vector<Lint>				m_playType;							//玩家小选项

	std::vector<Lint>               m_restCard;							//底牌
	Lint                            m_bankerScore;						//庄家叫分
	Lint                            m_tiPai[QIPAI_VIDEO_USER_MAX];      //提牌 0，1-踢 2-跟 3-回

	QiPaiVideoLogBase()
	{
		m_time = 0;
		m_zhuang = QIPAI_VIDEO_USER_MAX;
		m_fuZhuang = QIPAI_VIDEO_USER_MAX;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		memset(m_score, 0x00, sizeof(m_score));
		memset(m_userId, 0x00, sizeof(m_userId));
		memset(m_tiPai, 0x00, sizeof(m_tiPai));
		m_bankerScore = 0;
		m_flag = 0;

	}

	void clear()
	{
		m_time = 0;
		m_zhuang = QIPAI_VIDEO_USER_MAX;
		m_fuZhuang = QIPAI_VIDEO_USER_MAX;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		memset(m_score, 0x00, sizeof(m_score));
		memset(m_userId, 0x00, sizeof(m_userId));
		memset(m_tiPai, 0x00, sizeof(m_tiPai));
		m_bankerScore = 0;
		m_flag = 0;
		m_Id.clear();
		m_str.clear();
		m_str_playtype.clear();
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			m_handCard[i].clear();
		}
		m_oper.clear();
		m_playType.clear();
		m_restCard.clear();
	}

	void DrawClear()
	{
		m_zhuang = QIPAI_VIDEO_USER_MAX;
		m_fuZhuang = QIPAI_VIDEO_USER_MAX;
		m_curCircle = 0;
		memset(m_score, 0x00, sizeof(m_score));
		memset(m_tiPai, 0x00, sizeof(m_tiPai));
		m_bankerScore = 0;
		m_Id.clear();
		m_str.clear();
		m_str_playtype.clear();
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			m_handCard[i].clear();
		}
		m_oper.clear();
		m_restCard.clear();
	}

};

//录像结构
struct  QiPaiVideoLog :public QiPaiVideoLogBase
{
	//构造函数
	QiPaiVideoLog() :QiPaiVideoLogBase()
	{
		m_time = 0;
		m_zhuang = QIPAI_VIDEO_USER_MAX;
		m_fuZhuang = QIPAI_VIDEO_USER_MAX;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		memset(m_score, 0x00, sizeof(m_score));
		memset(m_userId, 0x00, sizeof(m_userId));
		memset(m_tiPai, 0x00, sizeof(m_tiPai));
		m_bankerScore = 0;
		m_flag = 0;
		m_oper.clear();

	}

	//析构函数
	~QiPaiVideoLog()
	{
	}

	//反序列化玩法小选项
	void PlayTypeFromString(const Lstring& str)
	{
		std::vector<Lstring>  str2;

		if (str.find(";") != std::string::npos)
		{
			L_ParseString(str, str2, ";");
			for (size_t j = 0; j < str2.size(); ++j)
			{
				m_playType.push_back(atoi(str2[j].c_str()));
			}
		}
	}

	/*
	反序列化入口，不同的玩法对应不同的反序列化函数
	flag 玩法类型 101斗地主 ... 111扯炫
	*/
	void FromString(Lstring str, Lint flag)
	{
		//斗地主
		if (101 == flag)
		{
			FromString_DouDiZhu(str);
		}
		//扯炫、三张牌、牛牛
		else if (111 == flag || 102 == flag || 108 == flag)
		{
			FromString_NoProcess(str);
		}
		//其他公用
		else
		{
			FromString_Comon(str);
		}
	}

	//斗地主反序列化
	void FromString_DouDiZhu(Lstring str)
	{
		std::vector<Lstring> str1, str2, str3;

		if (str.find("|") != std::string::npos)
		{
			L_ParseString(str, str2, "|");      //手牌；操作
			for (size_t j = 0; j < str2.size(); ++j)
			{
				//std::cout << "fromstring out::" << str2[j] << std::endl;
				str1.clear();
				L_ParseString(str2[j], str1, ";");

				if (j == 0)
				{
					if (str1.size() >= 2)
					{
						m_bankerScore = atoi(str1[0].c_str());
						str3.clear();
						L_ParseString(str1[1], str3, ",");
						for (int l = 0; l < str3.size() && l<3; ++l)
						{
							m_tiPai[l] = atoi(str3[l].c_str());
						}

					}
				}
				else if (j == 1)
				{
					for (int k = 0; k < str1.size() - 1 && k < QIPAI_VIDEO_USER_MAX; ++k)
					{
						str3.clear();
						L_ParseString(str1[k], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_handCard[k].push_back(atoi(str3[l].c_str()));
						}
					}
					if (1)
					{
						str3.clear();
						L_ParseString(str1[str1.size() - 1], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_restCard.push_back(atoi(str3[l].c_str()));
						}
					}

				}
				else
				{
					QiPaiVideoOper  qpvo;
					if (str1.size() >= 1)
						qpvo.m_pos = atoi(str1[0].c_str());
					if (str1.size() >= 2)
						qpvo.m_oper = atoi(str1[1].c_str());
					if (str1.size() >= 3)
						qpvo.m_score = atoi(str1[2].c_str());
					str3.clear();
					if (str1.size() >= 4)
					{
						L_ParseString(str1[3], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_cards.push_back(atoi(str3[l].c_str()));
					}

					str3.clear();
					if (str1.size() >= 5)
					{
						L_ParseString(str1[4], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_handcards.push_back(atoi(str3[l].c_str()));
					}
					if (str1.size() >= 6)
					{
						qpvo.m_sound = atoi(str1[5].c_str());
					}

					m_oper.push_back(qpvo);
				}
			}
		}
	}

	//扯炫反序列化108玩法
	void FromString_NiuNiu_New(Lstring str)
	{
		std::vector<Lstring> str1, str2;

		if (str.find(";") != std::string::npos)
		{
			L_ParseString(str, str1, ";");      //手牌
			for (size_t i = 0; i < str1.size(); ++i)
			{
				if (str1[i].find(",") != std::string::npos)
				{
					L_ParseString(str1[i], str2, ",");
					for (size_t j = 0; j < str2.size(); ++j)
					{
						m_handCard[i].push_back(atoi(str2[j].c_str()));
					}
				}
			}
		}

	}

	//扯炫反序列化111玩法
	void FromString_CheXuan(Lstring str)
	{
		std::vector<Lstring> str1, str2;

		if (str.find(";") != std::string::npos)
		{
			L_ParseString(str, str1, ";");      //手牌
			for (size_t i = 0; i < str1.size(); ++i)
			{
				if (str1[i].find(",") != std::string::npos)
				{
					L_ParseString(str1[i], str2, ",");
					for (size_t j = 0; j < str2.size(); ++j)
					{
						m_handCard[i].push_back(atoi(str2[j].c_str()));
					}
				}
			}
		}
	}

	//反序列化无过程的玩法Data：三张、牛牛、扯炫
	void FromString_NoProcess(Lstring str)
	{
		std::vector<Lstring> str1, str2;

		if (str.find(";") != std::string::npos)
		{
			L_ParseString(str, str1, ";");      //手牌
			for (size_t i = 0; i < str1.size(); ++i)
			{
				if (str1[i].find(",") != std::string::npos)
				{
					L_ParseString(str1[i], str2, ",");
					for (size_t j = 0; j < str2.size(); ++j)
					{
						m_handCard[i].push_back(atoi(str2[j].c_str()));
					}
				}
			}
		}
	}

	//通用反序列化，101,107，109,110玩法
	void FromString_Comon(Lstring str)
	{
		std::vector<Lstring> str1, str2, str3;

		if (str.find("|") != std::string::npos)
		{
			L_ParseString(str, str2, "|");      //手牌；炒作
			for (size_t j = 0; j < str2.size(); ++j)
			{
				//std::cout << "fromstring out::" << str2[j] << std::endl;
				str1.clear();
				L_ParseString(str2[j], str1, ";");

				if (j == 0)
				{
					if (str1.size() >= 2)
					{
						m_bankerScore = atoi(str1[0].c_str());
						str3.clear();
						L_ParseString(str1[1], str3, ",");
						for (int l = 0; l < str3.size() && l<3; ++l)
						{
							m_tiPai[l] = atoi(str3[l].c_str());
						}

					}
				}
				else if (j == 1)
				{
					for (int k = 0; k < str1.size() - 1 && k < QIPAI_VIDEO_USER_MAX; ++k)
					{
						str3.clear();
						L_ParseString(str1[k], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_handCard[k].push_back(atoi(str3[l].c_str()));
						}
					}
					if (1)
					{
						str3.clear();
						L_ParseString(str1[str1.size() - 1], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_restCard.push_back(atoi(str3[l].c_str()));
						}
					}

				}
				else
				{
					QiPaiVideoOper  qpvo;
					if (str1.size() >= 1)
						qpvo.m_pos = atoi(str1[0].c_str());
					if (str1.size() >= 2)
						qpvo.m_oper = atoi(str1[1].c_str());
					if (str1.size() >= 3)
						qpvo.m_score = atoi(str1[2].c_str());
					str3.clear();
					if (str1.size() >= 4)
					{
						L_ParseString(str1[3], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_cards.push_back(atoi(str3[l].c_str()));
					}

					str3.clear();
					if (str1.size() >= 5)
					{
						L_ParseString(str1[4], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_handcards.push_back(atoi(str3[l].c_str()));
					}
					if (str1.size() >= 6)
					{
						qpvo.m_sound = atoi(str1[5].c_str());
					}

					m_oper.push_back(qpvo);
				}
			}
		}
	}

	//填充玩家手牌
	void FillUserHandCards()
	{
		for (int i = 0; i < m_oper.size(); i++)
		{
			m_oper[i].m_userHandcards.m_handcards[0] = m_handCard[0];
			m_oper[i].m_userHandcards.m_handcards[1] = m_handCard[1];
			m_oper[i].m_userHandcards.m_handcards[2] = m_handCard[2];
			m_oper[i].m_userHandcards.m_handcards[3] = m_handCard[3];
			m_oper[i].m_userHandcards.m_handcards[4] = m_handCard[4];
			bool user0 = true;
			bool user1 = true;
			bool user2 = true;
			bool user3 = true;
			bool user4 = true;
			for (int k = i; k >= 0; k--)
			{
				if (m_oper[k].m_pos == 0 && user0)
				{
					m_oper[i].m_userHandcards.m_handcards[0].clear();
					m_oper[i].m_userHandcards.m_handcards[0] = m_oper[k].m_handcards;
					user0 = false;
				}
				else if (m_oper[k].m_pos == 1 && user1)
				{
					m_oper[i].m_userHandcards.m_handcards[1].clear();
					m_oper[i].m_userHandcards.m_handcards[1] = m_oper[k].m_handcards;
					user1 = false;
				}
				else if (m_oper[k].m_pos == 2 && user2)
				{
					m_oper[i].m_userHandcards.m_handcards[2].clear();
					m_oper[i].m_userHandcards.m_handcards[2] = m_oper[k].m_handcards;
					user2 = false;
				}
				else if (m_oper[k].m_pos == 3 && user3)
				{
					m_oper[i].m_userHandcards.m_handcards[3].clear();
					m_oper[i].m_userHandcards.m_handcards[3] = m_oper[k].m_handcards;
					user3 = false;
				}
				else if (m_oper[k].m_pos == 4 && user4)
				{
					m_oper[i].m_userHandcards.m_handcards[4].clear();
					m_oper[i].m_userHandcards.m_handcards[4] = m_oper[k].m_handcards;
					user4 = false;
				}
			}
		}
	}

	//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	void Write(LBuff& buff)
	{
		buff.Write(m_Id);
		buff.Write(m_time);
		buff.Write(m_userId[0]);
		buff.Write(m_userId[1]);
		buff.Write(m_userId[2]);
		buff.Write(m_userId[3]);
		buff.Write(m_userId[4]);
		buff.Write(m_userId[5]);
		buff.Write(m_userId[6]);
		buff.Write(m_userId[7]);
		buff.Write(m_userId[8]);
		buff.Write(m_userId[9]);
		buff.Write(m_zhuang);
		buff.Write(m_deskId);
		buff.Write(m_maxCircle);
		buff.Write(m_curCircle);
		buff.Write(m_score[0]);
		buff.Write(m_score[1]);
		buff.Write(m_score[2]);
		buff.Write(m_score[3]);
		buff.Write(m_score[4]);
		buff.Write(m_score[5]);
		buff.Write(m_score[6]);
		buff.Write(m_score[7]);
		buff.Write(m_score[8]);
		buff.Write(m_score[9]);
		buff.Write(m_flag);
		buff.Write(m_str);
		buff.Write(m_str_playtype);
		buff.Write(m_fuZhuang);
		//buff.Write(m_count);
		return;
	}
	//
	void Read(LBuff& buff)
	{
		buff.Read(m_Id);
		buff.Read(m_time);
		buff.Read(m_userId[0]);
		buff.Read(m_userId[1]);
		buff.Read(m_userId[2]);
		buff.Read(m_userId[3]);
		buff.Read(m_userId[4]);
		buff.Read(m_userId[5]);
		buff.Read(m_userId[6]);
		buff.Read(m_userId[7]);
		buff.Read(m_userId[8]);
		buff.Read(m_userId[9]);
		buff.Read(m_zhuang);
		buff.Read(m_deskId);
		buff.Read(m_maxCircle);
		buff.Read(m_curCircle);
		buff.Read(m_score[0]);
		buff.Read(m_score[1]);
		buff.Read(m_score[2]);
		buff.Read(m_score[3]);
		buff.Read(m_score[4]);
		buff.Read(m_score[5]);
		buff.Read(m_score[6]);
		buff.Read(m_score[7]);
		buff.Read(m_score[8]);
		buff.Read(m_score[9]);
		buff.Read(m_flag);
		buff.Read(m_str);
		buff.Read(m_str_playtype);
		buff.Read(m_fuZhuang);
		//buff.Read(m_count);
		return;
	}

	/*
	总序列化结构，根据不同玩法选择不同序列化函数入口
	flag 玩法类型
	*/
	Lstring ToString(Lint flag)
	{
		//扯炫、牛牛、三张牌
		if (111 == flag || 108 == flag || 102 == flag)
		{
			return ToString_NoProcess();
		}
		//其他
		else
		{
			return ToString_Comon();
		}
	}

	//新版牛牛序列化结构108
	Lstring ToString_NiuNiu_New()
	{
		std::stringstream ss;
		ss << HandCardToString();
		return ss.str();
	}

	//扯炫序列化结构111
	Lstring ToString_CheXuan()
	{
		std::stringstream ss;
		ss << HandCardToString();
		return ss.str();
	}

	//序列化无过程的玩法：牛牛、扯炫、三张牌等
	Lstring ToString_NoProcess()
	{
		std::stringstream ss;
		ss << HandCardToString();
		return ss.str();
	}

	// 公用序列化结构 101,107,109,110 
	Lstring ToString_Comon()
	{
		std::stringstream ss;
		ss << ZhuangScoreAndTiToString() << "|";
		ss << HandCardToString() << ";" << RestCardToString() << "|";
		for (int i = 0; i < m_oper.size(); i++)
		{
			ss << m_oper[i].ToString();
			if (i<m_oper.size() - 1)
				ss << "|";
		}
		//std::cout << "hhhhh..........................." << ss.str() << std::endl;
		return ss.str();
	}

	//序列化玩法小选项
	Lstring PlayTypeToStrint()
	{
		std::stringstream ss;
		for (Lsize i = 0; i < m_playType.size(); ++i)
		{
			ss << m_playType[i];
			if (i <m_playType.size() - 1)
				ss << ";";
		}
		//std::cout << "hhhhh...........................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx:" << ss.str() << std::endl;
		return ss.str();
	}

	//序列化手牌
	Lstring	HandCardToString()
	{
		std::stringstream ss;
		for (int i = 0; i< QIPAI_VIDEO_USER_MAX; i++)
		{
			for (int j = 0; j < m_handCard[i].size(); j++)
			{
				ss << m_handCard[i][j];
				if (j< m_handCard[i].size() - 1)
				{
					ss << ",";
				}
			}
			if (i<QIPAI_VIDEO_USER_MAX - 1)
				ss << ";";

			m_handCard[i].clear();
		}

		return ss.str();
	}

	//序列化底牌
	Lstring RestCardToString()
	{
		std::stringstream ss;
		for (int j = 0; j < m_restCard.size(); j++)
		{
			ss << m_restCard[j];
			if (j< m_restCard.size() - 1)
			{
				ss << ",";
			}
		}
		return ss.str();
	}

	//序列化庄家叫分与踢牌（斗地主）
	Lstring ZhuangScoreAndTiToString()
	{
		std::stringstream ss;
		ss << m_bankerScore;
		ss << ";";
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; ++i)
		{
			ss << m_tiPai[i];
			if (i < QIPAI_VIDEO_USER_MAX - 1)
				ss << ",";
		}
		return ss.str();
	}

	//获取写入数据库录像的数据SQL
	Lstring GetInsertSql()
	{
		//LTime t;
		std::stringstream ss;
		ss << "INSERT INTO video ";
		ss << "(Id, UserId1, Time, UserId2, UserId3, UserId4, UserId5, UserId6, UserId7, UserId8, UserId9, UserId10, ";
		ss << "Data, DeskId, Flag, CurCircle, MaxCircle, ";
		ss << "Score1, Score2, Score3, Score4, Score5, Score6, Score7, Score8, Score9, Score10, Zhuang, fuzhuang, PlayType) VALUES(";
		ss << "'" << m_Id << "',";
		ss << "'" << m_userId[0] << "',";
		ss << "'" << m_time << "',";
		for (int i = 1; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_userId[i] << "',";
		}
		ss << "'" << ToString(m_flag) << " ',";
		ss << "'" << m_deskId << "',";
		ss << "'" << m_flag << "',";
		ss << "'" << m_curCircle << "',";
		ss << "'" << m_maxCircle << "',";
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_score[i] << "',";
		}
		ss << "'" << m_zhuang << "',";
		ss << "'" << m_fuZhuang << "',";
		ss << "'" << PlayTypeToStrint() << "')";

		return ss.str();
	}

	void AddOper(Lint oper, Lint pos, std::vector<Lint>& cards, Lint score, std::vector<Lint> &handcards, Lint sound = -1)
	{
		QiPaiVideoOper  tempQPVideoOper;
		tempQPVideoOper.m_pos = pos;
		tempQPVideoOper.m_oper = oper;
		tempQPVideoOper.m_cards = cards;
		tempQPVideoOper.m_score = score;
		tempQPVideoOper.m_handcards = handcards;
		tempQPVideoOper.m_sound = sound;

		m_oper.push_back(tempQPVideoOper);

	}

	void FillDeskBaseInfo(Lint deskId, Lint flag, Lint max, const std::vector<Lint>& playType)
	{
		clear();
		m_deskId = deskId;
		//m_curCircle = cur;
		m_maxCircle = max;
		m_flag = flag;
		m_playType = playType;
		m_str_playtype = PlayTypeToStrint();

	}

	void DealCard(Lint cur, Lint userCount, Lint* userId, std::vector<Lint>* vec, std::vector<Lint> &retCard, Lint time = 0, Lint zhuang = 0, Lint zhuangScore = 0)
	{
		DrawClear();
		m_curCircle = cur;
		Lint userNum = QIPAI_VIDEO_USER_MAX;
		userNum = userNum > userCount ? userCount : userNum;
		for (int i = 0; i < userNum; i++)
		{
			m_userId[i] = userId[i];
			m_handCard[i] = vec[i];
		}
		m_time = time;
		m_restCard = retCard;
		m_zhuang = zhuang;
		m_bankerScore = zhuangScore;
	}

	void SetZhuang(std::vector<Lint> &retCard, Lint zhuang, Lint fuZhuang, Lint zhuangScore)
	{
		m_restCard = retCard;
		m_zhuang = zhuang;
		m_fuZhuang = fuZhuang;
		m_bankerScore = zhuangScore;
	}

	void doudizhuTiPai(Lint count, Lint *tipai)
	{
		for (int i = 0; i < count && i < QIPAI_VIDEO_USER_MAX; i++)
			m_tiPai[i] = tipai[i];
	}

	void addUserScore(Lint* score)
	{
		memcpy(m_score, score, sizeof(Lint) * QIPAI_VIDEO_USER_MAX);
	}

	//设置公共信息
	void setCommonInfo(Lint time, Lint curCircle, Lint userCount,
		Lint* userId, Lint zhuang = QIPAI_VIDEO_USER_MAX, Lint fuZhuang = QIPAI_VIDEO_USER_MAX)
	{
		m_time = time;
		m_curCircle = curCircle;
		Lint userNum = QIPAI_VIDEO_USER_MAX;
		userNum = userNum > userCount ? userCount : userNum;
		for (int i = 0; i < userNum; i++)
		{
			m_userId[i] = userId[i];
		}
		m_zhuang = zhuang;
		m_fuZhuang = fuZhuang;
	}

	//设置手牌
	void setHandCards(Lint userCount, std::vector<Lint>* vec)
	{
		Lint userNum = QIPAI_VIDEO_USER_MAX;
		userNum = userNum > userCount ? userCount : userNum;
		for (int i = 0; i < userNum; i++)
		{
			m_handCard[i] = vec[i];
		}
	}

	//设置本局得分
	void setRoundScore(Lint userCount, Lint* userRoundScore)
	{
		Lint userNum = QIPAI_VIDEO_USER_MAX;
		userNum = userNum > userCount ? userCount : userNum;
		for (int i = 0; i < userNum; ++i)
		{
			m_score[i] = userRoundScore[i];
		}
	}
};

//三打二录像结构107
struct QiPaiVideoLog_SanDaEr : public QiPaiVideoLogBase
{
	//构造函数
	QiPaiVideoLog_SanDaEr() : QiPaiVideoLogBase()
	{
		m_time = 0;
		m_zhuang = 0;
		m_fuZhuang = 0;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		memset(m_score, 0x00, sizeof(m_score));
		memset(m_userId, 0x00, sizeof(m_userId));
		memset(m_tiPai, 0x00, sizeof(m_tiPai));
		m_bankerScore = 0;
		m_flag = 0;
		m_oper.clear();
	}

	//析构函数
	~QiPaiVideoLog_SanDaEr()
	{
	}

	//反序列化
	void FromString(Lstring str)
	{
		std::vector<Lstring> str1, str2, str3;

		if (str.find("|") != std::string::npos)
		{
			L_ParseString(str, str2, "|"); //手牌；炒作
			for (size_t j = 0; j < str2.size(); ++j)
			{
				//std::cout << "fromstring out::" << str2[j] << std::endl;
				str1.clear();
				L_ParseString(str2[j], str1, ";");

				if (j == 0)
				{
					if (str1.size() >= 2)
					{
						m_bankerScore = atoi(str1[0].c_str());
						str3.clear();
						L_ParseString(str1[1], str3, ",");
						for (int l = 0; l < str3.size() && l < 3; ++l)
						{
							m_tiPai[l] = atoi(str3[l].c_str());
						}
					}
				}
				else if (j == 1)
				{
					for (int k = 0; k < str1.size() && k < QIPAI_VIDEO_USER_MAX; ++k)
					{
						str3.clear();
						L_ParseString(str1[k], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_handCard[k].push_back(atoi(str3[l].c_str()));
						}
					}
					if (str1.size() > QIPAI_VIDEO_USER_MAX)
					{
						str3.clear();
						L_ParseString(str1[QIPAI_VIDEO_USER_MAX], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
						{
							if (atoi(str3[l].c_str()) != 0)
								m_restCard.push_back(atoi(str3[l].c_str()));
						}
					}
				}
				else
				{
					QiPaiVideoOper qpvo;
					if (str1.size() >= 1)
						qpvo.m_pos = atoi(str1[0].c_str());
					if (str1.size() >= 2)
						qpvo.m_oper = atoi(str1[1].c_str());
					if (str1.size() >= 3)
						qpvo.m_score = atoi(str1[2].c_str());

					str3.clear();
					if (str1.size() >= 4)
					{
						L_ParseString(str1[3], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_cards.push_back(atoi(str3[l].c_str()));
					}

					str3.clear();
					if (str1.size() >= 5)
					{
						L_ParseString(str1[4], str3, ",");
						for (int l = 0; l < str3.size(); ++l)
							qpvo.m_handcards.push_back(atoi(str3[l].c_str()));
					}

					m_oper.push_back(qpvo);
				}
			}
		}
	}

	//填充玩家手牌
	void FillUserHandCards()
	{
		for (int i = 0; i < m_oper.size(); i++)
		{
			m_oper[i].m_userHandcards.m_handcards[0] = m_handCard[0];
			m_oper[i].m_userHandcards.m_handcards[1] = m_handCard[1];
			m_oper[i].m_userHandcards.m_handcards[2] = m_handCard[2];
			m_oper[i].m_userHandcards.m_handcards[3] = m_handCard[3];
			m_oper[i].m_userHandcards.m_handcards[4] = m_handCard[4];
			bool user0 = true;
			bool user1 = true;
			bool user2 = true;
			bool user3 = true;
			bool user4 = true;
			for (int k = i; k >= 0; k--)
			{
				if (m_oper[k].m_pos == 0 && user0)
				{
					m_oper[i].m_userHandcards.m_handcards[0].clear();
					m_oper[i].m_userHandcards.m_handcards[0] = m_oper[k].m_handcards;
					user0 = false;
				}
				else if (m_oper[k].m_pos == 1 && user1)
				{
					m_oper[i].m_userHandcards.m_handcards[1].clear();
					m_oper[i].m_userHandcards.m_handcards[1] = m_oper[k].m_handcards;
					user1 = false;
				}
				else if (m_oper[k].m_pos == 2 && user2)
				{
					m_oper[i].m_userHandcards.m_handcards[2].clear();
					m_oper[i].m_userHandcards.m_handcards[2] = m_oper[k].m_handcards;
					user2 = false;
				}
				else if (m_oper[k].m_pos == 3 && user3)
				{
					m_oper[i].m_userHandcards.m_handcards[3].clear();
					m_oper[i].m_userHandcards.m_handcards[3] = m_oper[k].m_handcards;
					user3 = false;
				}
				else if (m_oper[k].m_pos == 4 && user4)
				{
					m_oper[i].m_userHandcards.m_handcards[4].clear();
					m_oper[i].m_userHandcards.m_handcards[4] = m_oper[k].m_handcards;
					user4 = false;
				}
			}
		}
	}

	//反序列化玩法小选项
	void PlayTypeFromString(const Lstring &str)
	{
		std::vector<Lstring> str2;

		if (str.find(";") != std::string::npos)
		{
			L_ParseString(str, str2, ";");
			for (size_t j = 0; j < str2.size(); ++j)
			{
				m_playType.push_back(atoi(str2[j].c_str()));
			}
		}
	}

	//序列化手牌
	Lstring HandCardToString()
	{
		std::stringstream ss;
		for (size_t i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			for (int j = 0; j < m_handCard[i].size(); j++)
			{
				ss << m_handCard[i][j];
				if (j < m_handCard[i].size() - 1)
				{
					ss << ",";
				}
			}
			if (i < QIPAI_VIDEO_USER_MAX - 1)
				ss << ";";
		}

		return ss.str();
	}

	//序列化底牌
	Lstring RestCardToString()
	{
		std::stringstream ss;
		for (int j = 0; j < m_restCard.size(); j++)
		{
			ss << m_restCard[j];
			if (j < m_restCard.size() - 1)
			{
				ss << ",";
			}
		}
		return ss.str();
	}

	//序列化玩法小选项
	Lstring PlayTypeToStrint()
	{
		std::stringstream ss;
		for (Lsize i = 0; i < m_playType.size(); ++i)
		{
			ss << m_playType[i];
			if (i < m_playType.size() - 1)
				ss << ";";
		}
		//std::cout << "hhhhh...........................xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx:" << ss.str() << std::endl;
		return ss.str();
	}

	//序列化庄家叫分与踢牌（斗地主）
	Lstring ZhuangScoreAndTiToString()
	{
		std::stringstream ss;
		ss << m_bankerScore;
		ss << ";";
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; ++i)
		{
			ss << m_tiPai[i];
			if (i < QIPAI_VIDEO_USER_MAX - 1)
				ss << ",";
		}
		return ss.str();
	}

	//总序列化结构
	Lstring ToString()
	{
		std::stringstream ss;
		ss << ZhuangScoreAndTiToString() << "|";
		ss << HandCardToString() << ";" << RestCardToString() << "|";
		for (int i = 0; i < m_oper.size(); i++)
		{
			ss << m_oper[i].ToString();
			if (i < m_oper.size() - 1)
				ss << "|";
		}
		//std::cout << "hhhhh..........................." << ss.str() << std::endl;
		return ss.str();
	}

	//获取写入数据库录像的数据SQL
	Lstring GetInsertSql()
	{
		std::stringstream ss;
		ss << "INSERT INTO";
		ss << " video ";
		ss << "(Id, UserId1, Time, UserId2, UserId3, UserId4, UserId5, Data, DeskId, Flag, CurCircle, MaxCircle, Score1, Score2, Score3, Score4, Score5, Zhuang, fuzhuang, PlayType) VALUES (";
		ss << "'" << m_Id << "',";
		ss << "'" << m_userId[0] << "',";
		ss << "'" << m_time << "',";
		for (int i = 1; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_userId[i] << "',";
		}
		ss << "'" << ToString() << " ',";
		ss << "'" << m_deskId << "',";
		ss << "'" << m_flag << "',";
		ss << "'" << m_curCircle << "',";
		ss << "'" << m_maxCircle << "',";
		for (int i = 0; i < QIPAI_VIDEO_USER_MAX; i++)
		{
			ss << "'" << m_score[i] << "',";
		}
		ss << "'" << m_zhuang << "',";
		ss << "'" << m_fuZhuang << "',";
		ss << "'" << PlayTypeToStrint() << "')";

		return ss.str();
	}

	void AddOper(Lint oper, Lint pos, std::vector<Lint> &cards, Lint score, std::vector<Lint> &handcards, Lint sound = -1)
	{
		QiPaiVideoOper tempQPVideoOper;
		tempQPVideoOper.m_pos = pos;
		tempQPVideoOper.m_oper = oper;
		tempQPVideoOper.m_cards = cards;
		tempQPVideoOper.m_score = score;
		tempQPVideoOper.m_handcards = handcards;

		m_oper.push_back(tempQPVideoOper);
	}

	void FillDeskBaseInfo(Lint deskId, Lint flag, Lint max, const std::vector<Lint> &playType)
	{
		clear();
		m_deskId = deskId;
		//m_curCircle = cur;
		m_maxCircle = max;
		m_flag = flag;
		m_playType = playType;
		m_str_playtype = PlayTypeToStrint();
	}

	void DealCard(Lint cur, Lint userCount, Lint *userId, std::vector<Lint> *vec, Lint time/*, std::vector<Lint> &retCard, Lint zhuang, Lint zhuangScore*/)
	{
		DrawClear();
		m_curCircle = cur;
		Lint userNum = QIPAI_VIDEO_USER_MAX;
		userNum = userNum > userCount ? userCount : userNum;
		for (int i = 0; i < userNum; i++)
		{
			m_userId[i] = userId[i];
			m_handCard[i] = vec[i];
		}
		m_time = time;
		/*m_restCard = retCard;
		m_zhuang = zhuang;
		m_bankerScore = zhuangScore;*/
	}

	void SetZhuang(std::vector<Lint> &retCard, Lint zhuang, Lint fuzhuang, Lint zhuangScore)
	{
		m_restCard = retCard;
		m_zhuang = zhuang;
		m_fuZhuang = fuzhuang;
		m_bankerScore = zhuangScore;
	}

	void doudizhuTiPai(Lint count, Lint *tipai)
	{
		for (int i = 0; i < count && i < QIPAI_VIDEO_USER_MAX; i++)
			m_tiPai[i] = tipai[i];
	}

	void addUserScore(Lint *score)
	{
		memcpy(m_score, score, sizeof(Lint) * QIPAI_VIDEO_USER_MAX);
	}

	//
	void Read(LBuff &buff)
	{
		buff.Read(m_Id);
		buff.Read(m_time);
		buff.Read(m_userId[0]);
		buff.Read(m_userId[1]);
		buff.Read(m_userId[2]);
		buff.Read(m_userId[3]);
		buff.Read(m_userId[4]);
		buff.Read(m_zhuang);
		buff.Read(m_deskId);
		buff.Read(m_maxCircle);
		buff.Read(m_curCircle);
		buff.Read(m_score[0]);
		buff.Read(m_score[1]);
		buff.Read(m_score[2]);
		buff.Read(m_score[3]);
		buff.Read(m_score[4]);
		buff.Read(m_flag);
		buff.Read(m_str);
		buff.Read(m_str_playtype);
		//buff.Read(m_count);
		return;
	}

	//
	void Write(LBuff &buff)
	{
		buff.Write(m_Id);
		buff.Write(m_time);
		buff.Write(m_userId[0]);
		buff.Write(m_userId[1]);
		buff.Write(m_userId[2]);
		buff.Write(m_userId[3]);
		buff.Write(m_userId[4]);
		buff.Write(m_zhuang);
		buff.Write(m_deskId);
		buff.Write(m_maxCircle);
		buff.Write(m_curCircle);
		buff.Write(m_score[0]);
		buff.Write(m_score[1]);
		buff.Write(m_score[2]);
		buff.Write(m_score[3]);
		buff.Write(m_score[4]);
		buff.Write(m_flag);
		buff.Write(m_str);
		buff.Write(m_str_playtype);
		//buff.Write(m_count);
		return;
	}


};

//服务器返回回放内容 msg_id = 62106
struct LMsgS2CPokerVideo :public LMsgSC
{
	QiPaiVideoLog		m_video;//log信息
	Lint			m_sex[QIPAI_VIDEO_USER_MAX];
	Lstring			m_nike[QIPAI_VIDEO_USER_MAX];
	Lstring			m_imageUrl[QIPAI_VIDEO_USER_MAX];
	Lint			m_zhuang;
	Lint			m_fuZhuang;
	Lint			m_deskId;
	Lint			m_curCircle;
	Lint			m_maxCircle;
	Lint			m_flag;
	Lint			m_pos;
	Lint			m_score[QIPAI_VIDEO_USER_MAX];

	LMsgS2CPokerVideo() :LMsgSC(MSG_S_2_C_POKER_RESPOND_VIDEO_ID)
	{
		memset(m_sex, 0, sizeof(m_sex));
		m_zhuang = QIPAI_VIDEO_USER_MAX;
		m_fuZhuang = QIPAI_VIDEO_USER_MAX;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		m_flag = 0;
		m_pos = 0;
		memset(m_score, 0, sizeof(m_score));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 19 + QIPAI_VIDEO_USER_MAX);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(kZhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(kCurCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(kMaxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kZhuangScore), m_video.m_bankerScore);


		std::vector<Lint>vecid;
		std::vector<Lint>vecsex;
		std::vector<Lint>vecscore;
		std::vector<Lstring>vecnike;
		std::vector<Lstring>vecurl;
		std::vector<Lint> vectipai;

		for (Lint i = 0; i < QIPAI_VIDEO_USER_MAX; ++i)
		{
			if (m_video.m_userId[i] == 0)
			{
				continue;
			}
			vecid.push_back(m_video.m_userId[i]);
			vecnike.push_back(m_nike[i]);
			vecsex.push_back(m_sex[i]);
			vecurl.push_back(m_imageUrl[i]);
			vecscore.push_back(m_score[i]);
			vectipai.push_back(m_video.m_tiPai[i]);
		}

		WriteKey(pack, "kTipai");
		WriteKey(pack, vectipai);

		WriteKey(pack, "kScore");
		WriteKey(pack, vecscore);

		WriteKey(pack, "kUserid");
		WriteKey(pack, vecid);

		WriteKey(pack, "kNike");
		WriteKey(pack, vecnike);

		WriteKey(pack, "kSex");
		WriteKey(pack, vecsex);

		WriteKey(pack, "kImageUrl");
		WriteKey(pack, vecurl);

		WriteKey(pack, "kTime");
		WriteKey(pack, m_video.m_time);

		WriteKey(pack, "kOper");
		WriteKey(pack, m_video.m_oper);

		WriteKey(pack, "kPlaytype");
		WriteKey(pack, m_video.m_playType);

		WriteKey(pack, "kRestCard");
		WriteKey(pack, m_video.m_restCard);

		for (Lint i = 0; i < QIPAI_VIDEO_USER_MAX; ++i)
		{
			std::stringstream ss;
			ss << "kCard" << i;
			WriteKey(pack, ss.str());
			WriteKey(pack, m_video.m_handCard[i]);
		}
		//副庄家位置
		WriteKeyValue(pack, NAME_TO_STR(kFuZhuang), m_fuZhuang);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPokerVideo(); }

};

//////////////////////////////////////////////////////////////////////////
struct LMsgS2CWaitJoinRoom :public LMsgSC
{
	Lint            m_ErrorCode;
	Lint				m_MaxUser;
	Lint				m_CurrentUser;
	Lint            m_Time;            //倒计时
	Lstring     m_UseGpsList;

	LMsgS2CWaitJoinRoom() :LMsgSC(MSG_S_2_C_POKER_WAIT_JOIN_ROOM)
	{
		m_MaxUser = 0;
		m_CurrentUser = 0;
		m_Time = 0;
		m_ErrorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_Time);
		WriteKeyValue(pack, NAME_TO_STR(kMaxmax), m_MaxUser);
		WriteKeyValue(pack, NAME_TO_STR(kcurrent), m_CurrentUser);
		WriteKeyValue(pack, NAME_TO_STR(kUserGPSList), m_UseGpsList);
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CWaitJoinRoom(); }
};


//////////////////////////////////////////////////////////////////////////

struct LMsgC2SPokerExitWait :public LMsgSC
{
	Lint	 m_clubId;
	Lint	 m_playTypeId;

	LMsgC2SPokerExitWait() :LMsgSC(MSG_C_2_S_POKER_EXIT_WAIT)
	{
		m_clubId = 0;
		m_playTypeId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, "kClubId", m_clubId);
		ReadMapData(obj, "kPlayTypeId", m_playTypeId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SPokerExitWait();
	}
};


//////////////////////////////////////////////////////////////////////////

struct LMsgS2CPokerExitWait :public LMsgSC
{
	Lint				m_errorCode;
	Lint            m_type;
	  
	LMsgS2CPokerExitWait() :LMsgSC(MSG_S_2_C_POKER_EXIT_WAIT)
	{
		m_errorCode = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
	
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPokerExitWait(); }
};

//////////////////////////////////////////////////////////////////////////
//
struct LMsgC2SRequestUserGps :public LMsgSC
{
	Lint	 m_userId;


	LMsgC2SRequestUserGps() :LMsgSC(MSG_C_2_S_REQUEST_USER_GPS)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kUserId", m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SRequestUserGps();
	}
};

//////////////////////////////////////////////////////////////////////////

struct LMsgS2CRequestUserGps :public LMsgSC
{
	Lint				m_errorCode;
	Lint            m_userId;
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	LMsgS2CRequestUserGps() :LMsgSC(MSG_S_2_C_REQUEST_USER_GPS)
	{
		m_errorCode = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		WriteKeyValue(pack, NAME_TO_STR(kGpsLat), m_Gps_Lat);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CRequestUserGps(); }
};




//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
enum PHONE_LOGIN_TYPE
{
	PHONE_LOGIN_TYPE_MSG = 1,
	PHONE_LOGIN_TYPE_VOICE = 2,
	PHONE_LOGIN_TYPE_CHECK = 3,
};
//手机登录，客户端发送命令
struct LMsgC2SPhoneLogin :public LMsgSC
{
	Lint m_type;                 //  1--短信验证  2-电话验证   3-登录（验证码检查）
	Lstring m_Phone;      //手机号
	Lstring m_Vcode;      //验证码（kType ==1,2的时候，不去检查这个值）
	Lint       m_serverId;     //服务器Id

	LMsgC2SPhoneLogin() :LMsgSC(MSG_C_2_S_PHONE_LOGIN)
	{
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kPhone), m_Phone);
		ReadMapData(obj, NAME_TO_STR(kVcode), m_Vcode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kPhone), m_Phone);
		WriteKeyValue(pack, NAME_TO_STR(kVcode), m_Vcode);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SPhoneLogin(); }
};

///////////////////////////////////////////////////////////////////////////
enum PHONE_LOGIN_ERROR
{
	PHONE_LOGIN_ERROR_NOT_BINDING=11,      // 11-手机号还没有绑定，微信登录
	PHONE_LOGIN_ERROR_VCODE_NOT_COMPARE=12,           //12-验证码不匹配
	PHONE_LOGIN_ERROR_PHONE_NUM_ERROR=13,    //13-手机号位数不对
	PHONE_LOGIN_ERROR_VCODE_FIAL=14,               //14-验证码获取失败
	PHONE_LOGIN_ERROR_VCODE_EXPIRE=15,            //15-验证码超时
	PHONE_LOGIN_ERROR_TIME_NOT=16,              //60秒不能重复获取验证码
};

//服务器返回结果   验证返回的
struct LMsgS2CPhoneLogin1 :public LMsgSC
{
	Lint  m_error_code;                                   //0-成功，等待 11-手机号还没有绑定，微信登录    12-手机号位数不对
	Lint  m_type;                                              //  1--短信验证  2-电话验证 
	LMsgS2CPhoneLogin1() : LMsgSC(MSG_S_2_C_PHONE_LOGIN_1)
	{
		m_error_code = 0;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kErrorCode), m_error_code);
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error_code);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPhoneLogin1(); }
};


///////////////////////////////////////////////////////////////////////////
//服务器返回结果   验证返回的
struct LMsgS2CPhoneLogin2 :public LMsgSC
{
	Lint  m_error_code;                                  //0-成功， 11-手机号还没有绑定，微信登录  12-验证码失败    13-手机号位数不对      1-服务器还没启动成功 4-创建角色失败 5- 在原APP未退,6-错误的地区号,7-MD5验证失败,9-账号被封    
	Lint  m_type;                                              //  1--短信验证  2-电话验证 

															   //用户微信等信息 +8
	Lstring			m_openId;
	Lstring			m_nike;
	Lstring			m_sign;
	Lint			m_severID;
	Lstring			m_uuid;
	Lint			m_sex;
	Lstring			m_imageUrl;
	Lstring			m_nikename;

	//用户返回的信息 +8
	Lint			m_seed;
	Lint			m_id;
	Lstring			m_gateIp;
	Lshort			m_gatePort;
	Lstring			m_errorMsg;	//若登陆失败，返回玩家错误原因;
	Lint            m_totalPlayNum;
	Lint            m_TotalCircleNum;
	Lint            m_totalWinNum;

	LMsgS2CPhoneLogin2() : LMsgSC(MSG_S_2_C_PHONE_LOGIN_2)
	{
		m_error_code = 0;
		m_type = 0;
		m_severID = 0;
		m_sex = 0;
		m_seed = 0;
		m_id = 0;
		m_totalPlayNum = 0;
		m_TotalCircleNum = 0;
		m_totalWinNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kErrorCode), m_error_code);
		//ReadMapData(obj, NAME_TO_STR(kType), m_type);


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 19);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_error_code);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);

		WriteKeyValue(pack, "kOpenId", m_openId);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kSign", m_sign);
		WriteKeyValue(pack, "kSeverID", m_severID);
		WriteKeyValue(pack, "kUuid", m_uuid);
		WriteKeyValue(pack, "kSex", m_sex);
		WriteKeyValue(pack, "kImageUrl", m_imageUrl);
		WriteKeyValue(pack, "kNikename", m_nikename);

		WriteKeyValue(pack, "kSeed", m_seed);
		WriteKeyValue(pack, "kId", m_id);
		WriteKeyValue(pack, "kGateIp", m_gateIp);
		WriteKeyValue(pack, "kGatePort", m_gatePort);
		WriteKeyValue(pack, "kErrorMsg", m_errorMsg);
		WriteKeyValue(pack, "kTotalPlayNum", m_totalPlayNum);
		WriteKeyValue(pack, "kTotalCircleNum", m_TotalCircleNum);
		WriteKeyValue(pack, "kTotalWinNum", m_totalWinNum);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPhoneLogin2(); }
};


//////////////////////////////////////////////////////////////////////////
///客户端请求能量查询
struct LMsgC2SPowerPointList :public LMsgSC
{
	Lint m_clubId;
	Lstring m_search;
	std::vector<Lint>  m_oldUser;    //客户端已经收到的用户id
	
	LMsgC2SPowerPointList() :LMsgSC(MSG_C_2_S_POWER_POINT_LIST)
	{
		m_clubId = 0;
		//m_searchId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kClubId", m_clubId);
		ReadMapData(obj, "kSearchId", m_search);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(kOldUser), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_oldUser.push_back(v);
			}
		}


		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SPowerPointList();
	}
};

//////////////////////////////////////////////////////////////////////////
///能量列表返回
struct UserPowerInfo
{
	Lint m_userId;
	Lstring m_nike;
	Lstring m_photo;
	Lint       m_state;
	Lint       m_point;
	Lint       m_type;

	MSGPACK_DEFINE(m_userId, m_nike, m_photo,m_state,m_point, m_type);

	UserPowerInfo()
	{
		m_userId = 0;
		m_point = 0;
		m_state = 0;
		m_type = 0;
	}

	UserPowerInfo(Lint userId, Lint point)
	{
		m_userId = userId;
		m_point = point;
		m_state = 0;
		m_type = 0;
	}

	void print()
	{
		LLOG_ERROR(" list user[%d] name[%s] photo[%s] state[%d] point[%d] type[%d]",m_userId, m_nike.c_str(),m_photo.c_str(),m_state,m_point,m_type);
	}
	
};

struct LMsgS2CPowerPointList :public LMsgSC
{
	Lint				m_errorCode;          //错误码  0-成功 1-没有数据  2-俱乐部id不存在  3-userId 不为0，但该userId不是该俱乐部成员 4--没有权限请求列表
	Lint            m_clubId;
	Lint            m_count;
	Lint            m_HasSendAll;
	Lint            m_isChange;
	Lint            m_type;           //  =1  是其他用户只为空

	std::vector<UserPowerInfo>  m_userList;

	LMsgS2CPowerPointList() :LMsgSC(MSG_S_2_C_POWER_POINT_LIST)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_count = 0;
		m_HasSendAll = 0;
		m_isChange = 0;
		m_type = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kCount), m_count);
		WriteKeyValue(pack, NAME_TO_STR(kHasSendAll), m_HasSendAll);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kIsChange), m_isChange);
		WriteKeyValue(pack, NAME_TO_STR(kUserList), m_userList);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPowerPointList(); }
};

//////////////////////////////////////////////////////////////////////////
///客户端请求能量记录查询
struct LMsgC2SRequestPointRecord :public LMsgSC
{
	Lint m_clubId;
	Lstring m_search;
	Lint m_type;
	Lint m_time;
	Lint m_page;

	LMsgC2SRequestPointRecord() :LMsgSC(MSG_C_2_S_REQUEST_POINT_RECORD)
	{
		m_clubId = 0;
		m_type = 1;
		m_time = 0;
		m_page = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kClubId", m_clubId);
		ReadMapData(obj, "kSearch", m_search);
		ReadMapData(obj, "kType", m_type);
		ReadMapData(obj, "kTime", m_time);
		ReadMapData(obj, "kPage", m_page);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SRequestPointRecord();
	}
};

//////////////////////////////////////////////////////////////////////////
///能量列表返回
struct UserPowerRecord
{
public:   //通过函数调用，不用直接赋值
	Lint m_id;
	Lint  m_time;
	Lint m_userId;    
	Lint  m_type;       // 增加，减少
	Lint   m_point;    //变化的能量值
	Lint  m_operId;
	

public:
	Lstring m_nike;
	Lstring m_photo;


	Lstring m_operNike;
	Lstring m_operPhoto;
	Lstring m_operR;           //

	Lint m_crruPoint;        //当前最新能量值,没有用的时候要最新的

	Lint m_clubId;
	Lint m_clubSq;

	MSGPACK_DEFINE(m_time,m_userId, m_nike, m_photo, m_point, m_crruPoint,m_operId, m_operNike, m_operPhoto, m_operR, m_type);

	UserPowerRecord(Lint time,Lint userId,Lint type,Lint point,Lint operId)
	{
		m_id = 0;
		m_time = time;
		m_userId = userId;
		m_point = point;
		m_operId = operId;
		m_type = type;
		m_crruPoint = 0;
		m_clubId = 0;
		m_clubSq = 0;
	}

	UserPowerRecord()
	{
		m_id = 0;
		m_time = 0;
		m_userId = 0;
		m_point = 0;
		m_operId = 0;
		m_crruPoint = 0;
		m_type = 0;
		m_clubId = 0;
		m_clubSq = 0;
	}

	~UserPowerRecord(){}

	Lint getUserId() { return m_userId; }
	Lint getOperId() { return m_operId; }
	Lint getPoint() { return m_point; }
	Lint getTime() { return m_time; }

	UserPowerRecord& operator =(const UserPowerRecord& record)
	{
		m_id = record.m_id;
		m_time = record.m_time;
		m_userId = record.m_userId;
		m_nike = record.m_nike;
		m_photo = record.m_photo;
		m_point = record.m_point;
		m_type = record.m_type;
		m_crruPoint = record.m_crruPoint;

		m_operId = record.m_operId;
		m_operNike = record.m_operNike;
		m_operPhoto = record.m_operPhoto;
		m_operR = record.m_operR;

		m_clubId = record.m_clubId;
		m_clubSq = record.m_clubSq;

		return *this;
	}

	void setMode1(UserPowerRecord & record,Lint currPoint)
	{
	
		m_time = record.getTime();

		//m_userId = record.get()->getUserId();
		//m_nike = record.get()->m_nike;
		m_type = record.m_type;
		m_point = record.getPoint();
		m_crruPoint = currPoint;
		
		m_operId = record.getOperId();
		m_operNike = record.m_operNike;
		//m_operR = record.get()->m_operR;
	}

	void setMode2(UserPowerRecord & record, Lint currPoint=0)
	{
		m_time = record.getTime();

		m_userId = record.getUserId();
		m_nike = record.m_nike;
		m_type = record.m_type;
		m_point = record.getPoint();
		m_crruPoint = record.m_crruPoint;

		m_operId = record.getOperId();
		m_operNike = record.m_operNike;
		m_operR = record.m_operR;
	}

};

struct LMsgS2CReplyPointRecord :public LMsgSC
{
	Lint				m_errorCode;    //错误码  0-成功 1-没有数据  2-俱乐部id不存在  3-userId 不为0，但该userId不是该俱乐部成员
	Lint            m_clubId;
	Lint            m_count;
	Lint            m_type;
	Lint            m_mode;
	Lint            m_point;                   //查询用户当前的能量值
	Lint            m_HasSendAll;

	std::vector<UserPowerRecord>  m_List;

	LMsgS2CReplyPointRecord() :LMsgSC(MSG_S_2_C_REPLY_POINT_RECORD)
	{
		m_errorCode = 0;
		m_clubId = 0;
		m_count = 0;
		m_HasSendAll = 0;
		m_type = 0;
		m_mode = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kType), m_type);
		WriteKeyValue(pack, NAME_TO_STR(kMode), m_mode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);
		WriteKeyValue(pack, NAME_TO_STR(kPoint), m_point);
		WriteKeyValue(pack, NAME_TO_STR(kCount), m_count);
		WriteKeyValue(pack, NAME_TO_STR(kHasSendAll), m_HasSendAll);
		WriteKeyValue(pack, NAME_TO_STR(kList), m_List);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CReplyPointRecord(); }
};



//////////////////////////////////////////////////////////////////////////
enum CHAGNE_POINT_TYPE
{
	CHANGE_POINT_TYPE_ADD=1,
	CHANGE_POINT_TYPE_REDUCE=2,
};

//俱乐部能量操作
struct LMsgC2SChangePoint :public LMsgSC
{
	Lint m_type;                 // 1 -加  2-减	
	Lint m_clubId;
	Lint m_userId;
	Lstring m_operR;
	Lint m_operId;
	Lint m_point;
	Lint m_time;
	

	LMsgC2SChangePoint() :LMsgSC(MSG_C_2_S_CHANGE_POINT)
	{
		m_type = 0;
		m_clubId = 0;
		m_userId = 0;
		m_point = 0;
		m_time = 0;
		m_operId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kType), m_type);
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kPoint), m_point);
		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		ReadMapData(obj, NAME_TO_STR(kOperR), m_operR);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SChangePoint(); }
};

///////////////////////////////////////////////////////////////////////////
//服务器返回结果   验证返回的
struct LMsgS2CChangePoint :public LMsgSC
{
	Lint  m_errorCode;                                   //错误码  0-成功 1-失败，后台原因  2-俱乐部id不存在  3-该userId不是该俱乐部成员  4:没有权限  5:没有写更改原因 6：在游戏中，稍后重试  7-分数为负
	Lint  m_clubId;                                              

	UserPowerInfo  m_userPInfo;

	LMsgS2CChangePoint() : LMsgSC(MSG_S_2_C_CHANGE_POINT)
	{
		m_errorCode = 0;
		m_clubId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kErrorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(kClubId), m_clubId);

		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userPInfo.m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kNike), m_userPInfo.m_nike);
		//WriteKeyValue(pack, NAME_TO_STR(kPhoto), m_userPInfo.m_photo);
		WriteKeyValue(pack, NAME_TO_STR(kState), m_userPInfo.m_state);
		WriteKeyValue(pack, NAME_TO_STR(kPoint), m_userPInfo.m_point);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CChangePoint(); }
};

///////////////////////////////////////////////////////////////////////////


#endif