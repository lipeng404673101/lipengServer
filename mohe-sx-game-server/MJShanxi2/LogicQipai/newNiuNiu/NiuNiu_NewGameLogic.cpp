#include "StdAfx.h"
#include "NiuNiu_NewGameLogic.h"

/* 牌形倍数选择 */
static enum TYEP_TIME
{
	TYPE_TIME_NORMAL = 0,		//普通倍率
	TYPE_TIME_BOMB = 1,			//扫雷倍率
	TYPE_TIME_X4 = 2,			//新加倍率牛牛X4
};


//////////////////////////////////////////////////////////////////////////

//扑克数据（不含大小王）
BYTE NNGameLogic::m_cbCardListData[52] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 (48) A(49) - K(61)
};

//扑克数据（含大小王）
BYTE NNGameLogic::m_cbCardListData_LaiZi[54] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 (48) A(49) - K(61)
	0x4E,0x4F															//小王、大王
};

//////////////////////////////////////////////////////////////////////////

//构造函数
NNGameLogic::NNGameLogic()
{
	//初始化牛牛倍数选择
	this->m_niuNiuTimes = 1;

	//初始化允许癞子
	this->m_isAllowLaiZi = false;

	//初始化允许的特殊牌型
	this->m_isAllowKuaiLe = false;
	this->m_isAllowZhaDan = false;
	this->m_isAllowHuLu = false;
	this->m_isAllowTongHua = false;
	this->m_isAllowShunZi = false;
	this->m_isAllowWuHua = false;
	this->m_isAllowBiYi = false;
}

//析构函数
NNGameLogic::~NNGameLogic()
{
}

//设置牛牛倍数选择
void NNGameLogic::SetNiuNiuTimes(int niuNiuTimes)
{
	this->m_niuNiuTimes = niuNiuTimes;
}

//设置是否允许癞子
void NNGameLogic::SetAllowLaiZi(bool isAllowLaiZi)
{
	this->m_isAllowLaiZi = isAllowLaiZi;
}

//设置允许的特殊牌型
void NNGameLogic::SetAllowSecialType(bool isAllowBiYi, bool isAllowShunZi, bool isAllowWuHua, bool isAllowTongHua, bool isAllowHuLu, bool isAllowZhaDan, bool isAllowWuXiao, bool isAllowKuaiLe)
{
	this->m_isAllowBiYi = isAllowBiYi;
	this->m_isAllowShunZi = isAllowShunZi;
	this->m_isAllowWuHua = isAllowWuHua;
	this->m_isAllowTongHua = isAllowTongHua;
	this->m_isAllowHuLu = isAllowHuLu;
	this->m_isAllowZhaDan = isAllowZhaDan;
	this->m_isAllowWuXiao = isAllowWuXiao;
	this->m_isAllowKuaiLe = isAllowKuaiLe;
}

//判断手牌耗子数量
int NNGameLogic::GetHandCardsLaoZiCount(const BYTE handCards[], BYTE handCardsCount)
{
	int t_laiZiCount = 0;
	if (!m_isAllowLaiZi)
	{
		return 0;
	}
	for (int i = 0; i < handCardsCount; ++i)
	{
		if (handCards[i] == 0x4E || handCards[i] == 0x4F)
		{
			++t_laiZiCount;
		}
	}
	return t_laiZiCount;
}

