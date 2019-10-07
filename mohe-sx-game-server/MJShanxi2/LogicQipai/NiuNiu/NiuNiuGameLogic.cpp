#include "StdAfx.h"
#include "NiuNiuGameLogic.h"

/* 牌形倍数选择 */
static enum TYEP_TIME
{
	TYPE_TIME_NORMAL = 0,		//普通倍率
	TYPE_TIME_BOMB = 1,			//扫雷倍率
	TYPE_TIME_X4 = 2,			//新加倍率牛牛X4
};

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE NGameLogic::m_cbCardListData[52] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 (48) A(49) - K(61)
};

//////////////////////////////////////////////////////////////////////////

//构造函数
NGameLogic::NGameLogic()
{
}

//析构函数
NGameLogic::~NGameLogic()
{
}

//获取类型
BYTE NGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec)
{

	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT];
	//存储牌值（A = 1）
	BYTE bCardsTemp[NIU_NIU_POKER_MAX_COUNT];
	//存储牌值（A = 14）
	BYTE bShunCardTemp[NIU_NIU_POKER_MAX_COUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		bTemp[i] = GetCardLogicValue(cbCardData[i]);
		bCardsTemp[i] = GetCardValue(cbCardData[i]);
		bShunCardTemp[i] = GetCardValue(cbCardData[i]) == 1 ? 14 : GetCardValue(cbCardData[i]);
		bSum += bTemp[i];
	}
	SortCardList(bTemp, cbCardCount);
	SortCardList(bCardsTemp, cbCardCount);
	SortCardList(bShunCardTemp, cbCardCount);

	//花样玩法特殊牌形
	if (cbIsSpec)
	{
		//顺子牌形判断
		bool isPlusOne = TRUE;    //顺子牌形标志：true:顺子，false：不是顺子
		BYTE bLast = cbCardCount - 1;
		for (BYTE i = 0; i < bLast; i++)
		{
			if (bCardsTemp[i] - bCardsTemp[i + 1] != 1)
			{
				isPlusOne = FALSE;
			}
		}
		if (bCardsTemp[0] == 13 && bCardsTemp[1] == 12 && bCardsTemp[2] == 11 && bCardsTemp[3] == 10 && bCardsTemp[4] == 1)
		{
			isPlusOne = TRUE;
		}
		// 一条龙牌型
		//if (bTemp[0] == 5 && isPlusOne) return OX_DRAGON;

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

		//炸弹牌形判断
		BYTE cardValueCount[13] = { 0 };      //牌形点数数组，计数用
		BYTE maxSameCount = 1;              //返回最大相等数量,用于判断炸弹和葫芦牛,4：炸弹牛，3：葫芦牛
		BOOL twoSameCount = FALSE;
		for (BYTE i = 0; i < cbCardCount; ++i)
		{
			++cardValueCount[GetCardValue(cbCardData[i]) - 1];
		}
		for (BYTE i = 0; i < 13; ++i)
		{
			if (cardValueCount[i] > maxSameCount)
			{
				maxSameCount = cardValueCount[i];
			}
			if (2 == cardValueCount[i])
			{
				twoSameCount = TRUE;
			}
		}

		////炸弹牌型
		//BYTE cTemp[NIU_NIU_POKER_MAX_COUNT];
		//for (BYTE i = 0; i < cbCardCount; i++)
		//{
		//	cTemp[i] = GetCardValue(cbCardData[i]);
		//}
		//BYTE bSameCount = 1;
		//for (BYTE i = 0; i < 2; ++i)
		//{
		//	for (BYTE j = i + 1; j < cbCardCount; ++j)
		//	{
		//		if (cTemp[i] == cTemp[j])
		//			bSameCount++;
		//	}
		//	if (bSameCount == 4)break;
		//	bSameCount = 1;
		//}
		//if (bSameCount == 4) return OX_FOUR_SAME;


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

		//金牛、银牛牌形判断
		/*BYTE bKingCount = 0, bTenCount = 0;
		bool isJinNiu = FALSE, isYinNiu = FALSE;
		for (BYTE i = 0; i < cbCardCount; i++)
		{
		if (GetCardValue(cbCardData[i])>10)
		{
		bKingCount++;
		}
		else if (GetCardValue(cbCardData[i]) == 10)
		{
		bTenCount++;
		}
		}
		if (bKingCount == NIU_NIU_POKER_MAX_COUNT) isJinNiu = TRUE;
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) isYinNiu = TRUE;*/

		/*if (bKingCount == NIU_NIU_POKER_MAX_COUNT) return OX_FIVEKING;
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) return OX_ALLTEN;*/
		//else if(bKingCount==NIU_NIU_POKER_MAX_COUNT-1 && bTenCount==1) return OX_FOURKING;

		////葫芦牌型
		//if(bSameCount==3)
		//{
		//	if((bSecondValue!=GetCardValue(cbCardData[3]) && GetCardValue(cbCardData[3])==GetCardValue(cbCardData[4]))
		//	||(bSecondValue!=GetCardValue(cbCardData[1]) && GetCardValue(cbCardData[1])==GetCardValue(cbCardData[0])))
		//		return OX_THREE_SAME;
		//}


		//汇总判断，按照牌形大小返回

		//同花顺
		if (TRUE == isPlusOne && TRUE == isSameColor) return OX_TONGHUASHUN;
		//五小牛
		else if (TRUE == isWuXiaoNiu) return OX_FIVESMALL;
		//炸弹
		else if (4 == maxSameCount) return OX_FOUR_SAME;
		//葫芦牛
		else if (3 == maxSameCount && TRUE == twoSameCount) return OX_THREE_SAME;
		//同花牛
		//else if (TRUE == isSameColor && FALSE == isPlusOne) return OX_COLOR_SAME;
		//顺子牛
		else if (TRUE == isPlusOne && FALSE == isSameColor) return OX_SHUNZINIU;
		//五花牛
		else if (TRUE == isWuHuaNiu) return OX_WUHUANIU;
		//金牛
		//else if (TRUE == isJinNiu) return OX_FIVEKING;
		//银牛
		//else if (TRUE == isYinNiu) return OX_FOURKING;

	}
	//非花样玩法特殊牌形
	else
	{
		//TODO：需要判断五小牛
		if (bSum <= 10) return OX_FIVESMALL;

		//炸弹牌形判断
		BYTE cardValueCount[13] = { 0 };      //牌形点数数组，计数用
		BYTE maxSameCount = 1;              //返回最大相等数量,用于判断炸弹和葫芦牛,4：炸弹牛，3：葫芦牛
		for (BYTE i = 0; i < cbCardCount; ++i)
		{
			++cardValueCount[GetCardValue(cbCardData[i]) - 1];
		}
		for (BYTE i = 0; i < 13; ++i)
		{
			if (cardValueCount[i] > maxSameCount)
			{
				maxSameCount = cardValueCount[i];
			}
		}
		//炸弹
		if (4 == maxSameCount) return OX_FOUR_SAME;

		//TODO：需要判断金牛 银牛
		BYTE bKingCount = 0, bTenCount = 0;
		for (BYTE i = 0; i < cbCardCount; i++)
		{
			if (GetCardValue(cbCardData[i])>10)
			{
				bKingCount++;
			}
			else if (GetCardValue(cbCardData[i]) == 10)
			{
				bTenCount++;
			}
		}
		//金牛
		if (bKingCount == NIU_NIU_POKER_MAX_COUNT) return OX_FIVEKING;
		//银牛
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) return OX_ALLTEN;
	}

	//判断牛牛数，普通牌形
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				return ((bTemp[i] + bTemp[j])>10) ? (bTemp[i] + bTemp[j] - 10) : (bTemp[i] + bTemp[j]);
			}
		}
	}

	return OX_VALUE0;
}

