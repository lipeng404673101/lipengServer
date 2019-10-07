#ifndef CMD_GAME_LINFEN_HEAD_FILE
#define CMD_GAME_LINFEN_HEAD_FILE

#pragma once

#pragma pack(1)

#include <minwindef.h>
#include "../mhgamedef.h"



//////////////////////////////////////////////////////////////////////////////////
//命令定义


//临汾去两二斗地主  服务器端命令
//#define SUB_S_LINFEN_GAME_START                    120           //游戏开始,发牌
//#define SUB_S_LINFEN_CALL_SCORE                      121            //用户叫分
//#define SUB_S_LINFEN_BANKER_INFO                   122           //庄家信息
#define SUB_S_LINFEN_TIPAI                               123            //用户踢牌
#define SUB_S_LINFEN_GENTI                             124            //用户跟踢
#define SUB_S_LINFEN_HUIPAI                            125          //用户回牌
#define SUB_S_LINFEN_BANKER_OUT_CARD   126           //庄家开始选择出牌
//#define SUB_S_LINFEN_OUT_CARD                        127           //用户出牌
//#define SUB_S_LINFEN_PASS_CARD                        128           //用户放弃
//#define SUB_S_LINFEN_GAME_CONCLUDE            129           //游戏结束

#define SUB_S_LINFEN_SCORE_FRESH                  130        //刷新倍数
#define SUB_S_LINFEN_TIPAI_RESULT                         131      //用户踢牌结果
#define SUB_S_LINFEN_GENTI_RESULT                         132      //用户跟踢结果
#define SUB_S_LINFEN_HUIPAI_RESULT                         133      //用户回牌结果
#define SUB_S_LINFEN_USERS_TI_RESULT                     134      //所有用户的ti结果

struct CMD_S_BankerOutCard
{
	int m_Pos;                                       //庄家位置
};

struct CMD_S_NotifyUserTi
{
	int    m_pos;                     //踢用户位置
};

//所有用户的TI结果
struct CMD_S_UsersTiResult
{
	BYTE                             wUserStatue[DOUDIZHU_PLAY_USER_COUNT];                  //玩家是否选择  0-没有选择，1-踢牌，2-跟踢,3-回牌
};

//TI的结果
struct CMD_S_TiResult
{
	WORD							wActUser;							//动作玩家
	BYTE                              wActSelect;                     // 玩家是否选择
};

//刷新倍数
struct CMD_S_ScoreFresh
{
	LONG							score;						//是否选择
};


//用户踢牌
struct CMD_S_TiPai_Linfen
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCallScoreUser;						//踢牌玩家
	BYTE							cbCurrentScore;						//当前踢牌
	BYTE							cbUserCallScore;					//上家踢牌
};

//用户回踢
struct CMD_S_HuiTi_Linfen
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCallScoreUser;						//踢牌玩家
	BYTE							cbCurrentScore;						//当前踢牌
	BYTE							cbUserCallScore;					//上家踢牌
};


#define SUB_C_TIPAI                          5                                //用户踢牌
#define SUB_C_GENTI                        6                                 //用户跟踢
#define SUB_C_HUIPAI                      7                                 //用户会牌
//
////用户踢牌,跟踢，回牌
struct CMD_C_Ti
{
	BYTE							cbIsYes;						//是否选择
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif