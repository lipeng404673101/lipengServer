#pragma once
#ifndef _MH_GAME_DEF_H_
#define _MH_GAME_DEF_H_

// 棋牌类子游戏类型
typedef enum
{
	MH_GAME_TYPE_MIN = 100,
	MH_GAME_TYPE_DouDiZhu = 101,
	MH_GAME_TYPE_ZhaJinHua = 102,
	MH_GAME_TYPE_NiuNiu = 103,
	MH_GAME_TYPE_TuiTongZi = 104,
	MH_GAME_TYPE_ShuangSheng = 106,
	MH_GAME_TYPE_SanDaEr = 107,
	MH_GAME_TYPE_NiuNiuNew = 108,
	MH_GAME_TYPE_SanDaYi = 109,
	MH_GAME_TYPE_WuRenBaiFen = 110,
	MH_GAME_TYPE_CheXuan = 111,
	MH_GAME_TYPE_MAX = 120
} QiPaiGameType;

// 玩家在游戏中的状态
typedef enum
{
   STATE_SITDOWN = 0,     //坐下未准备
   STATE_READY = 1,       //处于准备状态
   GAME = 2 //游戏中
} UserGameState;

//桌子当前状态
typedef enum
{
	NEW_DESK_FREE = 0,	//桌子空闲状态
	NEW_DESK_WAIT,		//桌子等待状态
	NEW_DESK_PLAY,		//正在游戏中
					//	DESK_COUNT_RESULT,	//正在结算中
					//	DESK_RESET_ROOM		//正在申请解散房间中
}MH_DESK_STATE;

//游戏开始类型
typedef enum
{
	ALL_READY = 0, //满人开局
	ATLEAST_TWO_REDAY, //2人准备开局
} GameBeginType;

#define MAX_CHAIR_COUNT  20  //桌子上最大玩家数量限制
const int desk_max_user_number[] = {3, 5}; //玩家数量和游戏类型必须对应
const int game_begin_type[] = { ALL_READY, ALL_READY };

#define GetMaxPlayerCountByType(game_type)  desk_max_user_number[game_type - MH_GAME_TYPE_MIN];
#define GetGameBeginType(game_type) game_begin_type[game_type - MH_GAME_TYPE_MIN];

//////////////////////////////////////////////////////////////////////////
//赢三张
#define YINGSANZHANG_PLAY_USER_MAX       8
#define YINGSANZHANG_HAND_CARD_MAX     3
//#define YINGSANZHANG_INVAILD_POS_QIPAI               5


//////////////////////////////////////////////////////////////////////////
//推筒子
#define TUITONGZI_PLAY_USER_COUNT 8
#define TUITONGZI_HAND_CARD_MAX 2
#define TUITONGZI_INVAILD_POS 8
#define TUITONGZI_JINBEI_4_USER  4
/* 押 道 软 红*/
#define TUITONGZI_XIAZHU_XIANG 4

//////////////////////////////////////////////////////////////////////////
//牛牛
#define NIUNIU_PLAY_USER_COUNT 10
#define NIUNIU_HAND_CARD_MAX 5
#define NIUNIU_INVAILD_POS 10
#define NIUNIU_MAX_POKER_COUNT 54
#define NIUNIU_NORMAL_POKER_COUNT 52
#define NIUNIU_LAIZI_POKER_COUNT 54


//////////////////////////////////////////////////////////////////////////
//斗地主
#define DOUDIZHU_PLAY_USER_COUNT 3
#define DOUDIZHU_INVAILD_POS  3
enum DOUDIZHU_PALY_TYPE_CARD
{
	//牌总数
	PLAY_TYPE_CARD_MAX = 55,
	PLAY_TYPE_NORMAL_CARD_TOTAL = 54,
	PLAY_TYPE_LAIZI_CARD_TOTAL = 55,
	PLAY_TYPE_LINFEN_CARD_TOTAL = 52,