//获取类型
BYTE NNGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL)
	{
		return 0xFF;
	}

	//无赖子计算手牌类型
	if (!m_isAllowLaiZi || GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 0)
	{
		return CalcCardType(cbCardData, cbCardCount, cbSortOriginalCards);
	}
	//1个癞子
	else if (GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 1)
	{
		BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

		//分离普通牌和癞子牌
		BYTE t_nomalCards[4];
		int t_nomaluCardsCount = 0;
		BYTE t_laiZiCard = 0;

		int t_maxType = -1;
		BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

		int t_currType = 0;
		BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];
		
		for (int i = 0; i < cbCardCount; ++i)
		{
			if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
			{
				t_laiZiCard = cbCardData[i];
			}
			else
			{
				t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
			}
		}
		
		BYTE t_color = GetCardColor(t_nomalCards[0]);
		for (BYTE i = (t_color + 1); i < t_color + 14; ++i)
		{
			//过滤手中牌
			/*if (i == t_nomalCards[0] || i == t_nomalCards[1] || i == t_nomalCards[2] || i == t_nomalCards[3])
			{
				continue;
			}*/

			memcpy(t_currCards, t_nomalCards, sizeof(t_nomalCards));
			t_currCards[4] = i;

			t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

			if (t_currType < t_maxType)
			{
				continue;
			}
			else if (t_currType > t_maxType || 
				(t_currType == t_maxType && t_maxType != -1 && CompareCard(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
			{
				t_maxType = t_currType;
				memcpy(t_maxCards, t_currCards, sizeof(t_currCards));

				//将万能牌替换回癞子
				if (cbSortOriginalCards != NULL)
				{
					//替换回王癞子
					int t_laiZiIndex = -1;
					int t_laiZiReplaceCard = 0;
					
					for (int n = cbCardCount - 1; n >= 0; --n)
					{
						if (t_cbSortOriginalCards[n] == i)
						{
							t_laiZiReplaceCard = t_cbSortOriginalCards[n];
							t_laiZiIndex = n;
							t_cbSortOriginalCards[n] = t_laiZiCard;
							break;
						}
					}
					
					for (int n = t_laiZiIndex + 1; n < cbCardCount - 1; ++n)
					{
						if (GetCardValue(t_laiZiReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
						{
							break;
						}

						if (GetCardValue(t_laiZiReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) && 
							GetCardValue(t_laiZiReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
						{
							for (int m = t_laiZiIndex; m < n; ++m)
							{
								t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
							}
							t_cbSortOriginalCards[n] = t_laiZiCard;
							break;
						}
						else if (GetCardValue(t_laiZiReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) && 
							n + 1 == cbCardCount - 1)
						{
							for (int m = t_laiZiIndex; m < n + 1; ++m)
							{
								t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
							}
							t_cbSortOriginalCards[n + 1] = t_laiZiCard;
							break;
						}
					}

					memcpy(cbSortOriginalCards, t_cbSortOriginalCards, sizeof(t_cbSortOriginalCards));
				}
			}
		}
		return t_maxType;
	}
	//2个癞子
	else if (GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 2)
	{
		BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

		//分离普通牌和癞子牌
		BYTE t_nomalCards[3];
		int t_nomaluCardsCount = 0;
		BYTE t_laiZiCard[2] = { 0 };

		int t_maxType = -1;
		BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

		int t_currType = 0;
		BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];

		for (int i = 0; i < cbCardCount; ++i)
		{
			if (cbCardData[i] == 0x4F)
			{
				t_laiZiCard[0] = cbCardData[i];
			}
			else if (cbCardData[i] == 0x4E)
			{
				t_laiZiCard[1] = cbCardData[i];
			}
			else
			{
				t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
			}
		}

		BYTE t_color = GetCardColor(t_nomalCards[0]);
		for (int i = t_color + 1; i < t_color + 14; ++i)
		{
			for (int j = i; j < t_color + 14; ++j)
			{
				memcpy(t_currCards, t_nomalCards, sizeof(BYTE) * 3);
				t_currCards[3] = i;
				t_currCards[4] = j;

				t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

				if (t_currType < t_maxType)
				{
					continue;
				}
				else if (t_currType > t_maxType ||
					(t_currType == t_maxType && t_maxType != -1 && CompareCard(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
				{
					t_maxType = t_currType;
					memcpy(t_maxCards, t_currCards, sizeof(t_currCards));

					//将万能牌替换回癞子牌
					if (cbSortOriginalCards != NULL)
					{
						//从有到左依次使用小王和大王替换回来
						int t_daWangCount = 1, t_xiaoWangCount = 1;
						int t_daWangIndex = -1, t_xiaoWangIndex = -1;
						int t_tmpDaWangCard = 0, t_tmpXiaoWangCard = 0;
						int t_daWangReplaceCard = 0, t_xiaoWangReplaceCard = 0;

						//查找大王替换的牌位置
						for (int n = 0; n < cbCardCount; ++n)
						{
							if (t_cbSortOriginalCards[n] == i)
							{
								t_daWangIndex = n;
							}
						}
						//查找小王替换的牌位置
						for (int n = 0; n < cbCardCount; ++n)
						{
							if (t_cbSortOriginalCards[n] == j && n != t_daWangIndex)
							{
								t_xiaoWangIndex = n;
							}
						}
						//将大王排在小王前面
						if (t_daWangIndex > t_xiaoWangIndex)
						{
							int t_index = t_daWangIndex;
							t_daWangIndex = t_xiaoWangIndex;
							t_xiaoWangIndex = t_index;
						}
						//替换为大王
						t_daWangReplaceCard = t_cbSortOriginalCards[t_daWangIndex];
						t_cbSortOriginalCards[t_daWangIndex] = 0x4F;
						t_tmpDaWangCard = t_cbSortOriginalCards[t_daWangIndex];
						//替换为小王
						t_xiaoWangReplaceCard = t_cbSortOriginalCards[t_xiaoWangIndex];
						t_cbSortOriginalCards[t_xiaoWangIndex] = 0x4E;
						t_tmpXiaoWangCard = t_cbSortOriginalCards[t_xiaoWangIndex];

						//大小王充当相等的牌
						if (t_daWangReplaceCard == t_xiaoWangReplaceCard)
						{
							for (int n = t_daWangIndex + 2; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}
								if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_daWangIndex; m < n-1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 2];
									}
									t_cbSortOriginalCards[n-1] = t_tmpDaWangCard;
									t_cbSortOriginalCards[n] = t_tmpXiaoWangCard;
									break;
								}
								else if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_daWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 2];
									}
									t_cbSortOriginalCards[n] = t_tmpDaWangCard;
									t_cbSortOriginalCards[n + 1] = t_tmpXiaoWangCard;
									break;
								}
							}
						}
						else
						{
							//排序大王
							for (int n = t_daWangIndex + 1; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}

								if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_daWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n] = t_tmpDaWangCard;
									break;
								}
								else if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_daWangIndex; m < n + 1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n + 1] = t_tmpDaWangCard;
									break;
								}
							}

							//排序小王
							for (int n = t_xiaoWangIndex + 1; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}

								if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_xiaoWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_xiaoWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n] = t_tmpXiaoWangCard;
								}
								else if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_xiaoWangIndex; m < n + 1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n + 1] = t_tmpXiaoWangCard;
								}
							}
						}

						memcpy(cbSortOriginalCards, t_cbSortOriginalCards, sizeof(t_cbSortOriginalCards));
					}
				}
			}
		}
		return t_maxType;
	}
}

