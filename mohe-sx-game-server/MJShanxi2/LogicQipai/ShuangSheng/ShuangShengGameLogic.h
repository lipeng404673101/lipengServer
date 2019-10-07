#ifndef _SHUANG_SHENG_GAME_LOGIC_H_
#define _SHUANG_SHENG_GAME_LOGIC_H_
#pragma once

#include "Stdafx.h"
#include <cstdlib>
#include "mhgamedef.h"
#include <time.h>
#include <vector>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
//宏定义

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//属性定义
#define MAX_TRACKOR					18									//最大连牌
#define	COLOR_RIGHT					40									//花色权位

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

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_SAME_2					2									//对牌类型
#define CT_SAME_3					3									//三牌类型
#define CT_SAME_4					4									//四牌类型
#define CT_TRACKOR_2				5									//拖拉机型
#define CT_TRACKOR_3				6									//拖拉机型
#define CT_TRACKOR_4				7									//拖拉机型
#define CT_THROW_CARD				8									//甩牌类型

//////////////////////////////////////////////////////////////////////////
//结构定义

//对牌信息
struct tagSameDataInfo
{
	BYTE							cbCardCount;									//扑克数目（对子数 * 2）cbSameData[]中的数量
	BYTE							cbBlockCount;									//牌段数目（对子数）
	BYTE							cbSameData[SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//拖拉机牌（存放所有对牌）
};

//拖拉机信息
struct tagTractorDataInfo
{
	BYTE							cbCardCount;						//扑克数目  cbTractorData[]中牌的数量
	BYTE							cbTractorCount;						//拖牌个数
	BYTE							cbTractorMaxIndex;					//索引位置
	BYTE							cbTractorMaxLength;					//最大长度（最大的拖拉机数）
	BYTE							cbTractorData[SHUANGSHENG_MAX_HAND_CARDS_COUNT * 3 / 2];		//拖拉机牌
};

//牌型结构
struct tagAnalyseResult
{
	BYTE							cbCardColor;								//扑克花色
	tagSameDataInfo					SameDataInfo[SHUANGSHENG_PACK_COUNT];		//同牌数据：SameDataInfo[0]存单牌，SameDataInfo[1]存对牌
	tagTractorDataInfo				TractorDataInfo[SHUANGSHENG_PACK_COUNT - 1];//拖牌数据：TractorDataInfo[0]
};

//需求结构
struct tagDemandInfo
{
	BYTE 							cbMaxTractor[SHUANGSHENG_PACK_COUNT - 1];			//长度需求
	BYTE							cbSameCardCount[SHUANGSHENG_PACK_COUNT];			//同牌需求
	BYTE 							cbTractorCardCount[SHUANGSHENG_PACK_COUNT - 1];		//拖牌需求
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[SHUANGSHENG_MAX_HAND_CARDS_COUNT];			//结果扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class SSGameLogic
{
	//属性变量
protected:
	BYTE							m_cbPackCount;						//牌副数目（默认双升2副牌）
	BYTE							m_cbDispatchCount;					//玩家发数目（默认双升25张）

	//逻辑变量
protected:
	BYTE							m_cbMainColor;						//主牌花色
	BYTE							m_cbMainValue;						//主牌数值

	//辅助变量
protected:
	BYTE							m_cbSortRight[5];					//排序权位

protected:
	BYTE							m_cbNTValue;						//常主数值(不带花色)

	//变量定义
public:
	//static const BYTE				m_cbNTValue;						//常主数值
	static const BYTE				m_cbCardData[SHUANGSHENG_CELL_PACK];//扑克数据

	//函数定义
public:
	//构造函数
	SSGameLogic();
	//析构函数
	virtual ~SSGameLogic();

	//状态设置
public:
	//设置副数
	bool SetPackCount(BYTE cbPackCount);
	//获取副数
	BYTE GetPackCount() { return m_cbPackCount; }
	//设置常主
	bool SetChangZhu(BYTE cbNTValue);
	//获取常主
	BYTE GetChangZhu() { return m_cbNTValue; }
	//派发数目
	BYTE GetDispatchCount() { return m_cbDispatchCount; }

	//信息函数
public:
	//主牌花色
	bool SetMainColor(BYTE cbMainColor);
	//主牌数值
	bool SetMainValue(BYTE cbMainValue);
	//主牌花色
	BYTE GetMainColor() { return m_cbMainColor; }
	//主牌数值
	BYTE GetMainValue() { return m_cbMainValue; }

	//控制函数
public:
	//找出集合中最小的逻辑牌值
	BYTE GetMinCardByLogicValue(const BYTE cardList[], int cardListCount);
	//找出集合中最大的逻辑牌值
	BYTE GetMaxCardByLogicValue(const BYTE cardList[], int cardListCount);
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//积分函数
public:
	//获取积分
	WORD GetCardScore(const BYTE cbCardData[], BYTE cbCardCount);
	//积分扑克
	BYTE GetScoreCard(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbScoreCard[], BYTE cbMaxCount);

	//功能函数
public:
	//分析扑克
	bool AnalyseCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//出牌判断
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const  BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);
	//效验出牌
	bool EfficacyOutCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, const BYTE cbFirstCardData[], BYTE cbFirstCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount);
	//效验甩牌
	bool EfficacyThrowCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, WORD wOutCardIndex, const BYTE cbHandCardData[SHUANGSHENG_PLAY_USER_COUNT][SHUANGSHENG_MAX_HAND_CARDS_COUNT], BYTE cbHandCardCount, tagOutCardResult & OutCardResult);
	//校验甩牌，失败强制出小的单张
	bool EfficacyThrowCard_Small(const BYTE cbOutCardData[], BYTE cbOutCardCount, WORD wOutCardIndex, const BYTE cbHandCardData[SHUANGSHENG_PLAY_USER_COUNT][SHUANGSHENG_MAX_HAND_CARDS_COUNT], BYTE cbHandCardCount, tagOutCardResult & OutCardResult);

	//对比函数
