#include "Card.h"
#include "LTool.h"
#include "LLog.h"
#include "LTime.h"
#include "BaseCommonFunc.h"

bool PlayType::GetNeedTing()
{
	auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_BaoTing)
		{
			return true;
		}
	}
	return false;
}

bool PlayType::gametypeDianPao() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_DIANPAOHU)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::gametypeQiXiaoDui() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_QIXIAODUI)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::gametypeHuanSanZhang() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_HUANSANZHANG)
		{
			return true;
		}
	}*/
	return false;
}

Lint PlayType::gametypeGetFan() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_TWOFAN)
		{
			return 2;
		}
		else if (*iTor == PT_THREEFAN)
		{
			return 3;
		}
		else if (*iTor == PT_FOURFAN)
		{
			return 4;
		}
		else if (*iTor == PT_EIGHTFAN)
		{
			return 8;
		}
	}*/
	return 4;
}

bool PlayType::gametypeGetJiangdui() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_JIANGDUI)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::gametypeGetTiandihu() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_TIANDIHU)
		{
			return true;
		}
	}*/
	return false;
}

Lint PlayType::gametypeGetDGH() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_DIANGANG)
		{
			return PT_DIANGANG;
		}
		else if (*iTor == PT_DIANGANG_THR)
		{
			return PT_DIANGANG_THR;
		}
	}*/
	return 1;
}

Lint PlayType::gametypeGetMenNum() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_PLAY_TWO_MEN)
		{
			return PT_PLAY_TWO_MEN;
		}
		else if (*iTor == PT_PLAY_THR_MEN)
		{
			return PT_PLAY_THR_MEN;
		}
	}*/
	return 1;
}

bool PlayType::gametypeGetMenZhang() const
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_MENZHONG)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::gametypeIsKa2Tiao()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_FT_KA2TIAO)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::gametypeDBombCanPingh()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_TT_DBOMB_CANHU)
		{
			return true;
		}
	}*/
	return false;
}

Lint PlayType::gametypeGetDDhuFan()
{
	//auto iTor = m_playtype.begin();
	//for (; iTor != m_playtype.end(); ++iTor)
	//{
	//	if (*iTor == PT_TT_DDHU_DOUBLE)
	//	{
	//		return 2;   //对对胡 2 番 选项
	//	}
	//}
	return 1;           //对对胡 默认 1 番
}

bool PlayType::gametypeKa5Heart()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_TT_KFIVE_HEART)
		{
			return true;
		}
	}*/
	return false;
}

//是否需要风牌
bool PlayType::gameNeedWind()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_NEEDWIND)
		{
			return true;
		}
	}*/
	return false;
}

Lint PlayType::gametypeGetCardNum()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_FT_CARD_7)
		{
			return 7;
		}
		else if (*iTor == PT_FT_CARD_10)
		{
			return 10;
		}
		else if(*iTor == PT_FT_CARD_13)
		{
			return 13;
		}
	}*/
	return 13;
}

Lint PlayType::ybGametypeGetMaxFan()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_FIVE_FAN)
		{
			return 5;
		}
		else if (*iTor == PT_SIX_FAN)
		{
			return 6;
		}
	}*/
	return 6;
}

bool PlayType::ybGametypeCanph()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_CAN_DPHU)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::ybGametypeGetPiao()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_FLY_PIAO)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::myTwoPlayerCanotTang()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_2PLAYER_NTANG)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::hasTangMustHu()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_HAS_TANG_MUSTHU)
		{
			return true;
		}
	}*/
	return false;
}

bool PlayType::wzGametypeXueZhan()
{
	//auto iTor = m_playtype.begin();
	//for (; iTor != m_playtype.end(); ++iTor)
	//{
	//	if (*iTor == PT_WANZHOU_XUEZHAN)
	//	{
	//		return true;
	//	}
	//}
	return false;
}

bool PlayType::wzGametypeYaoJiDai()
{
	/*auto iTor = m_playtype.begin();
	for (; iTor != m_playtype.end(); ++iTor)
	{
		if (*iTor == PT_WANZHOU_YAOJIDAI)
		{
			return true;
		}
	}*/
	return false;
}

CardManager* CardManager::mInstance = NULL;
static bool CardSortFun(Card* c1,Card* c2)
{
	if(c2->m_color > c1->m_color)
	{
		return true;
	}
	else if(c2->m_color == c1->m_color)
	{
		return c2->m_number > c1->m_number;
	}
	
	return false;
}

