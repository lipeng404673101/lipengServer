#ifndef YING_SAN_ZHANG_GAME_LOGIC_HEAD_FILE
#define YING_SAN_ZHANG_GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <cstdlib>

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//////////////////////////////////////////////////////////////////////////
//宏定义

#define MAX_COUNT					3									//最大数目
#define	DRAW						2									//和局类型

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define	CT_SHUN_ZI					3									//顺子类型
#define CT_JIN_HUA					4									//金花类型
#define	CT_SHUN_JIN					5									//顺金类型
#define	CT_BAO_ZI					6									//豹子类型
#define CT_SPECIAL					7									//特殊类型

//天龙地龙特殊牌型
#define  CT_SHUN_DI_DRAGON          11             //顺子 地龙
#define  CT_SHUN_TIAN_DRAGON    12            //顺子 天龙
#define CT_JIN_DI_DRAGON                13            //金花 地龙
#define CT_JIN_TIAN_DRAGON          14           //金花 天龙


//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
private:
	static BYTE						m_cbCardListData[52];				//扑克定义
	static BYTE                       m_cbCardListData_BigCard[20];           //扑克定义,大牌模式  10,J,Q,K,A

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取类型--排序，排除特殊牌型
	BYTE GetCardTypeSord(BYTE cbCardData[], BYTE cbCardCount);
	//获取类型，天龙地龙
	BYTE GetCardTypeDragon(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }
	//判断顺子，天龙，地龙， -这个函数默认输入参数牌型已经是顺子，且降序排列 AKQJ109....
	BYTE GetSpecTypeOnShun(BYTE cbCardData[]);

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//混乱扑克-大牌模式    by wyz
	void RandCardList_BigCard(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//混乱扑克-模式选项        by wyz
	void RandCardList_PlayType(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE playType);


	//功能函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE tianDiDragon = 0);
};

//////////////////////////////////////////////////////////////////////////

#endif
