#ifndef _CHE_XUAN_H_
#define _CHE_XUAN_H_
#pragma once

#include "mhgamedef.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效定义
#define CT_ERROR					0									//错误类型
#define VALUE_ERROR					0x00								//数值定义
#define COLOR_ERROR					0xFF								//花色定义

//花色定义
#define COLOR_NT					0x40								//花色定义：大王
#define COLOR_HEI_TAO				0x30								//花色定义：黑桃
#define COLOR_HONG_TAO				0x20								//花色定义：红桃
#define COLOR_MEI_HUA				0x10								//花色定义：梅花
#define COLOR_FANG_KUAI				0x00								//花色定义：方块

#define COLOR_HONG					0									//花色红：方块 & 红桃
#define COLOR_HEI					1									//花色黑：梅花 & 黑桃

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

/* 手牌信息分析结果结构 */
struct CalcHandCardsInfo
{
	/* 手牌，优化：等玩家选好组合后，前两逻辑大小排序，后两逻辑大小排序 */
	int m_handCards[CHEXUAN_HAND_CARDS_COUNT];

	/* 手牌数量 */
	int m_handCardsCount;

	/* 手牌逻辑值，和手牌同步刷新 */
	int m_handCardsLogic[CHEXUAN_HAND_CARDS_COUNT];

	/* 对牌类型：前两对牌类型，后两对牌类型 */
	int m_handCardsType[2];

	/* 点数：前两点数，后两点数*/
	int m_handCardsNum[2];
};

/*
扯炫：单牌类型（逻辑值）
大小从上到下排列，同一组不分大小
天：方块Q  红桃Q
地：方块2  红桃2
人：方块8  红桃8
和：方块4  红桃4
梅板三：
梅（梅十）：梅花10  黑桃10
板（板凳）：梅花4   黑桃4
三（长三）：梅花6   黑桃6

斧十猫膏：
斧（斧头）：梅花J   黑桃J
十（苕十）：方块10  红桃10
猫（猫猫）：方块6   红桃6
膏（膏药）：方块7   红桃7
下四滥：
梅花9   黑桃9
梅花8   黑桃8
梅花7   黑桃7
梅花5   黑桃5
丁丁：红桃3
二皇：大王
数值大的大
*/
enum SINGLE_TYPE
{
	STYPE_TIAN = 7,					/* 天：红Q */
	STYPE_DI = 6,					/* 地：红2 */
	STYPE_REN = 5,					/* 人：红8 */
	STYPE_HE = 4,					/* 和：红4 */
	STYPE_MEI_BAN_SAN = 3,			/* 梅板三：黑10 | 黑4 | 黑6 */
	STYPE_FU_SHI_MAO_GAO = 2,		/* 斧十猫膏：黑J | 红10 | 红6 | 红7 */
	STYPE_XIA_SI_LAN = 1,			/* 下四滥：黑9 | 黑8 | 黑7 | 黑5 | 红桃3(丁丁) | 大王(二皇) */
	STYTE_ERROR = -1,				/* 无效类型 */
};

/* 扯炫：对牌类型*/
enum DOUBLE_TYPE
{
	DTYPE_DINGERHUANG = 6,			/* 丁二皇：红桃3 + 大王 */
	DTYPE_DUIZI = 5,				/* 对子：同色等值的两张牌 */
	DTYPE_WANG = 4,					/* 王：天牌 + 黑9 */
	DTYPE_TIANGANG = 3,				/* 天杠：天牌 + 红/黑8 */
	DTYPE_DIGANG = 2,				/* 地杠：地牌 + 红/黑8 */
	DYTPE_DIAN = 1,					/* 点：不是上面的特殊牌型成为点，比点数，天牌12点，斧头11点，大鬼6点，其余各为其本身点数*/
	DYTPE_ERROR = -1,				/* */
};

enum DOUBLE_TYPE_VALUE
{
	DTV_MIN_INVALID = -1,

	DTV_DIAN_LING,					//零点不分大小