bool CardManager::Init()
{
	//加万饼条
	Lint index = 0;
	for (Lint j = 0; j < 3; j ++)	//三种色
	{
		for (Lint k = 0; k < 9; k ++)  //9张牌
		{
			for(Lint i = 0; i < 4 ;++i)		//循环加四次
			{
				m_card[index].m_color = j + 1;
				m_card[index].m_number = k + 1;
				m_card_Laizi[index].m_color = j + 1;
				m_card_Laizi[index].m_number = k + 1;
				index ++ ;
			}
		}
	}
	for(Lint i = 0 ; i < index; ++i)
	{
		m_cardVec.push_back(&m_card[i]);
		m_cardVec_Laizi.push_back(&m_card_Laizi[i]);
	}

	//加癞子
	for (Lint j = 3; j < 4; j ++)	//只加红中
	{
		for (Lint k = 4; k < 5; k ++)  //一种牌	红中值是 5
		{
			for(Lint i = 0; i < 4 ;++i)		//循环加四次
			{
				m_card_Laizi[index].m_color = j + 1;
				m_card_Laizi[index].m_number = k + 1;
				index ++ ;
			}
		}
	}
	for(Lint i = 108 ; i < index; ++i)
	{
		m_cardVec_Laizi.push_back(&m_card_Laizi[i]);
	}
	SortCard(m_cardVec);
	SortCard(m_cardVec_Laizi);
	return true;
}

void CardManager::SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT])
{
	Lint nCardCount = cvIn.size();
	for (Lint i = 0; i < nCardCount; i ++)
	{
		if (specialCard[i].m_color == 0 || specialCard[i].m_number == 0)
		{
			break;
		}
		for (Lint j = i + 1; j < nCardCount; j ++)
		{
			if (cvIn[j]->m_color == specialCard[i].m_color && cvIn[j]->m_number == specialCard[i].m_number)
			{
				Card* tmp = cvIn[j];
				cvIn[j] = cvIn[i];
				cvIn[i] = tmp;	
				break;
			}
		}
	}
}

