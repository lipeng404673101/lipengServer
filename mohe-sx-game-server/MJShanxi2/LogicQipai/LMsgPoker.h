#ifndef _LMSG_POKER_SC_H_
#define _LMSG_POKER_SC_H_

//#include "LMsg.h"
#include "LMsgBase.h"
#include "mhgamedef.h"
//斗地主
#include "doudizhu\CMD_Game.h"
#include "doudizhu\CMD_Game_Linfen.h"

//新牛牛
#include "newNiuNiu\newNiuNiuMsg.h"
//扯炫
#include "CheXuan\CheXuanMsg.h"




//////////////////////////////////////////////////////////////////////////
//  扑克游戏内部命令 --通用的
#define  INVAILD_SUB_CMD           0

///////////////////////////////////////////////////////////////////
/////////////////////begin 62001-63000  关于扑克,client--><--server 


enum  POKER_MSG_ID
{
	//棋牌新添加的消息号
	//MH_MSG_S_2_C_USER_DESK_COMMAND = 61200, // 桌子级别总命令码 

	//三张牌  62001-62050
	MSG_C_2_S_POKER_GAME_MESSAGE = 62001,   //扑克游戏  -可以根据子命令区分
	MSG_S_2_C_POKER_GAME_MESSAGE = 62010,  //扑克游戏  -可以根据子命令区分

	MSG_S_2_C_POKER_GAME_END_RESULT = 62002,      //服务器发送扑克游戏大结算命令

	MSG_C_2_S_POKER_ROOM_LOG = 62003,                     //请求战绩记录-match
	MSG_S_2_C_POKER_ROOM_LOG = 62004,                     //服务端返回战绩记录-match

	MSG_C_2_S_POKER_MATCH_LOG = 62005,                  //请求一场比赛每局战绩
	MSG_S_2_C_POKER_MATCH_LOG = 62006,

	MSG_C_2_S_POKER_RECONNECT = 62007,               //断线重连

	MSG_C_2_S_YINGSANZHANG_ADD_SCORE = 62012,                //用户加注

	MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL = 62013,  //客户端发给服务器玩家买大买小操作结果
	MSG_YINGSANZHANG_S_2_C_USER_SELECT_BIGSMALL = 62014,  //服务器广播玩家买大买小的操作结果

	MSG_YINGSANZHANG_S_2_C_CHANGE_BIGSMALL_MODE = 62015,  //服务器改变买大买小模式

	MSG_YINGSANZHANG_S_2_C_START_ADD_SCORE = 62016,       //服务器广播买大买小结束，通知客户端开始下注


	//推筒子 62050-62070
	MSG_C_2_S_TUITONGZI_SELECT_ZHUANG = 62051,          //选庄
	MSG_S_2_C_TUITONGZI_SELECT_ZHUANG = 62052,       //选庄

	MSG_C_2_S_TUITONGZI_DO_SHAIZI = 62053,       //庄家点色子
	MSG_S_2_C_TUITONGZI_DO_SHAIZI = 62054,

	MSG_C_2_S_TUITONGZI_ADD_SCROE = 62055,                  //用户选分
	MSG_S_2_C_TUITONGZI_ADD_SCORE = 62056,

	MSG_S_2_C_TUITONGZI_RECON = 62057,   //断线重连

	MSG_S_2_C_TUITONGZI_DRAW_RESULT = 62058,  //小结算

	MSG_C_2_S_TUITONGZI_OPEN_CARD = 62059,  //亮牌
	MSG_S_2_C_TUITONGZI_OPEN_CARD = 62060,

	MSG_S_2_C_TUITONGZI_START_GAME = 62061,

	MSG_S_2_C_READY_TIME_REMAIN = 62062,  //桌子wait状态倒计时


	/////////////////////////////
	//牛牛  62071-62100
	//MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071,          //选庄
	//MSG_S_2_C_NIUNIU_SELECT_ZHUANG = 62072,
	//
	//MSG_C_2_S_NIUNIU_ADD_SCORE = 62073,           //下注
	//MSG_S_2_C_NIUNIU_ADD_SCORE = 62074,
	//
	//MSG_C_2_S_NIUNIU_OPEN_CARD = 62075,           //亮牌
	//MSG_S_2_C_NIUNIU_OPEN_CARD = 62076,
	//
	//MSG_S_2_C_NIUNIU_START_GAME = 62077,         //游戏开始
	//
	//MSG_S_2_C_NIUNIU_DRAW_RESULT = 62078,        //小结算
	//
	//MSG_S_2_C_NIUNIU_RECON = 62079,              //断线重连
	//MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG = 62080,              //抢庄通知
	//
	//MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE = 62081,  //明牌下注
	//
	//MSG_C_2_S_NIUNIU_MAI_MA = 62082,			//玩家买码
	//MSG_S_2_C_NIUNIU_MAI_MA_BC = 62083,			//服务器广播玩家买码
	// 
	//MSG_S_2_C_LOOKON_PLAYER_FULL = 62065,   //服务器广播观战玩家，参与游戏玩家人数已满
	//MSG_C_2_S_ADD_ROOM_SEAT_DOWN = 62066,    //客户端发送观战状态下入座
	//MSG_S_2_C_ADD_ROOM_SEAT_DOWN = 62067,    	//服务器返回观战玩家入座结果


	/////////////////////////////
	//斗地主  62101-62150

	MSG_C_2_S_JOIN_ROOM_CHECK = 62063,    //客户端请求加入的房间
	MSG_S_2_C_JOIN_ROOM_CHECK = 62064,  //if游戏开始，询问是否加入


	MSG_S_2_C_DOUDIZHU_GAME_MESSAGE_BASE = 62101,  // 斗地主S->C
	MSG_C_2_S_DOUDIZHU_GAME_MESSAGE_BASE = 62102,  // 斗地主C->S
	MSG_S_2_C_DOUDIZHU_RECONNECT = 62103,  // 斗地主断线重连

	MSG_S_2_C_DOUDIZHU_JOIN_CLUB_ROOM_ANONYMOUS = 62104,  //俱乐部匿名加入房间

	//回放信息
	MSG_C_2_S_POKER_REQUEST_VIDEO_ID = 62105,          //客户端请求回放
	MSG_S_2_C_POKER_RESPOND_VIDEO_ID = 62106,         //服务器返回回放内容

	MSG_C_2_S_REQUEST_USER_GPS = 62107,                     //客户端申请房间其他用户gps
	MSG_S_2_C_REQUEST_USER_GPS = 62108,                     //服务器返回房间其他用户gps

	//俱乐部匹配  3人匹配
	MSG_S_2_C_POKER_WAIT_JOIN_ROOM = 62222,     //3人斗地主
	MSG_C_2_S_POKER_EXIT_WAIT = 62223,              //退出
	MSG_S_2_C_POKER_EXIT_WAIT = 62224,             //返回退出结果


																																																																									 //////////////////////////////////////////////////////////////////////////////////////
	//十点半消息号  62151-6199



																																																																									 //////////////////////////////////////////////////////////////////////////////////////
	//双升消息号  62200-62249

	//S->C 服务器给每个玩家发牌
	MSG_S_2_C_SHUANGSHENG_SEND_CARDS = 62200,

	//S->C 服务器通知玩家报主
	MSG_S_2_C_SHUANGSHENG_NOTICE_SELECT_ZHU = 62201,

	//C->S 玩家报主操作
	MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202,

	//S->C 服务器通知玩家反主
	MSG_S_2_C_SHUANGSHENG_NOTICE_FAN_ZHU = 62203,

	//C->S 玩家反主操作
	MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204,

	//S->C 服务器通知玩家拿底牌,当前庄家，主花色等信息
	MSG_S_2_C_SHUANGSHENG_NOTICE_BASE_INFO = 62205,

	//C->S 玩家盖底牌操作
	MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206,

	//S->C 服务器广播游戏开始，开始出牌
	MSG_S_2_C_SHUANGSHENG_BC_GAME_STARE = 62207,

	//C->S 玩家出牌操作
	MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208,

	//S->C 服务器广播玩家出牌结果
	MSG_S_2_C_SHUANGSHENG_BC_OUT_CARDS_RESULT = 62209,

	//S->C 服务器广播小结算
	MSG_S_2_C_SHUANGSHENG_BC_DRAW_RESULT = 62210,

	//S->C 服务器发送断线重连消息
	MSG_S_2_C_SHUANGSHENG_RECON = 62211,

	//S->C 服务器发送第一局的开局消息（设置防作弊的消息等）
	MSG_S_2_C_SHUANGSHENG_FIRST_ROUND = 62212,

	//S->C 服务器广播玩家叫主叫主结果（玩家每次叫主广播一下）
	MSG_S_2_C_SHUANGSHENG_BC_SELECT_ZHU = 62213,

	//S->C 服务器广播玩家埋底结束
	MSG_S_2_C_SHUANGSHENG_BC_BASE_OVER = 62216,

	//////////////////////////////////////////////////////////////////////////////////////
	//3打2消息号  62250-62299

	//S->C 服务器给每个玩家发牌
	MSG_S_2_C_SANDAER_SEND_CARDS = 62250,

	//S->C 服务器依次次给玩家推送叫分消息(待定)
	MSG_S_2_C_SANDAER_SELECT_SCORE = 62251,

	//S->C 服务器广播玩家叫分结果
	MSG_S_2_C_SANDAER_SELECT_SCORE_R = 62252,

	//C->S 玩家选择叫分
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253,

	//S->C 服务器通知玩家拿底牌，并选主花色
	MSG_S_2_C_SANDAER_BASE_CARD_AND_SELECT_MAIN_N = 62254,

	//C->S 玩家选主牌
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255,

	//S->C 服务器返回玩家选主牌结果
	MSG_S_2_C_SANDAER_SELECT_MAIN_R = 62256,

	//C->S 玩家埋底
	MSG_C_2_S_SANDAER_BASE_CARD = 62257,

	//S->C 服务器返回玩家埋底结果
	MSG_S_2_C_SANDAER_BASE_CARD_R = 62258,

	//C->S 玩家选对家牌
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259,

	//S->C 服务器返回庄家选对家的牌
	MSG_S_2_C_SANDAER_SELECT_FRIEND_BC = 62260,

	//C->S 玩家出牌
	MSG_C_2_S_SANDAER_OUT_CARD = 62261,

	//S->C 服务器广播出牌结果
	MSG_S_2_C_SANDAER_OUT_CARD_BC = 62262,

	//S->C 服务器发送断线重连
	MSG_S_2_C_SANDAER_RECON = 62263,

	//S->C 服务发送小结算
	MSG_S_2_C_SANDAER_DRAW_RESULT_BC = 62264,

	//S->C 服务发送闲家得分满105分,是否停止游戏
	MSG_S_2_C_SANDAER_SCORE_105 = 62265,

	//C->S 玩家回复闲家得分满105分,是否停止游戏
	MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,

	//C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	MSG_C_2_S_SANDAER_SCORE_105_RESULT = 62267,

	//S->C 服务器提示庄家可以摊牌
	MSG_S_2_C_SANDAER_ZHUANG_CAN_TAN_PAI = 62270,

	//C-S 庄家选择是否摊牌
	MSG_C_2_S_SANDAER_ZHUANG_SELECT_TAN_PAI = 62271,

	//C-S 庄家选择摊牌结果
	MSG_S_2_C_SANDAER_TAN_PAI_RET = 62272,

	MSG_C_2_S_USER_GET_BASECARDS = 62268,		// 庄家请求底牌 原始：61027
	MSG_S_2_C_USER_GET_BASECARDS_RET = 62269,	// 返回底牌     原始：61028

	//C->S 客户端选择交牌
	MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272,

	//S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
	MSG_S_2_C_SANDAER_ZHUANG_JIAO_PAI_BC = 62273,

	//C->S 非庄家客户端选择庄家交牌的申请
	MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274,

	//S->C 庄家交牌的最终结果
	MSG_S_2_C_SANDAER_JIAO_PAI_RESULT = 62275,


	//////////////////////////////////////////////////////////////////////////////////////
	//3打1消息号  62300-62349

	//S->C 服务器给每个玩家发牌
	MSG_S_2_C_SANDAYI_SEND_CARDS = 62300,

	//S->C 服务器依次次给玩家推送叫分消息(待定)
	MSG_S_2_C_SANDAYI_SELECT_SCORE = 62301,

	//S->C 服务器广播玩家叫分结果
	MSG_S_2_C_SANDAYI_SELECT_SCORE_R = 62302,

	//C->S 玩家选择叫分
	MSG_C_2_S_SANDAYI_SELECT_SCORE = 62303,

	//S->C 服务器通知玩家拿底牌，并选主花色
	MSG_S_2_C_SANDAYI_BASE_CARD_AND_SELECT_MAIN_N = 62304,

	//C->S 玩家选主牌
	MSG_C_2_S_SANDAYI_SELECT_MAIN = 62305,

	//S->C 服务器返回玩家选主牌结果
	MSG_S_2_C_SANDAYI_SELECT_MAIN_R = 62306,

	//C->S 玩家埋底
	MSG_C_2_S_SANDAYI_BASE_CARD = 62307,

	//S->C 服务器返回玩家埋底结果
	MSG_S_2_C_SANDAYI_BASE_CARD_R = 62308,

	//C->S 玩家出牌
	MSG_C_2_S_SANDAYI_OUT_CARD = 62309,

	//S->C 服务器广播出牌结果
	MSG_S_2_C_SANDAYI_OUT_CARD_BC = 62310,

	//S->C 服务器发送断线重连
	MSG_S_2_C_SANDAYI_RECON = 62311,

	//S->C 服务发送小结算
	MSG_S_2_C_SANDAYI_DRAW_RESULT_BC = 62312,

	//S->C 服务发送闲家得分满105分,是否停止游戏
	MSG_S_2_C_SANDAYI_SCORE_105 = 62313,

	//C->S 玩家回复闲家得分满105分,是否停止游戏
	MSG_C_2_S_SANDAYI_SCORE_105_RET = 62314,

	//C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	MSG_C_2_S_SANDAYI_SCORE_105_RESULT = 62315,


	//////////////////////////////////////////////////////////////////////////////////////
	//五人百分消息号  62350-62399

	//S->C 服务器给每个玩家发牌
	MSG_S_2_C_WURENBAIFEN_SEND_CARDS = 62350,

	//S->C 服务器依次次给玩家推送叫分消息(待定)
	MSG_S_2_C_WURENBAIFEN_SELECT_SCORE = 62351,

	//S->C 服务器广播玩家叫分结果
	MSG_S_2_C_WURENBAIFEN_SELECT_SCORE_R = 62352,

	//C->S 玩家选择叫分
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353,

	//S->C 服务器通知玩家拿底牌，并选主花色
	MSG_S_2_C_WURENBAIFEN_BASE_CARD_AND_SELECT_MAIN_N = 62354,

	//C->S 玩家选主牌
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355,

	//S->C 服务器返回玩家选主牌结果
	MSG_S_2_C_WURENBAIFEN_SELECT_MAIN_R = 62356,

	//C->S 玩家埋底
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357,

	//S->C 服务器返回玩家埋底结果
	MSG_S_2_C_WURENBAIFEN_BASE_CARD_R = 62358,

	//C->S 玩家选对家牌
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359,

	//S->C 服务器返回庄家选对家的牌
	MSG_S_2_C_WURENBAIFEN_SELECT_FRIEND_BC = 62360,

	//C->S 玩家出牌
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361,

	//S->C 服务器广播出牌结果
	MSG_S_2_C_WURENBAIFEN_OUT_CARD_BC = 62362,

	//S->C 服务器发送断线重连
	MSG_S_2_C_WURENBAIFEN_RECON = 62363,

	//S->C 服务发送小结算
	MSG_S_2_C_WURENBAIFEN_DRAW_RESULT_BC = 62364,

	//S->C 服务发送闲家得分满105分,是否停止游戏
	MSG_S_2_C_WURENBAIFEN_SCORE_105 = 62365,

	//C->S 玩家回复闲家得分满105分,是否停止游戏
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,

	//C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RESULT = 62367,

	//C->S 客户端选择交牌
	MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368,

	//S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
	MSG_S_2_C_WURENBAIFEN_ZHUANG_JIAO_PAI_BC = 62369,

	//C->S 非庄家客户端选择庄家交牌的申请
	MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370,

	//S->C 庄家交牌的最终结果
	MSG_S_2_C_WURENBAIFEN_JIAO_PAI_RESULT = 62371

};

//////////////////////////////////////////////////////////////////////////
//客户端游戏内部操作。游戏内命令都以子命令的形式m_subCmd在具体gamehander

struct LMsgS2CGameMessage :public LMsgSC
{
	Lint  m_subID;

	LMsgS2CGameMessage() :LMsgSC(MSG_S_2_C_POKER_GAME_MESSAGE)
	{
		m_subID=0;
	}

	LMsgS2CGameMessage(Lint subCmd) :LMsgSC(MSG_S_2_C_POKER_GAME_MESSAGE)
	{
		m_subID=subCmd;
	}

};

//////////////////////////////////////////////////////////////////////////
//游戏match结算信息--大结算
struct LMsgS2CGameEndResult:public LMsgSC
{

	Lint					m_IsBigWinner;
	Lint                    m_EffectiveUserCount;                                  //子游戏用户数量，赢三张-5，牛牛-6，斗地主-3
	Lint                    m_UserState[MAX_CHAIR_COUNT];                          //用户状态
	Lint					m_Score[MAX_CHAIR_COUNT];                              //总结算分数
	Lint					m_UserIds[MAX_CHAIR_COUNT];							   //游戏 I D
	Lstring					m_Nikes[MAX_CHAIR_COUNT];                              //游戏玩家昵称
	Lstring					m_HeadUrls[MAX_CHAIR_COUNT];						   //游戏玩家头像
	Lint                    m_zhuangPos;
	Lint					m_maxCircle;											//创建房间的最大局数

	Llong					m_Time;
	Lint                    m_CreatorId;

	Lstring              m_CreatorNike;
	Lstring              m_CreatorHeadUrl;
	Lstring              m_playId;



	Lint					m_qiangCount[MAX_CHAIR_COUNT];						//抢庄次数
	Lint					m_zhuangCount[MAX_CHAIR_COUNT];						//坐庄次数
	Lint					m_tuiCount[MAX_CHAIR_COUNT];						//推注次数

	
	LMsgS2CGameEndResult() :LMsgSC(MSG_S_2_C_POKER_GAME_END_RESULT)
	{
		memset(m_Score, 0, sizeof(m_Score));
		m_IsBigWinner=0;
		m_EffectiveUserCount=0;
		memset(m_UserState, 0, sizeof(m_UserState));
		memset(m_UserIds, 0, sizeof(m_UserIds));
		m_Time=0;
		m_CreatorId=0;
		m_zhuangPos = 0;
		m_maxCircle = 0;
		memset(m_qiangCount, 0, sizeof(m_qiangCount));
		memset(m_zhuangCount, 0, sizeof(m_zhuangCount));
		memset(m_tuiCount, 0, sizeof(m_tuiCount));

	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);

		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kIsBigWinner", m_IsBigWinner);
		WriteKeyValue(pack, "kEffectiveUserCount", m_EffectiveUserCount);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kMaxCircle", m_maxCircle);

		std::vector<Lint> veci;
		std::vector<Lstring> vecs;

