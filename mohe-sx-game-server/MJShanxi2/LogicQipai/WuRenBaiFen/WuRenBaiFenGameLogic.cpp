#include "WuRenBaiFenGameLogic.h"
#include "LLog.h"


//扑克数据
const int WRBFGameLogic::m_allCardsData[SHUANGSHENG_CELL_PACK] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};

/* 构造函数 */
WRBFGameLogic::WRBFGameLogic()
{
	//默认：一副扑克
	m_packCount = WURENBAIFEN_PACK_COUNT;

	//默认：底牌数量为4张
	m_baseCardsCount = WURENBAIFEN_BASE_CARDS_COUNT;

	//默认：发牌数量为10张
	m_sendCardsCount = WURENBAIFEN_SEND_CARD_COUNT;

	//默认：无常主
	m_commMainValue = VALUE_ERROR;

	//默认：无主花色
	m_mainColor = COLOR_ERROR;
}

/* 析构函数 */
WRBFGameLogic::~WRBFGameLogic()
{

}


/* 设置牌副数 */
bool WRBFGameLogic::SetPackCount(int packCount)
{
	if (packCount <= 0 || packCount > WURENBAIFEN_PACK_COUNT)
	{
		LLOG_ERROR("WRBFGameLogic::SetPackCount() Error... Set Pack Count Is Not Valid...");
		return false;
	}

	//设置牌副数
	m_packCount = packCount;

	return true;
}

/* 获取牌副数 */
int WRBFGameLogic::GetPackCount()
{
	return m_packCount;
}

/* 设置底牌数量 */
bool WRBFGameLogic::SetBaseCardsCount(int baseCardsCount)
{
	if (baseCardsCount <= 0 || baseCardsCount > WURENBAIFEN_BASE_CARDS_COUNT)
	{
		LLOG_ERROR("WRBFGameLogic::SetBaseCardsCount() Error... Set Base Cards Count Is Not Valid...");
		return false;
	}

	//设置底牌数量
	m_baseCardsCount = baseCardsCount;
	return true;
}

/* 获取底牌数量 */
int WRBFGameLogic::GetBaseCardsCount()
{
	return m_baseCardsCount;
}

/* 设置常主数值 */
bool WRBFGameLogic::SetCommMainValue(int commMainValue)
{
	if (false)
	{
		LLOG_ERROR("WRBFGameLogic::SetCommMainValue() Error... Set Common Cards Is Not Valid...");
		return false;
	}

	//设置常主
	m_commMainValue = commMainValue;

	return true;
}

/* 获取常主数值 */
int WRBFGameLogic::GetCommMainValue()
{
	return m_commMainValue;
}

/* 设置发手牌数量 */
bool WRBFGameLogic::SetSendCardsCount(int sendCardsCount)
{
	if (sendCardsCount <= 0)
	{
		LLOG_ERROR("WRBFGameLogic::SetSendCardsCount() Error... Set Send Cards Count Is Not Valid...");
		return false;
	}

	m_sendCardsCount = sendCardsCount;

	return true;
}

/* 获取发手牌数量 */
int WRBFGameLogic::GetSendCardsCount()
{
	return m_sendCardsCount;
}

/* 设置主花色 */
bool WRBFGameLogic::SetMainColor(int mainColor)
{
	if (mainColor < COLOR_FANG_KUAI || mainColor > COLOR_HEI_TAO)
	{
		LLOG_ERROR("WRBFGameLogic::SetMainColor() Error... Set Main Color Is Not Valid...");
		return false;
	}

	m_mainColor = mainColor;

	//更新花色权位
	UpDataSortRight();

	return true;
}

/* 获取主花色 */
int WRBFGameLogic::GetMainColor()
{
	return m_mainColor;
}

/* 获取牌面数值 */
int WRBFGameLogic::GetCardValue(int cardData)
{
	return cardData & LOGIC_MASK_VALUE;
}

/* 获取牌面花色 */
int WRBFGameLogic::GetCardColor(int cardData)
{
	return cardData & LOGIC_MASK_COLOR;
}

/* 有效牌值判断 */
bool WRBFGameLogic::IsValidCard(int cardData)
{
	//获取属性
	int cardColor = GetCardColor(cardData);
	int cardValue = GetCardValue(cardData);

	//大小王有效
	if ((cardData == 0x4E) || (cardData == 0x4F))
	{
		return true;
	}

	//普通牌有效
	if ((cardColor <= 0x30) && (cardColor >= 0x00) && (cardValue >= 0x01) && (cardValue <= 0x0D))
	{
		return true;
	}

	return false;
}