void CardManager::DealCard2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card specialCard[CARD_COUNT], bool needWind)
{
	CardVector mCards = m_cardVec;
	if (!needWind)
	{
		mCards.erase(mCards.begin() + 108, mCards.end());
	}

	//更新随机因子
	updateRandomFactor();

	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}
	
	SwapCardBySpecial(cardtmp, specialCard);

	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
	
	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
	
	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needWind)
{
	//更新随机因子
	updateRandomFactor();

	//随机算法修改
	CardVector mCards;
	CardVector cardtmp;
	if (!needWind)
	{
		mCards = m_cardVec;
		Lint nSize = mCards.size();
		while (nSize > 0)
		{
			Lint seed1 = L_Rand(0, nSize - 1);
			cardtmp.push_back(mCards[seed1]);
			mCards[seed1] = mCards[nSize - 1];
			nSize --;
		}
	}
	else
	{
		mCards = m_cardVec_Laizi;
		Lint nSize = mCards.size();
		while (nSize > 0)
		{
			Lint seed1 = L_Rand(0, nSize - 1);
			cardtmp.push_back(mCards[seed1]);
			mCards[seed1] = mCards[nSize - 1];
			nSize --;
		}
	}
	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::SortCard(CardVector& vec)
{
	std::sort(vec.begin(),vec.end(),CardSortFun);
}

void CardManager::EraseCard(CardVector& src,CardVector& des)
{
	//这里直接比较地址是否相等
	CardVector::iterator it2 = des.begin();
	for(; it2 != des.end(); ++it2)
	{
		CardVector::iterator it1 = src.begin();
		for(; it1 != src.end();)
		{
			if((*it1) == (*it2))
			{
				it1 = src.erase(it1);
			}
			else
			{
				it1++;
			}
		}
	}
}

void CardManager::EraseCard(CardVector& src,Card* pCard)
{
	//这里直接比较地址是否相等
	for(Lsize i = 0 ; i  < src.size(); ++i)
	{
		if(src[i] == pCard)
		{
			src.erase(src.begin()+i);
			break;
		}
	}
}

//删除src 与pCard 数据一样的牌 N张
bool CardManager::EraseCard(CardVector& src,Card* pCard,Lint n)
{
	Lint cnt = 0;
	CardVector::iterator it2 = src.begin();
	for(; it2 != src.end();)
	{
		if(cnt >= n)
			break;

		if(IsSame(pCard,*it2))
		{
			cnt ++;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}
	return cnt != 0;
}

void CardManager::EarseSameCard(CardVector& src)
{
	if(src.empty())
	{
		return;
	}

	Lint color = src.front()->m_color;
	Lint number = src.front()->m_number;
	CardVector::iterator it = src.begin()+1;
	for(; it != src.end();)
	{
		if(color == (*it)->m_color && number == (*it)->m_number)
		{
			it = src.erase(it);
		}
		else
		{
			color = (*it)->m_color;
			number= (*it)->m_number;
			++it;
		}
	}
}

ThinkVec CardManager::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo)
{
	ThinkVec result;
	/*ThinkUnit unit;
	gameInfo.bChaTing = false;
	if(gameInfo.b_canHu && CheckCanHu(handcard,pengCard,agangCard,mgangCard,eatCard, outCard,gameInfo,unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}
	if (!gameInfo.b_onlyHu)
	{
		if(!gameInfo.bNoCard && CheckCanGang(handcard,outCard))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
		if(CheckCanPeng(handcard,outCard))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_PENG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
	}*/

	return result;
}

ThinkVec CardManager::CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	gameInfo.bChaTing = false;
	/*if(gameInfo.b_canHu && CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCard,gameInfo,unit.m_hu,true))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		result.push_back(unit);
	}*/

	if(!gameInfo.b_onlyHu)
	{
		CardVector tmp = handcard;
		if (getCard)
		{
			tmp.push_back(getCard);
		}
		SortCard(tmp);

		std::vector<Card*> vec;
		if(!gameInfo.bNoCard && CheckAnGang(tmp,vec))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_ABU;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}
		}

		vec.clear();
		if(!gameInfo.bNoCard && CheckMGang(tmp,pengCard,vec))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MBU;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}
			if(getCard && vec.size() > 0 && (vec.at(0)->m_color != getCard->m_color || vec.at(0)->m_number != getCard->m_number))
			{
				gameInfo.m_pGscard = vec.at(0);
			}
		}
	}
	return result;
}

bool CardManager::CheckCanHu(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard)
{
	return !vec.empty();
}


bool CardManager::CheckCanGang(CardVector& handcard,Card* outCard)
{
	CardVector tmpVec;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}
	if(!tmpVec.empty())
	{
		if(tmpVec[0]->m_color==4)
			return false;
	}

	return tmpVec.size() >= 3;
}

bool CardManager::CheckCanPeng(CardVector& handcard,Card* outCard)
{
	CardVector tmpVec;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	return (tmpVec.size() >= 2);
}

bool CardManager::CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard,Lint& special)
{
	special = 0;
	// 如果有吃 碰或者明杠肯定不是七对
	if ( eatCard.size() > 0 || pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}

	int cardnum[3][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 3 )
		{
			return false;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			return false;
		}
		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
	}
	if ( outCard )
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 3 )
		{
			return false;
		}
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return false;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
	}

	// 检查牌的个数要么是4 要么是2 要么是0
	int four = 0;	// 四张牌的个数
	for ( int i = 0; i < 3; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] == 0 )
			{
			}
			else if ( cardnum[i][k] == 4 )
			{
				++four;
			}
			else if ( cardnum[i][k] == 2 )
			{
			}
			else
			{
				return false;
			}
		}
	}
	special = four;
	return true;
}


//检测清一色
bool CardManager::CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec)
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	Lint color = tmp.front()->m_color;
	for(Lsize i = 1 ; i < tmp.size(); ++i)
	{
		if(tmp[i]->m_color != color)
			return false;
	}

	if (CheckHoo(handcard, outCard) || vec.size() > 0)
	{
		return true;
	}
	return false;
}

//检测乱将胡
bool CardManager::CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	CardVector tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(tmp.empty())
	{
		return false;
	}

	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(!(tmp[i]->m_number == 2 || tmp[i]->m_number == 5 ||
			tmp[i]->m_number == 8))
		{
			return false;
		}
	}
	return true;
}

