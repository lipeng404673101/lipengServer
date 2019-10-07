#include "TuiTongZiGameLogic.h"



BYTE const TGameLogic::m_cbCardListData[36] =
{
	0x1,0x1,0x1,0x1,  /* 1筒 */
	0x2,0x2,0x2,0x2,  /* 2筒 */
	0x3,0x3,0x3,0x3,  /* 3筒 */
	0x4,0x4,0x4,0x4,  /* 4筒 */
	0x5,0x5,0x5,0x5,  /* 5筒 */
	0x6,0x6,0x6,0x6,  /* 6筒 */
	0x7,0x7,0x7,0x7,  /* 7筒 */
	0x8,0x8,0x8,0x8,  /* 8筒 */
	0x9,0x9,0x9,0x9   /* 9筒 */
};

BYTE const TGameLogic::m_cbCardListDataBaiBan[40] =
{
	0x1,0x1,0x1,0x1,  /* 1筒 */
	0x2,0x2,0x2,0x2,  /* 2筒 */
	0x3,0x3,0x3,0x3,  /* 3筒 */
	0x4,0x4,0x4,0x4,  /* 4筒 */
	0x5,0x5,0x5,0x5,  /* 5筒 */
	0x6,0x6,0x6,0x6,  /* 6筒 */
	0x7,0x7,0x7,0x7,  /* 7筒 */
	0x8,0x8,0x8,0x8,  /* 8筒 */
	0x9,0x9,0x9,0x9,   /* 9筒 */
	0x0A,0x0A,0x0A,0x0A    /*白板*/
};

/***********************************************
* 函数名：   SordHandCards()
* 描述：     排列手牌（按小牌在前）
* 参数：
*  @ handCards 玩家手牌数组（两张牌）
* 返回：     (void)
************************************************/
void TGameLogic::sordHandCards(BYTE* handCards)
{
	if (!isHandCardsValid(handCards))
	{
		return;
	}

	if (handCards[0] > handCards[1])
	{
		BYTE tmp = handCards[0];
		handCards[0] = handCards[1];
		handCards[0] = tmp;
	}
}

/******************************************
* 函数名：    IsDuiZi()
* 描述：      判断手牌是否为对子
* 参数：
*  @ handCards 玩家手牌数组（两张牌）
* 返回：      (bool) 对子返回true
*******************************************/
bool TGameLogic::isDuiZi(BYTE* handCards)
{
	if (!isHandCardsValid(handCards))
	{
		return false;
	}

	if (handCards[0] == handCards[1])
	{
		return true;
	}
	return false;
}

/*********************************************
* 函数名：    calcDianShu()
* 描述：      计算手牌点数和（两个牌值相加）
* 参数：
*  @ handCards 玩家手牌数组（两张牌）
* 返回：      (BYTE) 点数和
**********************************************/
BYTE TGameLogic::calcDianShu(BYTE* handCards)
{
	if (!isHandCardsValid(handCards))
	{
		return -1;
	}

	return handCards[0] + handCards[1];
}