		WriteKey(pack, "kUserState");
		veci.clear();
		for (Lint i=0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_UserState[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kScore");
		veci.clear();
		for (Lint i=0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_Score[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kUserIds");
		veci.clear();
		for (Lint i=0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_UserIds[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i=0; i < MAX_CHAIR_COUNT; ++i)
		{
			vecs.push_back(m_Nikes[i]);
		}
		WriteKey(pack, vecs);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i=0; i < MAX_CHAIR_COUNT; ++i)
		{
			vecs.push_back(m_HeadUrls[i]);
		}
		WriteKey(pack, vecs);

		WriteKeyValue(pack, "kTime", m_Time);
		WriteKeyValue(pack, "kCreatorId", m_CreatorId);
		WriteKeyValue(pack, "kCreatorNike", m_CreatorNike);
		WriteKeyValue(pack, "kCreatorHeadUrl", m_CreatorHeadUrl);
		WriteKeyValue(pack, "kPlayId", m_playId);

		veci.clear();
		WriteKey(pack, "kQiangCount");
		for (Lint i = 0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_qiangCount[i]);
		}
		WriteValue(pack, veci);

		veci.clear();
		WriteKey(pack, "kZhuangCount");
		for (Lint i = 0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_zhuangCount[i]);
		}
		WriteValue(pack, veci);

		veci.clear();
		WriteKey(pack, "kTuiCount");
		for (Lint i = 0; i < MAX_CHAIR_COUNT; ++i)
		{
			veci.push_back(m_tuiCount[i]);
		}
		WriteValue(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CGameEndResult();
	}

};

//////////////////////////////////////////////////////////////////////////
/*
  客户端请求match战绩--每场比赛战绩
  MSG_C_2_S_POKER_ROOM_LOG = 62003
*/
struct LMsgC2SQiPaiLog :public LMsgSC
{
	Lint		m_time;//id
	Lint		m_userId;//请求者id

	Lint   m_clubId;         //clubId!=0 的請求该用户在这个俱乐部中战绩
	Lint   m_searchId;

	LMsgC2SQiPaiLog() :LMsgSC(MSG_C_2_S_POKER_ROOM_LOG)
	{
		m_time=0;
		m_userId=0;
		m_clubId = 0;
		m_searchId = 0;

	}

	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kClubId), m_clubId);
		ReadMapData(obj, NAME_TO_STR(kSearch), m_searchId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SQiPaiLog(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器回复match战绩--每场比赛战绩
struct QiPaiLogItemInfo
{
	Lstring m_id;
	Lint m_time;
	Lint m_clubId;
	Lstring m_clubName;
	Lint m_flag;		//房间类型
	Lint m_userCount;           //有效的用户数量，下面数组有效值
	Lint m_createUserId;		//房主ID
	Lint m_userid[MAX_CHAIR_COUNT];
	Lstring m_nike[MAX_CHAIR_COUNT];
	Lstring	m_headUrl[MAX_CHAIR_COUNT];
	Lint m_score[MAX_CHAIR_COUNT];
	Lint m_reset;//是否解散
	Lint m_deskId;//桌子号
	Lint m_maxCircle;  //最大局数
	Lint m_curCircle;  //当前局数
	std::vector<Lint>				m_playType; //玩家小选项
	Lint m_qiangCount[MAX_CHAIR_COUNT];
	Lint m_zhuangCount[MAX_CHAIR_COUNT];
	Lint m_tuiCount[MAX_CHAIR_COUNT];
	//Lint m_size;                             
	//QiPaiLog m_item[20];        //这个消息里没有用上

	QiPaiLogItemInfo()
	{
		m_clubId = 0;
		m_flag=0;
		m_userCount=0;
		m_createUserId = 0;
		m_reset=0;
		m_deskId=0;
		m_time=0;
		m_maxCircle = 0;
		m_curCircle = 0;
		for (int i=0; i<MAX_CHAIR_COUNT; i++)
		{
			m_userid[i]=0;
			m_score[i]=0;
			m_qiangCount[i] = 0;
			m_zhuangCount[i] = 0;
			m_tuiCount[i] = 0;
		}
	}
};

struct LMsgS2CQiPaiLog :public LMsgSC
{
	Lint				m_size;//长度
	QiPaiLogItemInfo		m_info[20];//log信息

	LMsgS2CQiPaiLog() :LMsgSC(MSG_S_2_C_POKER_ROOM_LOG)
	{
		m_size=0;
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
		for (Lint i=0; i < m_size; ++i)
		{
			WriteMap(pack, 19);
			WriteKeyValue(pack, "kId", m_info[i].m_id);
			WriteKeyValue(pack, "kTime", m_info[i].m_time);
			WriteKeyValue(pack, "kClubId", m_info[i].m_clubId);
			WriteKeyValue(pack, "kClubName", m_info[i].m_clubName);
			WriteKeyValue(pack, "kFlag", m_info[i].m_flag);
			WriteKeyValue(pack, "kUserCount", m_info[i].m_userCount);
			WriteKeyValue(pack, "kCreateUserId", m_info[i].m_createUserId);
			WriteKeyValue(pack, "kReset", m_info[i].m_reset);
			WriteKeyValue(pack, "kDeskId", m_info[i].m_deskId);
			WriteKeyValue(pack, "kMaxCircle", m_info[i].m_maxCircle);
			WriteKeyValue(pack, "kCurCircle", m_info[i].m_curCircle);

			std::vector<Lstring> vecs, vecHandUrl;
			std::vector<Lint>veci, vec, vecPlayType;
			std::vector<Lint>vecQiangCount, vecZhuangCount, vecTuiCount;
			for (Lint j=0; j <m_info[i].m_userCount; ++j)
			{
				vecs.push_back(m_info[i].m_nike[j]);
				vecHandUrl.push_back(m_info[i].m_headUrl[j]);
				veci.push_back(m_info[i].m_score[j]);
				vec.push_back(m_info[i].m_userid[j]);

				vecQiangCount.push_back(m_info[i].m_qiangCount[j]);
				vecZhuangCount.push_back(m_info[i].m_zhuangCount[j]);
				vecTuiCount.push_back(m_info[i].m_tuiCount[j]);
			}
			/*for (int j = 0; i < m_info[i].m_playType.size(); ++j)
			{
				vecPlayType.push_back(m_info[i].m_playType[j]);
			}*/
			vecPlayType = m_info[i].m_playType;
			WriteKey(pack, "kUserid");
			WriteKey(pack, vec);

			WriteKey(pack, "kNike");
			WriteKey(pack, vecs);

			WriteKey(pack, "kScore");
			WriteKey(pack, veci);

			WriteKey(pack, "kPlayType");
			WriteKey(pack, vecPlayType);

			WriteKey(pack, "kHeadUrl");
			WriteKey(pack, vecHandUrl);

			WriteKeyValue(pack, "kQiangCount", vecQiangCount);
			WriteKeyValue(pack, "kZhuangCount", vecZhuangCount);
			WriteKeyValue(pack, "kTuiCount", vecTuiCount);
			//WriteKey(pack, "kMatch");
			//WriteArray(pack, m_info[i].m_size);
			
		}

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CQiPaiLog(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求round战绩--每局战绩
struct LMsgC2SMatchLog :public LMsgSC
{
	//Lstring		m_strUUID; //客户端没有传递，由服务器填写
	Lstring		m_id;
	Lint		m_userId; //请求某个玩家的战绩
	Lint        m_pos;    //pos1
	Lint        m_time;   //time
	Lint        m_deskId; //桌子id

	LMsgC2SMatchLog() :LMsgSC(MSG_C_2_S_POKER_MATCH_LOG)
	{
		//m_strUUID="";
		m_userId=0;
		m_pos=0;
		m_deskId=0;
		m_time=0;
	}
	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(kStrUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(kId), m_id);
		ReadMapData(obj, NAME_TO_STR(kTime), m_time);
		ReadMapData(obj, NAME_TO_STR(kUserId), m_userId);
		ReadMapData(obj, NAME_TO_STR(kPos), m_pos);
		ReadMapData(obj, NAME_TO_STR(kDeskId), m_deskId);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kId), m_id);
		//WriteKeyValue(pack, NAME_TO_STR(kStrUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(kUserId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(kTime), m_time);
		WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(kDeskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SMatchLog(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回round战绩--每局战绩
struct QiPaiLog
{
	Lint	m_time;
	Lstring	m_videoId;
	Lint m_userCount;
	Lint	m_score[MAX_CHAIR_COUNT];

	QiPaiLog()
	{
		m_time=0;
		m_userCount=0;
		memset(m_score, 0, sizeof(m_score));
	}
};

struct LMsgS2CMatchLog :public LMsgSC
{
	Lint m_size;
	Lint m_flag;		//房间类型
	QiPaiLog m_item[40];

	LMsgS2CMatchLog() :LMsgSC(MSG_S_2_C_POKER_MATCH_LOG)
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
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kSize", m_size);
		WriteKey(pack, "kData");
		WriteArray(pack, m_size);
		for (Lint k = 0; k < m_size && k < 40; ++k)
		{
			WriteMap(pack, 4);
			WriteKeyValue(pack, "kTime", m_item[k].m_time);
			WriteKeyValue(pack, "kUserCount", m_item[k].m_userCount);
			WriteKeyValue(pack, "kVideoId", m_item[k].m_videoId);

			std::vector<Lint> vec;
			for (Lint n = 0; n < MAX_CHAIR_COUNT; ++n)
			{
				vec.push_back(m_item[k].m_score[n]);
			}
			WriteKey(pack, "kScore");
			WriteKey(pack, vec);
		}

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CMatchLog(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求回放内容
struct LMsgC2SPokerVideo :public LMsgSC
{
	Lstring	m_videoId;

	LMsgC2SPokerVideo() :LMsgSC(MSG_C_2_S_POKER_REQUEST_VIDEO_ID)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kVideoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SPokerVideo(); }

};



//////////////////////////////////////////////////////////////////////////
//赢三张-断线重连
struct YingSanZhangS2CPlayScene :public LMsgSC
{
	Lint                                   m_UserStatus[YINGSANZHANG_PLAY_USER_MAX];                                //用户弃牌，输牌情况  ： 1-弃牌，2-输牌
	Lint                                   m_TableScoreCount[5];                                                                                   //桌面每个筹码记录
	Lint                                   m_CardType;                                                                                                      //扑克类型
	Lint                                   m_DynamicJoin;                                                                                                //动态加入,新加
	Lint                                   m_TableTotalScore;                                                                                           //桌面总下注数  ，新加
	Lint                                   m_AutoScore;                                                                                                     //是否自动下注

																																							//加注信息
	Lint        							   m_MaxCellScore;						     //单元上限
	Lint									   m_CellScore;							        //单元下注
	Lint									   m_CurrentTimes;						//当前倍数
	Lint									   m_UserMaxScore;						//用户分数上限

	//状态信息
	Lint										m_BankerUser;						                                                         //庄家用户
	Lint				 						m_CurrentUser;						                                                        //当前玩家
	Lint										m_PlayStatus[YINGSANZHANG_PLAY_USER_MAX];			//游戏状态
	Lint									    m_MingZhu[YINGSANZHANG_PLAY_USER_MAX];				//看牌状态
	Lint									    m_TableScore[YINGSANZHANG_PLAY_USER_MAX];			    //下注数目

	//扑克信息
	BYTE						           m_HandCardData[YINGSANZHANG_HAND_CARD_MAX];			//扑克数据

	//状态信息
	Lint							           m_CompareState;						//比牌状态
	Lint                                   m_remain_time;    //阶段剩余时间

	//Lint                                 m_TotalScore[YINGSANZHANG_PLAY_USER_MAX];                                                                                 //总得分，更新头像下的分数
	Lint                                  m_GameStatus;                 //0-游戏状态，1-买小，2-买大
	Lint                                  m_IsBuy;                            //0-没有买，1买了
	Lint                                  m_UserBuyStatus[YINGSANZHANG_PLAY_USER_MAX];               //用户买大买小分数  0--过， 255-用户还没有操作，1-10 实际选择分数
	Lint                                  m_BuyType;             // 0--买小  1--买大  ， 2-没有选择买大买小
	Lint                                  m_BuyPos;
	Lstring                            m_BuyNike;
	Lint                                  m_BuyScore;


	YingSanZhangS2CPlayScene() :LMsgSC(MSG_C_2_S_POKER_RECONNECT)
	{
		m_CardType=0;
		m_DynamicJoin=0;
		m_TableTotalScore=0;
		m_AutoScore=0;
		m_MaxCellScore=0;
		m_CellScore=0;
		m_CurrentTimes=0;
		m_UserMaxScore=0;

		m_BankerUser=INVAILD_POS_QIPAI;
		m_CurrentUser=INVAILD_POS_QIPAI;
		m_CompareState=0;

		memset(m_UserStatus, 0x00, sizeof(m_UserStatus));
		memset(m_PlayStatus, 0x00, sizeof(m_PlayStatus));
		memset(m_MingZhu, 0x00, sizeof(m_MingZhu));
		memset(m_TableScore, 0x00, sizeof(m_TableScore));
		memset(m_HandCardData, 0x00, sizeof(m_HandCardData));
		//memset(m_TotalScore, 0x00, sizeof(m_TotalScore));

		memset(m_TableScoreCount, 0x00, sizeof(m_TableScoreCount));

		m_remain_time = 0;
		
		m_GameStatus = 0;
		m_IsBuy = 1;
		m_BuyType = 2;
		m_BuyPos = INVAILD_POS_QIPAI;
		m_BuyScore = 0;
		memset(m_UserBuyStatus, 0x00, sizeof(m_UserBuyStatus));
	
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 26);
		WriteKeyValue(pack, "kMId", m_msgId);
		//WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kCardType", m_CardType);
		WriteKeyValue(pack, "kDynamicJoin", m_DynamicJoin);
		WriteKeyValue(pack, "kTableTotalScore", m_TableTotalScore);
		WriteKeyValue(pack, "kAutoScore", m_AutoScore);
		WriteKeyValue(pack, "kMaxCellScore", m_MaxCellScore);
		WriteKeyValue(pack, "kCellScore", m_CellScore);
		WriteKeyValue(pack, "kCurrentTimes", m_CurrentTimes);
		WriteKeyValue(pack, "kUserMaxScore", m_UserMaxScore);

		WriteKeyValue(pack, "kBankerUser", m_BankerUser);
		WriteKeyValue(pack, "kCurrentUser", m_CurrentUser);
		WriteKeyValue(pack, "kCompareState", m_CompareState);
		WriteKeyValue(pack, "kRemain_time", m_remain_time);

		WriteKeyValue(pack, "kGameStatus", m_GameStatus);
		WriteKeyValue(pack, "kIsBuy", m_IsBuy);
		WriteKeyValue(pack, "kBuyPos", m_BuyPos);
		WriteKeyValue(pack, "kBuyNike", m_BuyNike);
		WriteKeyValue(pack, "kBuyScore", m_BuyScore);
		WriteKeyValue(pack, "kBuyType", m_BuyType);

		std::vector<Lint> veci;
		WriteKey(pack, "kUserBuyStatus");
		veci.clear();
		for (Lint i = 0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_UserBuyStatus[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kUserStatus");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_UserStatus[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kTableScoreCount");
		veci.clear();
		for (Lint i=0; i < 5; ++i)
		{
			veci.push_back(m_TableScoreCount[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kPlayStatus");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_PlayStatus[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kMingZhu");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_MingZhu[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kTableScore");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_TableScore[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kHandCardData");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_HandCardData[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CPlayScene();
	}

};










//////////////////////////////////////////////////////////////////////////
//子游戏中的命令   --赢三张，推筒子，牛牛，斗地主

//////////////////////////////////////////////////////////////////////////
// POKER-子命令----只有赢三张在用子命令，后续最好不要用，太麻烦。直接在LMsg.h加

//////////////////////////////////////////////////////////////////////////
enum POKER_SUB_ID
{

	//赢三张的命令
	MSG_POKER_S_2_C_START_GAME_ROUND=1,                  //游戏开始
	MSG_POKER_S_2_C_END_GAME_ROUND=2,                       //游戏结束


	/////////////////////begin 赢三张游戏开始后命令  101-150   ,client--><--server 

	MSG_YINGSANZHANG_C_2_S_AUTO_SCORE=101,                //自动下注  LMsgC2SGameCommon
	MSG_YINGSANZHANG_S_2_C_AUTO_SCORE=102,               //
	
	//MSG_YINGSANZHANG_C_2_S_ADD_SCORE=103,                //用户加注
	MSG_YINGSANZHANG_S_2_C_ADD_SCORE=104,               //

	MSG_YINGSANZHANG_C_2_S_GIVE_UP=105,                      //用户弃牌
	MSG_YINGSANZHANG_S_2_C_GIVE_UP=106,                      //

	MSG_YINGSANZHANG_C_2_S_LOOK_CARD=107,               //用户看牌
	MSG_YINGSANZHANG_S_2_C_LOOK_CARD=108,               //

	MSG_YINGSANZHANG_C_2_S_COMPARE_CARD=109,     //用户比牌
	MSG_YINGSANZHANG_S_2_C_COMPARE_CARD=110,       //

	MSG_YINGSANZHANG_S_2_C_LUN=111,               //下注轮数

	MSG_YINGSANZHANG_C_2_S_FINISH_FLASH=112,       //完成动画

	MSG_YINGSANZHANG_S_2_C_SYSTEM_COMPARE_CARD=113,   //系统比牌


	/////////////////////end 赢三张游戏开始后命令  101-150   ,client--><--server 


	//////////////////////////////////////////////////////////////////////////
	/////////////////////begin 推筒子游戏开始后命令  151-170   ,client--><--server   推筒子不用子命令
	//MSG_TUITONGZI_C_2_S_SELECT_ZHUANG=151,     //选庄
	//MSG_TUITONGZI_S_2_C_SELECT_ZHUANG=152,  //广播庄
	//
	//MSG_TUITONGZI_C_2_S_DO_SHAZI=153,   //摇色子
	//MSG_TUITONGZI_S_2_C_DO_SHAZI=154,  //广播色子结果
	//
	//MSG_TUITONGZI_S_2_C_SCORE=155,   //广播选分结果
	//
	//MSG_TUITONGZI_C_2_S_OPEN_CARD=156,  //开牌
	//
	//MSG_TUITONGZI_S_2_C_RECON=157,   //断线重连
	//
	//MSG_TUITONGZI_S_2_C_RESULT=158,  //小结算


	




	/////////////////////end推筒子游戏开始后命令   151-170   ,client--><--server 


};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//通用结构体，简单命令用到
struct LMsgC2SGameCommon :public LMsgSC
{
	Lint m_value;
	Lint  m_subID;

	LMsgC2SGameCommon():LMsgSC(MSG_C_2_S_POKER_GAME_MESSAGE)
	{
		m_subID=0;
		m_value=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kSubId", m_subID);
		ReadMapData(obj, "kValue", m_value);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kValue", m_value);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SGameCommon();
	}

};


struct LMsgS2CGameCommon :public LMsgS2CGameMessage
{
	Lint m_value;

	LMsgS2CGameCommon(Lint subCmd) :LMsgS2CGameMessage(subCmd)
	{
		m_value=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kSubId", m_subID);
		ReadMapData(obj, "kValue", m_value);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kValue", m_value);

		return true;
	}

	//virtual LMsg* Clone()
	//{
	//	return new LMsgC2SGameCommon();
	//}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-游戏开始
struct  YingSanZhangStartGameRound :public LMsgS2CGameMessage
{
	//下注信息
	Lint                         m_Status[YINGSANZHANG_PLAY_USER_MAX];
	Lint							m_MaxScore;							//桌面所有人下注筹码总数
	Lint 						m_CellScore;							//单元下注														
	Lint							m_BankerUser;						//庄家用户
	Lint 				 		m_CurrentUser;						//当前玩家
	Lint                        m_TotalScore[YINGSANZHANG_PLAY_USER_MAX];
																	

	YingSanZhangStartGameRound() :LMsgS2CGameMessage(MSG_POKER_S_2_C_START_GAME_ROUND)
	{
		m_MaxScore=0;
		m_CellScore=0;
		m_BankerUser=INVAILD_POS_QIPAI;
		m_CurrentUser=INVAILD_POS_QIPAI;
		memset(m_Status, 0, sizeof(m_Status));
		memset(m_TotalScore, 0, sizeof(m_TotalScore));
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);

		std::vector<Lint> veci;
		WriteKey(pack, "kStatus");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_Status[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, "kMaxScore", m_MaxScore);
		WriteKeyValue(pack, "kCellScore", m_CellScore);
		WriteKeyValue(pack, "kBankerUser", m_BankerUser);
		WriteKeyValue(pack, "kCurrentUser", m_CurrentUser);

		WriteKey(pack, "kTotalScore");
		veci.clear();
		for (Lint i = 0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_TotalScore[i]);
		}
		WriteKey(pack, veci);
	
		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangStartGameRound();
	}

};

//////////////////////////////////////////////////////////////////////////
struct  YingSanZhangEndGameRound_s
{
	Lint              m_FinalDw;                                                                                                                                                           //最后一局
	Lint              m_CardType[YINGSANZHANG_PLAY_USER_MAX];                                                                                   //扑克类型
	Lint			      m_GameScore[YINGSANZHANG_PLAY_USER_MAX];			                                                                    //游戏得分
	Lint				  m_CardData[YINGSANZHANG_PLAY_USER_MAX][YINGSANZHANG_HAND_CARD_MAX];			//用户扑克
	Lint 			  m_CompareUser[YINGSANZHANG_PLAY_USER_MAX][YINGSANZHANG_PLAY_USER_MAX - 1];		                                                                 //比牌用户
	Lint              m_TotalScore[YINGSANZHANG_PLAY_USER_MAX];                                                                                 //总得分，更新头像下的分数
	Lint              m_GiveUp[YINGSANZHANG_PLAY_USER_MAX];                                                                                        //弃牌 1-弃牌 0--没有

	YingSanZhangEndGameRound_s()
	{
		m_FinalDw = 0;
		for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			m_CardType[i] = 0;
			m_GameScore[i] = 0;
			m_TotalScore[i] = 0;
			m_GiveUp[i] = 0;
			memset(m_CardData[i], 0, sizeof(Lint)*YINGSANZHANG_HAND_CARD_MAX);

			for (Lint j = 0; j < YINGSANZHANG_PLAY_USER_MAX - 1; j++)
				m_CompareUser[i][j] = INVAILD_POS_QIPAI;
		}

	}

	void reset()
	{
		//如果以后加入不可以如此初始化的变量，再改
		m_FinalDw = 0;

		for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			m_CardType[i] = 0;
			m_GameScore[i] = 0;
			m_TotalScore[i] = 0;
			m_GiveUp[i] = 0;
			memset(m_CardData[i], 0, sizeof(Lint)*YINGSANZHANG_HAND_CARD_MAX);

			for (Lint j = 0; j < YINGSANZHANG_PLAY_USER_MAX - 1; j++)
				m_CompareUser[i][j] = INVAILD_POS_QIPAI;
		}
	}
	//每个用户显示的其他用户的牌不一样，所有这个结构里面数据也不一样
	void reset_part()
	{
		for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			m_CardType[i] = 0;
			memset(m_CardData[i], 0, sizeof(Lint)*YINGSANZHANG_HAND_CARD_MAX);
		}
	}
};

//赢三张-小结算
struct  YingSanZhangEndGameRound :public LMsgS2CGameMessage ,public YingSanZhangEndGameRound_s
{
	
	std::vector<std::string>  m_usernikes;
	std::vector<Lint> m_userids;
	std::vector<std::string>  m_headUrl;

	YingSanZhangEndGameRound() :LMsgS2CGameMessage(MSG_POKER_S_2_C_END_GAME_ROUND)
	{
		m_FinalDw=0;
		for (int i=0; i<YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			m_CardType[i]=0;
			m_GameScore[i]=0;
			m_TotalScore[i]=0;
			m_GiveUp[i] = 0;
			memset(m_CardData[i], 0, sizeof(Lint)*YINGSANZHANG_HAND_CARD_MAX);
			
			for (Lint j=0; j<YINGSANZHANG_PLAY_USER_MAX - 1; j++)
					m_CompareUser[i][j]=INVAILD_POS_QIPAI;
		}

	}

	void copy(YingSanZhangEndGameRound_s s)
	{
		m_FinalDw = s.m_FinalDw;
		for (int i = 0; i < YINGSANZHANG_PLAY_USER_MAX; i++)
		{
			m_CardType[i] = s.m_CardType[i];
			m_GameScore[i] = s.m_GameScore[i];
			m_TotalScore[i] = s.m_TotalScore[i];
			m_GiveUp[i] = s.m_GiveUp[i];
			for (int m = 0; m < YINGSANZHANG_HAND_CARD_MAX; m++)
			{
				m_CardData[i][m] = s.m_CardData[i][m];
			}
			
			for (Lint j = 0; j < YINGSANZHANG_PLAY_USER_MAX - 1; j++)
				m_CompareUser[i][j] = s.m_CompareUser[i][j];
		}


	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7+ YINGSANZHANG_PLAY_USER_MAX+ YINGSANZHANG_PLAY_USER_MAX+3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kFinalDw", m_FinalDw);
	
		std::vector<Lint> veci;

		WriteKey(pack, "kGiveUp");
		veci.clear();
		for (Lint i = 0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_GiveUp[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kCardType");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_CardType[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kGameScore");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_GameScore[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kTotalScore");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			veci.push_back(m_TotalScore[i]);
		}
		WriteKey(pack, veci);

		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			char name[32];
			veci.clear();
			
			sprintf(name, "kCardData%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < YINGSANZHANG_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_CardData[i][j]);
			}
			WriteKey(pack, veci);
		}

		for (Lint i=0; i < YINGSANZHANG_PLAY_USER_MAX; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kCompareUser%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < YINGSANZHANG_PLAY_USER_MAX - 1; j++)
			{
				veci.push_back(m_CompareUser[i][j]);
			}
			WriteKey(pack, veci);
		}

		WriteKey(pack, NAME_TO_STR(kNikes));
		WriteValue(pack, m_usernikes);

		WriteKey(pack, NAME_TO_STR(kHeadUrls));
		WriteValue(pack, m_headUrl);

		WriteKey(pack, NAME_TO_STR(kUserIds));
		WriteValue(pack, m_userids);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangEndGameRound();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-客户端自动跟注
struct YingSanZhangC2SAutoScore :public LMsgC2SGameCommon
{
	YingSanZhangC2SAutoScore() :LMsgC2SGameCommon(){}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SAutoScore();
	}
};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务端自动跟注
struct YingSanZhangS2CAutoScore :public LMsgS2CGameCommon
{
	YingSanZhangS2CAutoScore() :LMsgS2CGameCommon(MSG_YINGSANZHANG_S_2_C_AUTO_SCORE) {}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CAutoScore();
	}
};

//////////////////////////////////////////////////////////////////////////
//赢三张-客户端加注
struct YingSanZhangC2SAddScore :public LMsgSC
{
	//Lint                m_subID;
	Lint					m_Score;								//下注数目
	Lint					m_State;								//当前状态
	Lint                m_AddScore;                        //加注数目，如果用户没有加注为0，加注--实际加注数目

	YingSanZhangC2SAddScore() :LMsgSC(MSG_C_2_S_YINGSANZHANG_ADD_SCORE)
	{
		//m_subID=0;
		m_Score=0;
		m_State=0;
		m_AddScore=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, "kSubId", m_subID);
		ReadMapData(obj, "kScore", m_Score);
		ReadMapData(obj, "kState", m_State);
		ReadMapData(obj, "kAddScore", m_AddScore);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		//WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kScore", m_Score);
		WriteKeyValue(pack, "kState", m_State);
		WriteKeyValue(pack, "kAddScore", m_AddScore);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SAddScore();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务端加注
struct YingSanZhangS2CAddScore :public LMsgS2CGameMessage
{
	Lint               m_AddScoreFlag;                                       //加注标志，0--不用，1--跟注，2--加注，3--比牌
	Lint				   m_CurrentUser;						                  //当前用户
	Lint				   m_AddScoreUser;						             //加注用户
	Lint				   m_CompareState;						             //比牌状态
	Lint				   m_AddScoreCount;						             //加注数目
	Lint				   m_CurrentTimes;						             //当前倍数
	Lint               m_UserScoreTotal;                                  //加注用户下注总数
	Lint               m_TableScoreTotal;                                //桌面下注总数
	Lint               m_AutoAdd;                                             //定时器，服务器主动下注，广播   服务器定时器下注=1 其他=0

	YingSanZhangS2CAddScore() :LMsgS2CGameMessage(MSG_YINGSANZHANG_S_2_C_ADD_SCORE)
	{
		m_AddScoreFlag=0;
		m_CurrentUser=INVAILD_POS_QIPAI;
		m_AddScoreUser=INVAILD_POS_QIPAI;
		m_CompareState=0;
		m_AddScoreCount=0;
		m_CurrentTimes=0;
		m_UserScoreTotal=0;
		m_TableScoreTotal=0;
		m_AutoAdd = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kAddScoreFlag", m_AddScoreFlag);
		WriteKeyValue(pack, "kCurrentUser", m_CurrentUser);
		WriteKeyValue(pack, "kAddScoreUser", m_AddScoreUser);
		WriteKeyValue(pack, "kCompareState", m_CompareState);
		WriteKeyValue(pack, "kAddScoreCount", m_AddScoreCount);
		WriteKeyValue(pack, "kCurrentTimes", m_CurrentTimes);
		WriteKeyValue(pack, "kUserScoreTotal", m_UserScoreTotal);
		WriteKeyValue(pack, "kTableScoreTotal", m_TableScoreTotal);
		WriteKeyValue(pack, "kAutoAdd", m_AutoAdd);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CAddScore();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-客户端弃牌
struct YingSanZhangC2SGiveUp :public LMsgC2SGameCommon
{
	YingSanZhangC2SGiveUp() :LMsgC2SGameCommon() {}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SGiveUp();
	}
};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务端弃牌
struct YingSanZhangS2CGiveUp :public LMsgS2CGameMessage
{
	Lint                            m_IsFinal;                                 //弃牌后是否结束比赛
	Lint								m_GiveUpUser;						//放弃用户

	YingSanZhangS2CGiveUp() :LMsgS2CGameMessage(MSG_YINGSANZHANG_S_2_C_GIVE_UP)
	{
		m_IsFinal=0;
		m_GiveUpUser=INVAILD_POS_QIPAI;
		
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kIsFinal", m_IsFinal);
		WriteKeyValue(pack, "kGiveUpUser", m_GiveUpUser);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CGiveUp();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-客户端看牌
struct YingSanZhangC2SLookCard :public LMsgC2SGameCommon
{
	YingSanZhangC2SLookCard() :LMsgC2SGameCommon() {}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SLookCard();
	}
};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务端看牌
struct YingSanZhangS2CLookCard :public LMsgS2CGameMessage
{
	Lint								m_LookCardUser;						                                                         //看牌用户
	Lint								m_CardData[YINGSANZHANG_HAND_CARD_MAX];				//用户扑克
	Lint                            m_CardType;                                                                                       //扑克类型

	YingSanZhangS2CLookCard() :LMsgS2CGameMessage(MSG_YINGSANZHANG_S_2_C_LOOK_CARD)
	{
		m_CardType=0;
		m_LookCardUser=INVAILD_POS_QIPAI;
		memset(m_CardData, 0, sizeof(m_CardData));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kLookCardUser", m_LookCardUser);

		std::vector<Lint> veci;
		WriteKey(pack, "kCardData");
		veci.clear();
		for (Lint i=0; i < YINGSANZHANG_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_CardData[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, "kCardType", m_CardType);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CLookCard();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-客户端比牌
struct YingSanZhangC2SCompareCard :public LMsgC2SGameCommon
{
	YingSanZhangC2SCompareCard() :LMsgC2SGameCommon() {}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SCompareCard();
	}
};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务端比牌
struct YingSanZhangS2CCompareCard :public LMsgS2CGameMessage
{
	Lint                             m_IsFinal;                                       //比牌后是否结束比赛
	Lint								 m_CurrentUser;						       //当前用户
	Lint								 m_CompareUser[2];					   //比牌用户
	Lint								 m_LostUser;							       //输牌用户

	YingSanZhangS2CCompareCard() :LMsgS2CGameMessage(MSG_YINGSANZHANG_S_2_C_COMPARE_CARD)
	{
		m_IsFinal=0;
		m_CurrentUser=INVAILD_POS_QIPAI;
		memset(m_CompareUser, 0, sizeof(m_CompareUser));
		m_LostUser=INVAILD_POS_QIPAI;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kIsFinal", m_IsFinal);
		WriteKeyValue(pack, "kCurrentUser", m_CurrentUser);
		
		std::vector<Lint> veci;
		WriteKey(pack, "kCompareUser");
		veci.clear();
		for (Lint i=0; i < 2; ++i)
		{
			veci.push_back(m_CompareUser[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, "kLostUser", m_LostUser);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CCompareCard();
	}

};

//////////////////////////////////////////////////////////////////////////
//赢三张-服务器端轮数
struct YingSanZhangS2CLun :public LMsgS2CGameCommon
{
	YingSanZhangS2CLun() :LMsgS2CGameCommon(MSG_YINGSANZHANG_S_2_C_LUN) {}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CLun();
	}
};

//////////////////////////////////////////////////////////////////////////
struct SystemCompareCard
{
	Lint								m_WinUser;						//赢家用户
	Lint								m_LostUser;							//输牌用户

	MSGPACK_DEFINE(m_WinUser, m_LostUser);
	SystemCompareCard()
	{
		m_WinUser=0;
		m_LostUser=0;
	}
	SystemCompareCard(int WinUser, int LostUser) {
		m_WinUser=WinUser;
		m_LostUser=LostUser;
	}

	bool Read(msgpack::object& obj)
	{
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(kWinUser), m_WinUser);
		WriteKeyValue(pack, NAME_TO_STR(kLostUser), m_LostUser);
		return true;
	}
};
//赢三张-系统比牌
struct YingSanZhangS2CSystemCompareCard :public LMsgS2CGameMessage
{
	std::vector<SystemCompareCard>  m_info;	//系统比牌结果

	YingSanZhangS2CSystemCompareCard() :LMsgS2CGameMessage(MSG_YINGSANZHANG_S_2_C_SYSTEM_COMPARE_CARD)
	{
	
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSubId", m_subID);
		WriteKeyValue(pack, "kInfo", m_info);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CSystemCompareCard();
	}

};

/* 三张牌：客户端玩家发给服务器买大买小的操作 */
struct YingSanZhangC2SUserSelectBigSmall :public LMsgSC
{
	Lint m_flag;    // 标记买大买小：0--买小，1--买大
	Lint m_score;   // 玩家买大买小操作结果：0--过，>0买大买小的分数

	YingSanZhangC2SUserSelectBigSmall() : LMsgSC(MSG_YINGSANZHANG_C_2_S_USER_SELECT_BIGSMALL) 
	{
		m_flag = 0;
		m_score = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kFlag", m_flag);
		ReadMapData(obj, "kScore", m_score);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangC2SUserSelectBigSmall();
	}
};

/* 三张牌：服务器广播玩家买大买小操作结果 */
struct YingSanZhangS2CUserSelectBigSmall :public LMsgSC
{
	Lint  m_pos;       //玩家位置
	Lint  m_flag;      //买大买小标记：0--买小，1--买大
	Lint  m_score;     //买大买小下的分数：0：过，>0--买大买小分数

	YingSanZhangS2CUserSelectBigSmall() : LMsgSC(MSG_YINGSANZHANG_S_2_C_USER_SELECT_BIGSMALL)
	{
		m_pos = INVAILD_POS_QIPAI;
		m_flag = 0;
		m_score = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kScore", m_score);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CUserSelectBigSmall();
	}
};

/* 三张牌：服务器广播更改买大买小方式为买大模式 */
struct YingSanZhangS2CChangeBigSmallMode :public LMsgSC
{
	Lint m_value;
	YingSanZhangS2CChangeBigSmallMode() : LMsgSC(MSG_YINGSANZHANG_S_2_C_CHANGE_BIGSMALL_MODE) 
	{
		m_value = 0;
	}
	

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kValue", m_value);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CChangeBigSmallMode();
	}
};

/* 三张牌：服务器广播买大买小结束，通知客户端开始下注 */
struct YingSanZhangS2CStartAddScore : public LMsgSC
{
	Lint m_addScorePos;   //开始下注的玩家位置
	Lint m_flag;          //买大买小最后确定的模式  0--选小  1- 选大    2--都选择过，默认的选择，没有人下分
	Lint m_score;         //买大买小最后下分结果
	Lint m_bigSmallPos;             //最终确定买大买小玩家
	Lstring m_nike;    //该玩家的昵称
	Lint    m_totalScore;      //桌上总筹码

	YingSanZhangS2CStartAddScore() : LMsgSC(MSG_YINGSANZHANG_S_2_C_START_ADD_SCORE)
	{
		m_addScorePos = INVAILD_POS_QIPAI;
		m_flag = 0;
		m_score = 0;
		m_bigSmallPos = INVAILD_POS_QIPAI;
		m_totalScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kAddScorePos", m_addScorePos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kScore", m_score);
		WriteKeyValue(pack, "kBigSmallPos", m_bigSmallPos);
		WriteKeyValue(pack, "kNike", m_nike);
		WriteKeyValue(pack, "kTotalScore", m_totalScore);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new YingSanZhangS2CStartAddScore();
	}
};

//////////////////////////////////////////////////////////////////////////
//推筒子-服务器游戏开始-选庄命令
struct TuiTongZiS2CStartGame:public LMsgSC
{
	Lint m_pos;

	TuiTongZiS2CStartGame() :LMsgSC(MSG_S_2_C_TUITONGZI_START_GAME)
	{
		m_pos = TUITONGZI_INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
	
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CStartGame();
	}

};


//推筒子-客户端选庄
struct TuiTongZiC2SSelectZhuang :public LMsgSC
{
	Lint m_value;

	TuiTongZiC2SSelectZhuang() :LMsgSC(MSG_C_2_S_TUITONGZI_SELECT_ZHUANG)
	{
		m_value=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kYes", m_value);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kYes", m_value);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiC2SSelectZhuang();
	}
};

//////////////////////////////////////////////////////////////////////////
//推筒子-服务器选庄
struct TuiTongZiS2CSelectZhuang :public LMsgSC
{
	Lint m_pos;
	Lint m_value;

	TuiTongZiS2CSelectZhuang() :LMsgSC(MSG_S_2_C_TUITONGZI_SELECT_ZHUANG)
	{
		m_pos=TUITONGZI_INVAILD_POS;
		m_value=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_value);
		ReadMapData(obj, "kYes", m_value);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kYes", m_value);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CSelectZhuang();
	}

};

//////////////////////////////////////////////////////////////////////////
//推筒子-客户端点色子
struct TuiTongZiC2SDoShaiZi :public LMsgSC
{
	Lint m_value;

	TuiTongZiC2SDoShaiZi() :LMsgSC(MSG_C_2_S_TUITONGZI_DO_SHAIZI)
	{
		m_value=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
	
		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiC2SDoShaiZi();
	}
};

//////////////////////////////////////////////////////////////////////////
//推筒子-服务器广播色子
struct TuiTongZiS2CDoShaiZi :public LMsgSC
{
	Lint                             m_type;                          //   0->广播 1->发给庄
	Lint								 m_dian1;						  //一个色子点
	Lint								 m_dian2;					    //另一个色子点
	Lint                             m_pos;                          //色子选中人的位置, 0-3
	
	TuiTongZiS2CDoShaiZi() :LMsgSC(MSG_S_2_C_TUITONGZI_DO_SHAIZI)
	{
		m_type=1;
		m_dian1=0;
		m_dian2=0;
		m_pos=TUITONGZI_INVAILD_POS;
	}

	void reset()
	{
		m_type=1;
		m_dian1=0;
		m_dian2=0;
		m_pos=TUITONGZI_INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kDian1", m_dian1);
		WriteKeyValue(pack, "kDian2", m_dian2);
		WriteKeyValue(pack, "kPos", m_pos);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CDoShaiZi();
	}

};

//////////////////////////////////////////////////////////////////////////
//推筒子-客户端用户选分
struct TuiTongZiC2SScore :public LMsgSC
{
	Lint  m_Ya;
	Lint  m_Dao;
	Lint  m_Ruan;
	Lint  m_Hong;

	TuiTongZiC2SScore() :LMsgSC(MSG_C_2_S_TUITONGZI_ADD_SCROE)
	{
		m_Ya=0;
		m_Dao=0;
		m_Ruan=0;
		m_Hong=0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kYa", m_Ya);
		ReadMapData(obj, "kDao", m_Dao);
		ReadMapData(obj, "kRuan", m_Ruan);
		ReadMapData(obj, "kHong", m_Hong);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiC2SScore();
	}

};
//////////////////////////////////////////////////////////////////////////
//推筒子-服务器用户选分
struct TuiTongZiS2CScore :public LMsgSC
{
	Lint                             m_pos;                          // 用户pos
	Lint								 m_score[TUITONGZI_XIAZHU_XIANG];    //下注分数
	
	TuiTongZiS2CScore() :LMsgSC(MSG_S_2_C_TUITONGZI_ADD_SCORE)
	{
		m_pos=0;
		memset(m_score, 0x00, sizeof(m_score));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> veci;
		WriteKey(pack, "kScore");
		veci.clear();
		for (Lint i=0; i < TUITONGZI_XIAZHU_XIANG; ++i)
		{
			veci.push_back(m_score[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CScore();
	}

};

//////////////////////////////////////////////////////////////////////////
//推筒子-服务器断线重连
struct TuiTongZiS2CRecon :public LMsgSC
{
	Lint m_play_status;        //游戏阶段，0-选庄 1-掷骰子 2-选分 3-开牌
	Lint m_remain_time;    //阶段剩余时间
	Lint m_currSelectPos;   //当前选庄位置
	Lint m_zhuangPos;        //庄位置
	Lint m_shaiZi;         //是否等待庄家掷色子 0-不是， 1-是
	Lint m_isScore;       //自己是否押分,0-没有 1-押分
	Lint m_isOpenCard;  //是否开牌
	Lint m_isSelectZhuang;  //是否选庄,0-没有, 1-选庄
	Lint m_userZhuang[TUITONGZI_PLAY_USER_COUNT];  //玩家选庄  0-没有选  1-选庄
	Lint m_score[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_XIAZHU_XIANG];  //  0-押  1-道 2-软 4-红   （0-押,不能未0） -庄家位置不用显示
	Lint	 m_CardData[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_HAND_CARD_MAX];			                    //用户扑克
	Lint m_DynamicJoin;               //是否是动态加入用户，0--不是 1--是

	TuiTongZiS2CRecon() :LMsgSC(MSG_S_2_C_TUITONGZI_RECON)
	{
		m_zhuangPos=TUITONGZI_INVAILD_POS;
		m_currSelectPos = TUITONGZI_INVAILD_POS;
		m_shaiZi=0;
		m_isScore=0;
		m_play_status = 0;
		m_isOpenCard = 0;
		m_remain_time = 0;
		m_isSelectZhuang = 0;
		m_DynamicJoin = 0;

		for (int i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			m_userZhuang[i]=0;
			memset(m_score[i], 0x00, sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
			memset(m_CardData[i], 0x00, sizeof(Lint)*TUITONGZI_HAND_CARD_MAX);
		}
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 26);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPlayStatus", m_play_status);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		//WriteKeyValue(pack, "kShaiZi", m_shaiZi);
		WriteKeyValue(pack, "kIsScore", m_isScore);
		WriteKeyValue(pack, "kCurrSelectPos", m_currSelectPos);
		WriteKeyValue(pack, "kIsOpenCard", m_isOpenCard);
		WriteKeyValue(pack, "kRemain_time", m_remain_time);
		WriteKeyValue(pack, "kIsSelectZhuang", m_isSelectZhuang);
		WriteKeyValue(pack, "kDynamicJoin", m_DynamicJoin);
	
		std::vector<Lint> veci;
		for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kScore%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < TUITONGZI_XIAZHU_XIANG; j++)
			{
				veci.push_back(m_score[i][j]);
			}
			WriteKey(pack, veci);
		}

		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kCardData%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < TUITONGZI_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_CardData[i][j]);
			}
			WriteKey(pack, veci);
		}

		WriteKey(pack, "kUserZhuang");
		veci.clear();
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_userZhuang[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CRecon();
	}

};

//////////////////////////////////////////////////////////////////////////
//推筒子-服务器小结算
struct TuiTongZiS2CResult :public LMsgSC
{
	Lint            m_FinalDw;                                                                                                                                                                  //最后一局 1-是最后一局，0-不是
	Lint            m_UserStatus[TUITONGZI_PLAY_USER_COUNT];                                                                                            //用户状态，0-没有参与游戏，1--参与游戏  , 2--动态加入者，没有参与本局游戏
	Lint			    m_CardData[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_HAND_CARD_MAX];			                    //用户扑克
	Lint			    m_GameScore[TUITONGZI_PLAY_USER_COUNT];	                                                                                       //每个用户游戏总得分
	Lint            m_Score[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_XIAZHU_XIANG];                                          //每个用户，游戏开始时押道软红选择的分数
	Lint            m_ResultScore[TUITONGZI_PLAY_USER_COUNT][TUITONGZI_XIAZHU_XIANG];                            //每个用户，押道软红输赢的分数
	Lint            m_ZhuangPos;                                                                                                                                                          //庄位
	Lint            m_TotalScore[TUITONGZI_PLAY_USER_COUNT];                                                                                           //用户总得分每局累计



	TuiTongZiS2CResult() :LMsgSC(MSG_S_2_C_TUITONGZI_DRAW_RESULT)
	{
		m_FinalDw=0;
		m_ZhuangPos=TUITONGZI_INVAILD_POS;
		for (int i=0; i<TUITONGZI_PLAY_USER_COUNT; i++)
		{
			m_UserStatus[i] = 0;
			memset(m_CardData[i], 0x00, sizeof(Lint)*TUITONGZI_HAND_CARD_MAX);
			m_GameScore[i]=0;
			memset(m_Score[i], 0x00, sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
			memset(m_ResultScore[i], 0x00, sizeof(Lint)*TUITONGZI_XIAZHU_XIANG);
			m_TotalScore[i] = 0;
		}
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 30);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kFinalDw", m_FinalDw);
		WriteKeyValue(pack, "kZhuangPos", m_ZhuangPos);

		std::vector<Lint> veci;

		for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kCardData%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < TUITONGZI_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_CardData[i][j]);
			}
			WriteKey(pack, veci);
		}

		WriteKey(pack, "kGameScore");
		veci.clear();
		for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_GameScore[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kUserStatus");
		veci.clear();
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_UserStatus[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kTotalScore");
		veci.clear();
		for (Lint i = 0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_TotalScore[i]);
		}
		WriteKey(pack, veci);



		for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kScore%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < TUITONGZI_XIAZHU_XIANG; j++)
			{
				veci.push_back(m_Score[i][j]);
			}
			WriteKey(pack, veci);
		}

		for (Lint i=0; i < TUITONGZI_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();
			sprintf(name, "kResultScore%d", i);
			WriteKey(pack, name);
			for (Lint j=0; j < TUITONGZI_XIAZHU_XIANG; j++)
			{
				veci.push_back(m_ResultScore[i][j]);
			}
			WriteKey(pack, veci);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2CResult();
	}

};


//////////////////////////////////////////////////////////////////////////
//推筒子-服务器发牌
struct TuiTongZiS2COpenCard :public LMsgSC
{
	Lint                             m_showCard;
	Lint                             m_pos;
	Lint								 m_userCard[TUITONGZI_HAND_CARD_MAX];    //下注分数

	TuiTongZiS2COpenCard() :LMsgSC(MSG_S_2_C_TUITONGZI_OPEN_CARD)
	{
		m_showCard = 0;
		m_pos=TUITONGZI_INVAILD_POS;
		memset(m_userCard, 0x00, sizeof(m_userCard));
	}

	void reset()
	{
		m_showCard=0;
		m_pos=TUITONGZI_INVAILD_POS;
		memset(m_userCard, 0x00, sizeof(m_userCard));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kShowCard", m_showCard);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> veci;
		WriteKey(pack, "kUserCard");
		veci.clear();
		for (Lint i=0; i < TUITONGZI_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_userCard[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiS2COpenCard();
	}

};

//////////////////////////////////////////////////////////////////////////
//推筒子-客户端亮牌
struct TuiTongZiC2SOpenCard :public LMsgSC
{
	
	TuiTongZiC2SOpenCard() :LMsgSC(MSG_C_2_S_TUITONGZI_OPEN_CARD)
	{
	
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new TuiTongZiC2SOpenCard();
	}

};


//////////////////////////////////////////////////////////////////////////
struct  ReadyRemainTime :public LMsgSC
{
	Lint m_remainTime;
	Lint m_currCirCle;

	ReadyRemainTime() :LMsgSC(MSG_S_2_C_READY_TIME_REMAIN)
	{
		m_remainTime = 0;
		m_currCirCle = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRemainTime", m_remainTime);
		WriteKeyValue(pack, "kCurCircle", m_currCirCle);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ReadyRemainTime();
	}

};

//////////////////////////////////////////////////////////////////////////
//牛牛消息体

/*
牛牛 服务器发送游戏开始消息 S->C（结构体）
MSG_S_2_C_NIUNIU_START_GAME = 62077
*/
struct NiuNiuS2CStartGame : public LMsgSC
{
	Lint m_remainTime;			  //下注阶段倒计时间(买码 + 下注)
	Lint m_zhuangPos;             //庄家位置
	Lint m_scoreTimes;            //倍数，默认为1倍，抢庄模式下可能为多倍
	Lint m_maiMa;				  //是否买码：0-不买码， 1-买码
	Lint m_carToon;					//是否执行动画  0：不执行动画  1：能量场中，庄家没能量了，执行随机选装动画
	Lint m_isAllNoQiang;			//是否所有玩家都没有抢庄   1：所有玩家都没有抢庄    0：有玩家抢庄
	Lint m_isCanTui[NIUNIU_PLAY_USER_COUNT];			//是否可以推注
	Lint m_playerTuiScore[NIUNIU_PLAY_USER_COUNT][4];	//推注分数，最多4个  0：没有推注资格，>0 可以推注的分数
	Lint m_playerXiaScore[NIUNIU_PLAY_USER_COUNT];  //下注限制 0：可以下最小分，1 不可以下最小份
	Lint m_playerAddScoreDouble[NIUNIU_PLAY_USER_COUNT];  //是否可以下注加倍 0：不可以下注加倍， 1：可以下注加倍
	Lint m_playerHandCard[NIUNIU_HAND_CARD_MAX];
	Lint m_playerStatus[NIUNIU_PLAY_USER_COUNT];
	Lint m_qingScore[NIUNIU_PLAY_USER_COUNT];       //客户端发来的抢庄分数

	NiuNiuS2CStartGame() : LMsgSC(MSG_S_2_C_NIUNIU_START_GAME)
	{
		m_remainTime = -1;
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_scoreTimes = 1;
		m_maiMa = 0;
		m_carToon = 0;
		m_isAllNoQiang = 1;
		memset(m_isCanTui, 0, sizeof(m_isCanTui));
		memset(m_playerTuiScore, 0, sizeof(m_playerTuiScore));
		memset(m_playerXiaScore, 0, sizeof(m_playerXiaScore));
		memset(m_playerAddScoreDouble, 0, sizeof(m_playerAddScoreDouble));
		memset(m_playerHandCard, 0x00, sizeof(m_playerHandCard));
		memset(m_playerStatus, 0x00, sizeof(m_playerStatus));
		memset(m_qingScore, 0x00, sizeof(m_qingScore));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 23);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRemainTime", m_remainTime);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kScoreTimes", m_scoreTimes);
		WriteKeyValue(pack, "kMaiMa", m_maiMa);
		WriteKeyValue(pack, "kCarToon", m_carToon);
		WriteKeyValue(pack, "kIsAllNoQiang", m_isAllNoQiang);
		std::vector<Lint> veci;

		veci.clear();
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_isCanTui[i]);
		}
		WriteKeyValue(pack, "kIsCanTui", veci);

		for (size_t i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.clear();
			char name[32];
			sprintf(name, "kPlayerTuiScore%d", i);
			WriteKey(pack, name);
			for (size_t j = 0; j < 4; ++j)
			{
				veci.push_back(m_playerTuiScore[i][j]);
			}
			WriteKey(pack, veci);
		}

		veci.clear();
		WriteKey(pack, "kPlayerXiaScore");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerXiaScore[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kPlayerAddScoreDouble");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerAddScoreDouble[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kPlayerHandCard");
		for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_playerHandCard[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kPlayerStatus");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kQiangScore");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_qingScore[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CStartGame();
	}
};

/*
牛牛玩家选庄消息 C->S (结构体)
MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071
*/
struct NiuNiuC2SSelectZhuang : public LMsgSC
{
	Lint m_qingScore;    //客户端发来的抢庄分数

	NiuNiuC2SSelectZhuang() : LMsgSC(MSG_C_2_S_NIUNIU_SELECT_ZHUANG)
	{
		m_qingScore = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kQiangScore", m_qingScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kQiangScore", m_qingScore);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SSelectZhuang();
	}
};

/*
牛牛广播玩家选庄结果消息 S->C (结构体)
MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG = 62080
*/
struct NiuNiuS2CQiangZhuangNotify : public LMsgSC
{
	Lint m_qiangZhuangScore;    //客户端发来的抢庄分数
	Lint m_pos;					//玩家位置