//检测碰碰胡
bool CardManager::CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard)
{
	if(eatCard.size())
	{
		return false;
	}

	int cardnum[64] = { 0 };	// 
	for( size_t i = 0; i < handcard.size(); ++i)
	{
		Card* pCard = handcard[i];
		int index = (pCard->m_color-1)*10 + (pCard->m_number-1);
		if ( index >= 0 && index < 64 )
		{
			cardnum[index]++;
		}
		else
		{
			return false;
		}
	}
	if(outCard)
	{
		int index = (outCard->m_color-1)*10 + (outCard->m_number-1);
		if ( index >= 0 && index < 64 )
		{
			cardnum[index]++;
		}
		else
		{
			return false;
		}
	}
	// 统计相同牌的个数
	int numcount[5] = {0};
	for ( int i = 0; i < 64; ++i )
	{
		if ( cardnum[i] > 0 && cardnum[i] <= 5 )	// 在查听的时候 玩家手里可能会有五张一样的牌
		{
			numcount[cardnum[i]-1]++;
		}
	}
	if ( numcount[0] > 0  )	// 存在单张的牌
	{
		return false;
	}
	if ( numcount[1] > 1 )	// 两张牌存在多个
	{
		return false;
	}
	if ( numcount[3] > 0 )	// 存在一个四张的牌
	{
		return false;
	}
	if ( numcount[4] > 1 )
	{
		return false;
	}
	if ( numcount[4] == 1  )	// 玩家手里可能有个杠 再加一个相同的牌组成将牌
	{
		return numcount[1] == 0;
	}
	return numcount[1] == 1;
}

//检测抢杠胡
bool CardManager::CheckQiangganghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MGANG && gameInfo.m_deskState != DESK_PLAY_END_CARD))
	{
		return true;
	}
	return false;
}

//检测全球人
bool CardManager::CheckQuanqiuren(CardVector& handcard,Card* outCard)
{
	CardVector check;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
	}

	return check.size() == 2 && CheckHoo(handcard,outCard);
}

//杠上开花
bool CardManager::CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	//判断是不是自已杠的
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}

//杠上炮
bool CardManager::CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MBU && gameInfo.m_deskState != DESK_PLAY_END_CARD))
	{
		if(gameInfo.m_upState == THINK_OPERATOR_ABU || gameInfo.m_upState == THINK_OPERATOR_MGANG || gameInfo.m_upState == THINK_OPERATOR_MBU)
			return true;
	}
	//杠上炮，判断是不是别人的杠
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}
//海底捞
bool CardManager::CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.bNoCard)
	{
		return true;
	}
	return false;
}
//海底炮
bool CardManager::CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.bNoCard)
	{
		return true;
	}
	return false;
}

bool CardManager::CheckAnGang(CardVector& handcard,  std::vector<Card*>& vec)
{
	if(handcard.size() < 4)
		return false;

	for(Lsize i = 0 ;i+3 < handcard.size(); ++i)
	{
		Card* pcarda = handcard[i];
		Card* pcardb = handcard[i+3];
		if(pcarda->m_color == pcardb->m_color && 
			pcarda->m_number == pcardb->m_number&& pcarda->m_color != 4)
		{
			vec.push_back(pcarda);
		}
	}
	return true;
}

bool CardManager::CheckMGang(CardVector& handcard,CardVector& pengCard,  std::vector<Card*>& vec)
{
	for(Lsize i = 0 ; i < pengCard.size(); i+=3)
	{
		Card* pcard = pengCard[i];
		for(Lsize j = 0 ; j < handcard.size(); ++j)
		{
			if(pcard->m_number == handcard[j]->m_number &&
				pcard->m_color == handcard[j]->m_color&& pcard->m_color != 4)
			{
				vec.push_back(pcard);
				break;
			}
		}
	}
	return true;
}

bool CardManager::IsContinue(CardVector& result)
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number+i || color != result[i]->m_color)
			return false;
	}

	return true;
}

bool CardManager::IsSame(CardVector& result)
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number || color != result[i]->m_color)
			return false;
	}

	return true;
}
bool CardManager::IsSame(Card* c1,Card* c2)
{
	return c1->m_color==c2->m_color&&c1->m_number==c2->m_number;
}

bool CardManager::IsNineOne(Card* c)
{
	return c->m_number == 0 || c->m_number == 9;
}


Card* CardManager::GetCard(Lint color,Lint number)
{
	return &m_card[(color-1)*9*4+(number-1)*4];
}