//获取一个癞子的牌型（cbSortCalcCards[] 为癞子最优解牌型）
BYTE NNGameLogic::GetOneLaiZiCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE& bestLaiZiCard, BYTE cbSortCalcCards[])
{
	BYTE t_bestLaiZiCard = 0;
	BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

	//分离普通牌和癞子牌
	BYTE t_nomalCards[4];
	int t_nomaluCardsCount = 0;
	BYTE t_laiZiCard = 0;

	int t_maxType = -1;
	BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

	int t_currType = 0;
	BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];

	for (int i = 0; i < cbCardCount; ++i)
	{
		if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
		{
			t_laiZiCard = cbCardData[i];
		}
		else
		{
			t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
		}
	}

	BYTE t_color = GetCardColor(t_nomalCards[0]);
	for (BYTE i = (t_color + 1); i < t_color + 14; ++i)
	{
		//过滤手中牌
		/*if (i == t_nomalCards[0] || i == t_nomalCards[1] || i == t_nomalCards[2] || i == t_nomalCards[3])
		{
		continue;
		}*/

		memcpy(t_currCards, t_nomalCards, sizeof(t_nomalCards));
		t_currCards[4] = i;

		t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

		if (t_currType < t_maxType)
		{
			continue;
		}
		else if (t_currType > t_maxType ||
			(t_currType == t_maxType && t_maxType != -1 && CompareCard_Sample(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
		{
			t_bestLaiZiCard = i;
			t_maxType = t_currType;
			memcpy(t_maxCards, t_currCards, sizeof(t_currCards));	
		}
	}

	bestLaiZiCard = t_bestLaiZiCard;
	if (cbSortCalcCards)
	{
		memcpy(cbSortCalcCards, t_maxCards, sizeof(t_maxCards));
	}
	return t_maxType;
}

//具体计算牌的类型
BYTE NNGameLogic::CalcCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL)
	{
		return 0xFF;
	}

	//存储原始手牌副本
	BYTE t_CardData[NIU_NIU_POKER_MAX_COUNT];
	memcpy(t_CardData, cbCardData, sizeof(t_CardData));

	//存储牌的逻辑值(J\Q\K = 10)
	BYTE t_logicValue[NIU_NIU_POKER_MAX_COUNT];
	//与t_logicValue顺序相同的手牌排序
	BYTE t_sortCardByLogic[NIU_NIU_POKER_MAX_COUNT];

	//存储牌值（A = 1，J = 11，Q = 12, K = 13）
	BYTE t_cardValue[NIU_NIU_POKER_MAX_COUNT];
	//与t_cardValue顺序相同的手牌顺序
	BYTE t_sortCardByValue[NIU_NIU_POKER_MAX_COUNT];

	//逻辑牌值的和
	BYTE bSum = 0;
	for (BYTE i = 0; i < cbCardCount; ++i)
	{
		t_logicValue[i] = GetCardLogicValue(t_CardData[i]);
		t_sortCardByLogic[i] = t_CardData[i];
		bSum += t_logicValue[i];

		t_cardValue[i] = GetCardValue(t_CardData[i]);
		t_sortCardByValue[i] = t_CardData[i];
	}
	SortCardByCardData(t_logicValue, cbCardCount, t_sortCardByLogic);
	SortCardByCardData(t_cardValue, cbCardCount, t_sortCardByValue);


	//顺子牌形判断
	bool isPlusOne = TRUE;    //顺子牌形标志：true:顺子，false：不是顺子
	BYTE bLast = cbCardCount - 1;
	for (BYTE i = 0; i < bLast; i++)
	{
		if (t_cardValue[i] - t_cardValue[i + 1] != 1)
		{
			isPlusOne = FALSE;
		}
	}
	if (t_cardValue[0] == 13 && t_cardValue[1] == 12 && t_cardValue[2] == 11 && t_cardValue[3] == 10 && t_cardValue[4] == 1)
	{
		isPlusOne = TRUE;

		//排序成AKQJ10
		BYTE t_cards = t_cardValue[4];
		for (int n = 3; n >= 0; --n)
		{
			t_sortCardByValue[n + 1] = t_sortCardByValue[n];
		}
		t_sortCardByValue[0] = t_cards;
	}

	// 一条龙牌型
	//if (t_logicValue[0] == 5 && isPlusOne) return OX_DRAGON;

	//同花牌形判断
	bool isSameColor = true;   //同花牌形标志：true:同花，false：不是同花
	BYTE firstCardColor = GetCardColor(cbCardData[0]);
	for (BYTE i = 1; i < cbCardCount; ++i)
	{
		if (GetCardColor(cbCardData[i]) != firstCardColor)
		{
			isSameColor = false;
			break;
		}
	}

	//炸弹牌形判断、葫芦、比翼(统计每张牌的数量)
	//牌形点数数组，计数用
	int cardValueCount[14];      
	memset(cardValueCount, 0, sizeof(cardValueCount));
	//返回最大相等数量,用于判断炸弹和葫芦牛,4：炸弹牛，3：葫芦牛
	int maxSameCount = 0;
	//返回对子数量
	int twoSameCount = 0;

	for (BYTE i = 0; i < cbCardCount; ++i)
	{
		++cardValueCount[t_cardValue[i]];
	}

	for (BYTE i = 1; i < 14; ++i)
	{
		if (cardValueCount[i] > maxSameCount)
		{
			maxSameCount = cardValueCount[i];
		}
		if (2 == cardValueCount[i])
		{
			++twoSameCount;
		}
	}

	// 五小牛牌形判断
	bool isWuXiaoNiu = FALSE;
	if (bSum <= 10)
	{
		isWuXiaoNiu = TRUE;
	}

	//五花牛判断
	bool isWuHuaNiu = true;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (GetCardValue(cbCardData[i]) < 10)
		{
			isWuHuaNiu = false;
			break;
		}
	}

	//汇总判断，按照牌形大小返回

	//同花顺（10倍）
	if (m_isAllowKuaiLe && (TRUE == isPlusOne && TRUE == isSameColor))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_TONGHUASHUN;
	}

	//五小牛（9倍） 
	else if (m_isAllowWuHua && isWuXiaoNiu)
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_FIVESMALL;
	}

	//炸弹（8倍）
	else if (m_isAllowZhaDan && 4 <= maxSameCount)
	{
		if (t_cardValue[0] != t_cardValue[1])
		{
			BYTE t_singleCard = t_sortCardByValue[0];
			for (int n = 0; n < cbCardCount - 1; ++n)
			{
				t_sortCardByValue[n] = t_sortCardByValue[n + 1];
			}
			t_sortCardByValue[4] = t_singleCard;
		}

		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_FOUR_SAME;
	}

	//葫芦牛（7倍）
	else if (m_isAllowHuLu && (3 == maxSameCount && 1 == twoSameCount))
	{
		if (t_cardValue[2] != t_cardValue[0])
		{
			BYTE t_towCard[2];
			t_towCard[0] = t_sortCardByValue[0];
			t_towCard[1] = t_sortCardByValue[1];
			for (int n = 0; n < cbCardCount - 2; ++n)
			{
				t_sortCardByValue[n] = t_sortCardByValue[n + 2];
			}
			t_sortCardByValue[3] = t_towCard[0];
			t_sortCardByValue[4] = t_towCard[1];
		}

		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_THREE_SAME;
	}

	//同花牛（6倍）
	else if (m_isAllowTongHua && (TRUE == isSameColor))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_COLOR_SAME;
	}

	//五花牛（5倍）
	else if (m_isAllowWuHua && TRUE == isWuHuaNiu)
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_WUHUANIU;
	}

	//顺子牛（5倍）
	else if (m_isAllowShunZi && (TRUE == isPlusOne))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_SHUNZINIU;
	}


	//判断牛牛数，普通牌形（比翼牛在此判断）
	int t_oxNum = OX_VALUE0;
	for (int i = cbCardCount - 1; i >= 1; --i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			if ((bSum - t_logicValue[i] - t_logicValue[j]) % 10 == 0)
			{
				t_oxNum = ((t_logicValue[i] + t_logicValue[j]) > 10) ? (t_logicValue[i] + t_logicValue[j] - 10) : (t_logicValue[i] + t_logicValue[j]);

				//牛几牌型
				if (cbSortOriginalCards != NULL)
				{
					int t_oxCount = 0;
					for (int n = 0; n < cbCardCount; ++n)
					{
						if (n == j)
						{
							cbSortOriginalCards[3] = t_sortCardByLogic[j];
						}
						else if (n == i)
						{
							cbSortOriginalCards[4] = t_sortCardByLogic[i];
						}
						else
						{
							cbSortOriginalCards[t_oxCount++] = t_sortCardByLogic[n];
						}
					}
				}

				//比翼牛：牛牛牌型：两个对子 + 一张单牌
				if (t_oxNum == 10 && m_isAllowBiYi && twoSameCount == 2)
				{
					return OX_BIYINIU;
				}

				return t_oxNum;
			}
		}
	}
	/*
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - t_logicValue[i] - t_logicValue[j]) % 10 == 0)
			{
				t_oxNum = ((t_logicValue[i] + t_logicValue[j]) > 10) ? (t_logicValue[i] + t_logicValue[j] - 10) : (t_logicValue[i] + t_logicValue[j]);

				//牛几牌型
				if (cbSortOriginalCards != NULL)
				{
					int t_oxCount = 0;
					for (int n = 0; n < cbCardCount; ++n)
					{
						if (n == i)
						{
							cbSortOriginalCards[3] = t_sortCardByLogic[i];
						}
						else if (n == j)
						{
							cbSortOriginalCards[4] = t_sortCardByLogic[j];
						}
						else
						{
							cbSortOriginalCards[t_oxCount++] = t_sortCardByLogic[n];
						}
					}
				}

				//比翼牛：牛牛牌型：两个对子 + 一张单牌
				if (t_oxNum == 10 && m_isAllowBiYi && twoSameCount == 2)
				{
					return OX_BIYINIU;
				}

				return t_oxNum;
			}
		}
	}
	*/
	if (cbSortOriginalCards != NULL)
	{
		memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
	}
	return OX_VALUE0;
}