	NiuNiuS2CQiangZhuangNotify() : LMsgSC(MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG)
	{
		m_qiangZhuangScore = 0;
		m_pos = NIUNIU_INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kQiangScore", m_qiangZhuangScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kQiangScore", m_qiangZhuangScore);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CQiangZhuangNotify();
	}
};

/*
牛牛服务器返回庄家消息 S->C (结构体)
MSG_S_2_C_NIUNIU_SELECT_ZHUANG = 62072
*/
struct NiuNiuS2CSelectZhuang : public LMsgSC
{
	Lint m_remainTime;		//抢庄倒计时间
	Lint m_pos;           //玩家位置
	Lint m_qingZhuang;    //通知客户端抢庄：0-非看牌抢庄模式，1-看牌抢庄模式，2-轮庄第一局抢庄
	Lint m_qiangTimes;	  //抢庄倍率：1-不抢/抢  2-不抢/1/2  3-不抢/1/2/3  4...
	Lint m_noQiangZhuang; //限制抢庄  0：不能抢庄  1：可以抢庄
	Lint m_isCanTui[NIUNIU_PLAY_USER_COUNT];	//
	Lint m_playerHandCard[NIUNIU_HAND_CARD_MAX];   //该玩家的手牌，看牌抢庄需要亮出前四张牌给玩家（暂定。。。不行再分开发消息哦）
	Lint m_playerStatus[NIUNIU_PLAY_USER_COUNT];

	NiuNiuS2CSelectZhuang() : LMsgSC(MSG_S_2_C_NIUNIU_SELECT_ZHUANG)
	{
		m_remainTime = -1;
		m_pos = NIUNIU_INVAILD_POS;
		m_qingZhuang = 0;
		m_qiangTimes = 1;
		m_noQiangZhuang = 1;
		memset(m_isCanTui, 0, sizeof(m_isCanTui));
		memset(m_playerHandCard, 0x00, sizeof(m_playerHandCard));
		memset(m_playerStatus, 0x00, sizeof(m_playerStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRemainTime", m_remainTime);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kQiangZhuang", m_qingZhuang);
		WriteKeyValue(pack, "kQiangTimes", m_qiangTimes);
		WriteKeyValue(pack, "kNoQiangZhuang", m_noQiangZhuang);

		std::vector<Lint> veci;

		veci.clear();
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_isCanTui[i]);
		}
		WriteKeyValue(pack, "kIsCanTui", veci);

		veci.clear();
		WriteKey(pack, "kPlayerHandCard");
		for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_playerHandCard[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kPlayerStatus");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerStatus[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CSelectZhuang();
	}

};

/*
牛牛玩家买码消息 C->S（结构体）
MSG_C_2_S_NIUNIU_MAI_MA = 62082
*/
struct NiuNiuC2SMaiMa : public LMsgSC
{
	Lint m_pos;			//买码的玩家位置
	Lint m_maiPos;		//买的玩家位置
	Lint m_maiScore;	//买码的分数

	NiuNiuC2SMaiMa() : LMsgSC(MSG_C_2_S_NIUNIU_MAI_MA)
	{
		m_pos = NIUNIU_INVAILD_POS;
		m_maiPos = NIUNIU_INVAILD_POS;
		m_maiScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kMaiPos", m_maiPos);
		ReadMapData(obj, "kMaiScore", m_maiScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SMaiMa();
	}
};

/*
牛牛服务器广播玩家买码结果
MSG_S_2_C_NIUNIU_MAI_MA_BC = 62083
*/
struct NiuNiuS2CMaiMaBC : public LMsgSC
{
	Lint m_pos;			//买码的玩家位置
	Lint m_maiMa[NIUNIU_PLAY_USER_COUNT][2]; //玩家买码操作结果[0]:分数 [1]:位置
	Lint m_maiPos;		//买的玩家位置
	Lint m_maiScore;	//买码的分数

	NiuNiuS2CMaiMaBC() : LMsgSC(MSG_S_2_C_NIUNIU_MAI_MA_BC)
	{
		m_pos = NIUNIU_INVAILD_POS;
		m_maiPos = NIUNIU_INVAILD_POS;
		m_maiScore = 0;
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			m_maiMa[i][0] = -1;
			m_maiMa[i][1] = NIUNIU_INVAILD_POS;
		}
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 14);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kMaiPos", m_maiPos);
		WriteKeyValue(pack, "kMaiScore", m_maiScore);

		std::vector<Lint> veci;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kMaiMa%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < 2; j++)
			{
				veci.push_back(m_maiMa[i][j]);
			}
			WriteKey(pack, veci);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CMaiMaBC();
	}
};

/*
牛牛玩家下注消息 C->S (结构体)
MSG_C_2_S_NIUNIU_ADD_SCORE = 62073
*/
struct NiuNiuC2SAddScore : public LMsgSC
{
	Lint m_score;				//客户端玩家下注分数
	Lint m_isAddScoreDouble;	//玩家下注是否加倍  0：没有加倍  1加倍
	Lint m_isTuiScore;			//玩家下注是否为推注   0：没有推注   1：推注

	NiuNiuC2SAddScore() : LMsgSC(MSG_C_2_S_NIUNIU_ADD_SCORE)
	{
		m_score = 0;
		m_isAddScoreDouble = 0;
		m_isTuiScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kScore", m_score);
		ReadMapData(obj, "kIsAddScoreDouble", m_isAddScoreDouble);
		ReadMapData(obj, "kIsTuiScore", m_isTuiScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SAddScore();
	}
};

/*
牛牛服务器返回玩家下注消息 S->C (结构体)
MSG_S_2_C_NIUNIU_ADD_SCORE = 62074
*/
struct NiuNiuS2CAddScore : public LMsgSC
{
	Lint m_pos;												//玩家位置
	Lint m_score;											//玩家下注分数
	Lint m_addScoreDoubleStatus[NIUNIU_PLAY_USER_COUNT];	//玩家下注加倍状态

	NiuNiuS2CAddScore() : LMsgSC(MSG_S_2_C_NIUNIU_ADD_SCORE)
	{
		m_pos = NIUNIU_INVAILD_POS;
		m_score = 0;
		memset(m_addScoreDoubleStatus, 0, sizeof(m_addScoreDoubleStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kScore", m_score);

		std::vector<Lint> veci;

		veci.clear();
		for (size_t i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_addScoreDoubleStatus[i]);
		}
		WriteKeyValue(pack, "kAddScoreDoubleStatus", veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CAddScore();
	}
};

/*
牛牛玩家发牌/亮牌消息 C->S (结构体)
MSG_C_2_S_NIUNIU_OPEN_CARD = 62075
*/
struct NiuNiuC2SOpenCard : public LMsgSC
{
	Lint m_pos;                             //玩家位置

	NiuNiuC2SOpenCard() : LMsgSC(MSG_C_2_S_NIUNIU_OPEN_CARD)
	{
		m_pos = NIUNIU_INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SOpenCard();
	}
};

/*
牛牛服务器返回玩家发牌 或 亮牌消息 S->C (结构体)
MSG_S_2_C_NIUNIU_OPEN_CARD = 62076
*/
struct NiuNiuS2COpenCard : public LMsgSC
{
	Lint m_remainTime;						//亮牌倒计时间（搓牌+翻牌+亮牌）
	Lint m_show;                             //发牌 & 亮牌：0-发牌，1-亮牌
	Lint m_pos;                              //玩家位置
	Lint m_oxNum;                            //牛牛数据
	Lint m_oxTimes;							 //牛牛倍数
	Lint m_gongCard;						 //公牌牛牛使用的公牌
	Lint m_openGongPos;						 //开公牌玩家位置
	Lint m_playerHandCard[NIUNIU_HAND_CARD_MAX];  //玩家手牌
	Lint m_showHandCard[NIUNIU_HAND_CARD_MAX];		//玩家手牌排序过后的
	Lint m_playerStatus[NIUNIU_PLAY_USER_COUNT];

	NiuNiuS2COpenCard() : LMsgSC(MSG_S_2_C_NIUNIU_OPEN_CARD)
	{
		m_remainTime = -1;
		m_show = 0;
		m_pos = NIUNIU_INVAILD_POS;
		m_oxNum = 0;
		m_oxTimes = 0;
		m_gongCard = 0;
		m_openGongPos = NIUNIU_INVAILD_POS;
		memset(m_playerHandCard, 0x00, sizeof(m_playerHandCard));
		memset(m_showHandCard, 0, sizeof(m_showHandCard));
		memset(m_playerStatus, 0x00, sizeof(m_playerStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRemainTime", m_remainTime);
		WriteKeyValue(pack, "kShow", m_show);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOxNum", m_oxNum);
		WriteKeyValue(pack, "kOxTimes", m_oxTimes);
		WriteKeyValue(pack, "kGongCard", m_gongCard);
		WriteKeyValue(pack, "kOpenGongPos", m_openGongPos);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kPlayerHandCard");
		for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_playerHandCard[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_showHandCard[i]);
		}
		WriteKeyValue(pack, "kShowHandCard", veci);

		veci.clear();
		WriteKey(pack, "kPlayerStatus");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerStatus[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2COpenCard();
	}
};

/*
牛牛服务器返回小结算消息 S->C （结构体）
MSG_S_2_C_NIUNIU_DRAW_RESULT = 62078
*/
struct NiuNiuS2CResult : public LMsgSC
{
	Lint m_zhuangPos;
	Lint m_isShowFee;	//是弹出扣费通知  0：不弹出   1：弹出
	Lint m_isZhuangAllWin;		//0:普通   1：庄家全赢   2：庄家全输
	Lint m_playerHandCard[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];
	Lint m_showHandCard[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];
	Lint m_playScore[NIUNIU_PLAY_USER_COUNT];
	Lint m_totleScore[NIUNIU_PLAY_USER_COUNT];
	Lint m_totalCoins[NIUNIU_PLAY_USER_COUNT];
	Lint m_oxNum[NIUNIU_PLAY_USER_COUNT];
	Lint m_oxTimes[NIUNIU_PLAY_USER_COUNT];
	Lint m_playerStatus[NIUNIU_PLAY_USER_COUNT];
	Lint m_playerCoinsStatus[NIUNIU_PLAY_USER_COUNT];    // 0：不用显示能量值不足   1：因为能量值不足输的不多

	NiuNiuS2CResult() : LMsgSC(MSG_S_2_C_NIUNIU_DRAW_RESULT)
	{
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_isShowFee = 0;
		m_isZhuangAllWin = 0;
		memset(m_playerHandCard, 0x00, sizeof(m_playerHandCard));
		memset(m_showHandCard, 0, sizeof(m_showHandCard));
		memset(m_playScore, 0x00, sizeof(m_playScore));
		memset(m_totleScore, 0x00, sizeof(m_totleScore));
		memset(m_oxNum, 0x00, sizeof(m_oxNum));
		memset(m_oxTimes, 0x00, sizeof(m_oxTimes));
		memset(m_totalCoins, 0, sizeof(m_totalCoins));
		memset(m_playerStatus, 0, sizeof(m_playerStatus));
		memset(m_playerCoinsStatus, 0, sizeof(m_playerCoinsStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, (11 + NIUNIU_PLAY_USER_COUNT * 2));
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kIsShowFee", m_isShowFee);
		WriteKeyValue(pack, "kIsZhuangAllWin", m_isZhuangAllWin);

		std::vector<Lint> veci;
		veci.clear();
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerStatus[i]);
		}
		WriteKeyValue(pack, "kPlayerStatus", veci);

		//算10个
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kPlayerHandCard%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_playerHandCard[i][j]);
			}
			WriteKey(pack, veci);
		}

		//算10个
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kShowHandCard%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_showHandCard[i][j]);
			}
			WriteValue(pack, veci);
		}
		//
		veci.clear();
		WriteKey(pack, "kPlayScore");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playScore[i]);
		}
		WriteKey(pack, veci);
		//
		veci.clear();
		WriteKey(pack, "kTotleScore");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);
		//
		veci.clear();
		WriteKey(pack, "kTotalCoins");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_totalCoins[i]);
		}
		WriteKey(pack, veci);
		//
		veci.clear();
		WriteKey(pack, "kOxNum");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_oxNum[i]);
		}
		WriteKey(pack, veci);
		//
		veci.clear();
		WriteKey(pack, "kOxTimes");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_oxTimes[i]);
		}
		WriteKey(pack, veci);

		//
		veci.clear();
		WriteKey(pack, "kPlayerCoinsStatus");
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_playerCoinsStatus[i]);
		}
		WriteValue(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CResult();
	}


};

/*
牛牛玩家断线重连消息 S->C（结构体）
MSG_S_2_C_NIUNIU_RECON = 62079
*/
struct NiuNiuS2CRecon :public LMsgSC
{
	Lint m_playStatus;         //游戏阶段，0-选庄 1-选分 2-开牌
	Lint m_remainTime;         //阶段剩余时间
	Lint m_zhuangPos;           //庄位置
	Lint m_DynamicJoin;       // 玩家是否动态加入的
	Lint m_isLookOn;			//是否为观战状态  0：入座状态， 1：观战状态
	Lint m_isCoinsGame;			//是否为竞技场   0：普通场   1：竞技场
	Lint m_totleScore[NIUNIU_PLAY_USER_COUNT];	//玩家当局当前能量值
	Lint m_isCanTui[NIUNIU_PLAY_USER_COUNT];
	Lint m_playerTuiScore[NIUNIU_PLAY_USER_COUNT][4];	//推注分数1  0：没有推注资格，>0 可以推注的分数
	Lint m_user_status[NIUNIU_PLAY_USER_COUNT];       //玩家状态
	Lint m_gongCard;						 //公牌牛牛使用的公牌
	Lint m_openGongPos;						 //开公牌玩家位置
	Lstring m_cuoPai;							//搓牌数据
	Lint m_maiMaPos[NIUNIU_PLAY_USER_COUNT];			//买码位置
	Lint m_maiMaScore[NIUNIU_PLAY_USER_COUNT];			//买码分数
	Lint m_hasMaiMa[NIUNIU_PLAY_USER_COUNT];			//是否买码过 0-没有  1-已经买码
	Lint m_hasScore[NIUNIU_PLAY_USER_COUNT];            //自己是否押分,0-没有 1-押分
	Lint m_hasOpenCard[NIUNIU_PLAY_USER_COUNT];         //是否开牌 0-未开牌，1-已开牌
	Lint m_hasSelectZhuang;     //是否选庄,0-没有, 1-选庄
	Lint m_noQiangZhuang; //限制抢庄  0：不能抢庄  1：可以抢庄
	Lint m_addZhuang[NIUNIU_PLAY_USER_COUNT];      //玩家选庄  0-没有选  1-选庄
	Lint m_addScore[NIUNIU_PLAY_USER_COUNT];       //下注分数
	Lint m_oxTimes[NIUNIU_PLAY_USER_COUNT];			//牛牛倍数
	Lint m_player_oxnum[NIUNIU_PLAY_USER_COUNT];    // 玩家牛数
	Lint m_player_ox[NIUNIU_PLAY_USER_COUNT];		//是否有牛
	Lint m_playerAddScoreDouble[NIUNIU_PLAY_USER_COUNT];  //是否可以下注加倍 0：不可以下注加倍， 1：可以下注加倍
	Lint m_addScoreDoubleStatus[NIUNIU_PLAY_USER_COUNT];	//玩家下注加倍状态
	Lint m_playerXiaScore[NIUNIU_PLAY_USER_COUNT];		//下注限制 0：不限制，可以下最小分， 1 不可以下最小份
	Lint m_playerHandCard[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];	 //玩家手牌
	Lint m_showHandCard[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];

	NiuNiuS2CRecon() :LMsgSC(MSG_S_2_C_NIUNIU_RECON)
	{
		m_playStatus = 0;
		m_remainTime = 0;
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_hasSelectZhuang = 0;
		m_noQiangZhuang = 1;
		m_DynamicJoin = 0;
		m_isLookOn = 0;
		m_gongCard = 0;
		m_openGongPos = NIUNIU_INVAILD_POS;
		m_isCoinsGame = 0;

		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			m_hasOpenCard[i] = 0;
			m_hasScore[i] = 0;
			m_addZhuang[i] = 0;
			m_addScore[i] = 0;
			m_hasMaiMa[i] = 0;
			m_maiMaPos[i] = NIUNIU_INVAILD_POS;
			m_maiMaScore[i] = 0;
			m_player_oxnum[i] = 0;
			m_user_status[i] = 0;
			m_player_ox[i] = 0;
			m_oxTimes[i] = 0;
			m_playerAddScoreDouble[i] = 0;
			m_addScoreDoubleStatus[i] = 0;
			m_playerXiaScore[i] = 0;
			m_isCanTui[i] = 0;
			m_totleScore[i] = 0;
			memset(m_playerHandCard[i], 0x00, sizeof(Lint) * NIUNIU_HAND_CARD_MAX);
			memset(m_showHandCard[i], 0, sizeof(Lint) * NIUNIU_HAND_CARD_MAX);
		}
		memset(m_playerTuiScore, 0, sizeof(m_playerTuiScore));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, (28 + 3*NIUNIU_PLAY_USER_COUNT));
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPlayStatus", m_playStatus);
		WriteKeyValue(pack, "kGongCard", m_gongCard);
		WriteKeyValue(pack, "kOpenGongPos", m_openGongPos);
		WriteKeyValue(pack, "kCuoPai", m_cuoPai);
		WriteKeyValue(pack, "kRemainTime", m_remainTime);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kHasSelectZhuang", m_hasSelectZhuang);
		WriteKeyValue(pack, "kNoQiangZhuang", m_noQiangZhuang);
		WriteKeyValue(pack, "kDynamicJoin", m_DynamicJoin);
		WriteKeyValue(pack, "kIsLookOn", m_isLookOn);
		WriteKeyValue(pack, "kIsCoinsGame", m_isCoinsGame);

		std::vector<Lint> vec_player_status;
		std::vector<Lint> vec_zhuang_scores;
		std::vector<Lint> vec_has_add_scores;
		std::vector<Lint> vec_scores;
		std::vector<Lint> vec_is_open_card;
		std::vector<Lint> vecOxTimes;
		std::vector<Lint> vec_niu_num;
		std::vector<Lint> vec_niu_ox;
		std::vector<Lint> vecHasMaiMa;
		std::vector<Lint> vecMaiMaPos;
		std::vector<Lint> vecMaiMaScore;
		std::vector<Lint> veciPlayerAddScoreDouble;
		std::vector<Lint> veciAddScoreDoubleStatus;
		std::vector<Lint> veciXiaScore;
		std::vector<Lint> veciCanTui;
		std::vector<Lint> veciTotlaScore;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			vec_player_status.push_back(m_user_status[i]);

			vec_zhuang_scores.push_back(m_addZhuang[i]);

			vec_has_add_scores.push_back(m_hasScore[i]);
			vec_scores.push_back(m_addScore[i]);

			vec_is_open_card.push_back(m_hasOpenCard[i]);
			vecOxTimes.push_back(m_oxTimes[i]);
			vec_niu_ox.push_back(m_player_ox[i]);
			vec_niu_num.push_back(m_player_oxnum[i]);
			vecHasMaiMa.push_back(m_hasMaiMa[i]);
			vecMaiMaPos.push_back(m_maiMaPos[i]);
			vecMaiMaScore.push_back(m_maiMaScore[i]);
			veciPlayerAddScoreDouble.push_back(m_playerAddScoreDouble[i]);
			veciAddScoreDoubleStatus.push_back(m_addScoreDoubleStatus[i]);
			veciXiaScore.push_back(m_playerXiaScore[i]);
			veciCanTui.push_back(m_isCanTui[i]);
			veciTotlaScore.push_back(m_totleScore[i]);
		}

		WriteKey(pack, "kPlayerStatus");
		WriteValue(pack, vec_player_status);

		WriteKey(pack, "kAddZhuang");
		WriteValue(pack, vec_zhuang_scores);

		WriteKey(pack, "kHasScore");
		WriteValue(pack, vec_has_add_scores);

		WriteKey(pack, "kAddScore");
		WriteValue(pack, vec_scores);

		//是否开牌
		WriteKey(pack, "kHasOpenCard");
		WriteValue(pack, vec_is_open_card);

		//牛牛倍数
		WriteKey(pack, "kOxTimes");
		WriteValue(pack, vecOxTimes);
		//有牛
		WriteKey(pack, "kHasOx");
		WriteValue(pack, vec_niu_ox);

		//牛牛数
		WriteKey(pack, "kOxNum");
		WriteValue(pack, vec_niu_num);

		//买码
		WriteKey(pack, "kHasMaiMa");
		WriteValue(pack, vecHasMaiMa);
		WriteKey(pack, "kMaiMaPos");
		WriteValue(pack, vecMaiMaPos);
		WriteKey(pack, "kMaiMaScore");
		WriteValue(pack, vecMaiMaScore);

		//是否加倍
		WriteKeyValue(pack, "kPlayerAddScoreDouble", veciPlayerAddScoreDouble);
		WriteKeyValue(pack, "kAddScoreDoubleStatus", veciAddScoreDoubleStatus);

		//下注限制
		WriteKeyValue(pack, "kPlayerXiaScore", veciXiaScore);

		WriteKeyValue(pack, "kIsCanTui", veciCanTui);

		WriteKeyValue(pack, "kTotleScore", veciTotlaScore);

		std::vector<Lint> veci;
		//推注，算10个
		for (size_t i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			veci.clear();
			char name[32];
			sprintf(name, "kPlayerTuiScore%d", i);
			WriteKey(pack, name);
			for (size_t j = 0; j < 4; ++j)
			{
				veci.push_back(m_playerTuiScore[i][j]);
			}
			WriteKey(pack, veci);
		}
		//手牌算10个
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();
			sprintf(name, "kPlayerHandCard%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_playerHandCard[i][j]);
			}
			WriteKey(pack, veci);
		}
		//排序手牌算10个
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kShowHandCard%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				veci.push_back(m_showHandCard[i][j]);
			}
			WriteValue(pack, veci);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CRecon();
	}
};

/*
牛牛明牌下注消息 S->C（结构体）
MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE = 62081
*/
struct NiuNiuS2CMingPaiAddScore :public LMsgSC
{
	Lint m_pos;                                    //玩家位置
	Lint m_playerHandCard[NIUNIU_HAND_CARD_MAX];   //该玩家的手牌，看牌抢庄需要亮出前四张牌给玩家（暂定。。。不行再分开发消息哦）