public:
	//对比扑克
	bool CompareCardData(BYTE cbFirstCardData, BYTE cbNextCardData);
	//对比扑克
	bool CompareCardResult(const tagAnalyseResult WinnerResult, const tagAnalyseResult UserResult);
	//比较已出牌玩家中的最大的牌
	WORD CompareCardArray_n(const BYTE cbOutCardData[SHUANGSHENG_PLAY_USER_COUNT][SHUANGSHENG_HAND_CARDS_COUNT], BYTE cbCardCount, WORD wFirstIndex, WORD outPlayerCount = 4);
	//比较4家出牌中最大的牌
	WORD CompareCardArray(const BYTE cbOutCardData[SHUANGSHENG_PLAY_USER_COUNT][SHUANGSHENG_HAND_CARDS_COUNT], BYTE cbCardCount, WORD wFirstIndex);

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//排序等级
	BYTE GetCardSortOrder(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//逻辑花色
	BYTE GetCardLogicColor(BYTE cbCardData);
	//牌列花色
	BYTE GetCardLogicColor(const BYTE cbCardData[], BYTE cbCardCount);

	//逻辑辅助
protected:
	//是否相连
	bool IsLineValue(BYTE cbFirstCard, BYTE cbSecondCard);
	//是否同花
	bool IsSameColor(BYTE cbFirstCard, BYTE cbSecondCard);
	//交集数目
	BYTE GetIntersectionCount(const BYTE cbCardData1[], BYTE cbCardCount1, const BYTE cbCardData2[], BYTE cbCardCount2);

	//提取函数
protected:
	/************************************************************************
	* 函数名：	DistillCardByColor
	* 描述：	提取牌集合中指定的颜色的牌
	* 详细描述：提取cbCardData[]中所有cbCardColor指定的花色
	* 参数：
	*  @ cbCardData[] 要提取的牌的集合
	*  @ cbCardCount 要提取的牌集合的牌数量
	*  @ cbCardColor 要提取的颜色
	*  @ cbResultCard[] 提取牌的结果集合（传出参数）
	* 返回：	(BYTE)提取牌的数量
	*************************************************************************/
	BYTE DistillCardByColor(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardColor, BYTE cbResultCard[]);
	
	/************************************************************************
	* 函数名：	DistillCardByCount
	* 描述：	提取牌集合中指定数量的相同牌
	* 详细描述：提取cbCardData[]中所与cbSameCount数量相同的牌
	* 参数：
	*  @ cbCardData[] 要提取的牌的集合
	*  @ cbCardCount 要提取的牌集合的牌数量
	*  @ cbSameCount 要提取的牌的数量
	*  @ SameDataInfo 提取对子牌的结果
	* 返回：	(BYTE)提取牌的对子数量
	*************************************************************************/
	BYTE DistillCardByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagSameDataInfo & SameDataInfo);

	/************************************************************************
	* 函数名：	DistillTrackorByCount
	* 描述：	提取牌集合中拖拉机数量信息
	* 详细描述：提取cbCardData[]中所与cbSameCount数量相同的牌，且连牌（拖拉机）
	* 参数：
	*  @ cbCardData[] 要提取的牌的集合
	*  @ cbCardCount 要提取的牌集合的牌数量
	*  @ cbSameCount 要提取的牌的数量
	*  @ TractorDataInfo 提取的拖拉机信息
	* 返回：	(BYTE)提取牌的拖拉机数量
	*************************************************************************/
	BYTE DistillTrackorByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagTractorDataInfo & TractorDataInfo);

	//内部函数
protected:
	//更新权位
	void UpdateSortRight();
	//调整扑克
	bool RectifyCardWeave(const BYTE cbCardData[], BYTE cbCardCount, const tagAnalyseResult & TargetResult, tagAnalyseResult & RectifyResult);
};

//////////////////////////////////////////////////////////////////////////




#endif // ! _SHUANG_SHENG_GAME_LOGIC_H_