//获取倍数
BYTE NGameLogic::GetTimes_normal(BYTE cbCardData[], BYTE cbCardCount, bool isSpecPlay)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, isSpecPlay);
	// 无牛 - 牛六: 1倍
	if (bTimes <= 6)return 1;
	// 牛七 - 牛九: 2倍
	else if (bTimes >= 7 && bTimes <= 9) return 2;
	// 牛牛: 3倍
	else if (bTimes == 10)return 3;
	// 银牛 4倍
	else if (bTimes == OX_ALLTEN)return 4;
	// 金牛 5倍
	else if (bTimes == OX_FIVEKING)return 5;
	//五花牛 5倍
	else if (bTimes == OX_WUHUANIU) return 5;
	//顺子牛：6倍
	else if (bTimes == OX_SHUNZINIU) return 6;
	//葫芦牛：7倍
	else if (bTimes == OX_THREE_SAME) return 7;
	//同花牛：8倍
	//else if (bTimes == OX_COLOR_SAME) return 8;
	// 炸弹 8倍
	else if (bTimes == OX_FOUR_SAME)return 8;
	//五小牛：10倍
	else if (bTimes == OX_FIVESMALL)return 10;
	//同花顺牛：12倍
	else if (bTimes == OX_TONGHUASHUN)return 12;

	return 0;
}