	NiuNiuS2CMingPaiAddScore() : LMsgSC(MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE)
	{
		m_pos = INVAILD_POS_QIPAI;
		memset(m_playerHandCard, 0x00, sizeof(m_playerHandCard));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kPlayerHandCard");
		for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; ++i)
		{
			veci.push_back(m_playerHandCard[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CMingPaiAddScore();
	}
};

/*
  牛牛：玩家选择托管消息
  MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
*/
struct NiuNiuC2STuoGuan : public LMsgSC
{
	Lint	m_pos;
	Lint	m_qiangScore;
	Lint	m_addScore;
	Lint	m_tuiScore;

	NiuNiuC2STuoGuan() : LMsgSC(MSG_C_2_S_NIUNIU_TUO_GUAN)
	{
		m_pos = NIUNIU_INVAILD_POS;
		m_qiangScore = 0;
		m_addScore = 0;
		m_tuiScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kQiangScore", m_qiangScore);
		ReadMapData(obj, "kAddScore", m_addScore);
		ReadMapData(obj, "kTuiScore", m_tuiScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2STuoGuan();
	}
};

/*
牛牛：玩家取消托管消息
MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
*/
struct NiuNiuC2SCancelTuoGuan : public LMsgSC
{
	Lint	m_pos;

	NiuNiuC2SCancelTuoGuan() : LMsgSC(MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN)
	{
		m_pos = NIUNIU_INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SCancelTuoGuan();
	}
};

/*
  牛牛：广播玩家托管状态
  MSG_S_2_C_NIUNIU_TUO_GUAN_BC = 62086
*/
struct NiuNiuS2CTuoGuanBC : public LMsgSC
{
	Lint	m_pos;
	Lint	m_tuoGuanStatus;		//托管状态  0：未托管   1：已托管

	NiuNiuS2CTuoGuanBC() : LMsgSC(MSG_S_2_C_NIUNIU_TUO_GUAN_BC)
	{
		m_pos = NIUNIU_INVAILD_POS;
		m_tuoGuanStatus = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kTuoGuanStatus", m_tuoGuanStatus);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CTuoGuanBC();
	}
};

/*
  牛牛：玩家搓公牌
  MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
*/
struct NiuNiuC2SCuoGongPai : public LMsgSC
{
	//Lint m_precent;		//搓牌百分比
	Lstring m_cuoPai;


	NiuNiuC2SCuoGongPai() : LMsgSC(MSG_C_2_S_NIUNIU_CUO_GONG_PAI)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kCuoPai", m_cuoPai);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SCuoGongPai();
	}
};

/*
  牛牛：广播玩家搓公牌
  MSG_S_2_C_NIUNIU_CUO_GONG_PAI_BC = 62089
*/
struct NiuNiuS2CCuoGongPaiBC : public LMsgSC
{
	Lstring m_cuoPai;

	NiuNiuS2CCuoGongPaiBC() : LMsgSC(MSG_S_2_C_NIUNIU_CUO_GONG_PAI_BC)
	{
	}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kCuoPai", m_cuoPai);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuS2CCuoGongPaiBC();
	}
};
/*
  客户端通知服务器开始计时
  MSG_C_2_S_START_TIME = 61198
*/
struct NiuNiuC2SStartTime : public LMsgSC
{
	Lint m_pos;


	NiuNiuC2SStartTime() : LMsgSC(MSG_C_2_S_START_TIME)
	{
		m_pos = INVAILD_POS_QIPAI;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new NiuNiuC2SStartTime();
	}
};

////////////////////////////////////////////////////////////////////////////////////
//斗地主
//服务器端消息
struct MHLMsgDouDiZhuS2CGameMessage : public LMsgSC
{
	Lint sub_cmd_code;
	union
	{
		CMD_S_GameStart cmd_s2c_game_start;   //开始游戏
		CMD_S_PassCard  cmd_s2c_pass_card;    //过牌
		CMD_S_CallScore cmd_s2c_call_score;   //叫分信息
		CMD_S_BankerInfo cmd_s2c_banker_info; //庄家信息
		CMD_S_GameConclude cmd_s2c_game_conclude;  // 游戏结束

		CMD_S_ScoreFresh cmd_s2c_score_fresh; //刷新倍数
		CMD_S_OutCard cmd_s2c_out_card;        //出牌

		CMD_S_TiResult cmd_s2c_ti_result;                //踢牌结果

		CMD_S_BankerOutCard   cmd_s2c_bank_out_card;   //庄家出牌

		CMD_S_NotifyUserTi       cmd_s_notify_user_ti;         //通知用户ti

	} cmdData;
	std::vector<std::string>  m_usernikes;
	std::vector<Lint> m_userids;
	std::vector<std::string>  m_headUrl;
	std::vector<Lint>  m_totalScore;
	MHLMsgDouDiZhuS2CGameMessage() : LMsgSC(MSG_S_2_C_DOUDIZHU_GAME_MESSAGE_BASE)
	{
		sub_cmd_code = 0;
		memset(&cmdData, 0, sizeof(cmdData));
	}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		switch (sub_cmd_code)
		{
		case SUB_S_LINFEN_TIPAI:
			WriteMap(pack, 3);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_TIPAI);
			WriteKeyValue(pack, NAME_TO_STR(kpos), cmdData.cmd_s_notify_user_ti.m_pos);
			break;

		case SUB_S_LINFEN_GENTI:
			WriteMap(pack, 3);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_GENTI);
			WriteKeyValue(pack, NAME_TO_STR(kpos), cmdData.cmd_s_notify_user_ti.m_pos);
			break;
		case SUB_S_LINFEN_HUIPAI:
			WriteMap(pack, 3);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_HUIPAI);
			WriteKeyValue(pack, NAME_TO_STR(kpos), cmdData.cmd_s_notify_user_ti.m_pos);
			break;

		case SUB_S_LINFEN_TIPAI_RESULT:
			WriteMap(pack, 4);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_TIPAI_RESULT);
			WriteKeyValue(pack, NAME_TO_STR(kActUser), cmdData.cmd_s2c_ti_result.wActUser);
			WriteKeyValue(pack, NAME_TO_STR(kActSelect), cmdData.cmd_s2c_ti_result.wActSelect);
			break;
		case SUB_S_LINFEN_GENTI_RESULT:
			WriteMap(pack, 4);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_GENTI_RESULT);
			WriteKeyValue(pack, NAME_TO_STR(kActUser), cmdData.cmd_s2c_ti_result.wActUser);
			WriteKeyValue(pack, NAME_TO_STR(kActSelect), cmdData.cmd_s2c_ti_result.wActSelect);
			break;
		case SUB_S_LINFEN_HUIPAI_RESULT:
			WriteMap(pack, 4);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_HUIPAI_RESULT);
			WriteKeyValue(pack, NAME_TO_STR(kActUser), cmdData.cmd_s2c_ti_result.wActUser);
			WriteKeyValue(pack, NAME_TO_STR(kActSelect), cmdData.cmd_s2c_ti_result.wActSelect);
			break;

		case SUB_S_LINFEN_BANKER_OUT_CARD:
			WriteMap(pack, 3);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_BANKER_OUT_CARD);
			WriteKeyValue(pack, NAME_TO_STR(kPos), cmdData.cmd_s2c_bank_out_card.m_Pos);
			break;
		case SUB_S_LINFEN_SCORE_FRESH:
			WriteMap(pack, 3);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_LINFEN_SCORE_FRESH);
			WriteKeyValue(pack, NAME_TO_STR(kActUser), cmdData.cmd_s2c_score_fresh.score);
			break;


		case SUB_S_GAME_START:
			WriteMap(pack, 7);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_GAME_START);
			WriteKeyValue(pack, NAME_TO_STR(kStartUser), cmdData.cmd_s2c_game_start.wStartUser);
			WriteKeyValue(pack, NAME_TO_STR(kCardCount), cmdData.cmd_s2c_game_start.cbCardCount);
			WriteKeyValue(pack, NAME_TO_STR(kOnlyCall), cmdData.cmd_s2c_game_start.cbOnlyCall);

			WriteKey(pack, NAME_TO_STR(kCardData));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_start.cbCardData));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_start.cbCardData); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_start.cbCardData[i]);
			}

			WriteKey(pack, NAME_TO_STR(kScore));
			WriteValue(pack, m_totalScore);
			//WriteKey(pack, NAME_TO_STR(kScore));
			//WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_start.cbScore));
			//for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_start.cbScore); i++)
			//{
			//	WriteValue(pack, cmdData.cmd_s2c_game_start.cbScore[i]);
			//}


			break;
		case SUB_S_CALL_SCORE:
			WriteMap(pack, 7);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_CALL_SCORE);
			WriteKeyValue(pack, NAME_TO_STR(kCurrentUser), cmdData.cmd_s2c_call_score.wCurrentUser);
			WriteKeyValue(pack, NAME_TO_STR(kCallScoreUser), cmdData.cmd_s2c_call_score.wCallScoreUser);
			WriteKeyValue(pack, NAME_TO_STR(kCurrentScore), cmdData.cmd_s2c_call_score.cbCurrentScore);
			WriteKeyValue(pack, NAME_TO_STR(kUserCallScore), cmdData.cmd_s2c_call_score.cbUserCallScore);
			WriteKeyValue(pack, NAME_TO_STR(kOnlyCall), cmdData.cmd_s2c_call_score.cbOnlyCall);
			break;
		case SUB_S_BANKER_INFO:
			WriteMap(pack, 7);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_BANKER_INFO);
			WriteKeyValue(pack, NAME_TO_STR(kBankerUser), cmdData.cmd_s2c_banker_info.wBankerUser);
			WriteKeyValue(pack, NAME_TO_STR(kCurrentUser), cmdData.cmd_s2c_banker_info.wCurrentUser);
			WriteKeyValue(pack, NAME_TO_STR(kBankerScore), cmdData.cmd_s2c_banker_info.cbBankerScore);
			WriteKeyValue(pack, NAME_TO_STR(kCardCount), cmdData.cmd_s2c_banker_info.cbBankerCardCount);


			WriteKey(pack, NAME_TO_STR(kBankerCard));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_banker_info.cbBankerCard));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_banker_info.cbBankerCard); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_banker_info.cbBankerCard[i]);
			}
			break;
		case SUB_S_OUT_CARD:
			WriteMap(pack, 6);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_OUT_CARD);
			WriteKeyValue(pack, NAME_TO_STR(kOutCardUser), cmdData.cmd_s2c_out_card.wOutCardUser);
			WriteKeyValue(pack, NAME_TO_STR(kCurrentUser), cmdData.cmd_s2c_out_card.wCurrentUser);
			WriteKeyValue(pack, NAME_TO_STR(kCardCount), cmdData.cmd_s2c_out_card.cbCardCount);


			WriteKey(pack, NAME_TO_STR(kCardData));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_out_card.cbCardData));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_out_card.cbCardData); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_out_card.cbCardData[i]);
			}
			break;
		case SUB_S_PASS_CARD:
			WriteMap(pack, 5);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_PASS_CARD);
			WriteKeyValue(pack, NAME_TO_STR(kPassCardUser), cmdData.cmd_s2c_pass_card.wPassCardUser);
			WriteKeyValue(pack, NAME_TO_STR(kCurrentUser), cmdData.cmd_s2c_pass_card.wCurrentUser);
			WriteKeyValue(pack, NAME_TO_STR(kTurnOver), cmdData.cmd_s2c_pass_card.cbTurnOver);
			break;
		case SUB_S_GAME_CONCLUDE:
			WriteMap(pack, 19);
			WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
			WriteKeyValue(pack, NAME_TO_STR(kSubCmd), SUB_S_GAME_CONCLUDE);
			WriteKeyValue(pack, NAME_TO_STR(kIsFinalDraw), cmdData.cmd_s2c_game_conclude.cbIsFinalDraw);
			WriteKeyValue(pack, NAME_TO_STR(kBankerUser), cmdData.cmd_s2c_game_conclude.cbBankerUser);
			WriteKeyValue(pack, NAME_TO_STR(kCellScore), cmdData.cmd_s2c_game_conclude.lCellScore);
			WriteKeyValue(pack, NAME_TO_STR(kChunTian), cmdData.cmd_s2c_game_conclude.bChunTian);
			WriteKeyValue(pack, NAME_TO_STR(kFanChunTian), cmdData.cmd_s2c_game_conclude.bFanChunTian);
			WriteKeyValue(pack, NAME_TO_STR(kBombCount), cmdData.cmd_s2c_game_conclude.cbBombCount);
			WriteKeyValue(pack, NAME_TO_STR(kBankerScore), cmdData.cmd_s2c_game_conclude.cbBankerCall);

			WriteKey(pack, NAME_TO_STR(kRestCardData));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbRestCardData));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbRestCardData); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_conclude.cbRestCardData[i]);
			}

			WriteKey(pack, NAME_TO_STR(kEachBombCount));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbEachBombCount));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbEachBombCount); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_conclude.cbEachBombCount[i]);
			}

			WriteKey(pack, NAME_TO_STR(kGameScore));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.lGameScore));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.lGameScore); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_conclude.lGameScore[i]);
			}

			WriteKey(pack, NAME_TO_STR(kCardCount));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbCardCount));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbCardCount); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_conclude.cbCardCount[i]);
			}


			WriteKey(pack, NAME_TO_STR(kHandCardData));
			WriteArray(pack, DOUDIZHU_PLAY_USER_COUNT);
			for (int i = 0; i <DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbHandCardData[0]));
				for (int j = 0; j < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbHandCardData[0]); j++)
				{
					WriteValue(pack, cmdData.cmd_s2c_game_conclude.cbHandCardData[i][j]);
				}
			}

			WriteKey(pack, NAME_TO_STR(kUserNames));
			WriteValue(pack, m_usernikes);

			WriteKey(pack, NAME_TO_STR(kHeadUrl));
			WriteValue(pack, m_headUrl);

			WriteKey(pack, NAME_TO_STR(kUserIds));
			WriteValue(pack, m_userids);

			WriteKey(pack, NAME_TO_STR(kScore));
			WriteValue(pack, m_totalScore);

			WriteKey(pack, NAME_TO_STR(kActSelect));
			WriteArray(pack, ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbActSelect));
			for (int i = 0; i < ARRAYSIZE(cmdData.cmd_s2c_game_conclude.cbActSelect); i++)
			{
				WriteValue(pack, cmdData.cmd_s2c_game_conclude.cbActSelect[i]);
			}

			break;
		default:
			break;
		}

		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgDouDiZhuS2CGameMessage(); }

};

//客户端消息
struct MHLMsgDouDiZhuC2SGameMessage : public LMsgSC
{
	Lint sub_cmd_code;
	union
	{
		CMD_C_CallScore  cmd_c2s_call_score;
		CMD_C_OutCard    cmd_c2s_out_card;
		CMD_C_Ti                cmd_c2s_ti;
	} cmdData;
	MHLMsgDouDiZhuC2SGameMessage() : LMsgSC(MSG_C_2_S_DOUDIZHU_GAME_MESSAGE_BASE)
	{
		sub_cmd_code = 0;
		memset(&cmdData, 0, sizeof(cmdData));
	}

	// 客户端叫分
	void ReadCallScore(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(kCallScore), cmdData.cmd_c2s_call_score.cbCallScore);
	}

	// 出牌
	void ReadOutCard(msgpack::object& obj)
	{

		int card_count = 0;
		int card_value = 0;
		ReadMapData(obj, NAME_TO_STR(kCardCount), card_count);
		cmdData.cmd_c2s_out_card.cbCardCount = (BYTE)card_count;
		//LLOG_ERROR("ReadOutCard 1: count = %d %d", card_count, cmdData.cmd_c2s_out_card.cbCardCount);
		int nCount = 0;
		// 牌值
		msgpack::object cardsArray;
		ReadMapData(obj, NAME_TO_STR(kCardData), cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				cmdData.cmd_c2s_out_card.cbCardData[i] = (BYTE)card_value;
				//LLOG_ERROR("ReadOutCard 2: count = %d %d", card_value, cmdData.cmd_c2s_out_card.cbCardData[i]);
				nCount += 1;
			}
		}
	}

	//ti
	void ReadTi(msgpack::object& obj)
	{
		Lint isYes = 0;
		ReadMapData(obj, NAME_TO_STR(kIsYes), isYes);
		cmdData.cmd_c2s_ti.cbIsYes = (BYTE)isYes;
		LLOG_DEBUG("Logwyz ....  cmdData.cmd_c2_s_ti.cbIsYes[%d] ", cmdData.cmd_c2s_ti.cbIsYes);
	}


	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, NAME_TO_STR(kSubCmd), sub_cmd_code);
		//ReadMapData(obj, NAME_TO_STR(m_pos), cmdData.wStartUser);
		switch (sub_cmd_code)
		{
		case SUB_C_CALL_SCORE:   //用户叫分
			ReadCallScore(obj);
			LLOG_ERROR("Message here, callscore : %d", cmdData.cmd_c2s_call_score.cbCallScore);
			break;
		case  SUB_C_OUT_CARD:	//用户出牌
			ReadOutCard(obj);
			break;
		case SUB_C_PASS_CARD:	//用户放弃
			break;
		case SUB_C_TIPAI:
		case SUB_C_GENTI:
		case SUB_C_HUIPAI:
			ReadTi(obj);
			break;

		default:break;
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//WriteMap(pack, 4);
		//WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(kPos), m_pos);
		//WriteKeyValue(pack, NAME_TO_STR(kFlag), m_flag);
		//WriteKeyValue(pack, NAME_TO_STR(kUserid), m_userid);
		/*
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_userid), m_userid);*/
		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgDouDiZhuC2SGameMessage(); }

};

//断线重连消息
struct MHLMsgDouDiZhuS2CReconn : public LMsgSC
{
	Lint kPlayStatus;                   // 游戏状态
	Lint kCellScore;						//单元积分
	Lint kCurrentUser;						//当前玩家
	Lint kBankerScore;						//庄家叫分
	Lint kScoreInfo[DOUDIZHU_PLAY_USER_COUNT];			//叫分信息-每个人的叫庄分数   255-没有叫过，0-不叫 ，123-分数
	//Lint kHandCardData[NORMAL_COUNT];	//手上扑克

	Lint kBombCount;						//炸弹次数
	Lint kBankerUser;						//庄家用户	

											//出牌信息
	Lint kTurnWiner;						//胜利玩家
	Lint kTurnCardCount;					//出牌数目
	Lint kTurnCardData[PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_COUNT_4];		//出牌数据

	Lint kBankerCard[PLAY_TYPE_CARD_REST_COUNT_4];					//底牌
	Lint kHandCardData[PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_COUNT_4];		//用户手上扑克
	Lint kHandCardCount[DOUDIZHU_PLAY_USER_COUNT];	//每个用户扑克数目
	//Lint kUserTiStatue[DOUDIZHU_PLAY_USER_COUNT];                  //玩家是否选择  0-没有选择，1-踢牌，2-跟踢,3-回牌
	Lint ktype[DOUDIZHU_PLAY_USER_COUNT];                    //类型
	Lint kActSelect[DOUDIZHU_PLAY_USER_COUNT];          //选择

	Lint kOnlyCall;                                                                           //当前用户只能叫庄



	MHLMsgDouDiZhuS2CReconn() : LMsgSC(MSG_S_2_C_DOUDIZHU_RECONNECT)
	{
		kPlayStatus = 0;
		kCellScore = 0;
		kCurrentUser = 0;
		kBankerScore = 0;
		memset(kScoreInfo, 0, sizeof(kScoreInfo));
		kBombCount = 0;
		kBankerUser = 0;
		kTurnCardCount = 0;
		memset(kTurnCardData, 0, sizeof(kTurnCardData));
		memset(kBankerCard, 0, sizeof(kBankerCard));
		memset(kHandCardData, 0, sizeof(kHandCardData));
		memset(kHandCardCount, 0, sizeof(kHandCardCount));
		memset(ktype, 0, sizeof(ktype));
		memset(kActSelect, 0, sizeof(kActSelect));
		kOnlyCall = 0;
		//memset(this, 0, sizeof(*this));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		WriteMap(pack, 17);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kPlayStatus), kPlayStatus);
		WriteKeyValue(pack, NAME_TO_STR(kCurrentUser), kCurrentUser);
		WriteKeyValue(pack, NAME_TO_STR(kBankerScore), kBankerScore);
		WriteKeyValue(pack, NAME_TO_STR(kBankerUser), kBankerUser);
		WriteKeyValue(pack, NAME_TO_STR(kBombCount), kBombCount);
		WriteKeyValue(pack, NAME_TO_STR(kCellSore), kCellScore);

		WriteKeyValue(pack, NAME_TO_STR(kOnlyCall), kOnlyCall);

		WriteKey(pack, NAME_TO_STR(kScoreInfo));
		WriteArray(pack, ARRAYSIZE(kScoreInfo));
		for (int i = 0; i < ARRAYSIZE(kScoreInfo); i++)
		{
			WriteValue(pack, kScoreInfo[i]);
		}

		WriteKey(pack, NAME_TO_STR(kBankerCard));
		WriteArray(pack, ARRAYSIZE(kBankerCard));
		for (int i = 0; i < ARRAYSIZE(kBankerCard); i++)
		{
			WriteValue(pack, kBankerCard[i]);
		}

		WriteKeyValue(pack, NAME_TO_STR(kTurnWiner), kTurnWiner);
		WriteKeyValue(pack, NAME_TO_STR(kTurnCardCount), kTurnCardCount);

		WriteKey(pack, NAME_TO_STR(kTurnCardData));
		WriteArray(pack, ARRAYSIZE(kTurnCardData));
		for (int i = 0; i < ARRAYSIZE(kTurnCardData); i++)
		{
			WriteValue(pack, kTurnCardData[i]);
		}

		WriteKey(pack, NAME_TO_STR(kHandCardCount));
		WriteArray(pack, ARRAYSIZE(kHandCardCount));
		for (int i = 0; i < ARRAYSIZE(kHandCardCount); i++)
		{
			WriteValue(pack, kHandCardCount[i]);
		}


		WriteKey(pack, NAME_TO_STR(kHandCardData));
		WriteArray(pack, ARRAYSIZE(kHandCardData));
		for (int i = 0; i < ARRAYSIZE(kHandCardData); i++)
		{
			WriteValue(pack, kHandCardData[i]);
		}

		WriteKey(pack, NAME_TO_STR(ktype));
		WriteArray(pack, ARRAYSIZE(ktype));
		for (int i = 0; i < ARRAYSIZE(ktype); i++)
		{
			WriteValue(pack, ktype[i]);
		}

		WriteKey(pack, NAME_TO_STR(kActSelect));
		WriteArray(pack, ARRAYSIZE(kActSelect));
		for (int i = 0; i < ARRAYSIZE(kActSelect); i++)
		{
			WriteValue(pack, kActSelect[i]);
		}

		return true;
	}

	virtual LMsg* Clone() { return new MHLMsgDouDiZhuS2CReconn(); }

};

//匿名加入俱乐部房间
struct MHLMsgC2SJoinRoomAnonymous : public LMsgSC
{
	Lint m_clubId;                             //俱乐部id
	Lint m_playTypeId;                   //玩家id
	Lstring     m_Gps_Lng;
	Lstring     m_Gps_Lat;

	MHLMsgC2SJoinRoomAnonymous() : LMsgSC(MSG_S_2_C_DOUDIZHU_JOIN_CLUB_ROOM_ANONYMOUS)
	{
		m_clubId = 0;
		m_playTypeId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kClubId", m_clubId);
		ReadMapData(obj, "kPlayTypeId", m_playTypeId);
		ReadMapData(obj, NAME_TO_STR(kGpsLng), m_Gps_Lng);
		ReadMapData(obj, NAME_TO_STR(kGpsLat), m_Gps_Lat);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new MHLMsgC2SJoinRoomAnonymous();
	}
};


/////////////////////////////////////双升消息/////////////////////////////////////////


/* 
  双升：S->C 服务器发送第一局的开局消息（设置防作弊的消息等）
  MSG_S_2_C_SHUANGSHENG_FIRST_ROUND = 62212
 */
struct ShuangShengS2CFirstRound : public LMsgSC
{
	Lint m_newSeatUserId[SHUANGSHENG_PLAY_USER_COUNT];	//调整后玩家ID
	Lint m_gradeCard[2];		//两拨玩家各个级数，[0]位置玩家，[1]位置玩家