int CardManager::CheckJiangdui( CardVector& handcard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	CardVector tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}

	Lsize doubleCount = 0;
	SortCard(tmp);
	for(int i = 0; i < (int)tmp.size()-1; i+=2)
	{
		if(tmp[i]->m_number == tmp[i+1]->m_number && tmp[i]->m_color == tmp[i+1]->m_color)
		{
			++doubleCount;
		}
	}

	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(tmp.empty())
	{
		return 0;
	}

	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(tmp[i]->m_number != 2 && tmp[i]->m_number != 5 && tmp[i]->m_number != 8)
		{
			return 0;
		}
	}

	if(tmp.size() == 8 && doubleCount == 4)
	{
		return HU_JIANGDUI_7;
	}

	if(doubleCount == 7)
	{
		return HU_JIANGDUI_7;
	}

	if(doubleCount >= 1)
	{
		return HU_JIANGDUI;
	}

	return 0;
}

Lint CardManager::CheckGen(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& agangcard,CardVector& mgangcard, Card* getCard)
{
	Lint gen = 0;
	CardVector tmp = handcard;
	if ( getCard )
	{
		tmp.push_back(getCard);
	}

	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),agangcard.begin(),agangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());

	SortCard(tmp);

	for(Lsize i = 0 ;i+3 < tmp.size(); ++i)
	{
		Card* pcarda = tmp[i];
		Card* pcardb = tmp[i+3];
		if(pcarda->m_color == pcardb->m_color && 
			pcarda->m_number == pcardb->m_number)
		{
			++gen;
			i += 3;
		}
	}

	return gen;
}

bool CardManager::CheckOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	auto  handcard = tmp;
	if(outCard)
	{
		handcard.push_back(outCard);
	}
	// 简单检查数据的合法性
	if ( pengCard.size()%3 != 0 || gangcard.size()%4 != 0 || mgangcard.size()%4 != 0 )
	{
		return false;
	}
	// 检查牌数
// 	if ( (handcard.size() + pengCard.size() + gangcard.size()/4*3 + mgangcard.size()/4*3) != 14 )
// 	{
// 		return false;
// 	}
	// 碰牌检查
	for ( size_t k = 0; k < pengCard.size(); k+=3 )
	{
		if ( !pengCard.at(k) || !pengCard.at(k+2) )
		{
			return false;
		}
		Lint color = pengCard[k]->m_color;
		Lint number1 = pengCard[k]->m_number;
		Lint number3 = pengCard[k+2]->m_number;
		if ( !( color == CARD_COLOR_WAN || color == CARD_COLOR_TUO || color == CARD_COLOR_SUO ) )
		{
			return false;
		}
		if ( !(number1 == 1 || number1 == 9 || number3 == 1 || number3 == 9 ) )
		{
			return false;
		}
	}
	// 杠牌检查
	CardVector* group[2] = { &gangcard, &mgangcard };
	for ( int i = 0; i < 2; ++i )
	{
		for ( Lsize k = 0; k < group[i]->size(); k+=4 )
		{
			if ( !group[i]->at(k) )
			{
				return false;
			}
			if ( !(group[i]->at(k)->m_color == CARD_COLOR_WAN || group[i]->at(k)->m_color == CARD_COLOR_TUO || group[i]->at(k)->m_color == CARD_COLOR_SUO ) )
			{
				return false;
			}
			if ( !(group[i]->at(k)->m_number == 1 || group[i]->at(k)->m_number == 9) )
			{
				return false;
			}
		}
	}

	// 判断手牌中是否能能组合成 1 or 9
	// 按花色统计牌数
	int cardnum[3][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color == CARD_COLOR_WAN || (*it)->m_color == CARD_COLOR_TUO || (*it)->m_color == CARD_COLOR_SUO )
		{
			cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		}
	}
	int handvalidsize = 0;
	// 判断的原则就是 2==3<=1 7==8<=9 handsize=1+2+3+7+8+9==handcard.size()
	for ( int i = 0; i < 3; ++i )
	{
		if ( !(cardnum[i][1] == cardnum[i][2] && cardnum[i][1] <= cardnum[i][0]) )	// 2==3<=1
		{
			return false;
		}
		if ( !(cardnum[i][6] == cardnum[i][7] && cardnum[i][6] <= cardnum[i][8]) )	// 7==8<=9
		{
			return false;
		}
		handvalidsize += ( cardnum[i][1] + cardnum[i][2] + cardnum[i][0] );
		handvalidsize += ( cardnum[i][6] + cardnum[i][7] + cardnum[i][8] );
	}
	return handvalidsize == (int)handcard.size();
}