/* 判断是否为方梅红黑的花色 */
bool WRBFGameLogic::IsValidColor(int cardColor)
{
	int t_cardColor = GetCardColor(cardColor);

	if (t_cardColor == COLOR_FANG_KUAI ||
		t_cardColor == COLOR_MEI_HUA ||
		t_cardColor == COLOR_HONG_TAO ||
		t_cardColor == COLOR_HEI_TAO)
	{
		return true;
	}
	return false;
}

/* 获取牌的排序值 */
int WRBFGameLogic::GetCardSortValue(int cardData)
{
	if (!IsValidCard(cardData)) return 0;

	//逻辑数值
	int t_cardColor = GetCardColor(cardData);
	int t_cardValue = GetCardValue(cardData);

	//鬼牌
	if ((cardData == 0x4E) || (cardData == 0x4F))
	{
		int t_sortValue = t_cardValue + 11;
		return t_sortValue + m_sortRight[4];
	}

	//2常主
	if (t_cardValue == m_commMainValue)
	{
		int t_sortValue = (t_cardColor >> 4) + 15;
		if (t_cardColor == m_mainColor)
			t_sortValue = 19;
		return t_sortValue + m_sortRight[4];
	}

	//花色变换
	int t_sortColor = t_cardColor;
	if (t_cardColor == m_mainColor) t_sortColor = 0x40;

	//A
	int t_sortValue = t_cardValue;
	if (t_cardValue == 1) t_sortValue += 13;

	return t_sortValue + m_sortRight[t_sortColor >> 4];
}

/* 获取牌的逻辑值 */
int WRBFGameLogic::GetCardLogicValue(int cardData)
{
	if (!IsValidCard(cardData)) return 0;

	//逻辑数值
	int t_cardColor = GetCardColor(cardData);
	int t_cardValue = GetCardValue(cardData);

	//设置常主权位（主2、副2占两个大位置）
	int t_commMainRight = (m_commMainValue == VALUE_ERROR) ? 0 : 4;

	//小王过滤：(2常主，小王20)，(2不是常主，小王16)
	if (cardData == 0x4E)
	{
		int t_logicValue = t_cardValue + 1 + t_commMainRight;
		return t_logicValue + m_sortRight[4];
	}

	//大王过滤：(2常主，大王22)，(2不是常主，大王18)
	if (cardData == 0x4F)
	{
		BYTE t_logicValue = t_cardValue + 2 + t_commMainRight;
		return t_logicValue + m_sortRight[4];
	}

	//2常主过滤 主2：18  副2：16
	if (t_cardValue == m_commMainValue)
	{
		int t_logicValue = (t_cardColor == m_mainColor) ? 18 : 16;
		return t_logicValue + m_sortRight[4];
	}

	//主花色变换
	int t_logicColor = t_cardColor;
	if (t_cardColor == m_mainColor) t_logicColor = 0x40;

	//A转换变换为14
	int t_logicValue = t_cardValue;
	if (t_cardValue == 1) t_logicValue += 13;

	return t_logicValue + m_sortRight[t_logicColor >> 4];
}

/* 获取牌的逻辑花色 */
int WRBFGameLogic::GetCardLogicColor(int cardData)
{
	//逻辑数值
	BYTE cardColor = GetCardColor(cardData);
	BYTE cardValue = GetCardValue(cardData);

	//如果是主花色
	if (cardColor == m_mainColor)
		return COLOR_NT;

	//如果是常主，则为主花色
	if (cardValue == m_commMainValue)
		return COLOR_NT;

	//如果是大小王，则为主花色
	if (cardColor == 0x40)
		return COLOR_NT;

	return cardColor;
}

/* 是否同花 */
bool WRBFGameLogic::IsSameColor(int firstCard, int secondCard)
{
	//获取花色
	BYTE firstLogicColor = GetCardLogicColor(firstCard);
	BYTE secondLogicColor = GetCardLogicColor(secondCard);

	//同花判断
	return (firstLogicColor == secondLogicColor);
}

/* 判断选对家的牌是否符合要求 */
bool WRBFGameLogic::IsValidFirendCard(int selectCard, int selectScore, bool can_select_main_color)
{
	Lint t_color = GetCardColor(selectCard);
	Lint t_value = GetCardValue(selectCard);
	bool bCanSelectMainColor = true;

	//庄家叫牌分100分，可选大小王，所有的A、K、Q、J
	if (selectScore == 100)
	{
		if (selectCard == 0x4E || selectCard == 0x4F || t_value == 1 || (t_value >= 10 && t_value <= 13))
		{
			return true;
		}
		return false;
	}
	else
	{
		if (selectCard == 0x4E || selectCard == 0x4F)
			return false;
		if (t_color == m_mainColor)
		{
			if (can_select_main_color)
			{			
			}
			else
			{
				return false;
			}
		}
		if (t_value > 1 && t_value < 10)
			return false;
		return true;
	}
	return true;
}