	ShuangShengS2CFirstRound() : LMsgSC(MSG_S_2_C_SHUANGSHENG_FIRST_ROUND)
	{
		memset(m_newSeatUserId, 0, sizeof(m_newSeatUserId));
		m_gradeCard[0] = 2;
		m_gradeCard[1] = 2;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "mNewSeatUserId");
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_newSeatUserId[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kGradeCard");
		for (Lint i = 0; i < 2; ++i)
		{
			veci.push_back(m_gradeCard[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CFirstRound();
	}
};

/* 
  双升：S->C 服务器给每个玩家发牌
  MSG_S_2_C_SHUANGSHENG_SEND_CARDS = 62200
*/
struct ShuangShengS2CSendCards : public LMsgSC
{
	Lint m_outTime;				//发牌结束后叫主默认超时时间
	Lint m_pos;					//摸牌玩家位置
	Lint m_zhuangPos;			//庄家位置，第1局：庄家位置为无效值，以后：庄家位置确定
	Lint m_handCardsCount;		//发牌数量
	Lint m_handCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//玩家手牌
	Lint m_currGrade;			//当前级数
	Lint m_gradeCard[2];		//两拨玩家各个级数，[0]位置玩家，[1]位置玩家
	Lint m_totleScore[SHUANGSHENG_PLAY_USER_COUNT];  //玩家总分

	ShuangShengS2CSendCards() : LMsgSC(MSG_S_2_C_SHUANGSHENG_SEND_CARDS)
	{
		m_outTime = 0;
		m_pos = SHUANGSHENG_INVALID_POS;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
		m_handCardsCount = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		m_currGrade = 0;
		memset(m_gradeCard, 0, sizeof(m_gradeCard));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kCurrGrade", m_currGrade);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);
		
		veci.clear();
		WriteKey(pack, "kGradeCard");
		for (Lint i = 0; i < 2; ++i)
		{
			veci.push_back(m_gradeCard[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kTotleScore");
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CSendCards();
	}

};

/*
  双升：S->C 服务器通知玩家报主(暂时不用)
  MSG_S_2_C_SHUANGSHENG_NOTICE_SELECT_ZHU = 62201
*/
struct ShuangShengS2CNoticeSelectZhu : public LMsgSC
{
	Lint m_pos;					//通知的玩家位置
	Lint m_canSelect[5];		//玩家可以报主的牌

	ShuangShengS2CNoticeSelectZhu() : LMsgSC(MSG_S_2_C_SHUANGSHENG_NOTICE_SELECT_ZHU)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		memset(m_canSelect, 0, sizeof(m_canSelect));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kCanSelect");
		for (Lint i = 0; i < 5; ++i)
		{
			veci.push_back(m_canSelect[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CNoticeSelectZhu();
	}
};

/*
  双升：C->S 玩家报主操作
  MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
*/
struct ShuangShengC2SSelectZhu : public LMsgSC
{
	Lint m_pos;					//报主玩家位置
	Lint m_selectCardCount;     //玩家报主牌的张数
	Lint m_selectCard;			//玩家报主的牌
	Lint m_count;				//客户端回传值

	ShuangShengC2SSelectZhu() : LMsgSC(MSG_C_2_S_SHUANGSHENG_SELECT_ZHU)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		m_selectCardCount = 0;
		m_selectCard = 0;
		m_count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectCardCount", m_selectCardCount);
		ReadMapData(obj, "kSelectCard", m_selectCard);
		ReadMapData(obj, "kCount", m_count);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengC2SSelectZhu();
	}

};

/*
  双升：S->C 服务器广播玩家叫主叫主结果（玩家每次叫主广播一下）
  MSG_S_2_C_SHUANGSHENG_BC_SELECT_ZHU = 62213
*/
struct ShuangShengS2CBCSelectZhu : public LMsgSC
{
	Lint m_outTime;				//有玩家叫主后默认超时时间
	Lint m_pos;					//叫主玩家
	Lint m_selectZhu;			//玩家叫主的牌
	Lint m_selectZhuCount;		//玩家叫主的张数
	Lint m_count;				//客户端回传值

	ShuangShengS2CBCSelectZhu() : LMsgSC(MSG_S_2_C_SHUANGSHENG_BC_SELECT_ZHU)
	{
		m_outTime = 0;
		m_pos = SHUANGSHENG_INVALID_POS;
		m_selectZhu = 0;
		m_selectZhuCount = 0;
		m_count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectZhu", m_selectZhu);
		WriteKeyValue(pack, "kSelectZhuCount", m_selectZhuCount);
		WriteKeyValue(pack, "kCount", m_count);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CBCSelectZhu();
	}
};


/*
  双升：S->C 服务器通知玩家反主(暂时不用)
  MSG_S_2_C_SHUANGSHENG_NOTICE_FAN_ZHU = 62203
*/
struct ShuangShengS2CNoticeFanZhu : public LMsgSC
{
	Lint m_outTime;				//反主超时时间,毫秒
	Lint m_pos;					//反主玩家
	Lint m_fanCards[5];			//可以反主的牌

	ShuangShengS2CNoticeFanZhu() : LMsgSC(MSG_S_2_C_SHUANGSHENG_NOTICE_FAN_ZHU)
	{
		m_outTime = 0;
		m_pos = SHUANGSHENG_INVALID_POS;
		memset(m_fanCards, 0, sizeof(m_fanCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kFanCards");
		for (Lint i = 0; i < 5; ++i)
		{
			veci.push_back(m_fanCards[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CNoticeFanZhu();
	}
};

/*
  双升：C->S 玩家反主操作（暂时不用）
  MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
*/
struct ShuangShengC2SFanZhu : public LMsgSC
{
	Lint m_pos;					//反主玩家位置
	Lint m_fanCards[2];			//反主的牌

	ShuangShengC2SFanZhu() : LMsgSC(MSG_C_2_S_SHUANGSHENG_FAN_ZHU)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		memset(m_fanCards, 0, sizeof(m_fanCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kFanCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_fanCards[i] = (BYTE)card_value;
				nCount += 1;
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
		return new ShuangShengC2SFanZhu();
	}
};

/*
  双升：S->C 服务器通知玩家拿底牌,当前庄家，主花色等信息
  MSG_S_2_C_SHUANGSHENG_NOTICE_BASE_INFO = 62205
*/
struct ShuangShengS2CNoticeBaseInfo : public LMsgSC
{
	Lint m_outTime;				//玩家盖底牌默认消息
	Lint m_zhuangPos;			//庄家位置
	Lint m_selectCard;			//主牌花色，王为无主
	Lint m_baseCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];		//原始手牌 + 底牌

	ShuangShengS2CNoticeBaseInfo() : LMsgSC(MSG_S_2_C_SHUANGSHENG_NOTICE_BASE_INFO)
	{
		m_outTime = 0;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
		m_selectCard = 0x40;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kSelectCard", m_selectCard);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_baseCards[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CNoticeBaseInfo();
	}
};

/*
  双升：C->S 玩家盖底牌操作
  MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
*/
struct ShuangShengC2SBaseCards : public LMsgSC
{
	Lint m_pos;					//盖底牌玩家位置
	Lint m_baseCardsCount;		//玩家盖底牌数量
	Lint m_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];		//玩家盖的底牌

	ShuangShengC2SBaseCards() : LMsgSC(MSG_C_2_S_SHUANGSHENG_BASE_CARDS)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kBaseCardsCount", m_baseCardsCount);
		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kBaseCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_baseCards[i] = card_value;
				nCount += 1;
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
		return new ShuangShengC2SBaseCards();
	}
};

/*
  双升：S->C 服务器广播玩家埋底结束
  MSG_S_2_C_SHUANGSHENG_BC_BASE_OVER = 62216,
*/
struct ShuangShengS2CBCBaseCardOver : public LMsgSC
{
	Lint m_pos;
	Lint m_flag;				//埋底是否成功，0：失败，1：成功
	Lint m_selectCard;			//亮主牌，无主为0x40
	Lint m_handCardsCount;		//手牌数量
	Lint m_handCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//玩家手牌

	ShuangShengS2CBCBaseCardOver() : LMsgSC(MSG_S_2_C_SHUANGSHENG_BC_BASE_OVER)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		m_flag = 0;
		m_selectCard = 0x40;
		m_handCardsCount = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kSelectCard", m_selectCard);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kHandCards");
		for (int i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CBCBaseCardOver();
	}
};

/*
  双升：S->C 服务器广播游戏开始，开始出牌
  MSG_S_2_C_SHUANGSHENG_BC_GAME_STARE = 62207
*/
struct ShuangShengS2CBCGameStart : public LMsgSC
{
	Lint m_pos;					//出牌玩家位置
	Lint m_outTime;				//出牌超时时间

	ShuangShengS2CBCGameStart() : LMsgSC(MSG_S_2_C_SHUANGSHENG_BC_GAME_STARE)
	{
		m_outTime = 20;
		m_pos = SHUANGSHENG_INVALID_POS;
	}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CBCGameStart();
	}
};

/*
  双升：C->S 玩家出牌操作
  MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
*/
struct ShuangShengC2SOutCards : public LMsgSC
{
	Lint m_pos;					//出牌玩家位置
	Lint m_outCardsCount;		//出牌数量
	Lint m_outCards[SHUANGSHENG_HAND_CARDS_COUNT];		//玩家出的牌

	ShuangShengC2SOutCards() : LMsgSC(MSG_C_2_S_SHUANGSHENG_OUT_CARDS)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		m_outCardsCount = 0;
		memset(m_outCards, 0, sizeof(m_outCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kOutCardsCount", m_outCardsCount);

		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kOutCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_outCards[i] = card_value;
				nCount += 1;
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOutCardsCount", m_outCardsCount);
		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kOutCards");
		for (int i = 0; i < 25; ++i)
		{
			veci.push_back(m_outCards[i]);
		}
		WriteKey(pack, veci);
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengC2SOutCards();
	}
};

/*
  双升：S->C 服务器广播玩家出牌结果
  MSG_S_2_C_SHUANGSHENG_BC_OUT_CARDS_RESULT = 62209
*/
struct ShuangShengS2CBCOutCardsResult : public LMsgSC
{
	Lint m_pos;					//当前出牌玩家位置
	Lint m_turnStart;			//标记本轮第一次出牌  0：非第一个出牌， 1：是第一个出牌
	Lint m_turnOver;			//标记本轮出牌结束  0：未结束， 1：结束
	Lint m_nextPos;				//下一玩家出牌的位置
	Lint m_turnWiner;			//一轮出牌最大的玩家
	Lint m_currBig;				//当前最大出牌玩家
	Lint m_outCardsCount;		//玩家出牌的数量
	Lint m_outCards[SHUANGSHENG_HAND_CARDS_COUNT];		//当前出的牌
	Lint m_resultOutCardsCount;	//可出牌的结果数量
	Lint m_resultOutCards[SHUANGSHENG_HAND_CARDS_COUNT];	//可出牌的结果
	Lint m_mode;				//出牌的类型
	Lint m_currScore;			//本轮出牌的分数
	Lint m_getTotleScore;		//本局总得分
	Lint m_scoreCardsCount;		//闲家抓分牌数量
	Lint m_scoreCards[24];		//闲家抓的分牌
	Lint m_soundType;			//声音-1：默认不处理 0：管不上领牌 1：大你 / 管上 2：主杀

	ShuangShengS2CBCOutCardsResult() : LMsgSC(MSG_S_2_C_SHUANGSHENG_BC_OUT_CARDS_RESULT)
	{
		m_pos = SHUANGSHENG_INVALID_POS;
		m_turnStart = 0;
		m_turnOver = 0;
		m_nextPos = SHUANGSHENG_INVALID_POS;
		m_turnWiner = SHUANGSHENG_INVALID_POS;
		m_currBig = SHUANGSHENG_INVALID_POS;
		m_outCardsCount = 0;
		memset(m_outCards, 0, sizeof(m_outCards));
		m_resultOutCardsCount = 0;
		memset(m_resultOutCards, 0 , sizeof(m_resultOutCards));
		m_mode = 0;
		m_currScore = 0;
		m_getTotleScore = 0;
		m_scoreCardsCount = 0;
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		m_soundType = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kTurnStart", m_turnStart);
		WriteKeyValue(pack, "kTurnOver", m_turnOver);
		WriteKeyValue(pack, "kNextPos", m_nextPos);
		WriteKeyValue(pack, "kTurnWiner", m_turnWiner);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kOutCardsCount", m_outCardsCount);
		WriteKeyValue(pack, "kResultOutCardsCount", m_resultOutCardsCount);
		WriteKeyValue(pack, "kMode", m_mode);
		WriteKeyValue(pack, "kCurrScore", m_currScore);
		WriteKeyValue(pack, "kGetTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kScoreCardsCount", m_scoreCardsCount);
		WriteKeyValue(pack, "kSoundType", m_soundType);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kOutCards");
		for (Lint i = 0; i < SHUANGSHENG_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_outCards[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kResultOutCards");
		for (Lint i = 0; i < SHUANGSHENG_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_resultOutCards[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 24; ++i)
		{
			veci.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CBCOutCardsResult();
	}
};

/*
  双升：S->C 服务器广播小结算
  MSG_S_2_C_SHUANGSHENG_BC_DRAW_RESULT = 62210
*/
struct ShuangShengS2CBCDrawResult : public LMsgSC
{
	Lint m_winnerPos;			//本局赢家位置
	Lint m_zhuangPos;			//本局庄家位置
	Lint m_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];		//底牌
	Lint m_baseScore;			//闲家扣底牌分（闲家赢已经加倍过的）（庄家赢则为0）
	Lint m_baseTimes;			//闲家扣底分倍数
	Lint m_getTotleScore;		//闲家总抓分（包括底牌分）
	Lint m_thisScore[SHUANGSHENG_PLAY_USER_COUNT];		//玩家本局得分
	
	Lint m_upDate[2];			//两对各个级数
	Lint m_nextZhuangPos;		//下局庄家位置
	Lint m_type;				//升级结果：0：庄家升级  1：闲家升级  2：闲家上庄（都不升级）
	Lint m_nextCard;			//升级级数
	Lint m_isFinish;			//标记match是否结束 0：未结束  1：结束

	Lint    m_totleScore[SHUANGSHENG_PLAY_USER_COUNT];		//玩家总得分
	Lint	m_userIds[SHUANGSHENG_PLAY_USER_COUNT];		//游戏ID
	Lstring	m_userNames[SHUANGSHENG_PLAY_USER_COUNT];   //游戏玩家昵称
	Lstring	m_headUrl[SHUANGSHENG_PLAY_USER_COUNT];     //游戏玩家头像

	ShuangShengS2CBCDrawResult() : LMsgSC(MSG_S_2_C_SHUANGSHENG_BC_DRAW_RESULT)
	{
		m_winnerPos = SHUANGSHENG_INVALID_POS;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
		memset(m_baseCards, 0x00, sizeof(m_baseCards));
		m_baseScore = 0;
		m_getTotleScore = 0;
		memset(m_thisScore, 0, sizeof(m_thisScore));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_upDate, 0, sizeof(m_upDate));
		m_nextZhuangPos = SHUANGSHENG_INVALID_POS;
		memset(m_userIds, 0, sizeof(m_userIds));
		m_type = 3;
		m_nextCard = 0;
		m_isFinish = 0;
		m_baseTimes = 0;
		
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kWinnerPos", m_winnerPos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kBaseScore", m_baseScore);
		WriteKeyValue(pack, "kBaseTimes", m_baseTimes);
		WriteKeyValue(pack, "kGetTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kNextZhuangPos", m_nextZhuangPos);
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kNextCard", m_nextCard);
		WriteKeyValue(pack, "kFinish", m_isFinish);

		std::vector<Lint> veci;
		std::vector<Lstring> vecs;

		veci.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			veci.push_back(m_baseCards[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		//显示总分
		WriteKey(pack, "kTotleScore");
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kUpDate");
		for (Lint i = 0; i < 2; ++i)
		{
			veci.push_back(m_upDate[i]);
		}
		WriteKey(pack, veci);
		
		//小结算显示每局得分，大结算显示总得分
		WriteKey(pack, "kScore");
		veci.clear();
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_thisScore[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kUserIds");
		veci.clear();
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_userIds[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			vecs.push_back(m_userNames[i]);
		}
		WriteKey(pack, vecs);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			vecs.push_back(m_headUrl[i]);
		}
		WriteKey(pack, vecs);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CBCDrawResult();
	}
};

/*
  双升：S->C 服务器发送断线重连消息
  MSG_S_2_C_SHUANGSHENG_RECON = 62211
*/
struct ShuangShengS2CRecon : public LMsgSC
{
	Lint m_outTime;				//默认出牌超时时间
	Lint m_status;				//断线重连阶段
	Lint m_zhuangPos;			//庄家位置
	Lint m_currGrade;			//当前级数
	Lint m_mainColor;			//主牌花色
	Lint m_selectCardValue;		//亮主牌值，无人亮主为0
	Lint m_selectCard;			//亮主牌，无主为0x40
	Lint m_selectCardCount;		//亮主牌的张数
	Lint m_selectCardPos;		//亮主牌位置
	Lint m_gradeCard[2];		//两拨玩家各个级数，[0]位置玩家，[1]位置玩家
	Lint m_firstOutPos;			//第一个出牌玩家位置
	Lint m_outCardPos;			//当前出牌玩家
	Lint m_currBig;				//当前最大出牌玩家
	Lint m_handCardsCount;		//玩家手牌数量
	Lint m_handCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//断线玩家手牌
	Lint m_outCardsCount[SHUANGSHENG_PLAY_USER_COUNT];		//玩家出牌数量
	Lint m_outCards[SHUANGSHENG_PLAY_USER_COUNT][SHUANGSHENG_HAND_CARDS_COUNT];		//当前桌面上出的牌
	Lint m_baseCards[8];		//底牌
	Lint m_baseScore;			//底牌分
	Lint m_getTotleScore;		//闲家本局总抓分
	Lint m_thisScore[4];		//玩家本局得分
	Lint m_totleScore[4];		//玩家总得分
	Lint m_scoreCardsCount;		//闲家抓分牌数量
	Lint m_scoreCards[24];		//闲家抓的分牌

	ShuangShengS2CRecon() : LMsgSC(MSG_S_2_C_SHUANGSHENG_RECON)
	{
		m_outTime = 20;
		m_status = 0;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
		m_currGrade = 0;
		m_mainColor = 0x40;
		m_selectCardValue = 0;
		m_selectCard = 0x40;
		m_selectCardCount = 0;
		m_selectCardPos = SHUANGSHENG_INVALID_POS;
		m_gradeCard[0] = 2;
		m_gradeCard[1] = 2;
		m_outCardPos = SHUANGSHENG_INVALID_POS;
		m_firstOutPos = SHUANGSHENG_INVALID_POS;
		m_currBig = SHUANGSHENG_INVALID_POS;
		m_handCardsCount = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_outCardsCount, 0, sizeof(m_outCardsCount));
		memset(m_outCards, 0, sizeof(m_outCards));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		m_baseScore = 0;
		m_getTotleScore = 0;
		memset(m_thisScore, 0 ,sizeof(m_thisScore));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		m_scoreCardsCount = 0;
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 28);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kStatus", m_status);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kCurrGrade", m_currGrade);
		WriteKeyValue(pack, "kMainColor", m_mainColor);
		WriteKeyValue(pack, "kSelectCardValue", m_selectCardValue);
		WriteKeyValue(pack, "kSelectCard", m_selectCard);
		WriteKeyValue(pack, "kSelectCardCount", m_selectCardCount);
		WriteKeyValue(pack,	"kSelectCardPos", m_selectCardPos);
		WriteKeyValue(pack, "kFirstOutPos", m_firstOutPos);
		WriteKeyValue(pack, "kOutCardPos", m_outCardPos);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kBaseScore", m_baseScore);
		WriteKeyValue(pack, "kGetTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kScoreCardsCount", m_scoreCardsCount);

		std::vector<Lint> veci;
		veci.clear();
		WriteKey(pack, "kGradeCard");
		for (Lint i = 0; i < 2; ++i)
		{
			veci.push_back(m_gradeCard[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kOutCardsCount");
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			veci.push_back(m_outCardsCount[i]);
		}
		WriteKey(pack, veci);

		for (Lint i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kOutCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; j++)
			{
				veci.push_back(m_outCards[i][j]);
			}
			WriteKey(pack, veci);
		}

		veci.clear();
		WriteKey(pack, "mBaseCards");
		for (Lint i = 0; i < 8; ++i)
		{
			veci.push_back(m_baseCards[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kThisScore");
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_thisScore[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "TotleScore");
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 24; ++i)
		{
			veci.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new ShuangShengS2CRecon();
	}

};


////////////////////////////////// 三打二 /////////////////////////////////////////

/*
S->C 服务器给每个玩家发牌
MSG_S_2_C_SANDAER_SEND_CARDS = 62250
*/
struct SanDaErS2CSendCards : public LMsgSC
{
	Lint m_outTime;				//发牌结束后叫主默认超时时间
	Lint m_pos;					//发牌玩家位置
	Lint m_nextSelectScorePos;		//第一个叫牌位置
	Lint m_handCardsCount;		//发牌数量
	Lint m_is2ChangZhu;			//2是否是常主 ,1:是常主.0.不是常主
	Lint m_handCards[SANDAER_HAND_CARDS_COUNT];	//玩家手牌
	Lint m_totleScore[SANDAER_PLAYER_COUNT]; //玩家头像下的总得分
	Lint m_clubAllHandCards[SANDAER_PLAYER_COUNT][SANDAER_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战

	SanDaErS2CSendCards() : LMsgSC(MSG_S_2_C_SANDAER_SEND_CARDS)
	{
		m_outTime = 0;
		m_pos = SANDAER_INVALID_POS;
		m_nextSelectScorePos = SANDAER_INVALID_POS;
		m_handCardsCount = 0;
		m_is2ChangZhu = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8 + SANDAER_PLAYER_COUNT);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kIs2ChangZhu", m_is2ChangZhu);

		std::vector<Lint> veci;

		//写入玩家手牌
		veci.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SANDAER_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);

		//写入每个玩家头像下的分数
		veci.clear();
		WriteKey(pack, "kTotleScore");
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SANDAER_HAND_CARDS_COUNT_MAX; j++)
			{
				veci.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, veci);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CSendCards();
	}
};

/*
S->C 服务器广播玩家叫分结果
MSG_S_2_C_SANDAER_SELECT_SCORE_R = 62252
*/
struct SanDaErS2CSelecScoreR : public LMsgSC
{
	Lint m_outTime;				//叫牌分默认超时时间
	Lint m_pos;					//当前选择叫牌分数玩家位置
	Lint m_selectScore;			//当前玩家选择的叫分：  默认不操作：-1， 不叫：0， 80，85，90，95，100
	Lint m_currMaxScore;		//当前叫牌最大分数
	Lint m_nextSelectScorePos;	//下一个叫分玩家的位置
	Lint m_zhuangPos;			//庄家位置
	Lint m_isFinish;			//叫分是否结束  0：未结束， 1：结束， 2：无人叫分从新洗牌开始	

	SanDaErS2CSelecScoreR() : LMsgSC(MSG_S_2_C_SANDAER_SELECT_SCORE_R)
	{
		m_outTime = 0;
		m_pos = SANDAER_INVALID_POS;
		m_selectScore = -1;
		m_currMaxScore = 0;
		m_nextSelectScorePos = SANDAER_INVALID_POS;
		m_zhuangPos = SANDAER_INVALID_POS;
		m_isFinish = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelelctScore", m_selectScore);
		WriteKeyValue(pack, "kCurrMaxScore", m_currMaxScore);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CSelecScoreR();
	}
};

/*
C->S 玩家选择叫牌分
MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
*/
struct SanDaErC2SSelectScore : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_selecScore;			//玩家叫牌的分数

	SanDaErC2SSelectScore() : LMsgSC(MSG_C_2_S_SANDAER_SELECT_SCORE)
	{
		m_pos = SANDAER_INVALID_POS;
		m_selecScore = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelecScore", m_selecScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SSelectScore();
	}
};

/*
S->C 服务器通知玩家拿底牌，并选主花色
MSG_S_2_C_SANDAER_BASE_CARD_AND_SELECT_MAIN = 62254
*/
struct SanDaErS2CBaseCardsAndSelectMainN : public LMsgSC
{
	Lint m_postmd;
	Lint m_zhuangPos;
	Lint m_maxSelectScore;
	Lint m_baseCardsCount;
	Lint m_baseCards[SANDAER_BASE_CARDS_COUNT];

	SanDaErS2CBaseCardsAndSelectMainN() : LMsgSC(MSG_S_2_C_SANDAER_BASE_CARD_AND_SELECT_MAIN_N)
	{
		m_postmd = SANDAER_INVALID_POS;
		m_zhuangPos = SANDAER_INVALID_POS;
		m_maxSelectScore = 0;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kTmd", m_postmd);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kMaxSelectScore", m_maxSelectScore);
		WriteKeyValue(pack, "kBaseCardsCount", m_baseCardsCount);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (int i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CBaseCardsAndSelectMainN();
	}
};

/*
C->S 玩家选主牌
MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
*/
struct SanDaErC2SSelectMain : public LMsgSC
{
	Lint m_pos;						//叫牌分玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	SanDaErC2SSelectMain() : LMsgSC(MSG_C_2_S_SANDAER_SELECT_MAIN)
	{
		m_pos = SANDAER_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectMainColor", m_selectMainColor);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SSelectMain();
	}
};

/*
S->C 服务器返回玩家选主牌结果
MSG_S_2_C_SANDAER_SELECT_MAIN_R = 62256
*/
struct SanDaErS2CSelectMainColorR : public LMsgSC
{
	Lint m_pos;						//选主玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	SanDaErS2CSelectMainColorR() : LMsgSC(MSG_S_2_C_SANDAER_SELECT_MAIN_R)
	{
		m_pos = SANDAER_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kBaseCardsCount", m_selectMainColor);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CSelectMainColorR();
	}
};

/*
C->S 玩家埋底
MSG_C_2_S_SANDAER_BASE_CARD = 62257
*/
struct SanDaErC2SBaseCard : public LMsgSC
{
	Lint m_pos;					//埋底牌玩家位置
	Lint m_baseCardsCount;		//底牌数量
	Lint m_baseCards[SANDAER_BASE_CARDS_COUNT];	//底牌

	SanDaErC2SBaseCard() : LMsgSC(MSG_C_2_S_SANDAER_BASE_CARD)
	{
		m_pos = SANDAER_INVALID_POS;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kBaseCardsCount", m_baseCardsCount);
		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kBaseCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_baseCards[i] = card_value;
				nCount += 1;
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
		return new SanDaErC2SBaseCard();
	}
};

/*
S->C 服务器返回玩家埋底结果
MSG_S_2_C_SANDAER_BASE_CARD_R = 62258
*/
struct SanDaErS2CBaseCardR : public LMsgSC
{
	Lint m_pos;					//埋底玩家位置
	Lint m_flag;				//是否埋底成功  0：失败，1：成功
	Lint m_handCardsCount;		//手牌数量
	Lint m_pokerTen;			//选对家是否含10   0:不含,1:含十
	Lint m_handCards[SANDAER_HAND_CARDS_COUNT_MAX];	//手牌
	Lint m_baseCards[SANDAER_BASE_CARDS_COUNT];		//底牌


	SanDaErS2CBaseCardR() : LMsgSC(MSG_S_2_C_SANDAER_BASE_CARD_R)
	{
		m_pos = SANDAER_INVALID_POS;
		m_flag = 0;
		m_handCardsCount = 0;
		m_pokerTen = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kPokerTen", m_pokerTen);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SANDAER_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CBaseCardR();
	}
};

/*
C->S 玩家选对家牌
MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
*/
struct SanDaErC2SSelectFriend : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_selectFriendCard;	//庄家选副庄家的牌

	SanDaErC2SSelectFriend() : LMsgSC(MSG_C_2_S_SANDAER_SELECT_FRIEND)
	{
		m_pos = SANDAER_INVALID_POS;
		m_selectFriendCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectFriendCard", m_selectFriendCard);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SSelectFriend();
	}
};

/*
S->C 服务器返回庄家选对家的牌
MSG_S_2_C_SANDAER_SELECT_FRIEND_BC = 62260
*/
struct SanDaErS2CSelectFriendCardBC : public LMsgSC
{
	Lint m_pos;							//选对家玩家的位置
	Lint m_selectFriendCard;			//玩家选对家牌
	Lint m_firendZhuangPos;				//对家位置
	Lint m_nextOutCardPos;				//出牌位置

	SanDaErS2CSelectFriendCardBC() : LMsgSC(MSG_S_2_C_SANDAER_SELECT_FRIEND_BC)
	{
		m_pos = SANDAER_INVALID_POS;
		m_firendZhuangPos = SANDAER_INVALID_POS;
		m_selectFriendCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectFriendCard", m_selectFriendCard);
		WriteKeyValue(pack, "kFirendZhuangPos", m_firendZhuangPos);
		WriteKeyValue(pack, "kNextOutCardPos", m_nextOutCardPos);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CSelectFriendCardBC();
	}
};

/*
C->S 玩家出牌
MSG_C_2_S_SANDAER_OUT_CARD = 62261
*/
struct SanDaErC2SOutCard : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_outCard;				//玩家出的牌

	SanDaErC2SOutCard() : LMsgSC(MSG_C_2_S_SANDAER_OUT_CARD)
	{
		m_pos = SANDAER_INVALID_POS;
		m_outCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kOutCard", m_outCard);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SOutCard();
	}
};

/*
S->C 服务器广播出牌结果
MSG_S_2_C_SANDAER_OUT_CARD_BC = 62262
*/
struct SanDaErS2COutCardBC : public LMsgSC
{
	Lint m_pos;										//出牌玩家位置
	Lint m_outCard;									//出的牌
	Lint m_turnStart;								//标记本轮第一次出牌  0：非第一个出牌， 1：是第一个出牌
	Lint m_turnOver;								//标记本轮出牌结束  0：未结束， 1：结束
	Lint m_nextPos;									//下一玩家出牌的位置
	Lint m_turnWiner;								//一轮出牌最大的玩家
	Lint m_currBig;									//当前最大出牌玩家
	Lint m_mode;									//出牌的类型
	Lint m_thisTurnScore;							//本轮出牌的分数
	Lint m_getTotleScore;							//本局总得分
	Lint m_scoreCardsCount;							//闲家抓分牌数量
	Lint m_scoreCards[12];							//闲家抓的分牌
	Lint m_soundType;								//声音-1：默认不处理 0：管不上领牌 1：大你 / 管上 2：主杀
	Lint m_leftover_score;							//如果m_turnOver=1,则为剩余的分
	Lint m_handCardCount[SANDAER_PLAYER_COUNT];		//玩家手牌数量

	Lint m_prevOutCard[SANDAER_PLAYER_COUNT];		//如果m_turnOver=1,则未这轮出的牌
	Lint m_outCardArray[SANDAER_PLAYER_COUNT];		//本轮出牌结果

	SanDaErS2COutCardBC() : LMsgSC(MSG_S_2_C_SANDAER_OUT_CARD_BC)
	{
		m_pos = SANDAER_INVALID_POS;
		m_outCard = 0;
		m_turnStart = 0;
		m_turnOver = 0;
		m_nextPos = SANDAER_INVALID_POS;
		m_turnWiner = SANDAER_INVALID_POS;
		m_currBig = SANDAER_INVALID_POS;
		m_mode = 0;
		m_thisTurnScore = 0;
		m_getTotleScore = 0;
		m_scoreCardsCount = 0;
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		m_soundType = 0;
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_outCardArray, 0, sizeof(m_outCardArray));
		m_leftover_score = 0;
		memset(m_handCardCount, 0, sizeof(m_handCardCount));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOutCard", m_outCard);
		WriteKeyValue(pack, "kTurnStart", m_turnStart);
		WriteKeyValue(pack, "kTurnOver", m_turnOver);
		WriteKeyValue(pack, "kNextPos", m_nextPos);
		WriteKeyValue(pack, "kTurnWiner", m_turnWiner);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kMode", m_mode);
		WriteKeyValue(pack, "kThisTurnScore", m_thisTurnScore);
		WriteKeyValue(pack, "kTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kScoreCardsCount", m_scoreCardsCount);
		WriteKeyValue(pack, "kSoundType", m_soundType);
		WriteKeyValue(pack, "kLeftoverScore", m_leftover_score);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kOutCardArray");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCardArray[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kHandCardCount");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardCount[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2COutCardBC();
	}
};


/*
S->C 服务器发送断线重连
MSG_S_2_C_SANDAER_RECON = 62263
*/
struct SanDaErS2CReConnect : public LMsgSC
{
	Lint		m_pos;										//玩家位置
	Lint		m_curPos;									//当前操作玩家位置
	Lint		m_zhuang;									//庄的位置
	Lint		m_fuzhuangCard;								//副庄的牌
	Lint		m_currBig;									//当前最大出牌玩家
	Lint		m_state;									//当前桌子状态
	Lint		m_selectMainColor;							//主牌
	Lint		m_Monoy;									//闲家本局总得份
	Lint		m_hand_cards_count;							//手牌个数
	Lint		m_score[SANDAER_PLAYER_COUNT];				//玩家本场截止目前总积分
	Lint		m_outCard[SANDAER_PLAYER_COUNT];			//这一轮前面几家打出去的牌
	Lint		m_handCards[SANDAER_HAND_CARDS_COUNT_MAX];	//手牌
	Lint		m_scoreCards[12];							//闲家抓的分牌
	Lint		m_prevOutCard[SANDAER_PLAYER_COUNT];		//上轮的牌

	Lint		m_playType[5];								//参数  0:底分 1:2是否常主 2:是否含十  3:GPS 
	Lint		m_max_select_score;							//本局庄家叫分
	Lint		m_leftover_score;							//如果m_turnOver=1,则为剩余的分

	Lint		m_baseCards[SANDAER_BASE_CARDS_COUNT];		//底牌
	Lint		m_fuzhuangPos;								//副庄位置
	Lint		m_firstOutPos;								//第一个出牌的位置
	Lint		m_pokerTen;									//选对家是否含10

	Lint		m_clubAllHandCards[SANDAER_PLAYER_COUNT][SANDAER_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战
	Lint		m_handCardsCountArr[SANDAER_PLAYER_COUNT];	//每个人手牌数量，观战用

	Lint     m_nJiaoPaiState;       // 交牌的状态变量
	Lint     m_nTimeLeftJiaoPaiRequest;
	Lint     m_nTimeLeftJiaoPaiAck;
	Lint     m_jiaopaiAgree[SANDAER_PLAYER_COUNT]; //拒绝同意状态

	SanDaErS2CReConnect() : LMsgSC(MSG_S_2_C_SANDAER_RECON)
	{
		m_pos = SANDAER_INVALID_POS;
		m_curPos = SANDAER_INVALID_POS;
		m_zhuang = SANDAER_INVALID_POS;
		m_currBig = SANDAER_INVALID_POS;
		m_fuzhuangCard = 0;
		m_state = 0;
		m_selectMainColor = 0x40;
		m_Monoy = 0;
		m_hand_cards_count = 0;
		m_max_select_score = 0;
		m_leftover_score = 0;
		m_fuzhuangPos = SANDAER_INVALID_POS;
		m_firstOutPos = 0;
		m_pokerTen = 0;
		m_nJiaoPaiState = 0;       // 交牌的状态变量
		m_nTimeLeftJiaoPaiRequest = 0;
		m_nTimeLeftJiaoPaiAck = 0;
		memset(m_score, 0, sizeof(m_score));
		memset(m_outCard, 0, sizeof(m_outCard));
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_playType, 0, sizeof(m_playType));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
		memset(m_handCardsCountArr, 0, sizeof(m_handCardsCountArr));
		memset(m_jiaopaiAgree, 0, sizeof(m_jiaopaiAgree)); //拒绝同意状态
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 27 + SANDAER_PLAYER_COUNT);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kCurPos", m_curPos);
		WriteKeyValue(pack, "kZhuang", m_zhuang);
		WriteKeyValue(pack, "kFuzhuangCard", m_fuzhuangCard);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kMainColor", m_selectMainColor);
		WriteKeyValue(pack, "kMonoy", m_Monoy);
		WriteKeyValue(pack, "kHandCardCount", m_hand_cards_count);
		WriteKeyValue(pack, "kMaxselectscore", m_max_select_score);
		WriteKeyValue(pack, "kLeftoverScore", m_leftover_score);
		WriteKeyValue(pack, "kFuzhuangPos", m_fuzhuangPos);
		WriteKeyValue(pack, "kFirstPos", m_firstOutPos);
		WriteKeyValue(pack, "kPokerTen", m_pokerTen);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_nJiaoPaiState);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRequest), m_nTimeLeftJiaoPaiRequest);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRespone), m_nTimeLeftJiaoPaiAck);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kOutCard");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCard[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kHandCards");
		for (int i = 0; i < SANDAER_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();

		WriteKey(pack, "kPlayType");
		for (int i = 0; i < 4; ++i)
		{
			vect.push_back(m_playType[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SANDAER_HAND_CARDS_COUNT_MAX; j++)
			{
				vect.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, vect);
		}

		vect.clear();
		WriteKey(pack, "kHandCardsCountArr");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardsCountArr[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kJiaoPaiAgreeStatus");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_jiaopaiAgree[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CReConnect();
	}
};

/*
S->C 服务发送小结算
MSG_S_2_C_SANDAER_DRAW_RESULT_BC = 62264
*/
struct SanDaErS2CDrawResult :public LMsgSC
{
	Lint			m_pos;								//位置，0-4
	Lint			m_zhuangPos;						//庄位置
	Lint			m_type;								//0:小结.1:总结
	Lint			m_score[SANDAER_PLAYER_COUNT];		//玩家积分
	Lint			m_baseCards[SANDAER_BASE_CARDS_COUNT];		//底牌
	Lint			m_state;									//状态 1:庄家保底，2:庄家保底，叫分100，得分翻倍，3:闲家扣底，4:闲家扣底，叫分100，得分翻倍
	Lint			m_fuzhuangPos;								//副庄位置
	Lint			m_jiaofen;									//庄家叫分
	Lint			m_defen;									//闲家得分
	Lint			m_isFinish;									//标记match是否结束 0：未结束  1：结束
	Lint			m_winnerPos;								//最后一轮出牌最大玩家的位置;
	Lint			m_sandaer_over_state;
	Lint			m_totleScore[SANDAER_PLAYER_COUNT];		//玩家总得分
	Lint			m_userIds[SANDAER_PLAYER_COUNT];		//游戏ID
	Lstring			m_userNames[SANDAER_PLAYER_COUNT];   //游戏玩家昵称
	Lstring			m_headUrl[SANDAER_PLAYER_COUNT];     //游戏玩家头像

	SanDaErS2CDrawResult() :LMsgSC(MSG_S_2_C_SANDAER_DRAW_RESULT_BC)
	{
		m_pos = INVAILD_POS;
		m_zhuangPos = 0;
		m_type = 0;
		m_state = 0;
		m_fuzhuangPos = SANDAER_INVALID_POS;
		m_jiaofen = 0;
		m_defen = 0;
		m_isFinish = 0;
		m_winnerPos = SANDAER_INVALID_POS;
		m_sandaer_over_state = 0;
		memset(m_score, 0, sizeof(m_score));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_userIds, 0, sizeof(m_userIds));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 17);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kSate", m_state);
		WriteKeyValue(pack, "kFuzhuangPos", m_fuzhuangPos);
		WriteKeyValue(pack, "kJiaofen", m_jiaofen);
		WriteKeyValue(pack, "kDefen", m_defen);
		WriteKeyValue(pack,	"kIsFinish", m_isFinish);
		WriteKeyValue(pack, "kWinnerPos", m_winnerPos );
		WriteKeyValue(pack, "kSanDaErOverState", m_sandaer_over_state);

		std::vector<Lint> vect;
		std::vector<Lstring> vecs;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAER_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		//显示总分
		WriteKey(pack, "kTotleScore");
		vect.clear();
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_totleScore[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kUserIds");
		vect.clear();
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userIds[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userNames[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_headUrl[i]);
		}
		WriteKey(pack, vecs);

		return true;
	}

	virtual LMsg* Clone() { return new SanDaErS2CDrawResult(); }
};

/*
S->C 服务发送闲家得分慢105分,是否停止游戏
MSG_S_2_C_SANDAER_SCORE_105 = 62265
*/
struct SanDaErS2CScore105 : public LMsgSC
{
	Lint m_Score;				//下家目前得分
	Lint m_resTime;				//剩余倒计时

	SanDaErS2CScore105() : LMsgSC(MSG_S_2_C_SANDAER_SCORE_105)
	{
		m_Score = 0;
		m_resTime = 15;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kScore), m_Score);
		WriteKeyValue(pack, NAME_TO_STR(kResTime), m_resTime)
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CScore105();
	}
};

/*
C->S 玩家回复闲家得分慢105分,是否停止游戏
/MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
*/
struct SanDaErC2SScore105Ret : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 

	SanDaErC2SScore105Ret() : LMsgSC(MSG_C_2_S_SANDAER_SCORE_105_RET)
	{
		m_pos = SANDAER_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SScore105Ret();
	}
};