/**********************************************************************************
* 函数名：   CompareCardByYa()
* 描述：     闲家和庄家比较牌的点数（押）
* 参数：
*  @ zhuang_handCards 庄家手牌数组（两张牌）
*  @ xian_handCards 闲家手牌数组（两张牌）
* 返回：     (int) -1：手牌有错，0：庄家赢，1：庄家输
***********************************************************************************/
int TGameLogic::compareCardByYa(BYTE* zhuang_handCards, BYTE* xian_handCards)
{
	if (!isHandCardsValid(zhuang_handCards) || !isHandCardsValid(xian_handCards))
	{
		return -1;
	}

	//是否为对子
	bool zhuang_is_dui = isDuiZi(zhuang_handCards);
	bool xian_is_dui = isDuiZi(xian_handCards);

	//点数和
	BYTE zhuang_dianShu = calcDianShu(zhuang_handCards);
	BYTE xian_dianShu = calcDianShu(xian_handCards);
	
	//如果都是对子，则点数大的一方赢
	if (zhuang_is_dui && xian_is_dui)
	{
		return zhuang_handCards[0] >= xian_handCards[0] ? 0 : 1;
	}

	//庄家是对子，闲家不是对子，则庄家赢
	if (zhuang_is_dui && !xian_is_dui) return 0;

	//如果庄家不是对子，闲家是对子，则闲家赢
	if (!zhuang_is_dui && xian_is_dui) return 1;
	
	//非对子情况，闲家点数为0，肯定是庄家赢
	if (0 == xian_dianShu % 10) return 0;

	//点数不同，比点数
	if (zhuang_dianShu % 10 != xian_dianShu % 10)
	{
		return zhuang_dianShu > xian_dianShu ? 0 : 1;
	}
	//下面就是点数相等的非对子比大小了
	if (zhuang_dianShu % 10 == xian_dianShu % 10)
	{
		//庄家个和闲家牌一样，庄家赢
		if (zhuang_handCards[0] == xian_handCards[0] && zhuang_handCards[1] == xian_handCards[1])
		{
			return 0;
		}
		//如：8-9：7点 最大的7点（1-6、2-5、3-4）
		if (zhuang_dianShu > xian_dianShu) return 0;
		if (zhuang_dianShu < xian_dianShu) return 1;

		if (zhuang_dianShu > 10 && xian_dianShu > 10 && zhuang_handCards[0] < xian_handCards[0])
		{
			return 0;
		}
		else
		{
			return 1;
		}

		if (zhuang_dianShu < 10 && xian_dianShu < 10 && zhuang_handCards[0] < xian_handCards[0])
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

	/**********************************************
	* 函数名：    isUpDao()
	* 描述：      判断手牌是否上道
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：      (bool) 上道返回true
	************************************************/
	bool TGameLogic::isUpDao(BYTE* handCards)
	{
		if (!isHandCardsValid(handCards))
		{
			false;
		}

		if (isDuiZi(handCards)) return true;

		return (calcDianShu(handCards) % 10) >= UP_DAO ? true : false;
	}
	
	/******************************************************
	* 函数名：     isHandCards()
	* 描述：       检测手牌是否合法
	* 参数：
	*  @ handCards 玩家手牌数组（两张牌）
	* 返回：       (bool)合法返回true
	*******************************************************/
	bool TGameLogic::isHandCardsValid(BYTE* handCards)
	{
		if (NULL != handCards && handCards[0] > 0 && handCards[0] < 10 && handCards[1] > 0 && handCards[1] < 10)
		{
			return true;
		}
		return false;
	}

	void TGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE isBaiBan)
	{
		if (isBaiBan==1)
			RandCardListBaiBan(cbCardBuffer, cbBufferCount);
		else
			RandCardList1and9(cbCardBuffer, cbBufferCount);
	}


	void TGameLogic::RandCardList1and9(BYTE cbCardBuffer[], BYTE cbBufferCount)
	{
		//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);

		//混乱准备
		BYTE cbCardData[sizeof(m_cbCardListData)];
		CopyMemory(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

		//混乱扑克
		BYTE bRandCount=0, bPosition=0;
		do
		{
			bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
			cbCardBuffer[bRandCount++]=cbCardData[bPosition];
			cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
		} while (bRandCount<cbBufferCount);

		return;
	}

	void TGameLogic::RandCardListBaiBan(BYTE cbCardBuffer[], BYTE cbBufferCount)
	{
		//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);

		//混乱准备
		BYTE cbCardData[sizeof(m_cbCardListDataBaiBan)];
		CopyMemory(cbCardData, m_cbCardListDataBaiBan, sizeof(m_cbCardListDataBaiBan));

		//混乱扑克
		BYTE bRandCount = 0, bPosition = 0;
		do
		{
			bPosition = rand() % (CountArray(m_cbCardListDataBaiBan) - bRandCount);
			cbCardBuffer[bRandCount++] = cbCardData[bPosition];
			cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListDataBaiBan) - bRandCount];
		} while (bRandCount<cbBufferCount);

		return;
	}

	bool TGameLogic:: isDui(BYTE* handCards)
	{
		if (handCards==NULL)return false;
		return handCards[0]==handCards[1]?true:false;
	}

	BYTE TGameLogic::getBigCard(BYTE* handCards)
	{
		if (handCards==NULL)return 0;
		return handCards[0]>handCards[1]?handCards[0]:handCards[1];
	}
	BYTE TGameLogic::getLogicCard(BYTE* handCards)
	{
		return (handCards[0]+handCards[1])%10;
	}

	bool  TGameLogic::isDao(BYTE* handCards)
	{
		if (isDui(handCards))return true;
		return getLogicCard(handCards)>=UP_DAO?true:false;
	}
	   
	// 0--first输  1-first赢  2-平  3-错误
	int  TGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
	{
		if (cbCardCount!=TUITONGZI_HAND_CARD_MAX)return 3;
		bool firstDui=isDui(cbFirstData);
		bool nextDui=isDui(cbNextData);

		if (firstDui&&!nextDui)return 1;
		else if (!firstDui&&nextDui)return 0;
		else if (firstDui&&nextDui)
		{
			int firstBig=getBigCard(cbFirstData);
			int nextBig=getBigCard(cbNextData);

			if (firstBig>nextBig)return 1;
			else if (firstBig<nextBig)return 0;
			else  return 2;
		}
		else 
		{
			int firstLogic=getLogicCard(cbFirstData);
			int nextLogic=getLogicCard(cbNextData);

			if (firstLogic>nextLogic)return 1;
			else if (firstLogic<nextLogic)return 0;
			else if (firstLogic==0 && nextLogic==0)return 1;
			else
			{
				int firstBig=getBigCard(cbFirstData);
				int nextBig=getBigCard(cbNextData);

				if (firstBig>nextBig)return 1;
				else if (firstBig<nextBig)return 0;
				else return 2;
			}
		}
	}






	
