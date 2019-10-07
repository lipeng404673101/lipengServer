#pragma once
#ifndef NIU_NIU_NEW_GAME_LOGIC_HEAD_FILE
#define NIU_NIU_NEW_GAME_LOGIC_HEAD_FILE

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
#define OX_BIYINIU					100									//比翼牛
#define OX_FOURKING					101									//****天王牌型
#define OX_SHUNZINIU                102									//顺子牛：（5倍）
#define OX_WUHUANIU                 103									//五花牛：所有牌的点数 >= 10点（5倍）
#define OX_ALLTEN					104									//  ****银牛：五张牌都不小于10-银牛：所有牌点数 >= 10 / 五花牛
#define OX_FIVEKING					105									//  ****金牛：天王牌型-金牛 ，所有牌的点数 >= J
#define OX_DRAGON				    106									//****一条龙：一条龙牌型
#define OX_COLOR_SAME               107									//同花牛：花色相同（6倍）
#define OX_THREE_SAME				108									//葫芦牛：三带二牌型 / 葫芦牛（7倍）
#define OX_FOUR_SAME				109									//炸弹牛：四条牌型-炸弹（8倍）
#define OX_FIVESMALL				110									//五小牛：五张牌总和<=10-五小牛
#define OX_TONGHUASHUN              111									//快乐牛：同花顺牛（10倍）


#define NIU_NIU_POKER_MAX_COUNT		5									//最大数目

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

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
class NNGameLogic
{
//变量定义
private:

	//扑克定义（不含大小王）
	static BYTE						m_cbCardListData[52];				

	//扑克定义（含大小王）
	static BYTE						m_cbCardListData_LaiZi[54];

	/*
	牛牛倍率选择：
	1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
	2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
	*/
	int m_niuNiuTimes;

	//否允许癞子
	int m_isAllowLaiZi;

	//允许特殊牌型
	bool m_isAllowBiYi;
	bool m_isAllowShunZi;
	bool m_isAllowWuHua;
	bool m_isAllowTongHua;
	bool m_isAllowHuLu;
	bool m_isAllowZhaDan;
	bool m_isAllowWuXiao;
	bool m_isAllowKuaiLe;

//函数定义
public:

	//构造函数
	NNGameLogic();

	//析构函数
	virtual ~NNGameLogic();

//类型函数
public:

	//设置牛牛倍数选择
	void SetNiuNiuTimes(int niuNiuTimes);

	//设置是否允许癞子
	void SetAllowLaiZi(bool isAllowLaiZi);

	//设置允许的特殊牌型
	void SetAllowSecialType(bool isAllowBiYi, bool isAllowShunZi, bool isAllowWuHua, bool isAllowTongHua, bool isAllowHuLu, bool isAllowZhaDan, bool isAllowWuXiao, bool isAllowKuaiLe);

	//判断手牌癞子数量
	int GetHandCardsLaoZiCount(const BYTE handCards[], BYTE handCardsCount);

	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//获取一个癞子的牌型（cbSortCalcCards[] 为癞子最优解牌型）
	BYTE GetOneLaiZiCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE& bestLaiZiCard, BYTE cbSortCalcCards[] = NULL);

	//具体计算牌的类型
	BYTE CalcCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }

	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//获取牛牛牌形倍数
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount);

	//1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
	BYTE GetTimes_x3(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes);

	//2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
	BYTE GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes);

	//获取牛牛
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);

	//获取整数
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//判断4张牌的牛牛数
	int GetCardsOxNumber(const BYTE handCards[], int handCardsCount);

//控制函数
public:

	//按扑克逻辑值排列扑克（J/Q/K = 10）
	void SortCardByLogicValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//按扑克牌值排列扑克（J=11  Q=12  K=13）
	void SortCardByCardValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//根据cbCardData值排列扑克
	void SortCardByCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//洗牌
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//洗牌-不含大小王
	void RandCardList_normal(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//洗牌-含大小王
	void RandCardList_laizi(BYTE cbCardBuffer[], BYTE cbBufferCount);

//功能函数
public:

	//逻辑数值（J,Q,K 返回的为10）
	BYTE GetCardLogicValue(BYTE cbCardData);

	/*
	*  描述：对比扑克
	*  参数：
	*   @cbFirstData[] 闲家手牌
	*   @cbNextData[] 庄家手牌
	*   @cbCardCount 手牌数量
	*   @XianOX 闲家牛牛类型
	*   @ZhuangOX 庄家牛牛牌型
	*  返回：
	*   闲家 > 庄家  true
	*   闲家 <= 庄家  false
	*/
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE FirstOX, BYTE NextOX);

	//癞子查找最优解中使用的函数
	bool CompareCard_Sample(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX);

};

//////////////////////////////////////////////////////////////////////////

#endif