/*
  C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
  MSG_C_2_S_SANDAER_SCORE_105_RESULT = 62267,
*/
struct SanDaErS2CScore105Result :public LMsgSC
{
	//Lint					m_errorCode;	//0-等待操作中，1-五个人同意，结束成功，2-时间到，结束成功，3-有一个人拒绝，结束失败
	//Lint					m_time;			//倒计时
	//Lstring				m_refluse;		//拒绝的玩家,有一个玩家拒绝，则解算房间失败
	//Lint					m_flag;			//0-等待我同意或者拒绝，1-我已经同意或者拒绝                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	//std::vector<Lstring>	m_agree;		//同意的玩家
	//std::vector<Lstring>	m_wait;			//等待操作的玩家

	Lint					m_agree[SANDAER_PLAYER_COUNT];		//玩家状态

	SanDaErS2CScore105Result() :LMsgSC(MSG_C_2_S_SANDAER_SCORE_105_RESULT)
	{
		/*m_errorCode = -1;
		m_time = 0;
		m_flag = 0;*/
		memset(m_agree, 0, sizeof(m_agree));
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);


		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kAgree");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_agree[i]);
		}
		WriteKey(pack, vect);
		return true;
	}

	virtual LMsg* Clone() { return new SanDaErS2CScore105Result(); }
};

/*
  S->C 服务器提示庄家可以摊牌
  MSG_S_2_C_SANDAER_ZHUANG_CAN_TAN_PAI = 62270
*/
struct SanDaErS2CZhuangCanTanPai : public LMsgSC
{
	Lint m_pos;				//摊牌位置

	SanDaErS2CZhuangCanTanPai() : LMsgSC(MSG_S_2_C_SANDAER_ZHUANG_CAN_TAN_PAI)
	{
		m_pos = SANDAER_INVALID_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CZhuangCanTanPai();
	}
};

/*
  C-S 庄家选择是否摊牌
  MSG_C_2_S_SANDAER_ZHUANG_SELECT_TAN_PAI = 62271
*/
struct SanDaErC2SZhuangSelectTanPai : public LMsgSC
{
	Lint m_pos;				//选择的操作的位置
	Lint m_selectRet;		//庄家做出的选择 0:不摊牌  1:摊牌

	SanDaErC2SZhuangSelectTanPai() : LMsgSC(MSG_C_2_S_SANDAER_ZHUANG_SELECT_TAN_PAI)
	{
		m_pos = SANDAER_INVALID_POS;
		m_selectRet = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectRet", m_selectRet);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SZhuangSelectTanPai();
	}
};

/*
  S-C 庄家选择摊牌结果
  MSG_S_2_C_SANDAER_TAN_PAI_RET = 62272
*/
struct SanDaErS2CTanPaiRet : LMsgSC
{
	Lint m_selectRet;			//庄家做出的选择 0:不摊牌  1:摊牌
	Lint m_tanPai[SANDAER_PLAYER_COUNT][SANDAER_HAND_CARDS_COUNT];
	Lint m_tanPaiCount[SANDAER_PLAYER_COUNT];

	SanDaErS2CTanPaiRet() : LMsgSC(MSG_S_2_C_SANDAER_TAN_PAI_RET)
	{
		m_selectRet = 0;
		memset(m_tanPai, 0, sizeof(m_tanPai));
		memset(m_tanPaiCount, 0, sizeof(m_tanPaiCount));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3 + SANDAER_PLAYER_COUNT);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kSelectRet", m_selectRet);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "mTanPaiCount");
		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_tanPaiCount[i]);
		}
		WriteKey(pack, vect);

		for (Lint i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kTanPai%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SANDAER_HAND_CARDS_COUNT; j++)
			{
				vect.push_back(m_tanPai[i][j]);
			}
			WriteKey(pack, vect);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CTanPaiRet();
	}
};

/*
C->S 庄家请求底牌
MSG_C_2_S_USER_GET_BASECARDS = 62268 61027,		//
*/
struct SanDaErC2SGetBaseCards : public LMsgSC
{
	Lint m_pos;					//埋底牌玩家位置


	SanDaErC2SGetBaseCards() : LMsgSC(MSG_C_2_S_USER_GET_BASECARDS)
	{
		m_pos = SANDAER_INVALID_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SGetBaseCards();
	}
};

/*
返回底牌
MSG_S_2_C_USER_GET_BASECARDS_RET = 62269  61028,
*/
struct SanDaErS2CGetBaseCardsRet : public LMsgSC
{
	Lint m_zhuang_pos;								//庄家位置
	Lint m_baseCards[SANDAER_BASE_CARDS_COUNT];		//底牌


	SanDaErS2CGetBaseCardsRet() : LMsgSC(MSG_S_2_C_USER_GET_BASECARDS_RET)
	{
		m_zhuang_pos = SANDAER_INVALID_POS;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kzhuangPos", m_zhuang_pos);


		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAER_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CGetBaseCardsRet();
	}
};

/*
C->S 客户端庄家选择交牌
MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
*/
struct SanDaErC2SZhuangJiaoPai : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 


	SanDaErC2SZhuangJiaoPai() : LMsgSC(MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI)
	{
		m_pos = SANDAER_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SZhuangJiaoPai();
	}
};

/*
  S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
  MSG_S_2_C_SANDAER_ZHUANG_JIAO_PAI_BC = 62273
*/
struct SanDaErS2CZhuangJiaoPaiBC : public LMsgSC
{
	Lint m_jiaopaiState;            //交牌状态
	Lint m_zhuangPos;				//庄家位置
	Lint m_jiaopaiRequest;       // 0: 不用 1: 交牌 2:不交
	Lint m_xianResponseLeft;  // 闲家响应超时时间

	
	SanDaErS2CZhuangJiaoPaiBC() : LMsgSC(MSG_S_2_C_SANDAER_ZHUANG_JIAO_PAI_BC)
	{
		m_zhuangPos = SANDAER_INVALID_POS;
		m_jiaopaiState = 0;
		m_xianResponseLeft = 0;
		m_jiaopaiRequest = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kZhuangPos), m_zhuangPos);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_jiaopaiState);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiRequest), m_jiaopaiRequest);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRespone), m_xianResponseLeft);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErS2CZhuangJiaoPaiBC();
	}
};

/*
  C->S 非庄家客户端选择庄家交牌的申请
  MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
*/
struct SanDaErC2SXianSelectJiaoPai : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 

	SanDaErC2SXianSelectJiaoPai() : LMsgSC(MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI)
	{
		m_pos = SANDAER_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaErC2SXianSelectJiaoPai();
	}
};

/*
S->C 庄家交牌的最终结果
MSG_S_2_C_SANDAER_JIAO_PAI_RESULT = 62275
*/
struct SanDaErS2CJiaoPaiResult :public LMsgSC
{
	Lint m_zhuangPos;               //庄位置
	Lint m_jiaoPaiSate;            // 交牌状态
	Lint m_agree[SANDAER_PLAYER_COUNT];		//玩家状态

	SanDaErS2CJiaoPaiResult() :LMsgSC(MSG_S_2_C_SANDAER_JIAO_PAI_RESULT)
	{
		memset(m_agree, 0, sizeof(m_agree));
		m_jiaoPaiSate = 0;
		m_zhuangPos = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_jiaoPaiSate);
		WriteKeyValue(pack, NAME_TO_STR(kZhuangPos), m_zhuangPos);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kAgree");
		for (int i = 0; i < SANDAER_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_agree[i]);
		}
		WriteKey(pack, vect);
		return true;
	}

	virtual LMsg* Clone() { return new SanDaErS2CJiaoPaiResult(); }

};

////////////////////////////////// 三打一 /////////////////////////////////////////

/*
S->C 服务器给每个玩家发牌
MSG_S_2_C_SANDAYI_SEND_CARDS = 62300
*/
struct SanDaYiS2CSendCards : public LMsgSC
{
	Lint m_outTime;				//发牌结束后叫主默认超时时间
	Lint m_pos;					//发牌玩家位置
	Lint m_nextSelectScorePos;		//第一个叫牌位置
	Lint m_handCardsCount;		//发牌数量
	Lint m_is2ChangZhu;			//2是否是常主 ,1:是常主.0.不是常主
	Lint m_handCards[SANDAYI_HAND_CARDS_COUNT];	//玩家手牌
	Lint m_totleScore[SANDAYI_PLAYER_COUNT]; //玩家头像下的总得分
	Lint m_clubAllHandCards[SANDAYI_PLAYER_COUNT][SANDAYI_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战

	SanDaYiS2CSendCards() : LMsgSC(MSG_S_2_C_SANDAYI_SEND_CARDS)
	{
		m_outTime = 0;
		m_pos = SANDAYI_INVALID_POS;
		m_nextSelectScorePos = SANDAYI_INVALID_POS;
		m_handCardsCount = 0;
		m_is2ChangZhu = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8 + SANDAYI_PLAYER_COUNT);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kIs2ChangZhu", m_is2ChangZhu);

		std::vector<Lint> veci;

		//写入玩家手牌
		veci.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SANDAYI_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);

		//写入每个玩家头像下的分数
		veci.clear();
		WriteKey(pack, "kTotleScore");
		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SANDAYI_HAND_CARDS_COUNT_MAX; j++)
			{
				veci.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, veci);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CSendCards();
	}
};

/*
S->C 服务器广播玩家叫分结果
MSG_S_2_C_SANDAYI_SELECT_SCORE_R = 62302
*/
struct SanDaYiS2CSelecScoreR : public LMsgSC
{
	Lint m_outTime;				//叫牌分默认超时时间
	Lint m_pos;					//当前选择叫牌分数玩家位置
	Lint m_selectScore;			//当前玩家选择的叫分：  默认不操作：-1， 不叫：0， 60，65，70，75，80
	Lint m_currMaxScore;		//当前叫牌最大分数
	Lint m_nextSelectScorePos;	//下一个叫分玩家的位置
	Lint m_zhuangPos;			//庄家位置
	Lint m_isFinish;			//叫分是否结束  0：未结束， 1：结束， 2：无人叫分从新洗牌开始	

	SanDaYiS2CSelecScoreR() : LMsgSC(MSG_S_2_C_SANDAYI_SELECT_SCORE_R)
	{
		m_outTime = 0;
		m_pos = SANDAYI_INVALID_POS;
		m_selectScore = -1;
		m_currMaxScore = 0;
		m_nextSelectScorePos = SANDAYI_INVALID_POS;
		m_zhuangPos = SANDAYI_INVALID_POS;
		m_isFinish = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelelctScore", m_selectScore);
		WriteKeyValue(pack, "kCurrMaxScore", m_currMaxScore);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CSelecScoreR();
	}
};

/*
C->S 玩家选择叫牌分
MSG_C_2_S_SANDAYI_SELECT_SCORE = 62303
*/
struct SanDaYiC2SSelectScore : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_selecScore;			//玩家叫牌的分数

	SanDaYiC2SSelectScore() : LMsgSC(MSG_C_2_S_SANDAYI_SELECT_SCORE)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_selecScore = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelecScore", m_selecScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiC2SSelectScore();
	}
};

/*
S->C 服务器通知玩家拿底牌，并选主花色
MSG_S_2_C_SANDAYI_BASE_CARD_AND_SELECT_MAIN_N = 62304
*/
struct SanDaYiS2CBaseCardsAndSelectMainN : public LMsgSC
{
	Lint m_pos;									//
	Lint m_zhuangPos;							//庄家位置
	Lint m_maxSelectScore;						//庄家叫牌分数
	Lint m_baseCardsCount;						//底牌数量
	Lint m_baseCards[SANDAYI_BASE_CARDS_COUNT];	//底牌

	SanDaYiS2CBaseCardsAndSelectMainN() : LMsgSC(MSG_S_2_C_SANDAYI_BASE_CARD_AND_SELECT_MAIN_N)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_zhuangPos = SANDAYI_INVALID_POS;
		m_maxSelectScore = 0;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kTmd", m_pos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kMaxSelectScore", m_maxSelectScore);
		WriteKeyValue(pack, "kBaseCardsCount", m_baseCardsCount);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (int i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CBaseCardsAndSelectMainN();
	}
};

/*
C->S 玩家选主牌
MSG_C_2_S_SANDAYI_SELECT_MAIN = 62305
*/
struct SanDaYiC2SSelectMain : public LMsgSC
{
	Lint m_pos;						//叫牌分玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	SanDaYiC2SSelectMain() : LMsgSC(MSG_C_2_S_SANDAYI_SELECT_MAIN)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectMainColor", m_selectMainColor);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiC2SSelectMain();
	}
};

/*
S->C 服务器返回玩家选主牌结果
MSG_S_2_C_SANDAYI_SELECT_MAIN_R = 62306
*/
struct SanDaYiS2CSelectMainColorR : public LMsgSC
{
	Lint m_pos;						//选主玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	SanDaYiS2CSelectMainColorR() : LMsgSC(MSG_S_2_C_SANDAYI_SELECT_MAIN_R)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectMainColor", m_selectMainColor);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CSelectMainColorR();
	}
};

/*
C->S 玩家埋底
MSG_C_2_S_SANDAYI_BASE_CARD = 62307
*/
struct SanDaYiC2SBaseCard : public LMsgSC
{
	Lint m_pos;					//埋底牌玩家位置
	Lint m_baseCardsCount;		//底牌数量
	Lint m_baseCards[SANDAYI_BASE_CARDS_COUNT];	//底牌

	SanDaYiC2SBaseCard() : LMsgSC(MSG_C_2_S_SANDAYI_BASE_CARD)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kBaseCardsCount", m_baseCardsCount);
		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kBaseCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_baseCards[i] = card_value;
				nCount += 1;
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
		return new SanDaYiC2SBaseCard();
	}
};

/*
S->C 服务器返回玩家埋底结果
MSG_S_2_C_SANDAYI_BASE_CARD_R = 62308
*/
struct SanDaYiS2CBaseCardR : public LMsgSC
{
	Lint m_pos;					//埋底玩家位置
	Lint m_flag;				//是否埋底成功  0：失败，1：成功
	Lint m_handCardsCount;		//手牌数量
	Lint m_baseCardsCount;		//底牌数量
	Lint m_nextOutCardPos;		//出牌位置
	Lint m_handCards[SANDAYI_HAND_CARDS_COUNT_MAX];	//手牌
	Lint m_baseCards[SANDAYI_BASE_CARDS_COUNT];		//原始底牌
	Lint m_baseCardsMai[SANDAYI_BASE_CARDS_COUNT];  //庄家埋底的牌


	SanDaYiS2CBaseCardR() : LMsgSC(MSG_S_2_C_SANDAYI_BASE_CARD_R)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_flag = 0;
		m_handCardsCount = 0;
		m_baseCardsCount = 0;
		m_nextOutCardPos = SANDAER_INVALID_POS;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kBaseCardsCount", m_baseCardsCount);
		WriteKeyValue(pack, "kNextOutCardPos", m_nextOutCardPos);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < SANDAYI_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCardsMai");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCardsMai[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CBaseCardR();
	}
};

/*
C->S 玩家出牌
MSG_C_2_S_SANDAYI_OUT_CARD = 62309
*/
struct SanDaYiC2SOutCard : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_outCard;				//玩家出的牌

	SanDaYiC2SOutCard() : LMsgSC(MSG_C_2_S_SANDAYI_OUT_CARD)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_outCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kOutCard", m_outCard);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiC2SOutCard();
	}
};

/*
S->C 服务器广播出牌结果
MSG_S_2_C_SANDAYI_OUT_CARD_BC = 62310
*/
struct SanDaYiS2COutCardBC : public LMsgSC
{
	Lint m_pos;										//出牌玩家位置
	Lint m_outCard;									//出的牌
	Lint m_turnStart;								//标记本轮第一次出牌  0：非第一个出牌， 1：是第一个出牌
	Lint m_turnOver;								//标记本轮出牌结束  0：未结束， 1：结束
	Lint m_nextPos;									//下一玩家出牌的位置
	Lint m_turnWiner;								//一轮出牌最大的玩家
	Lint m_currBig;									//当前最大出牌玩家
	Lint m_mode;									//出牌的类型
	Lint m_thisTurnScore;							//本轮出牌的分数
	Lint m_getTotleScore;							//本局总得分
	Lint m_scoreCardsCount;							//闲家抓分牌数量
	Lint m_scoreCards[12];							//闲家抓的分牌
	Lint m_soundType;								//声音-1：默认不处理 0：管不上领牌 1：大你 / 管上 2：主杀
	Lint m_leftover_score;							//如果m_turnOver=1,则为剩余的分
	Lint m_handCardCount[SANDAYI_PLAYER_COUNT];		//玩家手牌数量

	Lint m_prevOutCard[SANDAYI_PLAYER_COUNT];		//如果m_turnOver=1,则未这轮出的牌
	Lint m_outCardArray[SANDAYI_PLAYER_COUNT];		//本轮出牌结果

	SanDaYiS2COutCardBC() : LMsgSC(MSG_S_2_C_SANDAYI_OUT_CARD_BC)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_outCard = 0;
		m_turnStart = 0;
		m_turnOver = 0;
		m_nextPos = SANDAYI_INVALID_POS;
		m_turnWiner = SANDAYI_INVALID_POS;
		m_currBig = SANDAYI_INVALID_POS;
		m_mode = 0;
		m_thisTurnScore = 0;
		m_getTotleScore = 0;
		m_scoreCardsCount = 0;
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		m_soundType = 0;
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_outCardArray, 0, sizeof(m_outCardArray));
		m_leftover_score = 0;
		memset(m_handCardCount, 0, sizeof(m_handCardCount));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOutCard", m_outCard);
		WriteKeyValue(pack, "kTurnStart", m_turnStart);
		WriteKeyValue(pack, "kTurnOver", m_turnOver);
		WriteKeyValue(pack, "kNextPos", m_nextPos);
		WriteKeyValue(pack, "kTurnWiner", m_turnWiner);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kMode", m_mode);
		WriteKeyValue(pack, "kThisTurnScore", m_thisTurnScore);
		WriteKeyValue(pack, "kTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kScoreCardsCount", m_scoreCardsCount);
		WriteKeyValue(pack, "kSoundType", m_soundType);
		WriteKeyValue(pack, "kLeftoverScore", m_leftover_score);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kOutCardArray");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCardArray[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kHandCardCount");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardCount[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2COutCardBC();
	}
};


/*
S->C 服务器发送断线重连
MSG_S_2_C_SANDAYI_RECON = 62311
*/
struct SanDaYiS2CReConnect : public LMsgSC
{
	Lint		m_pos;										//玩家位置
	Lint		m_curPos;									//当前操作玩家位置
	Lint		m_zhuang;									//庄的位置
	Lint		m_state;									//当前桌子状态
	Lint		m_selectMainColor;							//主牌
	Lint		m_currBig;									//当前最大出牌玩家
	Lint		m_Monoy;									//闲家本局总得份
	Lint		m_handCardsCount;							//手牌个数
	Lint		m_score[SANDAYI_PLAYER_COUNT];				//玩家本场截止目前总积分
	Lint		m_outCard[SANDAYI_PLAYER_COUNT];			//这一轮前面几家打出去的牌
	Lint		m_handCards[SANDAYI_HAND_CARDS_COUNT_MAX];	//手牌
	Lint		m_scoreCards[12];							//闲家抓的分牌
	Lint		m_prevOutCard[SANDAYI_PLAYER_COUNT];		//上轮的牌

	Lint		m_playType[5];								//参数  0:底分 1:2是否常主 2:是否含十  3:GPS 
	Lint		m_maxSelectScore;							//本局庄家叫分
	Lint		m_leftOverScore;							//如果m_turnOver=1,则为剩余的分

	Lint		m_baseCards[SANDAYI_BASE_CARDS_COUNT];		//原始底牌
	Lint		m_baseCardsMai[SANDAYI_BASE_CARDS_COUNT];	//庄家埋底的牌
	Lint		m_firstOutPos;								//第一个出牌的位置

	Lint		m_clubAllHandCards[SANDAYI_PLAYER_COUNT][SANDAYI_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战
	Lint		m_handCardsCountArr[SANDAYI_PLAYER_COUNT];	//每个人手牌数量，观战用

	SanDaYiS2CReConnect() : LMsgSC(MSG_S_2_C_SANDAYI_RECON)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_curPos = SANDAYI_INVALID_POS;
		m_zhuang = SANDAYI_INVALID_POS;
		m_currBig = SANDAYI_INVALID_POS;
		m_state = 0;
		m_selectMainColor = 0x40;
		m_Monoy = 0;
		m_handCardsCount = 0;
		m_maxSelectScore = 0;
		m_leftOverScore = 0;
		m_firstOutPos = 0;
		memset(m_score, 0, sizeof(m_score));
		memset(m_outCard, 0, sizeof(m_outCard));
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_playType, 0, sizeof(m_playType));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_baseCardsMai, 0, sizeof(m_baseCardsMai));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
		memset(m_handCardsCountArr, 0, sizeof(m_handCardsCountArr));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 21 + SANDAYI_PLAYER_COUNT);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kCurPos", m_curPos);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kZhuang", m_zhuang);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kSelectMainColor", m_selectMainColor);
		WriteKeyValue(pack, "kMonoy", m_Monoy);
		WriteKeyValue(pack, "kHandCardCount", m_handCardsCount);
		WriteKeyValue(pack, "kMaxselectscore", m_maxSelectScore);
		WriteKeyValue(pack, "kLeftoverScore", m_leftOverScore);
		WriteKeyValue(pack, "kFirstPos", m_firstOutPos);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kOutCard");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kHandCards");
		for (int i = 0; i < SANDAYI_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPlayType");
		for (int i = 0; i < 4; ++i)
		{
			vect.push_back(m_playType[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCardsMai");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCardsMai[i]);
		}
		WriteKey(pack, vect);

		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < SANDAYI_HAND_CARDS_COUNT_MAX; j++)
			{
				vect.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, vect);
		}

		vect.clear();
		WriteKey(pack, "kHandCardsCountArr");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardsCountArr[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CReConnect();
	}
};

/*
S->C 服务发送小结算
MSG_S_2_C_SANDAYI_DRAW_RESULT_BC = 62312
*/
struct SanDaYiS2CDrawResult :public LMsgSC
{
	Lint			m_pos;								//位置，0-4
	Lint			m_zhuangPos;						//庄位置
	Lint			m_type;								//0:小结.1:总结
	Lint			m_score[SANDAYI_PLAYER_COUNT];		//玩家积分
	Lint			m_baseCards[SANDAYI_BASE_CARDS_COUNT];		//原始底牌
	Lint			m_baseCardsMai[SANDAYI_BASE_CARDS_COUNT];	//庄家埋底的牌
	Lint			m_state;									//状态 1:庄家保底，2:庄家保底，叫分100，得分翻倍，3:闲家扣底，4:闲家扣底，叫分100，得分翻倍
	Lint			m_jiaoFen;									//庄家叫分
	Lint			m_deFen;									//闲家得分
	Lint			m_isFinish;									//标记match是否结束 0：未结束  1：结束
	Lint			m_currCircle;								//当前圈数
	Lint			m_maxCircle;								//最大圈数
	Lint			m_winnerPos;								//最后一轮出牌最大玩家的位置;

	Lint			m_totleScore[SANDAYI_PLAYER_COUNT];		//玩家总得分
	Lint			m_userIds[SANDAYI_PLAYER_COUNT];		//游戏ID
	Lstring			m_userNames[SANDAYI_PLAYER_COUNT];   //游戏玩家昵称
	Lstring			m_headUrl[SANDAYI_PLAYER_COUNT];     //游戏玩家头像

	SanDaYiS2CDrawResult() :LMsgSC(MSG_S_2_C_SANDAYI_DRAW_RESULT_BC)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_zhuangPos = 0;
		m_type = 0;
		m_state = 0;
		m_jiaoFen = 0;
		m_deFen = 0;
		m_isFinish = 0;
		m_winnerPos = SANDAYI_INVALID_POS;
		memset(m_score, 0, sizeof(m_score));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_userIds, 0, sizeof(m_userIds));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kSate", m_state);
		WriteKeyValue(pack, "kJiaofen", m_jiaoFen);
		WriteKeyValue(pack, "kDefen", m_deFen);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);
		WriteKeyValue(pack, "kCurCircle", m_currCircle);
		WriteKeyValue(pack, "kCurMaxCircle", m_maxCircle);
		WriteKeyValue(pack, "kWinnerPos" ,m_winnerPos);

		std::vector<Lint> vect;
		std::vector<Lstring> vecs;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);
		//原始底牌
		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);
		//庄家埋底牌
		vect.clear();
		WriteKey(pack, "kBaseCardsMai");
		for (Lint i = 0; i < SANDAYI_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCardsMai[i]);
		}
		WriteKey(pack, vect);

		//显示总分
		WriteKey(pack, "kTotleScore");
		vect.clear();
		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_totleScore[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kUserIds");
		vect.clear();
		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userIds[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userNames[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_headUrl[i]);
		}
		WriteKey(pack, vecs);

		return true;
	}

	virtual LMsg* Clone() { return new SanDaYiS2CDrawResult(); }
};

/*
S->C 服务发送闲家得分慢105分,是否停止游戏
MSG_S_2_C_SANDAYI_SCORE_105 = 62313
*/
struct SanDaYiS2CScore105 : public LMsgSC
{
	Lint m_score;				//下家目前得分
	Lint m_resTime;				//剩余显示时间

	SanDaYiS2CScore105() : LMsgSC(MSG_S_2_C_SANDAYI_SCORE_105)
	{
		m_score = 0;
		m_resTime = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kScore), m_score);
		WriteKeyValue(pack, NAME_TO_STR(kResTime), m_resTime);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiS2CScore105();
	}
};

/*
C->S 玩家回复闲家得分慢105分,是否停止游戏
/MSG_C_2_S_SANDAYI_SCORE_105_RET = 62314
*/
struct SanDaYiC2SScore105Ret : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 

	SanDaYiC2SScore105Ret() : LMsgSC(MSG_C_2_S_SANDAYI_SCORE_105_RET)
	{
		m_pos = SANDAYI_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new SanDaYiC2SScore105Ret();
	}
};

/*
C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
MSG_C_2_S_SANDAYI_SCORE_105_RESULT = 62315
*/
struct SanDaYiS2CScore105Result :public LMsgSC
{
	Lint					m_agree[SANDAYI_PLAYER_COUNT];		//玩家状态

	SanDaYiS2CScore105Result() :LMsgSC(MSG_C_2_S_SANDAYI_SCORE_105_RESULT)
	{
		memset(m_agree, 0, sizeof(m_agree));
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);


		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kAgree");
		for (int i = 0; i < SANDAYI_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_agree[i]);
		}
		WriteKey(pack, vect);
		return true;
	}

	virtual LMsg* Clone() { return new SanDaYiS2CScore105Result(); }
};


////////////////////////////////// 五人百分 /////////////////////////////////////////

/*
S->C 服务器给每个玩家发牌
MSG_S_2_C_WURENBAIFEN_SEND_CARDS = 62350
*/
struct WuRenBaiFenS2CSendCards : public LMsgSC
{
	Lint m_outTime;				//发牌结束后叫主默认超时时间
	Lint m_pos;					//发牌玩家位置
	Lint m_nextSelectScorePos;		//第一个叫牌位置
	Lint m_handCardsCount;		//发牌数量
	Lint m_is2ChangZhu;			//2是否是常主 ,1:是常主.0.不是常主
	Lint m_handCards[WURENBAIFEN_HAND_CARDS_COUNT];	//玩家手牌
	Lint m_totleScore[WURENBAIFEN_PLAYER_COUNT]; //玩家头像下的总得分
	Lint m_clubAllHandCards[WURENBAIFEN_PLAYER_COUNT][WURENBAIFEN_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战

	WuRenBaiFenS2CSendCards() : LMsgSC(MSG_S_2_C_WURENBAIFEN_SEND_CARDS)
	{
		m_outTime = 0;
		m_pos = WURENBAIFEN_INVALID_POS;
		m_nextSelectScorePos = WURENBAIFEN_INVALID_POS;
		m_handCardsCount = 0;
		m_is2ChangZhu = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8 + WURENBAIFEN_PLAYER_COUNT);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kIs2ChangZhu", m_is2ChangZhu);

		std::vector<Lint> veci;

		//写入玩家手牌
		veci.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			veci.push_back(m_handCards[i]);
		}
		WriteKey(pack, veci);

		//写入每个玩家头像下的分数
		veci.clear();
		WriteKey(pack, "kTotleScore");
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_totleScore[i]);
		}
		WriteKey(pack, veci);

		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < WURENBAIFEN_HAND_CARDS_COUNT_MAX; j++)
			{
				veci.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, veci);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CSendCards();
	}
};

/*
S->C 服务器广播玩家叫分结果
MSG_S_2_C_WURENBAIFEN_SELECT_SCORE_R = 62352
*/
struct WuRenBaiFenS2CSelecScoreR : public LMsgSC
{
	Lint m_outTime;				//叫牌分默认超时时间
	Lint m_pos;					//当前选择叫牌分数玩家位置
	Lint m_selectScore;			//当前玩家选择的叫分：  默认不操作：-1， 不叫：0， 80，85，90，95，100
	Lint m_currMaxScore;		//当前叫牌最大分数
	Lint m_nextSelectScorePos;	//下一个叫分玩家的位置
	Lint m_zhuangPos;			//庄家位置
	Lint m_isFinish;			//叫分是否结束  0：未结束， 1：结束， 2：无人叫分从新洗牌开始	
	Lint m_jiaopaiOption; // 0:不支持交牌 1: 8分交牌 2:12分交

	WuRenBaiFenS2CSelecScoreR() : LMsgSC(MSG_S_2_C_WURENBAIFEN_SELECT_SCORE_R)
	{
		m_outTime = 0;
		m_pos = WURENBAIFEN_INVALID_POS;
		m_selectScore = -1;
		m_currMaxScore = 0;
		m_nextSelectScorePos = WURENBAIFEN_INVALID_POS;
		m_zhuangPos = WURENBAIFEN_INVALID_POS;
		m_isFinish = 0;
		m_jiaopaiOption = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, "kOutTime", m_outTime);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelelctScore", m_selectScore);
		WriteKeyValue(pack, "kCurrMaxScore", m_currMaxScore);
		WriteKeyValue(pack, "kNextSelectScorePos", m_nextSelectScorePos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);
		WriteKeyValue(pack, "kJiaoPaiOption", m_jiaopaiOption);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CSelecScoreR();
	}
};

/*
C->S 玩家选择叫牌分
MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
*/
struct WuRenBaiFenC2SSelectScore : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_selecScore;			//玩家叫牌的分数

	WuRenBaiFenC2SSelectScore() : LMsgSC(MSG_C_2_S_WURENBAIFEN_SELECT_SCORE)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_selecScore = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelecScore", m_selecScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SSelectScore();
	}
};

/*
S->C 服务器通知玩家拿底牌，并选主花色
MSG_S_2_C_WURENBAIFEN_BASE_CARD_AND_SELECT_MAIN_N = 62354
*/
struct WuRenBaiFenS2CBaseCardsAndSelectMainN : public LMsgSC
{
	Lint m_postmd;
	Lint m_zhuangPos;
	Lint m_maxSelectScore;
	Lint m_baseCardsCount;
	Lint m_baseCards[WURENBAIFEN_BASE_CARDS_COUNT];
	Lint m_jiaopaiOption; // 0:不支持交牌 1: 8分交牌 2:12分交

	WuRenBaiFenS2CBaseCardsAndSelectMainN() : LMsgSC(MSG_S_2_C_WURENBAIFEN_BASE_CARD_AND_SELECT_MAIN_N)
	{
		m_postmd = WURENBAIFEN_INVALID_POS;
		m_zhuangPos = WURENBAIFEN_INVALID_POS;
		m_maxSelectScore = 0;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
		m_jiaopaiOption = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kTmd", m_postmd);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kMaxSelectScore", m_maxSelectScore);
		WriteKeyValue(pack, "kBaseCardsCount", m_baseCardsCount);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (int i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);
		WriteKeyValue(pack, "kJiaoPaiOption", m_jiaopaiOption);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CBaseCardsAndSelectMainN();
	}
};

/*
C->S 玩家选主牌
MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
*/
struct WuRenBaiFenC2SSelectMain : public LMsgSC
{
	Lint m_pos;						//叫牌分玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	WuRenBaiFenC2SSelectMain() : LMsgSC(MSG_C_2_S_WURENBAIFEN_SELECT_MAIN)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectMainColor", m_selectMainColor);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SSelectMain();
	}
};

