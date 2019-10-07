#pragma once

#ifndef _NEW_NIU_NIU_MSG_H_
#define _NEW_NIU_NIU_MSG_H_

#include "LTool.h"
#include "LMsgBase.h"
#include "..\mhgamedef.h"

//////////////////////////////////////////////////////////
// 新牛牛 消息号   
/////////////////////////////////////////////////////////
enum NEWNIUNIU_MSG_ID
{
	/////////////////////////////
	//牛牛  62071-62100
	MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071,          //选庄
	MSG_S_2_C_NIUNIU_SELECT_ZHUANG = 62072,

	MSG_C_2_S_NIUNIU_ADD_SCORE = 62073,           //下注
	MSG_S_2_C_NIUNIU_ADD_SCORE = 62074,

	MSG_C_2_S_NIUNIU_OPEN_CARD = 62075,           //亮牌
	MSG_S_2_C_NIUNIU_OPEN_CARD = 62076,

	MSG_S_2_C_NIUNIU_START_GAME = 62077,         //游戏开始

	MSG_S_2_C_NIUNIU_DRAW_RESULT = 62078,        //小结算

	MSG_S_2_C_NIUNIU_RECON = 62079,              //断线重连
	MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG = 62080,              //抢庄通知

	MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE = 62081,  //明牌下注

	MSG_C_2_S_NIUNIU_MAI_MA = 62082,			//玩家买码
	MSG_S_2_C_NIUNIU_MAI_MA_BC = 62083,			//服务器广播玩家买码

	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084,			//玩家牛牛托管
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085,			//玩家取消牛牛托管
	MSG_S_2_C_NIUNIU_TUO_GUAN_BC = 62086,		//牛牛托管广播
	MSG_S_2_C_NIUNIU_NOTIFY_CUO_GONG_PAI = 62087,		//通知玩家搓公牌(暂时不用)
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088,				//玩家搓公牌
	MSG_S_2_C_NIUNIU_CUO_GONG_PAI_BC = 62089,			//广播玩家搓公牌

	MSG_S_2_C_UPDATE_CONINS = 61199,			//服务器更新客户端玩家金币数量
	MSG_S_2_C_LOOKON_PLAYER_FULL = 62065,   //服务器广播观战玩家，参与游戏玩家人数已满
	MSG_C_2_S_ADD_ROOM_SEAT_DOWN = 62066,    //客户端发送观战状态下入座
	MSG_S_2_C_ADD_ROOM_SEAT_DOWN = 62067,    	//服务器返回观战玩家入座结果
	MSG_C_2_S_VIEW_LOOKON_LIST = 62068,			//客户端请求查看观战玩家列表
	MSG_S_2_C_RECT_LOOKON_LIST = 62069,			//服务器返回观战玩家列表
	MSG_S_2_C_SHOW_LOOKON_COUNT = 62070,		//服务器返回观战玩家数量
	MSG_C_2_S_START_TIME = 61198,				// 客户端通知服务器开始计时

};
















#endif