//新增倍X1
BYTE NGameLogic::GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, bool isSpecPlay)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, isSpecPlay);
	// 无牛 - 牛六: 1倍
	if (bTimes <= 6)return 1;
	// 牛七 - 牛八: 2倍
	else if (bTimes >= 7 && bTimes <= 8) return 2;
	// 牛九：3倍
	else if (bTimes == 9) return 3;
	// 牛牛: 4倍
	else if (bTimes == 10)return 4;
	// 银牛 5倍
	else if (bTimes == OX_ALLTEN)return 5;
	// 金牛 6倍
	else if (bTimes == OX_FIVEKING)return 6;
	//五花牛 6倍
	else if (bTimes == OX_WUHUANIU) return 6;
	//顺子牛：7倍
	else if (bTimes == OX_SHUNZINIU) return 7;
	//葫芦牛：8倍
	else if (bTimes == OX_THREE_SAME) return 8;
	//同花牛：9倍
	//else if (bTimes == OX_COLOR_SAME) return 9;
	// 炸弹 10倍
	else if (bTimes == OX_FOUR_SAME)return 10;
	//五小牛：12倍
	else if (bTimes == OX_FIVESMALL)return 12;
	//同花顺牛：14倍
	else if (bTimes == OX_TONGHUASHUN)return 14;

	return 0;
}

BYTE NGameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec, BYTE cbSweep)
{
	//非扫雷模式下(普通倍率)
	if (cbSweep == TYPE_TIME_NORMAL)
	{
		return GetTimes_normal(cbCardData, cbCardCount, cbIsSpec);
	}
	//新增倍率牛牛X4
	else if (cbSweep == TYPE_TIME_X4)
	{
		return GetTimes_x4(cbCardData, cbCardCount, cbIsSpec);
	}
	//扫雷模式下
	else
	{
		if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

		BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, cbIsSpec);
		//无牛：1倍
		if (bTimes == 0) return 1;
		//牛一 - 牛牛：1-10倍
		else if (bTimes >= 1 && bTimes <= 10)return bTimes;
		// 银牛 12倍
		else if (bTimes == OX_ALLTEN)return 12;
		// 金牛 15倍
		else if (bTimes == OX_FIVEKING)return 15;
		//五花牛 15倍
		//else if (bTimes == OX_WUHUANIU) return 15;
		//葫芦牛：18倍
		//else if (bTimes == OX_THREE_SAME) return 18;
		//顺子牛：20倍
		//else if (bTimes == OX_SHUNZINIU) return 20;
		//同花牛：22倍
		//else if (bTimes == OX_COLOR_SAME) return 22;
		// 炸弹 18倍
		else if (bTimes == OX_FOUR_SAME)return 18;
		//五小牛：20倍
		else if (bTimes == OX_FIVESMALL)return 20;
		//同花顺牛：30倍
		//else if (bTimes == OX_DRAGON)return 30;

		return 0;
	}
}


//获取牛牛
bool NGameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
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
bool NGameLogic::IsIntValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE sum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		sum += GetCardLogicValue(cbCardData[i]);
	}

	return (sum % 10 == 0);
}

//排列扑克
void NGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i<cbCardCount; i++) cbLogicValue[i] = GetCardValue(cbCardData[i]);

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i<bLast; i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i + 1]) ||
				((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i]<cbCardData[i + 1])))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i + 1];
				cbLogicValue[i + 1] = cbTempData;
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//混乱扑克
void NGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
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
	} while (bRandCount<cbBufferCount);

	return;
}

//逻辑数值
BYTE NGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor = GetCardColor(cbCardData);
	BYTE bCardValue = GetCardValue(cbCardData);

	//转换数值
	return (bCardValue>10) ? (10) : bCardValue;
}

//对比扑克
bool NGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BOOL FirstOX, BOOL NextOX, BOOL isSpecPlay)
{
	if (FirstOX != NextOX)return (FirstOX>NextOX);

	//获取牛牛类型
	BYTE cbNextType = GetCardType(cbNextData, cbCardCount, isSpecPlay);
	BYTE cbFirstType = GetCardType(cbFirstData, cbCardCount, isSpecPlay);

	//比较牛大小
	if (FirstOX == TRUE)
	{
		//类型判断
		if (cbFirstType != cbNextType) return (cbFirstType>cbNextType);

		//类型相同，继续判断
		switch (cbNextType)
		{
		case OX_FOUR_SAME:		//炸弹牌型	
		case OX_THREE_SAME:		//葫芦牌型
		{
			//排序大小
			BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
			CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
			CopyMemory(bNextTemp, cbNextData, cbCardCount);
			SortCardList(bFirstTemp, cbCardCount);
			SortCardList(bNextTemp, cbCardCount);

			return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2])>GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);

			break;
		}
		}
	}

	//排序大小
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//如果都是顺子牌形 或者同为同花顺
	if ((cbNextType == cbFirstType && (cbFirstType == OX_TONGHUASHUN || cbFirstType == OX_SHUNZINIU)))
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
	if (cbNextMaxValue != cbFirstMaxValue)return cbFirstMaxValue>cbNextMaxValue;

	//比较颜色
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//////////////////////////////////////////////////////////////////////////