//获取倍数
BYTE NNGameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)
	{
		return 0;
	}

	//计算牛牛倍数
	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT);

	//1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
	if (m_niuNiuTimes == 1)
	{
		return GetTimes_x3(cbCardData, cbCardCount, bTimes);
	}

	//2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
	else if (m_niuNiuTimes == 2)
	{
		return GetTimes_x4(cbCardData, cbCardCount, bTimes);
	}

	return 0;
}

//1、牛牛x3  牛九x2  牛八x2  牛七-无牛x1
BYTE NNGameLogic::GetTimes_x3(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes)
{
	// 无牛 - 牛七: 1倍
	if (bTimes <= 7)return 1;
	// 牛八 - 牛九: 2倍
	else if (bTimes >= 8 && bTimes <= 9) return 2;
	// 牛牛: 3倍
	else if (bTimes == 10)return 3;
	//比翼牛：4倍
	else if (bTimes == OX_BIYINIU) return 4;
	//顺子牛：5倍
	else if (bTimes == OX_SHUNZINIU) return 5;
	//五花牛 5倍
	else if (bTimes == OX_WUHUANIU) return 5;
	//同花牛：6倍
	else if (bTimes == OX_COLOR_SAME) return 6;
	//葫芦牛：7倍
	else if (bTimes == OX_THREE_SAME) return 7;
	//炸弹牛：8倍
	else if (bTimes == OX_FOUR_SAME)return 8;
	//五小牛：9倍
	else if (bTimes == OX_FIVESMALL) return 9;
	//同花顺牛：10倍
	else if (bTimes == OX_TONGHUASHUN)return 10;

	return 0;
}

