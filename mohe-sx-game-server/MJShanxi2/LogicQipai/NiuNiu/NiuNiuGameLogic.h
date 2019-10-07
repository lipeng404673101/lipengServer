#ifndef NIU_NIU_GAME_LOGIC_HEAD_FILE
#define NIU_NIU_GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////
//宏定义

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define OX_VALUE0					0									//混合牌型
#define OX_FOURKING					101									//天王牌型
#define OX_WUHUANIU                 102									//五花牛：所有牌的点数 >= 10点
#define OX_ALLTEN					103									//  银牛：五张牌都不小于10-银牛：所有牌点数 >= 10 / 五花牛
#define OX_FIVEKING					104									//  金牛：天王牌型-金牛 ，所有牌的点数 >= J
#define OX_SHUNZINIU                105									//顺子牛：
#define OX_THREE_SAME				106									//葫芦牛：三条牌型 / 葫芦牛
#define OX_DRAGON				    107									//一条龙：一条龙牌型
#define OX_COLOR_SAME               108									//同花牛：
#define OX_FOUR_SAME				109									//炸弹牛：四条牌型-炸弹
#define OX_FIVESMALL				110									//五小牛：五张牌总和<=10-五小牛
#define OX_TONGHUASHUN              111									//同花顺牛：




#define NIU_NIU_POKER_MAX_COUNT						5									//最大数目

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
	BYTE 							cbFourLogicVolue[1];				//四张列表
	BYTE 							cbThreeLogicVolue[1];				//三张列表
	BYTE 							cbDoubleLogicVolue[2];				//两张列表
	BYTE 							cbSignedLogicVolue[5];				//单张列表
	BYTE							cbFourCardData[NIU_NIU_POKER_MAX_COUNT];			//四张列表
	BYTE							cbThreeCardData[NIU_NIU_POKER_MAX_COUNT];			//三张列表
	BYTE							cbDoubleCardData[NIU_NIU_POKER_MAX_COUNT];		//两张列表
	BYTE							cbSignedCardData[NIU_NIU_POKER_MAX_COUNT];		//单张数目
};


//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class NGameLogic
{
	//变量定义
private:
	static BYTE						m_cbCardListData[52];				//扑克定义

																		//函数定义
public:
	//构造函数
	NGameLogic();
	//析构函数
	virtual ~NGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//获取牛牛牌形倍数
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec, BYTE cbSweep);
	//普通倍数
	BYTE GetTimes_normal(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);
	//新增倍率X1
	BYTE GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);


	//获取牛牛
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);
	//获取整数
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//功能函数
public:
	//逻辑数值（J,Q,K 返回的为10）
	BYTE GetCardLogicValue(BYTE cbCardData);

	//对比扑克
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BOOL FirstOX, BOOL NextOX, BOOL isSpecPlay);
};

//////////////////////////////////////////////////////////////////////////

#endif