/* 排列扑克 */
void WRBFGameLogic::SortCardList(int cardsData[], int cardsCount)
{
	if (cardsCount < 2)
		return;

	//将扑克牌值转换为排序值
	int cbSortValue[WURENBAIFEN_HAND_CARDS_COUNT_MAX];
	for (int i = 0; i < cardsCount; i++)
	{
		cbSortValue[i] = GetCardSortValue(cardsData[i]);
	}

	//排序操作
	bool bSorted = true;
	int bTempData;
	int bLast = cardsCount - 1;
	do
	{
		bSorted = true;
		for (int i = 0; i < bLast; i++)
		{
			if ((cbSortValue[i] < cbSortValue[i + 1]) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cardsData[i] < cardsData[i + 1])))
			{
				//交换位置
				bTempData = cardsData[i];
				cardsData[i] = cardsData[i + 1];
				cardsData[i + 1] = bTempData;
				bTempData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = bTempData;
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//混乱扑克
void WRBFGameLogic::RandCardList(int cardsBuffer[], int bufferCount)
{
	//混乱准备
	int cbCardData[WURENBAIFEN_CELL_PACK * WURENBAIFEN_PACK_COUNT];

	memcpy(cbCardData, m_allCardsData, sizeof(m_allCardsData));


	//混乱扑克
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (bufferCount - cbRandCount);
		cardsBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[bufferCount - cbRandCount];
	} while (cbRandCount < bufferCount);

	return;
}

/* 是否包含目标牌值 */
bool WRBFGameLogic::IncludeCard(const int cardsData[], int cardsDataCount, int destCard)
{
	for (int i = 0; i < cardsDataCount; ++i)
	{
		if (cardsData[i] == destCard) return true;
	}

	return false;
}

//删除扑克
bool WRBFGameLogic::RemoveCard(const int removeCards[], int removeCount, int cardsData[], int cardsDataCount)
{
	//定义变量
	int t_deleteCount = 0;
	int t_cardsData[WURENBAIFEN_HAND_CARDS_COUNT_MAX];

	//检验数据
	if (cardsDataCount > CountArray(t_cardsData))
	{
		return false;
	}
	memcpy(t_cardsData, cardsData, cardsDataCount * sizeof(cardsData[0]));

	//将手牌中要删掉的扑克置零
	for (int i = 0; i < removeCount; i++)
	{
		for (int j = 0; j < cardsDataCount; j++)
		{
			if (removeCards[i] == t_cardsData[j])
			{
				t_deleteCount++;
				t_cardsData[j] = 0;
				break;
			}
		}
	}
	if (t_deleteCount != removeCount) return false;

	//清理扑克
	int t_cardPos = 0;
	int t_invalidInde = cardsDataCount - removeCount;
	for (int i = 0; i < cardsDataCount; i++)
	{
		if (t_cardsData[i] != 0)
		{
			cardsData[t_cardPos++] = t_cardsData[i];
		}
		if (i >= t_invalidInde)
		{
			cardsData[i] = 0;  //**Ren 添加
		}
	}

	return true;
}

/* 获取积分 */
int WRBFGameLogic::GetCardsScore(const int cardsData[], int cardsDataCount)
{
	//变量定义
	int t_cardScore = 0;

	//扑克累计
	for (int i = 0; i < cardsDataCount; i++)
	{
		//获取数值
		int t_cardValue = GetCardValue(cardsData[i]);

		//累计积分
		if (t_cardValue == 5)
		{
			t_cardScore += 5;
			continue;
		}

		//累计积分
		if ((t_cardValue == 10) || (t_cardValue == 13))
		{
			t_cardScore += 10;
			continue;
		}
	}

	return t_cardScore;
}

/* 提取分牌 */
int WRBFGameLogic::GetScoreCards(const int cardsData[], int cardsDataCount, int scoreCards[], int maxCount)
{
	//变量定义
	int t_postion = 0;

	//扑克累计
	for (int i = 0; i < cardsDataCount; ++i)
	{
		//获取数值
		int t_cardValue = GetCardValue(cardsData[i]);

		//累计扑克
		if ((t_cardValue == 5) || (t_cardValue == 10) || (t_cardValue == 13))
		{
			scoreCards[t_postion++] = cardsData[i];
		}
	}

	return t_postion;
}

/* 对比扑克逻辑值 */
bool WRBFGameLogic::CompareCardData(int firstCard, int secondCard)
{
	//获取花色
	int t_logicColorNext = GetCardLogicColor(secondCard);
	int t_logicColorFirst = GetCardLogicColor(firstCard);   //逻辑花色

															//对比大小
	if ((t_logicColorNext == COLOR_NT) || (t_logicColorNext == t_logicColorFirst))
	{
		//获取大小
		int t_logicValueNext = GetCardLogicValue(secondCard);
		int t_logicValueFirst = GetCardLogicValue(firstCard);

		if (GetCommMainValue() != VALUE_ERROR &&
			t_logicValueNext == t_logicValueFirst  &&
			t_logicValueNext == GetCommMainValue() &&
			t_logicValueFirst == GetCommMainValue()) // 2是常主 两个人出的都是2
		{
			if (GetCardColor(secondCard) == GetMainColor())  //如果第二个人出的是主2,则第二个人的牌大
			{
				return true;
			}
		}

		//大小判断
		if (t_logicValueNext > t_logicValueFirst)
			return true;
	}
	else if (t_logicColorNext != t_logicColorFirst)
	{
		if (t_logicColorNext == m_mainColor)
		{
			return true;
		}
	}

	return false;
}

/* 对比出牌 */
int WRBFGameLogic::CompareOutCardArray(const int outCardArr[], int firstOutPos, int outPlayerCount)// 本轮每个玩家出的牌   第一个出牌的玩家  5个玩家
{
	//变量定义
	int t_winnerIndex = firstOutPos;

	//循环比较已经出牌的人
	//for (int i = t_winnerIndex; (i < WURENBAIFEN_PLAYER_COUNT + t_winnerIndex - 1) && (i < t_winnerIndex + outPlayerCount - 1); i++)
	for (int i = firstOutPos; (i < WURENBAIFEN_PLAYER_COUNT + firstOutPos - 1) && (i < firstOutPos + outPlayerCount - 1); i++)
	{
		int t_userIndex = (i + 1) % WURENBAIFEN_PLAYER_COUNT;

		//过滤无效牌值
		if (!IsValidCard(outCardArr[t_userIndex]))
		{
			continue;
		}

		//对比大小
		if (CompareCardData(outCardArr[t_winnerIndex], outCardArr[t_userIndex]))
		{
			t_winnerIndex = t_userIndex;
		}
	}

	return t_winnerIndex;
}

//根据牌逻辑花色提取相同逻辑花色扑克
//手牌 当前打出的牌  本轮上把出的牌
int WRBFGameLogic::DistillCardByColor(const int cardsData[], int cardsDataCount, int destCard, int resultCards[])
{
	//符合要求的牌的计数
	int t_resultCardsCount = 0;

	//目标牌的逻辑花色
	int t_destCardLogicColor = GetCardLogicColor(destCard);  //本轮上把出的花色  COLOR_NT  主花色

															 //按花色提取扑克
	for (int i = 0; i < cardsDataCount; i++)
	{
		//花色判断
		if (GetCardLogicColor(cardsData[i]) == t_destCardLogicColor)
		{
			resultCards[t_resultCardsCount++] = cardsData[i];
			continue;
		}

	}

	return t_resultCardsCount;
}

/* 校验跟牌玩家出牌 */
// 当前打出的牌  手牌  手牌数量  本轮上把出的牌  
bool WRBFGameLogic::EfficacyOutCard(int outCard, const int handCards[], int handCardsCount, int firstOutCard)
{
	//逻辑花色
	int t_outLogicColor = GetCardLogicColor(outCard);
	int t_firstLogicColor = GetCardLogicColor(firstOutCard);

	//同花色符合条件
	if (t_outLogicColor == t_firstLogicColor)
		return true;

	//检查手牌中是否含有同花色的牌
	int t_resultCards[WURENBAIFEN_HAND_CARDS_COUNT];

	//手牌中没有同花色符合条件  手牌 当前打出的牌  本轮上把出的牌
	//if (DistillCardByColor(handCards, outCard, firstOutCard, t_resultCards) == 0) 
	if (DistillCardByColor(handCards, handCardsCount, firstOutCard, t_resultCards) == 0)	//如果等于0,则没有相同的花色
	{
		//如果手里没有相同的花色,如果出的是主牌,则也可以
		//if (GetCardLogicColor(outCard) == COLOR_NT)
		return true;
	}

	return false;
}

//排序权位
bool WRBFGameLogic::UpDataSortRight()
{
	//设置权位
	for (BYTE i = 0; i < CountArray(m_sortRight); i++)
	{
		m_sortRight[i] = i * COLOR_RIGHT;
	}

	//主牌权位
	if (!IsValidColor(m_mainColor))
	{
		return false;
	}

	m_sortRight[m_mainColor >> 4] = 4 * COLOR_RIGHT;

	//调整权位
	switch (m_mainColor)
	{
	case COLOR_MEI_HUA:		//梅花
	{
		m_sortRight[COLOR_HEI_TAO >> 4] = COLOR_RIGHT;
		break;
	}
	case COLOR_HONG_TAO:	//红桃
	{
		m_sortRight[COLOR_FANG_KUAI >> 4] = 2 * COLOR_RIGHT;
		break;
	}
	}

	return true;
}