/*
S->C 服务器返回玩家选主牌结果
MSG_S_2_C_WURENBAIFEN_SELECT_MAIN_R = 62356
*/
struct WuRenBaiFenS2CSelectMainColorR : public LMsgSC
{
	Lint m_pos;						//选主玩家位置
	Lint m_selectMainColor;			//玩家叫牌的分数

	WuRenBaiFenS2CSelectMainColorR() : LMsgSC(MSG_S_2_C_WURENBAIFEN_SELECT_MAIN_R)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_selectMainColor = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectMainColor", m_selectMainColor);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CSelectMainColorR();
	}
};

/*
C->S 玩家埋底
MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
*/
struct WuRenBaiFenC2SBaseCard : public LMsgSC
{
	Lint m_pos;					//埋底牌玩家位置
	Lint m_baseCardsCount;		//底牌数量
	Lint m_baseCards[WURENBAIFEN_BASE_CARDS_COUNT];	//底牌

	WuRenBaiFenC2SBaseCard() : LMsgSC(MSG_C_2_S_WURENBAIFEN_BASE_CARD)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_baseCardsCount = 0;
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kBaseCardsCount", m_baseCardsCount);
		int card_value = 0;
		int nCount = 0;
		msgpack::object cardsArray;
		ReadMapData(obj, "kBaseCards", cardsArray);
		if (cardsArray.type == msgpack::v1::type::ARRAY)
		{
			for (Lsize i = 0; i < cardsArray.via.array.size; ++i)
			{
				msgpack::object pv = *(cardsArray.via.array.ptr + i);
				pv.convert(card_value);
				m_baseCards[i] = card_value;
				nCount += 1;
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
		return new WuRenBaiFenC2SBaseCard();
	}
};

/*
S->C 服务器返回玩家埋底结果
MSG_S_2_C_WURENBAIFEN_BASE_CARD_R = 62358
*/
struct WuRenBaiFenS2CBaseCardR : public LMsgSC
{
	Lint m_pos;					//埋底玩家位置
	Lint m_flag;				//是否埋底成功  0：失败，1：成功
	Lint m_handCardsCount;		//手牌数量
	Lint m_pokerTen;			//选对家是否含10   0:不含,1:含十
	Lint m_handCards[WURENBAIFEN_HAND_CARDS_COUNT_MAX];	//手牌
	Lint m_baseCards[WURENBAIFEN_BASE_CARDS_COUNT];		//底牌


	WuRenBaiFenS2CBaseCardR() : LMsgSC(MSG_S_2_C_WURENBAIFEN_BASE_CARD_R)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_flag = 0;
		m_handCardsCount = 0;
		m_pokerTen = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_baseCards, 0, sizeof(m_baseCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kFlag", m_flag);
		WriteKeyValue(pack, "kHandCardsCount", m_handCardsCount);
		WriteKeyValue(pack, "kPokerTen", m_pokerTen);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kHandCards");
		for (Lint i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CBaseCardR();
	}
};

/*
C->S 玩家选对家牌
MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
*/
struct WuRenBaiFenC2SSelectFriend : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_selectFriendCard;	//庄家选副庄家的牌

	WuRenBaiFenC2SSelectFriend() : LMsgSC(MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_selectFriendCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectFriendCard", m_selectFriendCard);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SSelectFriend();
	}
};

/*
S->C 服务器返回庄家选对家的牌
MSG_S_2_C_WURENBAIFEN_SELECT_FRIEND_BC = 62360
*/
struct WuRenBaiFenS2CSelectFriendCardBC : public LMsgSC
{
	Lint m_pos;							//选对家玩家的位置
	Lint m_selectFriendCard;			//玩家选对家牌
	Lint m_firendZhuangPos;				//对家位置
	Lint m_nextOutCardPos;				//出牌位置

	WuRenBaiFenS2CSelectFriendCardBC() : LMsgSC(MSG_S_2_C_WURENBAIFEN_SELECT_FRIEND_BC)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_firendZhuangPos = WURENBAIFEN_INVALID_POS;
		m_selectFriendCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectFriendCard", m_selectFriendCard);
		WriteKeyValue(pack, "kFirendZhuangPos", m_firendZhuangPos);
		WriteKeyValue(pack, "kNextOutCardPos", m_nextOutCardPos);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CSelectFriendCardBC();
	}
};

/*
C->S 玩家出牌
MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
*/
struct WuRenBaiFenC2SOutCard : public LMsgSC
{
	Lint m_pos;					//叫牌分玩家位置
	Lint m_outCard;				//玩家出的牌

	WuRenBaiFenC2SOutCard() : LMsgSC(MSG_C_2_S_WURENBAIFEN_OUT_CARD)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_outCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kOutCard", m_outCard);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SOutCard();
	}
};

/*
S->C 服务器广播出牌结果
MSG_S_2_C_WURENBAIFEN_OUT_CARD_BC = 62362
*/
struct WuRenBaiFenS2COutCardBC : public LMsgSC
{
	Lint m_pos;										//出牌玩家位置
	Lint m_outCard;									//出的牌
	Lint m_turnStart;								//标记本轮第一次出牌  0：非第一个出牌， 1：是第一个出牌
	Lint m_turnOver;								//标记本轮出牌结束  0：未结束， 1：结束
	Lint m_nextPos;									//下一玩家出牌的位置
	Lint m_turnWiner;								//一轮出牌最大的玩家
	Lint m_currBig;									//当前最大出牌玩家
	Lint m_mode;									//出牌的类型
	Lint m_thisTurnScore;							//本轮出牌的分数
	Lint m_getTotleScore;							//本局总得分
	Lint m_scoreCardsCount;							//闲家抓分牌数量
	Lint m_scoreCards[12];							//闲家抓的分牌
	Lint m_soundType;								//声音-1：默认不处理 0：管不上领牌 1：大你 / 管上 2：主杀
	Lint m_leftover_score;							//如果m_turnOver=1,则为剩余的分
	Lint m_handCardCount[WURENBAIFEN_PLAYER_COUNT];		//玩家手牌数量

	Lint m_prevOutCard[WURENBAIFEN_PLAYER_COUNT];		//如果m_turnOver=1,则未这轮出的牌
	Lint m_outCardArray[WURENBAIFEN_PLAYER_COUNT];		//本轮出牌结果

	WuRenBaiFenS2COutCardBC() : LMsgSC(MSG_S_2_C_WURENBAIFEN_OUT_CARD_BC)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_outCard = 0;
		m_turnStart = 0;
		m_turnOver = 0;
		m_nextPos = WURENBAIFEN_INVALID_POS;
		m_turnWiner = WURENBAIFEN_INVALID_POS;
		m_currBig = WURENBAIFEN_INVALID_POS;
		m_mode = 0;
		m_thisTurnScore = 0;
		m_getTotleScore = 0;
		m_scoreCardsCount = 0;
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		m_soundType = 0;
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_outCardArray, 0, sizeof(m_outCardArray));
		m_leftover_score = 0;
		memset(m_handCardCount, 0, sizeof(m_handCardCount));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kOutCard", m_outCard);
		WriteKeyValue(pack, "kTurnStart", m_turnStart);
		WriteKeyValue(pack, "kTurnOver", m_turnOver);
		WriteKeyValue(pack, "kNextPos", m_nextPos);
		WriteKeyValue(pack, "kTurnWiner", m_turnWiner);
		WriteKeyValue(pack, "kCurrBig", m_currBig);
		WriteKeyValue(pack, "kMode", m_mode);
		WriteKeyValue(pack, "kThisTurnScore", m_thisTurnScore);
		WriteKeyValue(pack, "kTotleScore", m_getTotleScore);
		WriteKeyValue(pack, "kScoreCardsCount", m_scoreCardsCount);
		WriteKeyValue(pack, "kSoundType", m_soundType);
		WriteKeyValue(pack, "kLeftoverScore", m_leftover_score);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kOutCardArray");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCardArray[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kHandCardCount");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardCount[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2COutCardBC();
	}
};


/*
S->C 服务器发送断线重连
MSG_S_2_C_WURENBAIFEN_RECON = 62363
*/
struct WuRenBaiFenS2CReConnect : public LMsgSC
{
	Lint		m_pos;										//玩家位置
	Lint		m_curPos;									//当前操作玩家位置
	Lint		m_zhuang;									//庄的位置
	Lint		m_fuzhuangCard;								//副庄的牌
	Lint		m_currBig;									//当前最大出牌玩家
	Lint		m_state;									//当前桌子状态
	Lint		m_selectMainColor;							//主牌
	Lint		m_Monoy;									//闲家本局总得份
	Lint		m_hand_cards_count;							//手牌个数
	Lint		m_score[WURENBAIFEN_PLAYER_COUNT];				//玩家本场截止目前总积分
	Lint		m_outCard[WURENBAIFEN_PLAYER_COUNT];			//这一轮前面几家打出去的牌
	Lint		m_handCards[WURENBAIFEN_HAND_CARDS_COUNT_MAX];	//手牌
	Lint		m_scoreCards[12];							//闲家抓的分牌
	Lint		m_prevOutCard[WURENBAIFEN_PLAYER_COUNT];		//上轮的牌

	Lint		m_playType[5];								//参数  0:底分 1:2是否常主 2:是否含十  3:GPS 
	Lint		m_max_select_score;							//本局庄家叫分
	Lint		m_leftover_score;							//如果m_turnOver=1,则为剩余的分

	Lint		m_baseCards[WURENBAIFEN_BASE_CARDS_COUNT];		//底牌
	Lint		m_fuzhuangPos;								//副庄位置
	Lint		m_firstOutPos;								//第一个出牌的位置

	Lint		m_clubAllHandCards[WURENBAIFEN_PLAYER_COUNT][WURENBAIFEN_HAND_CARDS_COUNT_MAX];  //俱乐部会长明牌观战
	Lint		m_handCardsCountArr[WURENBAIFEN_PLAYER_COUNT];	//每个人手牌数量，观战用

	Lint     m_nJiaoPaiState;       // 交牌的状态变量
	Lint     m_nTimeLeftJiaoPaiRequest;
	Lint     m_nTimeLeftJiaoPaiAck;
	Lint     m_jiaopaiAgree[WURENBAIFEN_PLAYER_COUNT]; //拒绝同意状态

	WuRenBaiFenS2CReConnect() : LMsgSC(MSG_S_2_C_WURENBAIFEN_RECON)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_curPos = WURENBAIFEN_INVALID_POS;
		m_zhuang = WURENBAIFEN_INVALID_POS;
		m_fuzhuangCard = WURENBAIFEN_INVALID_CARD;
		m_currBig = WURENBAIFEN_INVALID_POS;
		m_state = 0;
		m_selectMainColor = 0x40;
		m_Monoy = 0;
		m_hand_cards_count = 0;
		m_max_select_score = 0;
		m_leftover_score = 0;
		m_fuzhuangPos = WURENBAIFEN_INVALID_POS;
		m_firstOutPos = 0;
		m_nJiaoPaiState = 0;
		m_nTimeLeftJiaoPaiRequest = m_nTimeLeftJiaoPaiAck = 0;
		memset(m_score, 0, sizeof(m_score));
		memset(m_outCard, 0, sizeof(m_outCard));
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_scoreCards, 0, sizeof(m_scoreCards));
		memset(m_prevOutCard, 0, sizeof(m_prevOutCard));
		memset(m_playType, 0, sizeof(m_playType));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_clubAllHandCards, 0, sizeof(m_clubAllHandCards));
		memset(m_handCardsCountArr, 0, sizeof(m_handCardsCountArr));
		memset(m_jiaopaiAgree, 0, sizeof(m_jiaopaiAgree));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 22 + WURENBAIFEN_PLAYER_COUNT + 4);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kCurPos", m_curPos);
		WriteKeyValue(pack, "kZhuang", m_zhuang);
		WriteKeyValue(pack, "kFuzhuangCard", m_fuzhuangCard);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kMainColor", m_selectMainColor);
		WriteKeyValue(pack, "kMonoy", m_Monoy);
		WriteKeyValue(pack, "kHandCardCount", m_hand_cards_count);
		WriteKeyValue(pack, "kMaxselectscore", m_max_select_score);
		WriteKeyValue(pack, "kLeftoverScore", m_leftover_score);
		WriteKeyValue(pack, "kFuzhuangPos", m_fuzhuangPos);
		WriteKeyValue(pack, "kFirstPos", m_firstOutPos);
		WriteKeyValue(pack, "kCurrBig", m_currBig);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kOutCard");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_outCard[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kHandCards");
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT_MAX; ++i)
		{
			vect.push_back(m_handCards[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kScoreCards");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_scoreCards[i]);
		}
		WriteKey(pack, vect);

		//std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kPrevOutCard");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_prevOutCard[i]);
		}
		WriteKey(pack, vect);

		vect.clear();

		WriteKey(pack, "kPlayType");
		for (int i = 0; i < 4; ++i)
		{
			vect.push_back(m_playType[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kClubAllHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < WURENBAIFEN_HAND_CARDS_COUNT_MAX; j++)
			{
				vect.push_back(m_clubAllHandCards[i][j]);
			}
			WriteKey(pack, vect);
		}

		vect.clear();
		WriteKey(pack, "kHandCardsCountArr");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardsCountArr[i]);
		}
		WriteKey(pack, vect);

		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_nJiaoPaiState);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRequest), m_nTimeLeftJiaoPaiRequest);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRespone), m_nTimeLeftJiaoPaiAck);

		vect.clear();
		WriteKey(pack, "kJiaoPaiAgreeStatus");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_jiaopaiAgree[i]);
		}
		WriteKey(pack, vect);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CReConnect();
	}
};

/*
S->C 服务发送小结算
MSG_S_2_C_WURENBAIFEN_DRAW_RESULT_BC = 62364
*/
struct WuRenBaiFenS2CDrawResult :public LMsgSC
{
	Lint			m_pos;								//位置，0-4
	Lint			m_zhuangPos;						//庄位置
	Lint			m_type;								//0:小结.1:总结
	Lint			m_score[WURENBAIFEN_PLAYER_COUNT];		//玩家积分
	Lint			m_baseCards[WURENBAIFEN_BASE_CARDS_COUNT];		//底牌
	Lint			m_state;									//状态 1:庄家保底，2:庄家保底，叫分100，得分翻倍，3:闲家扣底，4:闲家扣底，叫分100，得分翻倍
	Lint			m_fuzhuangPos;								//副庄位置
	Lint			m_jiaofen;									//庄家叫分
	Lint			m_defen;									//闲家得分
	Lint			m_isFinish;									//标记match是否结束 0：未结束  1：结束

	Lint			m_totleScore[WURENBAIFEN_PLAYER_COUNT];		//玩家总得分
	Lint			m_userIds[WURENBAIFEN_PLAYER_COUNT];		//游戏ID
	Lstring			m_userNames[WURENBAIFEN_PLAYER_COUNT];   //游戏玩家昵称
	Lstring			m_headUrl[WURENBAIFEN_PLAYER_COUNT];     //游戏玩家头像
	Lint        m_out_cards_last_turn[WURENBAIFEN_PLAYER_COUNT];
	Lint        m_wurenbaifen_over_state;

	WuRenBaiFenS2CDrawResult() :LMsgSC(MSG_S_2_C_WURENBAIFEN_DRAW_RESULT_BC)
	{
		m_pos = INVAILD_POS;
		m_zhuangPos = 0;
		m_type = 0;
		m_state = 0;
		m_fuzhuangPos = 0;
		m_jiaofen = 0;
		m_defen = 0;
		m_isFinish = 0;
		memset(m_score, 0, sizeof(m_score));
		memset(m_baseCards, 0, sizeof(m_baseCards));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_userIds, 0, sizeof(m_userIds));
		ZeroMemory(m_out_cards_last_turn, sizeof(m_out_cards_last_turn));
		m_wurenbaifen_over_state = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 15 + 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kZhuangPos", m_zhuangPos);
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kSate", m_state);
		WriteKeyValue(pack, "kFuzhuangPos", m_fuzhuangPos);
		WriteKeyValue(pack, "kJiaofen", m_jiaofen);
		WriteKeyValue(pack, "kDefen", m_defen);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);

		std::vector<Lint> vect;
		std::vector<Lstring> vecs;
		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kBaseCards");
		for (Lint i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			vect.push_back(m_baseCards[i]);
		}
		WriteKey(pack, vect);

		//显示总分
		WriteKey(pack, "kTotleScore");
		vect.clear();
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_totleScore[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kUserIds");
		vect.clear();
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userIds[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userNames[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_headUrl[i]);
		}
		WriteKey(pack, vecs);

		WriteKey(pack, "kLastTurnOutCards");
		vect.clear();
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_out_cards_last_turn[i]);
		}
		WriteKey(pack, vect);

		WriteKeyValue(pack, "kWuRenBaiFenOverState", m_wurenbaifen_over_state);

		return true;
	}

	virtual LMsg* Clone() { return new WuRenBaiFenS2CDrawResult(); }
};

/*
S->C 服务发送闲家得分满105分,是否停止游戏
MSG_S_2_C_WURENBAIFEN_SCORE_105 = 62365
*/
struct WuRenBaiFenS2CScore105 : public LMsgSC
{
	Lint m_Score;				//下家目前得分

	WuRenBaiFenS2CScore105() : LMsgSC(MSG_S_2_C_WURENBAIFEN_SCORE_105)
	{
		m_Score = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kScore), m_Score);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CScore105();
	}
};

/*
C->S 玩家回复闲家得分慢105分,是否停止游戏
/MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366
*/
struct WuRenBaiFenC2SScore105Ret : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 

	WuRenBaiFenC2SScore105Ret() : LMsgSC(MSG_C_2_S_WURENBAIFEN_SCORE_105_RET)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SScore105Ret();
	}
};

/*
C->S 服务器广播复闲家得分满105分,是否停止游戏选择结果
MSG_C_2_S_WURENBAIFEN_SCORE_105_RESULT = 62367
*/
struct WuRenBaiFenS2CScore105Result :public LMsgSC
{	
	Lint					m_agree[WURENBAIFEN_PLAYER_COUNT];		//玩家状态

	WuRenBaiFenS2CScore105Result() :LMsgSC(MSG_C_2_S_WURENBAIFEN_SCORE_105_RESULT)
	{	 
 		memset(m_agree, 0, sizeof(m_agree));
	}

	virtual bool Read(msgpack::object& obj)
	{

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);	 

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kAgree");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_agree[i]);
		}
		WriteKey(pack, vect);
		return true;
	}

	virtual LMsg* Clone() { return new WuRenBaiFenS2CScore105Result(); }
};

/*
C->S 客户端庄家选择交牌
MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368
*/
struct WuRenBaiFenC2SZhuangJiaoPai : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 


	WuRenBaiFenC2SZhuangJiaoPai() : LMsgSC(MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{

		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SZhuangJiaoPai();
	}
};

/*
S->C 服务端广播庄家选择交牌，提示其他客户端选择是否同意庄家交牌
MSG_S_2_C_WURENBAIFEN_ZHUANG_JIAO_PAI_BC = 62369
*/
struct WuRenBaiFenS2CZhuangJiaoPaiBC : public LMsgSC
{
	Lint m_jiaopaiState;            //交牌状态
	Lint m_zhuangPos;				//庄家位置
	Lint m_jiaopaiRequest;       // 0: 不用 1: 交牌 2:不交
	Lint m_xianResponseLeft;  // 闲家响应超时时间

	WuRenBaiFenS2CZhuangJiaoPaiBC() : LMsgSC(MSG_S_2_C_WURENBAIFEN_ZHUANG_JIAO_PAI_BC)
	{
		m_zhuangPos = WURENBAIFEN_INVALID_POS;
		m_jiaopaiState = 0;
		m_xianResponseLeft = 0;
		m_jiaopaiRequest = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kZhuangPos), m_zhuangPos);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_jiaopaiState);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiRequest), m_jiaopaiRequest);
		WriteKeyValue(pack, NAME_TO_STR(kTimeJiaoPaiRespone), m_xianResponseLeft);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenS2CZhuangJiaoPaiBC();
	}
};

/*
C->S 非庄家客户端选择庄家交牌的申请
MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370
*/
struct WuRenBaiFenC2SXianSelectJiaoPai : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_agree;				//是否同意 0:拒绝 1:同意 

	WuRenBaiFenC2SXianSelectJiaoPai() : LMsgSC(MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI)
	{
		m_pos = WURENBAIFEN_INVALID_POS;
		m_agree = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAgree", m_agree);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new WuRenBaiFenC2SXianSelectJiaoPai();
	}
};

/*
S->C 庄家交牌的最终结果
MSG_S_2_C_WURENBAIFEN_JIAO_PAI_RESULT = 62371
*/
struct WuRenBaiFenS2CJiaoPaiResult :public LMsgSC
{
	Lint m_zhuangPos;               //庄位置
	Lint m_jiaoPaiSate;            // 交牌状态
	Lint	m_agree[WURENBAIFEN_PLAYER_COUNT];		//玩家状态

	WuRenBaiFenS2CJiaoPaiResult() :LMsgSC(MSG_S_2_C_WURENBAIFEN_JIAO_PAI_RESULT)
	{
		memset(m_agree, 0, sizeof(m_agree));
		m_jiaoPaiSate = 0;
		m_zhuangPos = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(kMId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(kJiaoPaiState), m_jiaoPaiSate);
		WriteKeyValue(pack, NAME_TO_STR(kZhuangPos), m_zhuangPos);

		std::vector<Lint> vect;
		vect.clear();
		WriteKey(pack, "kAgree");
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_agree[i]);
		}
		WriteKey(pack, vect);
		return true;
	}


	virtual LMsg* Clone() { return new WuRenBaiFenS2CJiaoPaiResult(); }
};

///////////////////////////////////////// 扯炫 ////////////////////////////////////////

/*
  S->C 服务器通知玩家起钵钵
  MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_QI_BOBO = 62400
*/
struct CheXuanS2CNotifyPlayerQiBoBo : public LMsgSC
{
	Lint		m_pos;					//玩家位置
	Lint		m_restTime;				//剩余操作时间
	Lint		m_piPool;				//当前皮池
	Lint		m_mangPool;				//当前芒果池
	Lint		m_dangPos;				//挡家位置
	Lint		m_minSelectBoScore;		//最小起钵钵数量
	Lint		m_maxSelectBoScore;		//最大起钵钵数量
	Lint		m_userStatus[CHEXUAN_PLAYER_COUNT];		//玩家状态   0:位置上没人  1：正常玩家  2：动态加入玩家
	Lint		m_userMustQiBoBo[CHEXUAN_PLAYER_COUNT];	//必须起钵钵玩家 0：不必须下钵钵  1：必须下钵钵
	Lint		m_playerStandUp[CHEXUAN_PLAYER_COUNT];	//玩家站起坐下标志  0：没有操作(默认坐下状态)   1：当局点击站起(当局还未站起)    2：已经站起状态     3：当局点击坐下（当局未坐下）

	CheXuanS2CNotifyPlayerQiBoBo() : LMsgSC(MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_QI_BOBO)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_restTime = 1;
		m_piPool = 0;
		m_mangPool = 0;
		m_dangPos = CHEXUAN_INVALID_POS;
		m_minSelectBoScore = -1;
		m_maxSelectBoScore = -1;
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_userMustQiBoBo, 0, sizeof(m_userMustQiBoBo));
		memset(m_playerStandUp, 0, sizeof(m_playerStandUp));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kRestTime", m_restTime);
		WriteKeyValue(pack, "kPiPool", m_piPool);
		WriteKeyValue(pack, "kMangPool", m_mangPool);
		WriteKeyValue(pack, "kDangPos", m_dangPos);
		WriteKeyValue(pack, "kMinSelectBoScore", m_minSelectBoScore);
		WriteKeyValue(pack, "kMaxSelectBoScore", m_maxSelectBoScore);

		std::vector<Lint> veci;

		veci.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kUserMustQiBoBo");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userMustQiBoBo[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kPlayerStandUp");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_playerStandUp[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone() 
	{
		return new CheXuanS2CNotifyPlayerQiBoBo(); 
	}
};

/*
  C->S 玩家起钵钵
  MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
*/
struct CheXuanC2SPlayerSelectBoBo : public LMsgSC
{
	Lint		m_pos;				//选择起钵钵的玩家位置
	Lint		m_selectBoScore;	//玩家选择起的钵钵数量

	CheXuanC2SPlayerSelectBoBo() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_selectBoScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectBoScore", m_selectBoScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerSelectBoBo();
	}
};

/*
  S->C 广播玩家起钵钵结果
  MSG_S_2_C_CHEXUAN_PLAYER_SELECT_BOBO_BC = 62402
*/
struct CheXuanS2CPlayerSelectBoBoBC : public LMsgSC
{
	Lint		m_pos;								
	Lint		m_lastBoScore[CHEXUAN_PLAYER_COUNT];	//最后一次下钵钵分数
	Lint		m_totalBoScore[CHEXUAN_PLAYER_COUNT];	//总下钵钵分数
	Lint		m_userStatus[CHEXUAN_PLAYER_COUNT];		//玩家状态   0:位置上没人  1：正常玩家  2：动态加入玩家
	Lint		m_optBoBoStatus[CHEXUAN_PLAYER_COUNT];	//玩家起钵钵状态  0：未操作   1：已操作

	CheXuanS2CPlayerSelectBoBoBC() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_SELECT_BOBO_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		memset(m_lastBoScore, 0, sizeof(m_lastBoScore));
		memset(m_totalBoScore, 0, sizeof(m_totalBoScore)); 
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_optBoBoStatus, 0, sizeof(m_optBoBoStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);

		std::vector<Lint> vec;
		
		vec.clear();
		WriteKey(pack, "kLastBoScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vec.push_back(m_lastBoScore[i]);
		}
		WriteKey(pack, vec);

		vec.clear();
		WriteKey(pack, "kTotalBoScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vec.push_back(m_totalBoScore[i]);
		}
		WriteKey(pack, vec);

		vec.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vec.push_back(m_userStatus[i]);
		}
		WriteKey(pack, vec);

		vec.clear();
		WriteKey(pack, "kOptBoBoStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vec.push_back(m_optBoBoStatus[i]);
		}
		WriteKey(pack, vec);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerSelectBoBoBC();
	}
};

/*
  C->S 客户端下芒注结果
  MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANG_SCORE_BC = 62419
*/
struct CheXuanC2SPlayerAddMangScoreBC : public LMsgSC
{
	Lint		m_pos;			//下芒注玩家位置
	Lint		m_mangScore;	//下忙芒注

	CheXuanC2SPlayerAddMangScoreBC() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANG_SCORE_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_mangScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kMangScore", m_mangScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerAddMangScoreBC();
	}
};

/*
  S->C 服务端通知玩家下芒果
  MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_ADD_MANGGUO = 62403
*/
struct CheXuanS2CNotifyPlayerAddMangGuo : public LMsgSC
{
	Lint		m_pos;

	CheXuanS2CNotifyPlayerAddMangGuo() : LMsgSC(MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_ADD_MANGGUO)
	{
		m_pos = CHEXUAN_INVALID_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CNotifyPlayerAddMangGuo();
	}
};

/*
  C->S 玩家下芒果
  MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANGGUO = 62404
*/
struct CheXuanC2SPlayerAddMangGuo : public LMsgSC
{
	Lint		m_pos;

	CheXuanC2SPlayerAddMangGuo() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANGGUO)
	{
		m_pos = CHEXUAN_INVALID_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerAddMangGuo();
	}
};

/*
  S->C 广播玩家下芒果
  MSG_S_2_C_CHEXUAN_PLAYER_ADD_MANGGUO_BC = 62405
*/
struct CheXuanS2CNotifyPlayerAddMangGuoBC : public LMsgSC
{
	Lint m_pos;						//下芒果玩家位置
	Lint m_restTime;				//倒计时剩余时间,有默分的情况下才有
	Lint m_totalBoScore;			//自己手里剩余的总芒果数量
	Lint m_totalAddMangScore;		//总的下芒果分数
	Lint m_addShouShouMangCount;	//本局需要下的手手芒数量
	Lint m_addXiuMangCount;			//本局需要下的休芒数量
	Lint m_addZouMangCount;			//本局需要下的揍芒数量
	Lint m_mangPoolCount;			//当前芒果池总数
	Lint m_piPoolCount;				//当前皮池总数
	Lint m_mangScore;				//下的芒注
	Lint m_isZouMang;				//是否为揍芒  0：非揍芒  1：揍芒
	Lint m_isXiuMang;				//是否休芒    0：非休芒  1：休芒

	CheXuanS2CNotifyPlayerAddMangGuoBC() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_ADD_MANGGUO_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_restTime = 0;
		m_totalBoScore = 0;
		m_totalAddMangScore = 0;
		m_addShouShouMangCount = 0;
		m_addXiuMangCount = 0;			//本局需要下的休芒数量
		m_addZouMangCount = 0;			//本局需要下的揍芒数量
		m_mangPoolCount = 0;			//当前芒果池总数
		m_piPoolCount = 0;				//当前皮池总数
		m_mangScore = 0;
		m_isZouMang = 0;
		m_isXiuMang = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 13);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kRestTime", m_restTime);
		WriteKeyValue(pack, "kTotalBoScore", m_totalBoScore);
		WriteKeyValue(pack, "kTotalAddMangScore", m_totalAddMangScore);
		WriteKeyValue(pack, "kAddShouShouMangCount", m_addShouShouMangCount);
		WriteKeyValue(pack, "kAddXiuMangCount", m_addXiuMangCount);
		WriteKeyValue(pack, "kAddZouMangCount", m_addZouMangCount);
		WriteKeyValue(pack, "kMangPoolCount", m_mangPoolCount);
		WriteKeyValue(pack, "kPiPoolCount", m_piPoolCount);
		WriteKeyValue(pack, "kMangScore", m_mangScore);
		WriteKeyValue(pack, "kIsZouMang", m_isZouMang);
		WriteKeyValue(pack, "kIsXiuMang", m_isXiuMang);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CNotifyPlayerAddMangGuo();
	}
};

/*
  S->C 服务器给玩家发牌
  MSG_S_2_C_CHEXUAN_SEND_CARD = 62406
*/
struct CheXuanS2CSendCard : public LMsgSC
{
	Lint m_pos;					//玩家位置
	Lint m_turnCount;			//发牌轮数 0:第1轮2张暗牌，1:第2轮1张明牌，2：第3轮1张暗牌
	Lint m_currSendCard;		//当前发的牌
	Lint m_handCardCount;		//当前手牌总数
	Lint m_handCards[CHEXUAN_HAND_CARDS_COUNT];	

	CheXuanS2CSendCard() : LMsgSC(MSG_S_2_C_CHEXUAN_SEND_CARD)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_turnCount = 0;
		m_currSendCard = 0;
		m_handCardCount = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kTurnCount", m_turnCount);
		WriteKeyValue(pack, "kCurrSendCard", m_currSendCard);
		WriteKeyValue(pack, "kHandCardCount", m_handCardCount);

		std::vector<Lint> vec;

		WriteKey(pack, "kHandCards");
		vec.clear();
		for (int i = 0; i < CHEXUAN_HAND_CARDS_COUNT; ++i)
		{
			vec.push_back(m_handCards[i]);
		}
		WriteKey(pack, vec);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CSendCard();
	}
};


/*
  S->C 服务器通知玩家操作
  MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_OPT = 62407
*/
struct CheXuanS2CNotifyPlayerOpt : public LMsgSC
{
	Lint m_pos;
	Lint m_restTime;				//倒计时剩余时间
	Lint m_optType[10];				//允许的操作类型
	Lint m_optMinScore[10];			//对应的操作类型最小操作数
	Lint m_optMaxScore[10];			//对应的操作类型最大操作数
	Lint m_isMeiDaDong;				//是否没搭动  0：不存在， 1：存在没搭动
	Lint m_hasPlayerQiao;			//是否有人敲了  0：没人敲  1：已经有人敲了
	Lint m_isMoFen;					//是否是默分  0：正常  1：默分

