#ifndef	_TUI_TONG_ZI_GAME_LOGIC_H_
#define _TUI_TONG_ZI_GAME_LOGIC_H_
#pragma once

#include "mhgamedef.h"
#include "Stdafx.h"
#include <cstdlib>
#include <time.h>

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

/* 上道 */
#define UP_DAO 8

/* 道的输赢结果（枚举）*/
enum DAO_RESULT
{
	DOWN_LOSE = 0,  //未上道输
	DOWN_WIN =  2,  //未上道赢
	UP_LOSE =   3,  //上道输
	UP_WIN =    4   //上道赢
};

/* 玩家每局手牌以及输赢的结果（结构体）*/
struct PlayerState
{
	BYTE        handCards[TUITONGZI_HAND_CARD_MAX];   //玩家手牌
	bool        ya_result;                            //玩家“押”上的输赢结果
	DAO_RESULT  dao_result;                           //玩家“道”上的输赢结果
	bool        ruan_result;                          //玩家“软”上的输赢结果
	bool        hong_result;                          //玩家“红”上的输赢结果

	/* 构造函数 */
	PlayerState();

	/* 析构函数 */
	~PlayerState();

	void clear();
};

/* 推筒子基础算法（类）*/
class TGameLogic
{
private:

	static const BYTE m_cbCardListData[36];  //推筒子使用1-9点，每个4张
	static const BYTE m_cbCardListDataBaiBan[40];
	BYTE m_randCardList[8];            //存储4个玩家的8张牌

public:
	TGameLogic()
	{}

	~TGameLogic()
	{}

public:
	/***********************************************
	* 函数名：   sordHandCards()
	* 描述：     排列手牌（按小牌在前）
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：     (void)
	************************************************/
	void sordHandCards(BYTE* handCards);

	/******************************************************************
	* 函数名：   compareCardByYa()
	* 描述：     闲家和庄家比较牌的点数（押）
	* 参数：
	*  @ zhuang_handCards 庄家手牌数组（两张牌）
	*  @ xian_handCards 闲家手牌数组（两张牌）
	* 返回：     (int) -1：手牌有错，0：庄家赢，1：庄家输
	*******************************************************************/
	int compareCardByYa(BYTE* zhuang_handCards, BYTE* xian_handCards);

	/***********************************
	* 函数名：    isDuiZi()
	* 描述：      判断手牌是否为对子
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：      (bool) 对子返回true
	************************************/
	bool isDuiZi(BYTE* handCards);

	/*********************************************
	* 函数名：    calcDianShu()
	* 描述：      计算手牌点数和（两个牌值相加）
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：      (BYTE) 点数和
	**********************************************/
	BYTE calcDianShu(BYTE* handCards);

	/***********************************
	* 函数名：    isUpDao()
	* 描述：      判断手牌是否上道
	* 参数：
	*  @ handCards 玩家手牌数组
	* 返回：      (bool) 上道返回true
	************************************/
	bool isUpDao(BYTE* handCards);

	/******************************************************
	* 函数名：     isHandCards()
	* 描述：       检测手牌是否合法
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：       (bool)合法返回true
	*******************************************************/
	bool isHandCardsValid(BYTE* handCards);

	/******************************************************
	* 函数名：     randCardList()
	* 描述：       从一副牌中随机抽取8张牌，准备发给每个玩家
	* 返回：       (void)
	*******************************************************/
	void randCardList();

	/******************************************************
	* 函数名：     randStart()
	* 描述：       打骰子
	* 返回：       (void)
	*******************************************************/
	int randStart();



	//混乱牌
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE isBaiBan=0);
	void RandCardList1and9(BYTE cbCardBuffer[], BYTE cbBufferCount);
	void RandCardListBaiBan(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//辅助函数
	bool  isDui(BYTE* handCards);
	BYTE getBigCard(BYTE* handCards);
	BYTE getLogicCard(BYTE* handCards);
	bool   isDao(BYTE* handCards);
	//比牌
	int CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);

};


#endif _TUI_TONG_ZI_GAME_LOGIC_H_