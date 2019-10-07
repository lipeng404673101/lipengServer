#pragma once

#ifndef _CHE_XUAN_MSG_H_
#define _CHE_XUAN_MSG_H_

#include "LTool.h"
#include "LMsgBase.h"
#include "..\mhgamedef.h"


//////////////////////////////////////////////////////////
// 扯炫 消息号   
/////////////////////////////////////////////////////////
enum CHEXUAN_MSG_ID
{
	//////////////////////////////////////////////////////////////////////////
	//扯炫消息号规划 62400-62449

	//S->C 服务器通知玩家起钵钵
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_QI_BOBO = 62400,

	//C->S 玩家起钵钵
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401,

	//S->C 广播玩家起钵钵结果
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_BOBO_BC = 62402,

	//S->C 服务端通知玩家下芒果
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_ADD_MANGGUO = 62403,

	//C->S 玩家下芒果
	MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANGGUO = 62404,

	//S->C 广播玩家下芒果
	MSG_S_2_C_CHEXUAN_PLAYER_ADD_MANGGUO_BC = 62405,

	//S->C 服务器给玩家发牌
	MSG_S_2_C_CHEXUAN_SEND_CARD = 62406,

	//S->C 服务器通知玩家操作
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_OPT = 62407,

	//C->S 玩家选择操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 62408,

	//S->C 服务器广播玩家操作结果
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_OPT_BC = 62409,

	//S->C 服务器通知玩家开始扯牌
	MSG_S_2_C_CHEXUAN_NOTIFY_CHE_PAI = 62410,

	//C->S 玩家扯牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 62411,

	//S->C 所有玩家扯牌结束，广播结果
	MSG_S_2_C_CHEXUAN_PLAYER_CHE_PAI_BC = 62413,

	//S->C 小结算消息
	MSG_S_2_C_CHEXUAN_DRAW_RESULT_BC = 62414,

	//S->C 服务器发送断线重连
	MSG_S_2_C_CHEXUAN_RECON = 62415,

	//S->C 服务器通知客户端开始搓牌
	MSG_S_2_C_CHEXUAN_NOTIFY_CUO_PAI = 62416,

	//C->S 玩家扯牌操作
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417,

	//S->C 所有玩家扯牌结束，广播结果
	MSG_S_2_C_CHEXUAN_PLAYER_CUO_PAI_BC = 62418,

	//C->S 客户端下芒注结果
	MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANG_SCORE_BC = 62419,

	//C->S 客户端牌局内请求战绩
	MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420,

	//S->C 服务器返回牌局内战绩结果
	MSG_S_2_C_CHEXUAN_PLAYER_SEND_RECORD = 62421,

	//C->S 客户端操作默分
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422,

	//S->C 服务端返回客户端操作默分
	MSG_S_2_C_CHEXUAN_PLAYER_OPT_MO_FEN = 62423,

	//C->S 客户端选择自动操作
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424,

	//S->C 服务器返回玩家选择的自动操作结果
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_AUTO_OPT_RET = 62425,

	//C->S 客户端选择站起或坐下
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427,

	//S->C 服务器返回玩家站起或坐下结果
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT_RET = 62428

};







#endif