	CheXuanS2CNotifyPlayerOpt() : LMsgSC(MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_OPT)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_restTime = -1;
		m_isMeiDaDong = 0;
		m_isMoFen = 0;
		for (int i = 0; i < 10; ++i)
		{
			m_optType[i] = -1;
		}
		memset(m_optMinScore, 0, sizeof(m_optMinScore));
		memset(m_optMaxScore, 0, sizeof(m_optMaxScore));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kRestTime", m_restTime);
		WriteKeyValue(pack, "kIsMeiDaDong", m_isMeiDaDong);
		WriteKeyValue(pack, "kHasPlayerQiao", m_hasPlayerQiao);
		WriteKeyValue(pack, "kIsMoFen", m_isMoFen);

		std::vector<Lint> vec;

		WriteKey(pack, "kOptType");
		vec.clear();
		for (int i = 0; i < 10; ++i)
		{
			vec.push_back(m_optType[i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "kOptMinScore");
		vec.clear();
		for (int i = 0; i < 10; ++i)
		{
			vec.push_back(m_optMinScore[i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "kOptMaxScore");
		vec.clear();
		for (int i = 0; i < 10; ++i)
		{
			vec.push_back(m_optMaxScore[i]);
		}
		WriteKey(pack, vec);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CNotifyPlayerOpt();
	}
};

/*
  C->S 玩家选择操作
  MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 62408
*/
struct CheXuanC2SPlayerSelectOpt : public LMsgSC
{
	Lint		m_pos;				//玩家位置
	Lint		m_selectType;		//玩家选择操作类型
	Lint		m_selectScore;		//玩家选择的下注分数

	CheXuanC2SPlayerSelectOpt() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_selectType = 0;
		m_selectScore = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectType", m_selectType);
		ReadMapData(obj, "kSelectScore", m_selectScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerSelectOpt();
	}
};

/*
  S->C 服务器广播玩家操作结果
  MSG_S_2_C_CHEXUAN_PLAYER_SELECT_OPT_BC = 62409
*/
struct CheXuanS2CPlayerOptBC : public LMsgSC
{
	Lint m_pos;
	Lint m_selectType;				//玩家选择的操作
	Lint m_selectScore;				//玩家选择的分数
	Lint m_turnAddTotalScore;		//玩家本轮下注总分数
	Lint m_addTotalScore;			//玩家本局总下注分数
	Lint m_totalBoScore;			//自己手里剩余的总分数
	Lint m_nextOptPos;				//下一个操作玩家的位置
	Lint m_piScore;					//皮里面的分数
	Lint m_mangScore;				//芒果里面的分数
	Lint m_is2TimesPre;				//是否是上一个玩家下注的两倍
	Lint m_delayTime;				//延时显示时间

	CheXuanS2CPlayerOptBC() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_SELECT_OPT_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_selectType = 0;
		m_selectScore = 0;
		m_turnAddTotalScore = 0;
		m_addTotalScore = 0;
		m_totalBoScore = 0;
		m_nextOptPos = CHEXUAN_INVALID_POS;
		m_piScore = 0;
		m_mangScore = 0;
		m_is2TimesPre = 0;
		m_delayTime = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectType", m_selectType);
		WriteKeyValue(pack, "kSelectScore", m_selectScore);
		WriteKeyValue(pack, "kTurnAddTotalScore", m_turnAddTotalScore);
		WriteKeyValue(pack, "kAddTotalScore", m_addTotalScore);
		WriteKeyValue(pack, "kTotalBoScore", m_totalBoScore);
		WriteKeyValue(pack, "kPiScore", m_piScore);
		WriteKeyValue(pack, "kMangScore", m_mangScore);
		WriteKeyValue(pack, "kNextOptPos", m_nextOptPos);
		WriteKeyValue(pack, "kIs2TimesPre", m_is2TimesPre);
		WriteKeyValue(pack, "kDelayTime", m_delayTime);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerOptBC();
	}
};

/*
  S->C 服务器通知玩家开始扯牌
  MSG_S_2_C_CHEXUAN_NOTIFY_CHE_PAI = 62410
*/
struct CheXuanS2CNotifyChePai : public LMsgSC
{
	Lint		m_restTime;									//剩余超时时间
	Lint		m_userStatus[CHEXUAN_PLAYER_COUNT];			//玩家状态 1：参与本局游戏  0：没参与本局游戏
	Lint		m_chePaiStatus[CHEXUAN_PLAYER_COUNT];		//扯牌状态 1：已经扯牌  0：未扯牌
	Lint		m_qiPaiStatus[CHEXUAN_PLAYER_COUNT];		//弃牌状态 1：已弃牌
	Lint		m_qiaoStatus[CHEXUAN_PLAYER_COUNT];			//敲的状态 1：已经敲了

	CheXuanS2CNotifyChePai() : LMsgSC(MSG_S_2_C_CHEXUAN_NOTIFY_CHE_PAI)
	{
		m_restTime = -1;
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_chePaiStatus, 0, sizeof(m_chePaiStatus));
		memset(m_qiPaiStatus, 0, sizeof(m_qiPaiStatus));
		memset(m_qiaoStatus, 0, sizeof(m_qiaoStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRestTime", m_restTime);

		std::vector<Lint> veci;

		veci.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kChePaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_chePaiStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kQiPaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_qiPaiStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kQiaoStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_qiaoStatus[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CNotifyChePai();
	}
};

/*
  C->S 玩家扯牌操作
  MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 62411
*/
struct CheXuanC2SPlayerChePai : public LMsgSC
{
	Lint		m_pos;				//玩家位置
	Lint		m_handCard1;		//玩家排序好的第1张牌
	Lint		m_handCard2;		//玩家排序好的第2张牌
	Lint		m_handCard3;		//玩家排序好的第3张牌
	Lint		m_handCard4;		//玩家排序好的第4张牌

	CheXuanC2SPlayerChePai() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_handCard1 = 0;
		m_handCard2 = 0;
		m_handCard3 = 0;
		m_handCard4 = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kHandCard1", m_handCard1);
		ReadMapData(obj, "kHandCard2", m_handCard2);
		ReadMapData(obj, "kHandCard3", m_handCard3);
		ReadMapData(obj, "kHandCard4", m_handCard4);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerChePai();
	}
};

/*
  S->C 所有玩家扯牌结束，广播结果
  MSG_S_2_C_CHEXUAN_PLAYER_CHE_PAI_BC = 62413
*/
struct CheXuanS2CPlayerChePaiBC : public LMsgSC
{
	Lint m_pos;
	Lint m_handCards[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];  //每个玩家手牌
	Lint m_cardsType[CHEXUAN_PLAYER_COUNT][2];  //每个玩家手牌两对类型
	Lint m_totalPiScore;						//皮池中的总分
	Lint m_totalMangScore;						//芒果池中总分
	Lint m_totalAddScore;						//桌子上剩余的每个玩家下注分数总和
	Lint m_userStatus[CHEXUAN_PLAYER_COUNT];			//玩家是否参与游戏标志   0：未参与   1：参与本局
	Lint m_qiPaiStatus[CHEXUAN_PLAYER_COUNT];		//玩家弃牌状态 1：已经弃牌

	CheXuanS2CPlayerChePaiBC() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_CHE_PAI_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_totalPiScore = 0;
		m_totalMangScore = 0;
		m_totalAddScore = 0;
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_cardsType, 0, sizeof(m_cardsType));
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_qiPaiStatus, 0, sizeof(m_qiPaiStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 23);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kTotalPiScore", m_totalPiScore);
		WriteKeyValue(pack, "kTotalMangScore", m_totalMangScore);
		WriteKeyValue(pack, "kTotalAddScore", m_totalAddScore);

		std::vector<Lint> veci;

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < CHEXUAN_HAND_CARDS_COUNT; j++)
			{
				veci.push_back(m_handCards[i][j]);
			}
			WriteKey(pack, veci);
		}

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			veci.clear();

			sprintf(name, "kCardsType%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < 2; j++)
			{
				veci.push_back(m_cardsType[i][j]);
			}
			WriteKey(pack, veci);
		}

		veci.clear();
		WriteKey(pack, "kUserStatus");
		for (size_t i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_qiPaiStatus[i]);
		}
		WriteKeyValue(pack, "kQiPaiStatus", veci);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerChePaiBC();
	}
};

/*
  S->C 小结算消息
  MSG_S_2_C_CHEXUAN_DRAW_RESULT_BC = 62414
*/
struct CheXuanS2CDrawResult :public LMsgSC
{
	Lint			m_nextRoundTime;							//下一局玩家自动准备时间
	Lint			m_firstSendCardsPos;						//发牌人位置
	Lint			m_dangPlayerPos;							//挡家位置
	Lint			m_type;										//0:小结.1:总结
	Lint			m_score[CHEXUAN_PLAYER_COUNT];				//玩家积分
	Lint			m_state;									//状态 0：正常结束，1:休芒，2:揍芒
	Lint			m_isFinish;									//标记match是否结束 0：未结束  1：结束
	Lint			m_curCircle;								//当前局数
	Lint			m_curMaxCircle;								//总局数
	Lint			m_piPool;									//皮池
	Lint			m_mangPool;									//芒果池
	Lint			m_lastGetYuMang;							//留座观战玩家最后获得余数芒果的位置
	Lint			m_lossOrWin[CHEXUAN_PLAYER_COUNT];			//输赢标记（除去失去的芒果分后，有赢的即为赢1，有输的即为输-1，平的再来2，默认0）
	Lint			m_userStatus[CHEXUAN_PLAYER_COUNT];			//玩家是否参与游戏标志   0：未参与   1：参与本局
	Lint			m_qiPaiStatus[CHEXUAN_PLAYER_COUNT];		//玩家弃牌状态 1：已经弃牌
	Lint			m_standUpStatus[CHEXUAN_PLAYER_COUNT];		//玩家是否要起身离座标志  1：起身离座  0：不需要
	Lint			m_cardsType[CHEXUAN_PLAYER_COUNT][2];		//玩家头尾牌型
	Lint			m_handCards[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];  //玩家手牌
	Lint			m_handCardsCount[CHEXUAN_PLAYER_COUNT];		//玩家手牌张数

	Lint			m_playerPiScore[CHEXUAN_PLAYER_COUNT];		//玩家输赢皮分
	Lint			m_playerMangScore[CHEXUAN_PLAYER_COUNT];	//玩家输赢芒果分
	Lint			m_playerAddScore[CHEXUAN_PLAYER_COUNT];		//玩家总下分
	
	Lint			m_curBoBoScore[CHEXUAN_PLAYER_COUNT];		//玩家现有的钵钵数
	Lint			m_totleScore[CHEXUAN_PLAYER_COUNT];		//玩家总得分
	Lint			m_userIds[CHEXUAN_PLAYER_COUNT];		//游戏ID
	Lstring			m_userNames[CHEXUAN_PLAYER_COUNT];		//游戏玩家昵称
	Lstring			m_headUrl[CHEXUAN_PLAYER_COUNT];		//游戏玩家头像

	CheXuanS2CDrawResult() :LMsgSC(MSG_S_2_C_CHEXUAN_DRAW_RESULT_BC)
	{
		m_nextRoundTime = 0;
		m_firstSendCardsPos = CHEXUAN_INVALID_POS;
		m_dangPlayerPos = CHEXUAN_INVALID_POS;
		m_type = 0;
		m_state = 0;
		m_isFinish = 0;
		m_curCircle = 0;
		m_curMaxCircle = 0;
		m_piPool = 0;
		m_mangPool = 0;
		m_lastGetYuMang = CHEXUAN_INVALID_POS;
		memset(m_lossOrWin, 0, sizeof(m_lossOrWin));
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_qiPaiStatus, 0, sizeof(m_qiPaiStatus));
		memset(m_standUpStatus, 0, sizeof(m_standUpStatus));
		memset(m_cardsType, 0, sizeof(m_cardsType));
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_handCardsCount, 0, sizeof(m_handCardsCount));
		memset(m_playerPiScore, 0, sizeof(m_playerPiScore));
		memset(m_playerMangScore, 0, sizeof(m_playerMangScore));
		memset(m_playerAddScore, 0, sizeof(m_playerAddScore));
		memset(m_score, 0, sizeof(m_score));
		memset(m_curBoBoScore, 0, sizeof(m_curBoBoScore));
		memset(m_totleScore, 0, sizeof(m_totleScore));
		memset(m_userIds, 0, sizeof(m_userIds));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 26 + 8 + 8);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kNextRoundTime", m_nextRoundTime)
		WriteKeyValue(pack, "kFirstSendCardsPos", m_firstSendCardsPos);
		WriteKeyValue(pack, "kDangPlayerPos", m_dangPlayerPos)
		WriteKeyValue(pack, "kType", m_type);
		WriteKeyValue(pack, "kIsFinish", m_isFinish);
		WriteKeyValue(pack, "kState", m_state);
		WriteKeyValue(pack, "kCurCircle", m_curCircle);
		WriteKeyValue(pack, "kCurMaxCircle", m_curMaxCircle);
		WriteKeyValue(pack, "kPiPool", m_piPool);
		WriteKeyValue(pack, "kMangPool", m_mangPool);
		WriteKeyValue(pack, "kLastGetYuMang", m_lastGetYuMang);

		std::vector<Lint> vect;
		std::vector<Lstring> vecs;

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kCardsType%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < 2; j++)
			{
				vect.push_back(m_cardsType[i][j]);
			}
			WriteKey(pack, vect);
		}

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();

			sprintf(name, "kHandCards%d", i);
			WriteKey(pack, name);
			for (Lint j = 0; j < CHEXUAN_HAND_CARDS_COUNT; j++)
			{
				vect.push_back(m_handCards[i][j]);
			}
			WriteKey(pack, vect);
		}

		vect.clear();
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_lossOrWin[i]);
		}
		WriteKeyValue(pack, "kLossOrWin", vect);

		vect.clear();
		WriteKey(pack, "kHandCardsCount");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardsCount[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPlayerPiScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerPiScore[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPlayerMangScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerMangScore[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPlayerAddScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerAddScore[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userStatus[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kQiPaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_qiPaiStatus[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kStandUpStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_standUpStatus[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_score[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kCurBoBoScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_curBoBoScore[i]);
		}
		WriteKey(pack, vect);

		//显示总分
		WriteKey(pack, "kTotleScore");
		vect.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_totleScore[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kUserIds");
		vect.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userIds[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kNikes");
		vecs.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userNames[i]);
		}
		WriteKey(pack, vect);

		WriteKey(pack, "kHeadUrls");
		vecs.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_headUrl[i]);
		}
		WriteKey(pack, vecs);

		return true;
	}

	virtual LMsg* Clone() { return new WuRenBaiFenS2CDrawResult(); }
};

/*
  S->C 服务器发送断线重连
  MSG_S_2_C_CHEXUAN_RECON = 62415
*/
struct CheXuanS2CReConnect : public LMsgSC
{
	Lint		m_pos;										//玩家位置
	Lint		m_restTime;									//倒计时剩余时间
	Lint		m_dangPos;									//挡家位置
	Lint		m_curPos;									//当前操作玩家位置
	Lint		m_curStatus;								//当前阶段：1：下钵钵...
	Lint		m_turnCount;								//发牌轮数：0：第1轮发2张暗牌，1：第2轮发1张明牌，2：第3轮发牌1张暗牌
	Lint		m_firstSendCardPos;							//第一个发牌的位置
	Lint		m_turnFirstSpeakPos;						//每轮下注第一个说话人位置
	Lint		m_selectMoFen;								//玩家选择的默分操作

	Lint		m_playerLastOpt[CHEXUAN_PLAYER_COUNT];		//玩家最后一次操作
	Lint		m_userStatus[CHEXUAN_PLAYER_COUNT];			//玩家状态 1：参与本局游戏  0：没参与本局游戏
	Lint		m_userMustQiBoBo[CHEXUAN_PLAYER_COUNT];		//起钵钵状态 1：需要起钵钵
	Lint		m_qiBoBoStatus[CHEXUAN_PLAYER_COUNT];		//是否起钵钵了  0：未操作  1:已经操作
	Lint		m_qiPaiStatus[CHEXUAN_PLAYER_COUNT];		//弃牌状态 1：已弃牌
	Lint		m_xiuStatus[CHEXUAN_PLAYER_COUNT];			//一轮休状态 1：休
	Lint		m_qiaoStatus[CHEXUAN_PLAYER_COUNT];			//玩家敲的状态  0：没有敲  1：敲了
	Lint		m_cuoPaiStatus[CHEXUAN_PLAYER_COUNT];		//玩家搓牌状态  0：没有搓牌   1：已经搓牌了
	Lint		m_chePaiStatus[CHEXUAN_PLAYER_COUNT];		//扯牌状态： 1：已扯牌  0：未扯牌

	Lint		m_piPool;									//皮池
	Lint		m_mangPool;									//芒果池
	Lint		m_lastBoScore[CHEXUAN_PLAYER_COUNT];		//最后一次下钵钵分数
	Lint		m_playerTotalBoScore[CHEXUAN_PLAYER_COUNT];		//各个玩家钵钵里当前分数
	Lint		m_playerTotalAddScore[CHEXUAN_PLAYER_COUNT];	//各个玩家下注的总分数
	Lint		m_playerTotalAddMang[CHEXUAN_PLAYER_COUNT];		//各个玩家下的芒果数
	Lint		m_turnOptType[CHEXUAN_PLAYER_COUNT];			//当前轮数各个玩家已经操作的类型
	Lint		m_turnOptScore[CHEXUAN_PLAYER_COUNT];			//当前轮数各个玩家操作类型对应的分数

	Lint		m_handCardsCount[CHEXUAN_PLAYER_COUNT];			//玩家手牌数量
	Lint		m_handCards[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];	//玩家手牌
	Lint		m_cardsType[CHEXUAN_PLAYER_COUNT][2];			//玩家手牌两组类型

	Lint		m_isCoinsLow;									//能量场玩家能量值是否低于预警值 0：能量值正常  1：能量值低于预警
	Lint		m_isStandShowSeat;								//能量场玩家是否能量值足够了显示入座按钮 0：不显示   1：加能量值后能量值足够入座，显示入座按钮
	Lint		m_playerAutoOpt[CHEXUAN_PLAYER_COUNT];			//玩家选择自动操作选项  0：未选择   1：自动弃牌   2：自动弃牌休牌   3：自动瞧
	Lint		m_playerStandUp[CHEXUAN_PLAYER_COUNT];		//玩家站起操作  0：没有点击站起   1：当局点击站起(当局还未站起)    2：已经站起状态

	CheXuanS2CReConnect() : LMsgSC(MSG_S_2_C_CHEXUAN_RECON)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_restTime = -1;
		m_curPos = CHEXUAN_INVALID_POS;
		m_dangPos = CHEXUAN_INVALID_POS;
		m_curStatus = 0;
		m_turnCount = -1;
		m_firstSendCardPos = CHEXUAN_INVALID_POS;
		m_turnFirstSpeakPos = CHEXUAN_INVALID_POS;
		m_selectMoFen = -1;

		memset(m_lastBoScore, 0, sizeof(m_lastBoScore));
		memset(m_userMustQiBoBo, 0, sizeof(m_userMustQiBoBo));
		memset(m_qiBoBoStatus, 0, sizeof(m_qiBoBoStatus));
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_qiPaiStatus, 0, sizeof(m_qiPaiStatus));
		memset(m_xiuStatus, 0, sizeof(m_xiuStatus));
		memset(m_qiaoStatus, 0, sizeof(m_qiaoStatus));
		memset(m_cuoPaiStatus, 0, sizeof(m_cuoPaiStatus));
		memset(m_chePaiStatus, 0, sizeof(m_chePaiStatus));

		m_piPool = 0;
		m_mangPool = 0;
		memset(m_playerTotalBoScore, 0, sizeof(m_playerTotalBoScore));
		memset(m_playerTotalAddScore, 0, sizeof(m_playerTotalAddScore));
		memset(m_playerTotalAddMang, 0, sizeof(m_playerTotalAddMang));
		memset(m_playerAutoOpt, 0, sizeof(m_playerAutoOpt));
		memset(m_playerStandUp, 0, sizeof(m_playerStandUp));
		
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			m_turnOptType[i] = -1;
			m_playerLastOpt[i] = -1;
		}
		memset(m_turnOptScore, 0, sizeof(m_turnOptScore));

		memset(m_handCardsCount, 0, sizeof(m_handCardsCount));
		memset(m_handCards, 0, sizeof(m_handCards));
		memset(m_cardsType, 0, sizeof(m_cardsType));
		m_isCoinsLow = 0;
		m_isStandShowSeat = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 32 + 8 + 8);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "RestTime", m_restTime);
		WriteKeyValue(pack, "kCurPos", m_curPos);
		WriteKeyValue(pack, "kDangPos", m_dangPos);
		WriteKeyValue(pack, "kCurStatus", m_curStatus);
		WriteKeyValue(pack, "kTurnCount", m_turnCount);
		WriteKeyValue(pack, "kFirstSendCardPos", m_firstSendCardPos);
		WriteKeyValue(pack, "kTurnFirstSpeakPos", m_turnFirstSpeakPos);

		WriteKeyValue(pack, "kPiPool", m_piPool);
		WriteKeyValue(pack, "kMangPool", m_mangPool);
		WriteKeyValue(pack, "kSelectMoFen", m_selectMoFen)

		std::vector<Lint> vect;

		//
		vect.clear();
		WriteKey(pack, "kLastBoScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_lastBoScore[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kUserMustQiBoBo");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userMustQiBoBo[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kQiBoBoStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_qiBoBoStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerLastOpt[i]);
		}
		WriteKeyValue(pack, "kPlayerLastOpt", vect);
		//
		vect.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_userStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kQiPaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_qiPaiStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kXiuStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_xiuStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kQiaoStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_qiaoStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kCuoPaiStatus");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_cuoPaiStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kChePaiStatus");
		for (int i = 0; i < 12; ++i)
		{
			vect.push_back(m_chePaiStatus[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kPlayerTotalBoScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerTotalBoScore[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kPlayerTotalAddScore");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerTotalAddScore[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kPlayerTotalAddMang");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerTotalAddMang[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kTurnOptType");
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_turnOptType[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kTurnOptScore");
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_turnOptScore[i]);
		}
		WriteKey(pack, vect);
		//
		vect.clear();
		WriteKey(pack, "kHandCardsCount");
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_handCardsCount[i]);
		}
		WriteKey(pack, vect);

		WriteKeyValue(pack, "kIsCoinsLow", m_isCoinsLow);
		WriteKeyValue(pack, "kIsStandShowSeat", m_isStandShowSeat);

		vect.clear();
		WriteKey(pack, "kPlayerAutoOpt");
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerAutoOpt[i]);
		}
		WriteKey(pack, vect);

		vect.clear();
		WriteKey(pack, "kPlayerStandUp");
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vect.push_back(m_playerStandUp[i]);
		}
		WriteKey(pack, vect);

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();
			sprintf(name, "kHandCards%d", i);

			WriteKey(pack, name);
			for (Lint j = 0; j < CHEXUAN_HAND_CARDS_COUNT; j++)
			{
				vect.push_back(m_handCards[i][j]);
			}
			WriteKey(pack, vect);
		}

		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			char name[32];
			vect.clear();
			sprintf(name, "kCardsType%d", i);

			WriteKey(pack, name);
			for (Lint j = 0; j < 2; j++)
			{
				vect.push_back(m_cardsType[i][j]);
			}
			WriteKey(pack, vect);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CReConnect();
	}
};

/* 
  S->C 服务器通知客户端开始搓牌
  MSG_S_2_C_CHEXUAN_NOTIFY_CUO_PAI = 62416
*/
struct CheXuanS2CNotifyCuoPai : public LMsgSC
{
	Lint		m_restTime;									//剩余超时时间
	Lint		m_userStatus[CHEXUAN_PLAYER_COUNT];			//玩家状态 1：参与本局游戏  0：没参与本局游戏
	Lint        m_cuoPaiStatus[CHEXUAN_PLAYER_COUNT];		//玩家搓牌状态   0：没有搓牌   1：已经搓牌
	Lint		m_qiPaiStatus[CHEXUAN_PLAYER_COUNT];		//弃牌状态 1：已弃牌
	Lint		m_qiaoStatus[CHEXUAN_PLAYER_COUNT];			//敲的状态 1：已经敲了

	CheXuanS2CNotifyCuoPai() : LMsgSC(MSG_S_2_C_CHEXUAN_NOTIFY_CUO_PAI)
	{
		m_restTime = -1;
		memset(m_userStatus, 0, sizeof(m_userStatus));
		memset(m_cuoPaiStatus, 0, sizeof(m_cuoPaiStatus));
		memset(m_qiPaiStatus, 0, sizeof(m_qiPaiStatus));
		memset(m_qiaoStatus, 0, sizeof(m_qiaoStatus));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kRestTime", m_restTime);
		
		std::vector<Lint> veci;

		veci.clear();
		WriteKey(pack, "kUserStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kCuoPaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_cuoPaiStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kQiPaiStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_qiPaiStatus[i]);
		}
		WriteKey(pack, veci);

		veci.clear();
		WriteKey(pack, "kQiaoStatus");
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_qiaoStatus[i]);
		}
		WriteKey(pack, veci);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CNotifyCuoPai();
	}

};


/*
  C->S 玩家搓牌操作
  MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
*/
struct CheXuanC2SPlayerCuoPai : public LMsgSC
{
	Lint		m_pos;				//玩家位置

	CheXuanC2SPlayerCuoPai() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI)
	{
		m_pos = CHEXUAN_INVALID_POS;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerCuoPai();
	}
};


/*
  S->C 所有玩家搓牌结束，广播结果
  MSG_S_2_C_CHEXUAN_PLAYER_CUO_PAI_BC = 62418
*/
struct CheXuanS2CPlayerCuoPaiBC : public LMsgSC
{
	Lint m_pos;
	Lint m_cuoCard;  //玩家第4张牌

	CheXuanS2CPlayerCuoPaiBC() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_CUO_PAI_BC)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_cuoCard = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kCuoCard", m_cuoCard);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerCuoPaiBC();
	}
};

/*
  C->S 客户端牌局内请求战绩
  MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
*/
struct CheXuanC2SPlayerRequestRecord : public LMsgSC
{
	Lint		m_pos;				//玩家位置

	CheXuanC2SPlayerRequestRecord() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD)
	{
		m_pos = CHEXUAN_INVALID_POS;

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerRequestRecord();
	}
};

/*
  S->C 服务器返回牌局内战绩结果
  MSG_S_2_C_CHEXUAN_PLAYER_SEND_RECORD = 62421
*/
struct CheXuanS2CPlayerSendRecord : public LMsgSC
{
	Lint m_userId[CHEXUAN_PLAYER_COUNT];			//玩家ID
	Lstring m_userHead[CHEXUAN_PLAYER_COUNT];		//玩家头像
	Lstring m_userNike[CHEXUAN_PLAYER_COUNT];		//玩家昵称
	Lint m_nowScore[CHEXUAN_PLAYER_COUNT];			//玩家当前总分
	Lint m_inScore[CHEXUAN_PLAYER_COUNT];			//玩家进入房间时候总分
	Lint m_changeScore[CHEXUAN_PLAYER_COUNT];		//玩家输赢总分
	
	//观战玩家列表
	Lint m_lookOnCount;
	std::vector<Lint> m_lookOnId;
	std::vector<Lstring> m_lookOnHead;
	std::vector<Lstring> m_lookOnNike;

	CheXuanS2CPlayerSendRecord() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_SEND_RECORD)
	{
		memset(m_userId, 0, sizeof(m_userId));
		memset(m_nowScore, 0, sizeof(m_nowScore));
		memset(m_inScore, 0, sizeof(m_inScore));
		memset(m_changeScore, 0, sizeof(m_changeScore));

		m_lookOnCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		std::vector<Lint> veci;
		std::vector<Lstring> vecs;

		WriteMap(pack, 11);
		WriteKeyValue(pack, "kMId", m_msgId);

		//玩家ID
		veci.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_userId[i]);
		}
		WriteKeyValue(pack, "kUserId", veci);

		//玩家头像
		vecs.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userHead[i]);
		}
		WriteKeyValue(pack, "kUserHead", vecs);

		//玩家昵称
		vecs.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			vecs.push_back(m_userNike[i]);
		}
		WriteKeyValue(pack, "kUserNike", vecs);

		//玩家当前分数
		veci.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_nowScore[i]);
		}
		WriteKeyValue(pack, "kNowScore", veci);

		//玩家进入房间分数
		veci.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_inScore[i]);
		}
		WriteKeyValue(pack, "kInScore", veci);

		//玩家输赢分数
		veci.clear();
		for (Lint i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			veci.push_back(m_changeScore[i]);
		}
		WriteKeyValue(pack, "kChangeScore", veci);

		WriteKeyValue(pack, "kLookOnCount", m_lookOnCount);
		WriteKeyValue(pack, "kLookOnId", m_lookOnId);
		WriteKeyValue(pack, "kLookOnHead", m_lookOnHead);
		WriteKeyValue(pack, "kLookOnNike", m_lookOnNike);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerSendRecord();
	}
};

/*
  C->S 客户端操作默分
  MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
*/
struct CheXuanC2SPlayerOptMoFen : public LMsgSC
{
	Lint m_pos;
	Lint m_optMo;

	CheXuanC2SPlayerOptMoFen() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_optMo = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kSelectMoFen", m_optMo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerOptMoFen();
	}
};

/*
  S->C 服务端返回客户端操作默分
  MSG_S_2_C_CHEXUAN_PLAYER_OPT_MO_FEN = 62423
*/
struct CheXuanS2CPlayerOptMoFen : public LMsgSC
{
	Lint m_pos;
	Lint m_optMo;

	CheXuanS2CPlayerOptMoFen() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_OPT_MO_FEN)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_optMo = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kSelectMoFen", m_optMo);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerOptMoFen();
	}
};

/*
  C->S 客户端选择自动操作
  MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
*/
struct CheXuanC2SPlayerSelectAutoOpt : public LMsgSC
{
	Lint m_pos;
	Lint m_autoOptType;		//玩家选择自动操作选项  0：未选择   1：自动弃牌   2：自动弃牌休牌   3：自动瞧

	CheXuanC2SPlayerSelectAutoOpt() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_autoOptType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kAutoOptType", m_autoOptType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerSelectAutoOpt();
	}
};

/*
  S->C 服务器返回玩家选择的自动操作结果
  MSG_S_2_C_CHEXUAN_PLAYER_SELECT_AUTO_OPT_RET = 62425
*/
struct CheXuanS2CPlayerSelectAutoOptRet : public LMsgSC
{
	Lint m_pos;
	Lint m_autoOptType;		//玩家选择自动操作选项  0：未选择   1：自动弃牌   2：自动弃牌休牌   3：自动瞧

	CheXuanS2CPlayerSelectAutoOptRet() : LMsgSC(MSG_S_2_C_CHEXUAN_PLAYER_SELECT_AUTO_OPT_RET)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_autoOptType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kAutoOptType", m_autoOptType);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerSelectAutoOptRet();
	}
};

/*
  C->S 客户端选择站起或坐下
  MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
*/
struct CheXuanC2SPlayerSelectStandOrSeat : public LMsgSC
{
	Lint m_pos;
	Lint m_standOrSeat;		//玩家站起操作  0：坐下   1：站起

	CheXuanC2SPlayerSelectStandOrSeat() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_standOrSeat = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "kPos", m_pos);
		ReadMapData(obj, "kStandOrSeat", m_standOrSeat);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanC2SPlayerSelectStandOrSeat();
	}
};

/*
  S->C 服务器返回玩家站起或坐下结果
  MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT_RET = 62428
*/
struct CheXuanS2CPlayerSelectStandOrSeatRet : public LMsgSC
{
	Lint m_pos;
	Lint m_standOrSeat;		//玩家站起操作  0：坐下   1：站起

	CheXuanS2CPlayerSelectStandOrSeatRet() : LMsgSC(MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT_RET)
	{
		m_pos = CHEXUAN_INVALID_POS;
		m_standOrSeat = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "kMId", m_msgId);
		WriteKeyValue(pack, "kPos", m_pos);
		WriteKeyValue(pack, "kStandOrSeat", m_standOrSeat);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new CheXuanS2CPlayerSelectStandOrSeatRet();
	}
};

#endif _LMSG_POKER_SC_H_