//2、牛牛x4  牛九x3  牛八x2  牛七x2  牛六 - 无牛x1
BYTE NNGameLogic::GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes)
{
	// 无牛 - 牛六: 1倍
	if (bTimes <= 6)return 1;
	// 牛七 - 牛八: 2倍
	else if (bTimes >= 7 && bTimes <= 8) return 2;
	// 牛九：3倍
	else if (bTimes == 9) return 3;
	// 牛牛: 4倍
	else if (bTimes == 10) return 4;
	//比翼牛：4倍
	else if (bTimes == OX_BIYINIU) return 4;
	//顺子牛：5倍
	else if (bTimes == OX_SHUNZINIU) return 5;
	//五花牛 5倍
	else if (bTimes == OX_WUHUANIU) return 5;
	//同花牛：6倍
	else if (bTimes == OX_COLOR_SAME) return 6;
	//葫芦牛：7倍
	else if (bTimes == OX_THREE_SAME) return 7;
	//炸弹牛：8倍
	else if (bTimes == OX_FOUR_SAME) return 8;
	//五小牛：9倍
	else if (bTimes == OX_FIVESMALL) return 9;
	//同花顺牛：10倍
	else if (bTimes == OX_TONGHUASHUN)return 10;

	return 0;
}

//获取牛牛
bool NNGameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置变量
	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT], bTempData[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bTempData, cbCardData, sizeof(bTempData));
	BYTE bSum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		bTemp[i] = GetCardLogicValue(cbCardData[i]);
		bSum += bTemp[i];
	}

	//查找牛牛
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				BYTE bCount = 0;
				for (BYTE k = 0; k<cbCardCount; k++)
				{
					if (k != i && k != j)
					{
						cbCardData[bCount++] = bTempData[k];
					}
				}

				cbCardData[bCount++] = bTempData[i];
				cbCardData[bCount++] = bTempData[j];

				return true;
			}
		}
	}

	return false;
}

//获取整数
bool NNGameLogic::IsIntValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE sum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		sum += GetCardLogicValue(cbCardData[i]);
	}

	return (sum % 10 == 0);
}

//判断4张牌的牛牛数
int NNGameLogic::GetCardsOxNumber(const BYTE handCards[], int handCardsCount)
{
	if (handCards == NULL || handCardsCount < 4)
	{
		return -1;
	}

	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i< handCardsCount; i++)
	{
		bTemp[i] = GetCardLogicValue(handCards[i]);
		bSum += bTemp[i];
	}
	SortCardList(bTemp, handCardsCount);


	int t_oxNum = OX_VALUE0;
	for (BYTE i = 0; i < handCardsCount - 1; i++)
	{
		for (BYTE j = i + 1; j < handCardsCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				t_oxNum = ((bTemp[i] + bTemp[j]) > 10) ? (bTemp[i] + bTemp[j] - 10) : (bTemp[i] + bTemp[j]);
				return t_oxNum;
			}
		}
	}
	return t_oxNum;
}

