#ifndef _SAN_DA_YI_GAME_LOGIC_H_
#define _SAN_DA_YI_GAME_LOGIC_H_

#pragma once

#include "mhgamedef.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>


////////////////////////////////////////// 宏定义 ////////////////////////////////////////////

//数组长度
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效定义
#define CT_ERROR					0									//错误类型
#define VALUE_ERROR					0x00								//数值定义
#define COLOR_ERROR					0xFF								//花色定义

//花色定义
#define COLOR_NT					0x40								//花色定义
#define COLOR_HEI_TAO				0x30								//花色定义
#define COLOR_HONG_TAO				0x20								//花色定义
#define COLOR_MEI_HUA				0x10								//花色定义
#define COLOR_FANG_KUAI				0x00								//花色定义

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//辅助变量
#define COLOR_RIGHT					40									//排序权位




class SDYGameLogic
{

	//属性变量
protected:

	/* 牌副数 */
	int	m_packCount;

	/* 底牌数量 */
	int m_baseCardsCount;

	/* 发牌数量 */
	int	m_sendCardsCount;

	//逻辑变量
protected:

	/* 主花色 */
	int	m_mainColor;

	/* 常主数值 */
	int	m_commMainValue;

	/* 排序权位 */
	int m_sortRight[5];

	/* 一副牌的牌值 */
	static const int m_allCardsData[SANDAYI_CELL_PACK];


	//函数定义
public:

	/* 构造函数 */
	SDYGameLogic();

	/* 析构函数 */
	virtual ~SDYGameLogic();

	//设置状态
public:

	/* 设置牌副数 */
	bool SetPackCount(int packCount);

	/* 获取牌副数 */
	int GetPackCount();

	/* 设置底牌数量 */
	bool SetBaseCardsCount(int baseCardsCount);

	/* 获取底牌数量 */
	int GetBaseCardsCount();

	/* 设置常主数值 */
	bool SetCommMainValue(int commMainValue);

	/* 获取常主数值 */
	int GetCommMainValue();

	/* 设置发手牌数量 */
	bool SetSendCardsCount(int sendCardsCount);

	/* 获取发手牌数量 */
	int GetSendCardsCount();

	//信息函数
public:

	/* 设置主花色 */
	bool SetMainColor(int mainColor);

	/* 获取主花色 */
	int GetMainColor();

	//逻辑函数
public:

	/* 获取牌面数值 */
	int GetCardValue(int cardData);

	/* 获取牌面花色 */
	int GetCardColor(int cardData);

	/* 获取牌的排序值 */
	int GetCardSortValue(int cardData);

	/* 获取牌的逻辑值 */
	int GetCardLogicValue(int cardData);

	/* 获取牌的逻辑花色 */
	int GetCardLogicColor(int cardData);

	/* 有效牌值判断 */
	bool IsValidCard(int cardData);

	/* 判断是否为方梅红黑的花色 */
	bool IsValidColor(int cardColor);

	/* 是否同花 */
	bool IsSameColor(int firstCard, int secondCard);

	/* 判断选对家的牌是否符合要求 */
	bool IsValidFirendCard(int selectCard, int selectScore);

	//控制函数
public:

	/* 排列扑克 */
	void SortCardList(int cardsData[], int cardsCount);

	/* 混乱扑克 */
	void RandCardList(int cardsBuffer[], int bufferCount);

	/* 是否包含目标牌值 */
	bool IncludeCard(const int cardsData[], int cardsDataCount, int destCard);

	/* 删除扑克 */
	bool RemoveCard(const int removeCards[], int removeCount, int cardsData[], int cardsDataCount);

	/* 获取积分 */
	int GetCardsScore(const int cardsData[], int cardsDataCount);

	/* 提取分牌 */
	int GetScoreCards(const int cardsData[], int cardsDataCount, int scoreCards[], int maxCount = 12);

	/* 对比扑克逻辑值 */
	bool CompareCardData(int firstCard, int secondCard);

	/* 找出集合中最大的逻辑牌值对应的位置 */
	int GetMaxCardIndexByLogicValue(const int cardsData[], int cardsCount);

	/* 对比出牌 */
	int CompareOutCardArray(const int outCardArr[], int firstOutPos, int outPlayerCount = 5);

	/* 根据牌花色提取扑克 */
	int DistillCardByColor(const int cardsData[], int cardsDataCount, int destCard, int resultCards[]);

	/* 校验跟牌玩家出牌 */
	bool EfficacyOutCard(int outCard, const int handCards[], int handCardsCount, int firstOutCard);

	/* 更新权位 */
	bool UpDataSortRight();
};



#endif // !_SAN_DA_YI_GAME_LOGIC_H_