	//每个人基础手牌
	PLAY_TYPE_CARD_HAND_MAX = 17,
	PLAY_TYPE_CARD_HAND_COUNT = 17,
	PLAY_TYPE_CARD_HAND_COUNT_LINFEN = 16,

	//底牌
	PLAY_TYPE_CARD_REST_MAX = 4,
	PLAY_TYPE_CARD_REST_COUNT = 3,
	PLAY_TYPE_CARD_REST_COUNT_4 = 4,

};

////////////////////////////////////////////////////////////////////////////
//双升
#define SHUANGSHENG_INVALID_CARD 0
#define SHUANGSHENG_PLAY_USER_COUNT 4
#define SHUANGSHENG_INVALID_POS 4
#define SHUANGSHENG_HAND_CARDS_COUNT 25
#define SHUANGSHENG_BASE_CARDS_COUNT 8
#define SHUANGSHENG_MAX_HAND_CARDS_COUNT 33
#define SHUANGSHENG_CELL_PACK 54
#define SHUANGSHENG_PACK_COUNT 2
#define SHUANGSHENG_MAX_COUNT  SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT / SHUANGSHENG_PLAY_USER_COUNT + SHUANGSHENG_BASE_CARDS_COUNT


//////////////////////////////////////////////////////////////////////////////
//三打二
#define SANDAER_PACK_COUNT 1
#define SANDAER_CELL_PACK 54
#define SANDAER_INVALID_CARD 0
#define SANDAER_HAND_CARDS_COUNT_MAX 14
#define SANDAER_HAND_CARDS_COUNT 10
#define SANDAER_BASE_CARDS_COUNT 4
#define SANDAER_PLAYER_COUNT 5
#define SANDAER_INVALID_POS 5
#define SANDAER_SEND_CARD_COUNT (SANDAER_CELL_PACK * SANDAER_PACK_COUNT - SANDAER_BASE_CARDS_COUNT) / SANDAER_PLAYER_COUNT
#define SANDAER_WIN_SCORE_MAX 105


//////////////////////////////////////////////////////////////////////////////
//三打一
#define SANDAYI_PACK_COUNT 1
#define SANDAYI_CELL_PACK 54
#define SANDAYI_INVALID_CARD 0
#define SANDAYI_HAND_CARDS_COUNT_MAX 18
#define SANDAYI_HAND_CARDS_COUNT 12
#define SANDAYI_BASE_CARDS_COUNT 6
#define SANDAYI_PLAYER_COUNT 4
#define SANDAYI_INVALID_POS 4
#define SANDAYI_SEND_CARD_COUNT (SANDAYI_CELL_PACK * SANDAYI_PACK_COUNT - SANDAYI_BASE_CARDS_COUNT) / SANDAYI_PLAYER_COUNT
#define SANDAYI_WIN_SCORE_MAX 105

//五人百分
#define WURENBAIFEN_PACK_COUNT 1
#define WURENBAIFEN_CELL_PACK 54
#define WURENBAIFEN_INVALID_CARD 0
#define WURENBAIFEN_HAND_CARDS_COUNT_MAX 14
#define WURENBAIFEN_HAND_CARDS_COUNT 10
#define WURENBAIFEN_BASE_CARDS_COUNT 4
#define WURENBAIFEN_PLAYER_COUNT 5
#define WURENBAIFEN_INVALID_POS 5
#define WURENBAIFEN_SEND_CARD_COUNT (WURENBAIFEN_CELL_PACK * WURENBAIFEN_PACK_COUNT - WURENBAIFEN_BASE_CARDS_COUNT) / WURENBAIFEN_PLAYER_COUNT
#define WURENBAIFEN_WIN_SCORE_MAX 105

//扯炫
#define CHEXUAN_PACK_COUNT 1
#define CHEXUAN_CELL_PACK 32
#define CHEXUAN_INVALID_CARD 0
#define CHEXUAN_HAND_CARDS_COUNT 4
#define CHEXUAN_PLAYER_COUNT 8
#define CHEXUAN_INVALID_POS 8



#endif