//按扑克逻辑值排列扑克
void NNGameLogic::SortCardByLogicValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}
	//存储牌的点数
	BYTE cbLogicValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);
	}

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbLogicValue[i] < cbLogicValue[i + 1]) ||
				((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i + 1];
				cbLogicValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//按扑克牌值排列扑克（J=11  Q=12  K=13）
void NNGameLogic::SortCardByCardValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}
	//存储牌的点数
	BYTE cbCardValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbCardValue[i] = GetCardValue(cbCardData[i]);
	}

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbCardValue[i] < cbCardValue[i + 1]) ||
				((cbCardValue[i] == cbCardValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbCardValue[i];
				cbCardValue[i] = cbCardValue[i + 1];
				cbCardValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//根据cbCardData值排列扑克
void NNGameLogic::SortCardByCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if (cbCardData[i] < cbCardData[i + 1] ||
				(cbCardData[i] == cbCardData[i + 1] && cbSortOriginalCards && GetCardValue(cbSortOriginalCards[i]) < GetCardValue(cbSortOriginalCards[i + 1])) ||
				(cbCardData[i] == cbCardData[i + 1] && cbSortOriginalCards && GetCardValue(cbSortOriginalCards[i]) == GetCardValue(cbSortOriginalCards[i + 1]) && cbSortOriginalCards[i] < cbSortOriginalCards[i + 1]))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//排列扑克
void NNGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}

	//存储牌的点数
	BYTE cbCardValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbCardValue[i] = GetCardValue(cbCardData[i]);
	}

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbCardValue[i] < cbCardValue[i + 1]) ||
				((cbCardValue[i] == cbCardValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbCardValue[i];
				cbCardValue[i] = cbCardValue[i + 1];
				cbCardValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//洗牌
void NNGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	if (m_isAllowLaiZi || cbBufferCount == 54)
	{
		RandCardList_laizi(cbCardBuffer, cbBufferCount);
	}
	else
	{
		RandCardList_normal(cbCardBuffer, cbBufferCount);
	}
}

//洗牌-不含大小王
void NNGameLogic::RandCardList_normal(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (CountArray(m_cbCardListData) - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListData) - bRandCount];
	} while (bRandCount < cbBufferCount);

	return;
}

//洗牌-含大小王
void NNGameLogic::RandCardList_laizi(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData_LaiZi)];
	CopyMemory(cbCardData, m_cbCardListData_LaiZi, sizeof(m_cbCardListData_LaiZi));

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (CountArray(m_cbCardListData_LaiZi) - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListData_LaiZi) - bRandCount];
	} while (bRandCount < cbBufferCount);

	return;
}

