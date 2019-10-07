
#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include <minwindef.h>
#include "../mhgamedef.h"

#pragma pack(1)


//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//逻辑类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_TAKE_ONE			7									//三带一单
#define CT_THREE_TAKE_TWO			8									//三带一对
#define CT_FOUR_TAKE_ONE			9									//四带两单
#define CT_FOUR_TAKE_TWO			10									//四带两对
#define CT_BOMB_SOFT               11
#define CT_BOMB_CARD				12									//炸弹类型
#define CT_MISSILE_CARD				13									//火箭类型

//////////////////////////////////////////////////////////////////////////////////


//空闲状态
struct CMD_S_StatusFree
{
	BYTE                            cbPlayStatus[DOUDIZHU_PLAY_USER_COUNT];          //用户状态
	//玩法类型
	BYTE                      cbPlayType;                         //斗地主玩法

	//游戏属性
	LONG							lCellScore;							//基础积分

	//时间信息
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeCallScore;					//叫分时间
	BYTE							cbTimeStartGame;					//开始时间
	BYTE							cbTimeHeadOutCard;					//首出时间

};

//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_CALL_SCORE			101									//用户叫分
#define SUB_S_BANKER_INFO			102									//庄家信息
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//用户放弃
#define SUB_S_GAME_CONCLUDE			105									//游戏结束
#define SUB_S_SET_BASESCORE			106									//设置基数
#define SUB_S_CHEAT_CARD			107									//作弊扑克
#define SUB_S_TRUSTEE				108									//托管

//发送扑克
struct CMD_S_GameStart
{
	WORD							wStartUser;							    //开始玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbValidCardData;					  //明牌扑克
	BYTE							cbValidCardIndex;					  //明牌位置
	BYTE                           cbCardCount;                             // 发牌数量
	BYTE							cbCardData[PLAY_TYPE_CARD_HAND_MAX];			//扑克列表
	//int                              cbScore[DOUDIZHU_PLAY_USER_COUNT];            //游戏积分
	BYTE                         cbOnlyCall;                             //当前玩家只能叫分
};


//用户叫分
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCallScoreUser;						//叫分玩家
	BYTE							cbCurrentScore;						//当前叫分
	BYTE							cbUserCallScore;					//上次叫分
	BYTE                         cbOnlyCall;                             //当前玩家只能叫分
};

//庄家信息
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//庄家玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbBankerScore;						//庄家叫分
	BYTE                           cbBankerCardCount;            //庄家底牌数量
	//BYTE							cbBankerCard[3];					//庄家扑克
	BYTE							cbBankerCard[4];					//庄家扑克， 这里改为4
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	//BYTE							cbCardData[MAX_COUNT];				//扑克列表
	BYTE							cbCardData[21];    //扑克列表,这里改为21
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							  cbTurnOver;							//一轮结束
	WORD				 			wCurrentUser;						//当前玩家
	WORD				 			wPassCardUser;						//放弃玩家
};

//游戏结束
struct CMD_S_GameConclude
{
	BYTE                          cbIsFinalDraw;              //是否是最后一局，达到限制的局数
	BYTE                          cbBankerUser;             // 地主位置
	BYTE                               cbBankerCall;              //庄家叫分
	//积分变量
	LONG							lCellScore;							//单元积分
	int							lGameScore[DOUDIZHU_PLAY_USER_COUNT];			//游戏积分

	//春天标志
	BYTE							bChunTian;							//春天标志
	BYTE							bFanChunTian;						//春天标志

	//炸弹信息
	BYTE							cbBombCount;						//炸弹个数
	BYTE							cbEachBombCount[DOUDIZHU_PLAY_USER_COUNT];		//炸弹个数

	//游戏信息
	BYTE							cbBankerScore[DOUDIZHU_PLAY_USER_COUNT];						//叫分数目,每个值都是庄家叫分的数值
	BYTE							cbCardCount[DOUDIZHU_PLAY_USER_COUNT];			               //扑克数目
	BYTE							cbHandCardData[DOUDIZHU_PLAY_USER_COUNT][PLAY_TYPE_CARD_HAND_MAX+ PLAY_TYPE_CARD_REST_MAX];		   //扑克列表
	BYTE                         cbActSelect[DOUDIZHU_PLAY_USER_COUNT];              //0-不提 ，1-踢  2跟 3回
	//int                              cbScore[DOUDIZHU_PLAY_USER_COUNT];            //游戏积分
	BYTE                         cbRestCardData[PLAY_TYPE_CARD_REST_MAX];     //底牌
};



//////////////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_CALL_SCORE			1									//用户叫分
#define SUB_C_OUT_CARD			2									//用户出牌
#define SUB_C_PASS_CARD			3									//用户放弃
#define SUB_C_TRUSTEE				4									//用户托管

//用户叫分
struct CMD_C_CallScore
{
	int						cbCallScore;						//   叫分数目
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						        //出牌数目
	BYTE							cbCardData[PLAY_TYPE_CARD_HAND_MAX+PLAY_TYPE_CARD_REST_COUNT_4];		//扑克数据
};


//////////////////////////////////////////////////////////////////////////////////




#pragma pack()

#endif