bool CardManager::CheckNoOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard )
{
	auto  handcard = tmp;
	if(outCard)
	{
		handcard.push_back(outCard);
	}
	// 简单检查数据的合法性
	if ( pengCard.size()%3 != 0 || gangcard.size()%4 != 0 || mgangcard.size()%4 != 0 )
	{
		return false;
	}
	
	// 判断牌组中是否都含有 1 or 9
	CardVector* group[3] = { &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 3; ++i )
	{
		for ( size_t k = 0; k < group[i]->size(); ++k )
		{
			if ( !group[i]->at(k) )
			{
				return false;
			}
			if ( group[i]->at(k)->m_color == CARD_COLOR_WAN || group[i]->at(k)->m_color == CARD_COLOR_TUO || group[i]->at(k)->m_color == CARD_COLOR_SUO )
			{
				if ( group[i]->at(k)->m_number == 1 || group[i]->at(k)->m_number == 9 )
				{
					return false;
				}
			}
		}
	}
	// 判断手牌中是否有幺九
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color == CARD_COLOR_WAN || (*it)->m_color == CARD_COLOR_TUO || (*it)->m_color == CARD_COLOR_SUO )
		{
			if ( (*it)->m_number == 1 || (*it)->m_number == 9 )
			{
				return false;
			}
		}
	}
	return true;
}

bool CardManager::CheckMenQing( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard )
{
	auto  handcard = tmp;
	if(outCard)
	{
		handcard.push_back(outCard);
	}
	// 简单检查数据的合法性
	if ( pengCard.size()%3 != 0 || gangcard.size()%4 != 0 || mgangcard.size()%4 != 0 )
	{
		return false;
	}
	// 检查牌数
// 	if ( (handcard.size() + pengCard.size() + gangcard.size()/4*3 + mgangcard.size()/4*3) != 14 )
// 	{
// 		return false;
// 	}
	// 如果有碰或者明杠肯定不是门前清
	if ( pengCard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}
	return true;
}

bool CardManager::CheckMidCard(const CardVector& handcard,const Card& mid,const Card* outCard )
{
	auto tmp = handcard;
	if(outCard && outCard->m_color == mid.m_color && outCard->m_number == mid.m_number)
	{
		SortCard(tmp);

		Lint num = mid.m_number - 1;
		for(auto it = tmp.begin(); it != tmp.end(); )
		{
			if((*it)->m_color == mid.m_color && (*it)->m_number == num)
			{
				it = tmp.erase(it);
				if(num == mid.m_number - 1)
					num = mid.m_number+1;
				else if(num == mid.m_number+1)
				{
					num = 0;
					break;
				}
			}
			else
				++ it;
		}

		if(num == 0 && CheckHoo(tmp,NULL))
		{
			return true;
		}
	}
	return false;
}