//逻辑数值
BYTE NNGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor = GetCardColor(cbCardData);
	BYTE bCardValue = GetCardValue(cbCardData);

	//转换数值
	return (bCardValue > 10) ? (10) : bCardValue;
}

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
bool NNGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX)
{
	//闲家无牛，则闲家输
	/*if (XianOX == 0)
	{
		return false;
	}*/

	//两家牛牛类型不相等，直接可以比较出大小
	if (XianOX != ZhuangOX)
	{
		return (XianOX > ZhuangOX);
	}

	//计算两家的手牌的癞子数
	BYTE t_XianLaiZiCount = GetHandCardsLaoZiCount(cbFirstData, cbCardCount);
	BYTE t_ZhuangLaiZiCount = GetHandCardsLaoZiCount(cbNextData, cbCardCount);

	//相同牌型，谁有癞子谁输
	if (XianOX == ZhuangOX && (t_XianLaiZiCount > 0 || t_ZhuangLaiZiCount > 0))
	{
		//癞子数不相等，癞子少的输
		if (t_XianLaiZiCount != t_ZhuangLaiZiCount)
		{
			return t_XianLaiZiCount < t_ZhuangLaiZiCount;
		}

		//癞子数量相同，只能是每个人一个癞子
		BYTE t_xianLaiZi = 0, t_zhuangLaiZi = 0;			//癞子牌（大王、小王）
		BYTE t_xianBestLaiZi = 0, t_zhuangBestLaiZi = 0;	//癞子替换的最优牌值（不是大小王）
		int t_xianLaiZiIndex = 0, t_zhuangLaiZiIndex = 0;	//癞子在经排序后的最优手牌中的位置（0-4）
		BYTE t_xianBestCards[NIU_NIU_POKER_MAX_COUNT], t_zhuangBestCards[NIU_NIU_POKER_MAX_COUNT];  //最优手牌
		memset(t_xianBestCards, 0, sizeof(t_xianBestCards));
		memset(t_zhuangBestCards, 0, sizeof(t_zhuangBestCards));

		//计算癞子最优牌型，以及癞子最优值
		GetOneLaiZiCardType(cbFirstData, cbCardCount, t_xianBestLaiZi, t_xianBestCards);
		GetOneLaiZiCardType(cbNextData, cbCardCount, t_zhuangBestLaiZi, t_zhuangBestCards);

		//排序癞子最优牌型的手牌
		SortCardList(t_xianBestCards, cbCardCount);
		SortCardList(t_zhuangBestCards, cbCardCount);

		//手牌排序后的最大值
		BYTE cbFirstMaxValue = GetCardValue(t_xianBestCards[0]);
		BYTE cbNextMaxValue = GetCardValue(t_zhuangBestCards[0]);

		//查找癞子的位置，以及庄家 闲家的癞子牌
		for (int i = 0; i < cbCardCount; ++i)
		{
			if (t_xianBestCards[i] == t_xianBestLaiZi)
			{
				t_xianLaiZiIndex = i;
			}
			if (t_zhuangBestCards[i] == t_zhuangBestLaiZi)
			{
				t_zhuangLaiZiIndex = i;
			}

			if (cbFirstData[i] == 0x4F || cbFirstData[i] == 0x4E)
			{
				t_xianLaiZi = cbFirstData[i];
			}
			if (cbNextData[i] == 0x4F || cbNextData[i] == 0x4E)
			{
				t_zhuangLaiZi = cbNextData[i];
			}
		}

		//两个癞子位置不相同
		if (t_zhuangLaiZiIndex != t_xianLaiZiIndex)
		{
			//两个最大值数值不同
			if (cbFirstMaxValue != cbNextMaxValue)
			{
				return cbFirstMaxValue > cbNextMaxValue;
			}

			//数值相等，有一个第一位是王癞子
			if (t_zhuangLaiZiIndex == 0 || t_xianLaiZiIndex == 0)
			{
				//庄家手牌第一张是癞子，闲家赢
				if (t_zhuangLaiZiIndex == 0) return true;
				else return false;
			}

			//比较颜色
			return GetCardColor(t_xianBestCards[0]) > GetCardColor(t_zhuangBestCards[0]);
		}

		//两个王癞子位置相同
		//两个癞子都是第一张牌
		if (t_zhuangLaiZiIndex == 0 && t_xianLaiZiIndex == 0)
		{
			if (t_xianLaiZi == 0x4F) return true;
			else return false;
		}
		
		//两张癞子都不是第一张牌
		if (cbFirstMaxValue != cbNextMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//比较颜色
		return GetCardColor(t_xianBestCards[0]) > GetCardColor(t_zhuangBestCards[0]);
	}

	//类型相同，无癞子，继续判断
	//按先数值，再花色，排序大小
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//炸弹、葫芦牌型比较
	if (ZhuangOX == OX_FOUR_SAME || ZhuangOX == OX_THREE_SAME)
	{
		return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2]) > GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);
	}

	//比翼牛牌型比较
	if (ZhuangOX == OX_BIYINIU)
	{
		//找出最大的对子
		int t_xianCardCountByValue[14];
		int t_zhuangCardCountByValue[14];
		int t_xianMaxCardValue = 0;
		int t_zhuangMaxCardValue = 0;
		int t_xianMaxCard = 0;
		int t_zhuangMaxCard = 0;
		memset(t_xianCardCountByValue, 0, sizeof(t_xianCardCountByValue));
		memset(t_zhuangCardCountByValue, 0, sizeof(t_zhuangCardCountByValue));
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			++t_xianCardCountByValue[GetCardValue(bFirstTemp[i])];
			++t_zhuangCardCountByValue[GetCardValue(bNextTemp[i])];
		}

		for (size_t i = 0; i < 14; ++i)
		{
			if (t_xianCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_xianMaxCardValue)
			{
				t_xianMaxCardValue = i;
			}
			if (t_zhuangCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_zhuangMaxCardValue)
			{
				t_zhuangMaxCardValue = i;
			}
		}
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			if (t_xianMaxCardValue == GetCardValue(bFirstTemp[i]) && bFirstTemp[i] > t_xianMaxCard)
			{
				t_xianMaxCard = bFirstTemp[i];
			}
			if (t_zhuangMaxCardValue == GetCardValue(bNextTemp[i]) && bNextTemp[i] > t_zhuangMaxCard)
			{
				t_zhuangMaxCard = bNextTemp[i];
			}
		}


		//比较数值
		BYTE cbNextMaxValue = GetCardValue(t_zhuangMaxCard);
		BYTE cbFirstMaxValue = GetCardValue(t_xianMaxCard);
		if (cbNextMaxValue != cbFirstMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//比较颜色
		return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

		return false;
	}

	//顺子、同花顺牌型比较
	if (ZhuangOX == OX_TONGHUASHUN || ZhuangOX == OX_SHUNZINIU)
	{
		BYTE bShunFirstTemp[NIU_NIU_POKER_MAX_COUNT], bShunNextTemp[NIU_NIU_POKER_MAX_COUNT];
		for (int i = 0; i < cbCardCount; ++i)
		{
			bShunFirstTemp[i] = GetCardValue(cbFirstData[i]) == 1 ? cbFirstData[i] + 13 : cbFirstData[i];
			bShunNextTemp[i] = GetCardValue(cbNextData[i]) == 1 ? cbNextData[i] + 13 : cbNextData[i];
		}
		SortCardList(bShunFirstTemp, cbCardCount);
		SortCardList(bShunNextTemp, cbCardCount);

		if (GetCardValue(bShunFirstTemp[0]) == 14 && (GetCardValue(bShunFirstTemp[0]) - GetCardValue(bShunFirstTemp[1]) == 1))
		{
			CopyMemory(bFirstTemp, bShunFirstTemp, cbCardCount);
		}
		if (GetCardValue(bShunNextTemp[0]) == 14 && (GetCardValue(bShunNextTemp[0]) - GetCardValue(bShunNextTemp[1]) == 1))
		{
			CopyMemory(bNextTemp, bShunNextTemp, cbCardCount);
		}
	}

	//比较数值
	BYTE cbNextMaxValue = GetCardValue(bNextTemp[0]);
	BYTE cbFirstMaxValue = GetCardValue(bFirstTemp[0]);
	if (cbNextMaxValue != cbFirstMaxValue)
	{
		return cbFirstMaxValue > cbNextMaxValue;
	}

	//比较颜色
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//癞子最优解使用的比较函数
bool NNGameLogic::CompareCard_Sample(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX)
{
	//闲家无牛，则闲家输
	/*if (XianOX == 0)
	{
	return false;
	}*/

	//两家牛牛类型不相等，直接可以比较出大小
	if (XianOX != ZhuangOX)
	{
		return (XianOX > ZhuangOX);
	}

	//类型相同，继续判断
	//按先数值，再花色，排序大小
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//炸弹、葫芦牌型比较
	if (ZhuangOX == OX_FOUR_SAME || ZhuangOX == OX_THREE_SAME)
	{
		return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2]) > GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);
	}

	//比翼牛牌型比较
	if (ZhuangOX == OX_BIYINIU)
	{
		//找出最大的对子
		int t_xianCardCountByValue[14];
		int t_zhuangCardCountByValue[14];
		int t_xianMaxCardValue = 0;
		int t_zhuangMaxCardValue = 0;
		int t_xianMaxCard = 0;
		int t_zhuangMaxCard = 0;
		memset(t_xianCardCountByValue, 0, sizeof(t_xianCardCountByValue));
		memset(t_zhuangCardCountByValue, 0, sizeof(t_zhuangCardCountByValue));
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			++t_xianCardCountByValue[GetCardValue(bFirstTemp[i])];
			++t_zhuangCardCountByValue[GetCardValue(bNextTemp[i])];
		}

		for (size_t i = 0; i < 14; ++i)
		{
			if (t_xianCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_xianMaxCardValue)
			{
				t_xianMaxCardValue = i;
			}
			if (t_zhuangCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_zhuangMaxCardValue)
			{
				t_zhuangMaxCardValue = i;
			}
		}
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			if (t_xianMaxCardValue == GetCardValue(bFirstTemp[i]) && bFirstTemp[i] > t_xianMaxCard)
			{
				t_xianMaxCard = bFirstTemp[i];
			}
			if (t_zhuangMaxCardValue == GetCardValue(bNextTemp[i]) && bNextTemp[i] > t_zhuangMaxCard)
			{
				t_zhuangMaxCard = bNextTemp[i];
			}
		}


		//比较数值
		BYTE cbNextMaxValue = GetCardValue(t_zhuangMaxCard);
		BYTE cbFirstMaxValue = GetCardValue(t_xianMaxCard);
		if (cbNextMaxValue != cbFirstMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//比较颜色
		return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

		return false;
	}

	//顺子、同花顺牌型比较
	if (ZhuangOX == OX_TONGHUASHUN || ZhuangOX == OX_SHUNZINIU)
	{
		BYTE bShunFirstTemp[NIU_NIU_POKER_MAX_COUNT], bShunNextTemp[NIU_NIU_POKER_MAX_COUNT];
		for (int i = 0; i < cbCardCount; ++i)
		{
			bShunFirstTemp[i] = GetCardValue(cbFirstData[i]) == 1 ? cbFirstData[i] + 13 : cbFirstData[i];
			bShunNextTemp[i] = GetCardValue(cbNextData[i]) == 1 ? cbNextData[i] + 13 : cbNextData[i];
		}
		SortCardList(bShunFirstTemp, cbCardCount);
		SortCardList(bShunNextTemp, cbCardCount);

		if (GetCardValue(bShunFirstTemp[0]) == 14 && (GetCardValue(bShunFirstTemp[0]) - GetCardValue(bShunFirstTemp[1]) == 1))
		{
			CopyMemory(bFirstTemp, bShunFirstTemp, cbCardCount);
		}
		if (GetCardValue(bShunNextTemp[0]) == 14 && (GetCardValue(bShunNextTemp[0]) - GetCardValue(bShunNextTemp[1]) == 1))
		{
			CopyMemory(bNextTemp, bShunNextTemp, cbCardCount);
		}
	}

	//比较数值
	BYTE cbNextMaxValue = GetCardValue(bNextTemp[0]);
	BYTE cbFirstMaxValue = GetCardValue(bFirstTemp[0]);
	if (cbNextMaxValue != cbFirstMaxValue)
	{
		return cbFirstMaxValue > cbNextMaxValue;
	}

	//比较颜色
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//////////////////////////////////////////////////////////////////////////