	DTV_DIAN_DING_BA_YI,			//丁八一：红桃3（丁丁）+ 黑8
	DTV_DIAN_FU_SHI_YI,				//斧十一：黑J（斧头）+ 红10（苕十）|| 红6（猫猫）+ 黑5
	DTV_DIAN_MEI_SHI_YI,			//梅十一：黑10（梅十）+ 黑J（斧头）|| 黑4（板凳）+ 红黑7 || 黑6（长三）+ 黑5
	DTV_DIAN_HE_SHI_YI,				//和十一：红4（和牌）+ 红黑7
	DTV_DIAN_REN_SHI_YI,			//人十一：红8（人牌）+ 红桃3（丁丁）
	DTV_DIAN_DI_JIU_YI,				//地九一：红2（地牌）+ 黑9

	DTV_DIAN_DING_JIU_ER,			//丁九二：红桃3（丁丁）+ 黑9 || 黑7 + 黑5
	DTV_DIAN_GAO_YAO_ER,			//膏药二：红7（膏药）+ 黑5
	DTV_DIAN_SI_BA_ER,				//四八二：黑4（板凳）+ 黑8 || 黑6（长三）+ 红6（猫猫）
	DTV_DIAN_HE_SHI_ER,				//和十二：红4（和牌）+ 黑8
	DTV_DIAN_REN_SHI_ER,			//人十二：红8（人牌）+ 红黑4
	DTV_DIAN_DI_SHI_ER,				//地十二：红2（地牌）+ 红黑10
	DTV_DIAN_TIAN_SHI_ER,			//天十二：红Q（天牌）+ 红黑10

	DTV_DIAN_WU_BA_SAN,				//五八三：黑8 + 黑5
	DTV_DIAN_TIAO_SHI_SAN,			//苕十三：红10（苕十）+ 红桃3（丁丁）|| 红6（猫猫）+ 红黑7
	DTV_DIAN_MEI_SHI_SAN,			//梅十三：黑10（梅十）+ 红桃3（丁丁）|| 黑4（板凳）+ 黑9 || 黑6（长三）+ 红黑7
	DTV_DIAN_HE_SHI_SAN,			//和十三：红4（和牌）+ 黑9
	DTV_DIAN_REN_SHI_SAN,			//人十三：红8（人牌）+ 黑5
	DTV_DIAN_DI_FU_SAN,				//地斧三：红2（地牌）+ 黑J（斧头）
	DTV_DIAN_TIAN_FU_SAN,			//天斧三：红Q（天牌）+ 黑J（斧头）

	DTV_DIAN_WU_JIU_SI,				//五九四：黑9 + 黑5
	DTV_DIAN_DING_FU_SI,			//丁斧四：红桃3（丁丁）+ 黑J（斧头）|| 红6（猫猫）+ 黑8 || 红7（膏药）+ 黑7
	DTV_DIAN_MEI_SHI_SI,			//梅十四：黑10（梅十）+ 黑4（板凳）|| 黑6（长三）+ 黑8
	DTV_DIAN_HE_SHI_SI,				//和十四：红4（和牌）+ 红黑10
	DTV_DIAN_REN_SHI_SI,			//人十四：红8（人牌）+ 红黑6
	DTV_DIAN_TIAN_DI_SI,			//天地公子四：红Q（天牌）+ 红2（地牌）

	DTV_DIAN_QI_BA_WU,				//七八五：黑7 + 黑8
	DTV_DIAN_TIAO_SHI_WU,			//苕十五：红10（苕十）+ 黑5 || 红6（猫猫）+ 黑9 || 红7（膏药）+ 黑8
	DTV_DIAN_MEI_SHI_WU,			//梅十五：黑10（梅十）+ 黑5 || 黑4（板凳）+ 黑J（斧头）|| 黑6（长三）+ 黑9
	DTV_DIAN_HE_FU_WU,				//和斧五：红4（和牌）+ 黑J（斧头）
	DTV_DIAN_REN_SHI_WU,			//人十五：红8（人牌）+ 红黑7
	DTV_DIAN_DI_GUAN_WU,			//地丁五：红2（地牌）+ 红桃3
	DTV_DIAN_TIAN_GUAN_WU,			//天丁五：红Q（天牌）+ 红桃3