bool CardManager::CheckOne2Nine(const CardVector& handcard,const Card* curCard )
{
	int mycard[3][10];   
	memset(mycard,0,sizeof(mycard));

	for(size_t i = 0; i < handcard.size(); i++)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	if(curCard)
	{
		const int color = curCard->m_color;
		const int number = curCard->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	for(int i = 0; i < 3; i++)
	{
		if(mycard[i][0] >= 9)  //3种花色 中只能找到一种 9张牌的
		{
			for(int j = 1; j < 10; j++)
			{
				if(mycard[i][j])
				{
					mycard[i][j]--;
				}
				else
				{
					return false;
				}
			}
			mycard[i][0] -= 9;

			if(CheckMyHoo(mycard))
			{
				return true;
			}
			break;
		}
	}
	return false;
}

bool CardManager::CheckMyHoo( int allPai[3][10])
{
	int jiangPos;    
	int yuShu;       
	bool jiangExisted = false;

	for(int i=0;i<3;i++)    
	{        
		yuShu=allPai[i][0]%3;         
		if (yuShu==1) 
			return false;        
		if (yuShu==2)         
		{            
			if (jiangExisted) 
				return false;            
			jiangPos=i;   
			jiangExisted=true;    
		}   
	} 

	if(!jiangExisted)
	{
		return false;
	}

	for(int i=0;i<3;i++) 
	{        
		if (i!=jiangPos)        
		{   
			if (!Analyzefor(allPai[i]))   
				return false;   
		}    
	}  

	bool success=false;  
	for(int j = 1; j < 10; j ++)
	{        
		if (allPai[jiangPos][j] >= 2)       
		{                      
			allPai[jiangPos][j]-=2;      
			allPai[jiangPos][0]-=2;           
			if(Analyzefor(allPai[jiangPos]))      
			{            
				success=true;  
			}             
			allPai[jiangPos][j]+=2;         
			allPai[jiangPos][0]+=2;           
			if (success) 
				break;     
		}   
	}    
	return success;
}

bool CardManager::Analyzefor(int* kindcard)
{
	int tcard[12] = {0};
	for(int i = 0; i < 10; i ++)
		tcard[i] = kindcard[i];

	for(int i = 1; i < 10; )
	{
		if(tcard[i] >= 3)
		{
			tcard[i] -= 3;
		}

		if(tcard[i] == 0)
		{
			++i;
			continue;
		}

		if(tcard[i+1] && tcard[i+2])
		{
			--tcard[i];
			--tcard[i+1];
			--tcard[i+2];
			continue;
		}
		else
			return false;
	}
	return true;
}

bool CardManager::CheckSisterPairs(const CardVector& handcard,const Card* curCard )
{
	int mycard[3][10];   
	memset(mycard,0,sizeof(mycard));

	for(size_t i = 0; i < handcard.size(); i++)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	if(curCard)
	{
		const int  color = curCard->m_color;
		const int  number = curCard->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	for(int i = 0; i < 3; i++)
	{
		if(mycard[i][0] >= 6)
		{
			for(int j = 1; j < 10-2; j++)
			{
				if(mycard[i][j] > 1 && mycard[i][j+1] > 1 && mycard[i][j+2] > 1)
				{
					//remove the sisters
					mycard[i][j] -= 2;
					mycard[i][j+1] -= 2;
					mycard[i][j+2] -= 2;
					mycard[i][0] -= 6;
					if(CheckMyHoo(mycard))    //满足条件 return
					{
						return true;
					}
					//reback the sisters
					mycard[i][j] += 2;
					mycard[i][j+1] += 2;
					mycard[i][j+2] += 2;
					mycard[i][0] += 6;
				}
			}
		}
	}
	return false;
}

bool CardManager::CheckHoo( const CardVector& handcard,const Card* curCard )
{
	int mycard[3][10];   
	memset(mycard,0,sizeof(mycard));

	for(size_t i = 0; i < handcard.size(); i++)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	if(curCard)
	{
		const int color = curCard->m_color;
		const int number = curCard->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number]++;
			mycard[color-1][0]++;
		}
	}

	return CheckMyHoo(mycard);
}
 
void CardManager::DealCardThrThr( CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5, int cardNum)
{
	updateRandomFactor();

	//随机算法修改
	CardVector mCards = m_cardVec;
	CardVector cardtmp;

	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards[seed1] = mCards[nSize - 1];
		nSize --;
	}

	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin() + cardNum);
	v2.insert(v2.begin(),cardtmp.begin() + cardNum, cardtmp.begin() + cardNum*2);
	v3.insert(v3.begin(),cardtmp.begin() + cardNum*2, cardtmp.begin() + cardNum*3);

	v5.insert(v5.begin(),cardtmp.begin() + cardNum*3, cardtmp.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
}

void CardManager::DealCardThrThr2( CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,int cardNum,const Card specialCard[])
{
	//随机算法修改
	CardVector mCards = m_cardVec;
	CardVector cardtmp;

	//设置随机因子
	LTime timeNow;
	srand(timeNow.Secs());

	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards[seed1] = mCards[nSize - 1];
		nSize --;
	}

	SwapCardBySpecial(cardtmp, specialCard);

	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin() + cardNum);
	v2.insert(v2.begin(),cardtmp.begin() + cardNum, cardtmp.begin() + cardNum*2);
	v3.insert(v3.begin(),cardtmp.begin() + cardNum*2, cardtmp.begin() + cardNum*3);
	
	v5.insert(v5.begin(),cardtmp.begin() + cardNum*3, cardtmp.end());

	std::reverse(v5.begin(),v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
}
