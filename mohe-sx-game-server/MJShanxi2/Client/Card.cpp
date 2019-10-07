#include "Card.h"
#include "LTool.h"
#include "LLog.h"
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
	for(Lint i = 0; i < 9 ;++i)
	{
		for(Lint j = 0; j < 4; ++j)//万
		{
			m_card[i*12+j].m_color = 1;
			m_card[i*12+j].m_number = i+1;
		}

		for(Lint j = 4; j < 8; ++j)//筒
		{
			m_card[i*12+j].m_color = 2;
			m_card[i*12+j].m_number = i+1;
		}

		for(Lint j = 8; j < 12; ++j)//条
		{
			m_card[i*12+j].m_color = 3;
			m_card[i*12+j].m_number = i+1;
		}
	}

	for(Lint i = 0 ; i < CARD_COUNT; ++i)
	{
		m_cardVec.push_back(&m_card[i]);
	}
	SortCard(m_cardVec);
	return true;
}

bool CardManager::Final()
{
	return true;
}

void CardManager::DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5)
{
	CardVector cardtmp = m_cardVec;
	
	for(Lint i = 0 ; i < 80; ++i)
	{
		Lint seed1 = L_Rand(0,CARD_COUNT-1);
		Lint seed2 = L_Rand(0,CARD_COUNT-1);
		Card* tmp = cardtmp[seed1];
		cardtmp[seed1] = cardtmp[seed2];
		cardtmp[seed2] = tmp;
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
			it++;
		}
	}
}

ThinkVec CardManager::CheckOperator(CardVector& handcard, Card* outCard)
{
	ThinkVec result;
	if(CheckCanHu(handcard, outCard))
	{
		result.push_back(THINK_OPERATOR_BOMB);
	}

	if(CheckCanGang(handcard,outCard))
	{
		result.push_back(THINK_OPERATOR_GANG);
	}
	if(CheckCanPeng(handcard,outCard))
	{
		result.push_back(THINK_OPERATOR_PENG);
	}

	return result;
}

bool CardManager::CheckCanHu(CardVector& handcard, Card* outCard)
{ 
	CardVector all = handcard;
	if(outCard)
	{
		all.push_back(outCard);
	}
	gCardMgr.SortCard(all);

	
	CardVector continueh,checkh,oneh, twoh, fourh, threeh, specialh;

	//把所有连续的3张牌找出来，第一种从开始往后面找
	bool find = true;
	checkh = all;
	while (find && checkh.size() >= 3)
	{
		CardVector tmp;
		find = false;
		tmp.push_back(checkh.front());
		for (Lsize i = 1; i < checkh.size(); ++i)
		{
			if (checkh[i]->m_color == tmp.back()->m_color &&
				checkh[i]->m_number == tmp.back()->m_number + 1)
			{
				tmp.push_back(checkh[i]);

				if (tmp.size() == 3)
				{
					EraseCard(checkh, tmp);
					continueh.insert(continueh.end(), tmp.begin(), tmp.end());
					find = true;
					break;
				}
			}
			else if (!(checkh[i]->m_color == tmp.back()->m_color &&
				checkh[i]->m_number == tmp.back()->m_number))
			{
				tmp.clear();
				tmp.push_back(checkh[i]);
			}
		}
	}
	
	//是否需要反向在找一遍
	bool retail = false;
	GetSpecialOneTwoThreeFour(checkh, oneh, twoh, threeh, fourh);
	if (twoh.size() / 2 > 1 || oneh.size() || fourh.size())
	{
		retail = true;
		continueh.clear();
		checkh.clear();
		oneh.clear();
		twoh.clear();
		threeh.clear();
		fourh.clear();
	}

	//反过来找一遍
	if (retail)
	{
		bool find = true;
		checkh = all;
		while (find && checkh.size() >= 3)
		{
			CardVector tmp;
			find = false;
			tmp.push_back(checkh.back());
			for (Lint i = (Lint)checkh.size()-1; i >= 0; --i)
			{
				if (checkh[i]->m_color == tmp.back()->m_color &&
					checkh[i]->m_number+1 == tmp.back()->m_number)
				{
					tmp.push_back(checkh[i]);

					if (tmp.size() == 3)
					{
						EraseCard(checkh, tmp);
						continueh.insert(continueh.end(), tmp.begin(), tmp.end());
						find = true;
						break;
					}
				}
				else if (!(checkh[i]->m_color == tmp.back()->m_color &&
					checkh[i]->m_number == tmp.back()->m_number))
				{
					tmp.clear();
					tmp.push_back(checkh[i]);
				}
			}
		}

		GetSpecialOneTwoThreeFour(checkh, oneh, twoh, threeh, fourh);
	}

	if (twoh.size() / 2 > 1 || oneh.size() || fourh.size())
	{
		return false;
	}

	return true;
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

bool CardManager::CheckCanTing(CardVector& handcard)
{
	Card* checkCard[28] = {NULL};
	for(Lsize i = 0 ; i < handcard.size(); ++i)
	{
		for(Lint j = -1; j < 2; ++j)
		{
			if(handcard[i]->m_number+j > 0 && handcard[i]->m_number+j <= 9)
			{
				Card* pc = GetCard(handcard[i]->m_color,handcard[i]->m_number+j);

				checkCard[pc->m_number-1+(pc->m_color-1)*9] = pc;
			}
		}	
	}

	for(Lint i = 0 ; i < 28; ++i)
	{
		if (checkCard[i] && CheckCanHu(handcard, checkCard[i]))
		{
			return true;
		}
	}
	
	return false;
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

bool CardManager::GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four)
{
	if(src.empty())
	{
		return false;
	}

	Lsize i = 0 ;
	Lint number = 0,color = 0 ,length = 0 ;
	for(; i < src.size(); ++i)
	{
		Card* pCard = src[i];
		if(number == pCard->m_number && color == pCard->m_color)
		{
			length += 1;
		}
		else
		{
			if(length == 1)
			{
				one.push_back(src[i-1]);
			}
			else if(length == 2)
			{
				two.push_back(src[i-2]);
				two.push_back(src[i-1]);
			}
			else if(length == 3)
			{
				three.push_back(src[i-3]);
				three.push_back(src[i-2]);
				three.push_back(src[i-1]);
			}
			else if(length == 4)
			{
				four.push_back(src[i-4]);
				four.push_back(src[i-3]);
				four.push_back(src[i-2]);
				four.push_back(src[i-1]);
			}
			length = 1;
			number = pCard->m_number;
			color = pCard->m_color;
		}
	}

	if(length == 1)
	{
		one.push_back(src[i-1]);
	}
	else if(length == 2)
	{
		two.push_back(src[i-2]);
		two.push_back(src[i-1]);
	}
	else if(length == 3)
	{
		three.push_back(src[i-3]);
		three.push_back(src[i-2]);
		three.push_back(src[i-1]);
	}
	else if(length == 4)
	{
		four.push_back(src[i-4]);
		four.push_back(src[i-3]);
		four.push_back(src[i-2]);
		four.push_back(src[i-1]);
	}

	return true;
}

Card* CardManager::GetCard(Lint color,Lint number)
{
	return &m_card[(color-1)*4+(number-1)*12];
}