	DTV_DIAN_QI_JIU_LIU,			//七九六：黑9 + 黑7
	DTV_DIAN_FU_WU_LIU,				//斧五六：黑J（斧头）+ 黑5 || 红10（苕十）+ 红6（猫猫）|| 红7（膏药）+ 黑9
	DTV_DIAN_MEI_SHI_LIU,			//梅十六：黑10（梅十）+ 红黑6 || 黑6（长三）+ 红10（苕十）
	DTV_DIAN_REN_GUAN_LIU,			//人官六：红8（人牌）+ 黑8
	DTV_DIAN_DI_GUAN_LIU,			//地官六：红2（地牌）+ 红黑4
	DTV_DIAN_TIAN_GUAN_LIU,			//天官六：红Q（天牌）+ 红黑4

	DTV_DIAN_FEI_ZHOU_QI,			//非洲七：黑9 + 黑8
	DTV_DIAN_FU_MAO_QI,				//斧猫七：黑J（斧头）+ 红6（猫猫） || 红10（苕十）+ 红黑7
	DTV_DIAN_MEI_SHI_QI,			//梅十七：黑10（梅十）+ 红黑7 || 黑4（板凳）+ 红桃3 || 黑J（斧头）+ 黑6（长三）
	DTV_DIAN_HE_GUAN_QI,			//和官七：红4（和牌）+ 红桃3
	DTV_DIAN_REN_GUAN_QI,			//人官七：红8（人牌）+ 黑9
	DTV_DIAN_DI_GUAN_QI,			//地官七：红2（地牌）+ 黑5
	DTV_DIAN_TIAN_GUAN_QI,			//天官七：红Q（天牌）+ 黑5

	DTV_DIAN_DING_WU_BA,			//丁五八：红桃3（丁丁）+ 黑5
	DTV_DIAN_FU_QI_BA,				//斧七八：黑J（斧头）+ 红黑7 || 红10（苕十）+ 黑8
	DTV_DIAN_MEI_SHI_BA,			//梅十八：黑10（梅十）+ 黑8
	DTV_DIAN_HE_GUAN_BA,			//和官八：红4（和牌）+ 黑4（板凳）
	DTV_DIAN_REN_GUAN_BA,			//人官八：红8（人牌）+ 红黑10
	DTV_DIAN_DI_GUAN_BA,			//地官八：红2（地牌）+ 红黑6 || 红2（地牌）+ 大王
	DTV_DIAN_TIAN_GUAN_BA,			//天官八：红Q（天牌）+ 红黑6 || 红Q（天牌）+ 大王

	DTV_DIAN_WU_LONG_JIU,			//乌龙九：黑J（斧头）+ 黑8 || 红10（苕十）+ 黑9 || 红桃3（丁丁）+ 红6（猫猫）
	DTV_DIAN_MEI_SHI_JIU,			//梅十九：黑10（梅十）+ 黑9 || 黑4（板凳）+ 黑5 || 红桃3（丁丁）+ 黑6（长三）
	DTV_DIAN_HE_GUAN_JIU,			//和官九：红4（和牌）+ 黑5
	DTV_DIAN_REN_GUAN_JIU,			//人官九：红8（人牌）+ 黑J（斧头）
	DTV_DIAN_DI_GUAN_JIU,			//地官九：红2（地牌）+ 红黑7
	DTV_DIAN_TIAN_GUAN_JIU,			//天官九：红Q（天牌）+ 红黑7

	DTV_DI_GANG,					//地杠：红2（地牌）+ 红8（人牌）|| 红2（地牌）+ 黑8

	DTV_TIAN_GANG,					//天杠：红Q（天牌）+ 红8（人牌）|| 红Q（天牌）+ 黑8

	DTV_DI_JIU_WANG,				//地九王：红2（地牌）+ 黑9

	DTV_TIAN_JIU_WANG,				//天九王：红Q（天牌） + 黑9

	DTV_XIA_SI_LAN_DUI,				//下四滥对：黑9对、黑8对、黑7对、黑5对
	DTV_FU_SHI_MAO_GAO_DUI,			//斧十猫膏对：黑J对、红10对、红6对、红7对
	DTV_MEI_BAN_SAN_DUI,			//梅板三对：黑4对、黑6对、黑10对
	DTV_HE_DUI,						//和牌对：红桃4 + 方块4
	DTV_REN_DUI,					//人牌对：红桃8 + 方块8
	DTV_DI_DUI,						//地牌对：红桃2 + 方块2
	DTV_TIAN_DUI,					//天牌对：红桃Q + 方块Q

	DTV_DING_ER_HUANG,				//丁二皇：红桃3 + 大王

	DTV_MAX_INVALID,
};


struct HandCardsTypeInfo
{
	int m_userIndex;						//玩家位置
	DOUBLE_TYPE_VALUE m_firstType;			//前两张牌的逻辑值
	DOUBLE_TYPE_VALUE m_secondType;			//后两张牌的逻辑值
	int m_pretHandCards[2];					//前两张牌
	int m_nextHandCards[2];					//后两张牌
	int m_isSameNext;						//是否和下一个玩家牌型相等

	HandCardsTypeInfo()
	{
		memset(this, 0, sizeof(this));
		m_userIndex = CHEXUAN_INVALID_POS;
	}

	bool operator > (HandCardsTypeInfo& right)
	{
		return this->m_secondType > right.m_secondType;
	}

	bool operator == (HandCardsTypeInfo& right)
	{
		return this->m_secondType == right.m_secondType;
	}

	bool operator < (HandCardsTypeInfo& right)
	{
		return this->m_secondType < right.m_secondType;
	}

	void operator = (HandCardsTypeInfo& right)
	{
		this->m_userIndex = right.m_userIndex;
		this->m_firstType = right.m_firstType;
		this->m_secondType = right.m_secondType;
		this->m_pretHandCards[0] = right.m_pretHandCards[0];
		this->m_pretHandCards[1] = right.m_pretHandCards[1];
		this->m_nextHandCards[0] = right.m_nextHandCards[0];
		this->m_nextHandCards[1] = right.m_nextHandCards[1];
		this->m_isSameNext = right.m_isSameNext;
	}

};

/* 游戏逻辑处理类 */
class CXGameLogic
{
private:

	/* 扑克定义 只使用其中的32张扑克 */
	static int m_cardsPool[CHEXUAN_CELL_PACK];

	/* 是否允许地九王(默认：不允许) */
	bool m_is_allow_di_jiu;

public:

	/* 构造函数 */
	CXGameLogic();

	/* 析构函数 */
	virtual ~CXGameLogic();

public:

	/* 获取地九王选项 */
	bool getDiJiu();

	/* 设置地九王选项 */
	void setDiJiu(bool diJiu);

	/* 获取牌的数值 */
	int getCardValue(int card);

	/* 获取牌的花色 */
	int getCardColor(int card);

	/* 获取颜色类型：方块&红桃 = 红  梅花&黑桃 = 黑 */
	int getCardColorType(int card);

	/* 是否为有效牌值 */
	bool isValidCard(int card);

	/* 获取单牌逻辑值 */
	SINGLE_TYPE getLogicCardValue(int card);

	/* 获取对牌类型 */
	DOUBLE_TYPE getDoubleCardsType();

	/* 获取对牌值 */
	DOUBLE_TYPE_VALUE getDoubleCardsTypeValue(int card1, int card2);

	/* 检测是否为三花牌型 */
	bool checkSanHuaType(const int* handCards, int handCardsCount);

	/* 检测是否为三花成对 */
	bool checkSanHuaDouble(const int* handCards, int handCardsCount);

	/* 校验组手牌是否一样 */
	bool checkTwoHandCardsIsSame(const int* srcCards, const int* desCards, int cardCount);

	/* 排列手牌中的两组牌，大的在前 */
	bool sortHandCardsByDouble(int* handCards, int cardCount, int* handCardsType);

	/* 按照第二类型大小排序所有参与玩家的手牌，方便后面比较 */
	bool sortAllPlayerCards(HandCardsTypeInfo info[], int infoCount, int firstDangPos, int playerCount);

	/* 查找最大牌对应的位置 */
	int getMaxCardPos(const int* srcCards, int cardCount, int playerCount, int firstSpeckPos);

	/* 计算两张牌的点数 */
	int getDoubleCardsNum();

	/* 洗牌 */
	void RandCardList(int cardBuffer[], int cardBufferCount);
};

#endif // !_CHE_XUAN_H_



