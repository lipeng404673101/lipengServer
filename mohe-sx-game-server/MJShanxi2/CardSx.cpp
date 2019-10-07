#include "Card.h"
#include "LTool.h"
#include "LLog.h"
#include "LTime.h"

static bool SetRange(Card* outCard)
{
	if (outCard->m_number >= 6)
	{
		return true;
	}
	return false;
}

static bool CardSortFun(Card* c1, Card* c2)
{
	if (c2->m_color > c1->m_color)
	{
		return true;
	}
	else if (c2->m_color == c1->m_color)
	{
		return c2->m_number > c1->m_number;
	}
	return false;
}

bool CardManager::Init()
{
	//万
	Lint index = 0;
	for (Lint k = 0; k < 9; k++)  //9张牌
	{
		for (Lint i = 0; i < 4; ++i)		//循环加四次
		{
			wan_base_card[index].m_color = 1;
			wan_base_card[index].m_number = k + 1;
			index++;
		}
	}
	//饼
	index = 0;
	for (Lint k = 0; k < 9; k++)  //9张牌
	{
		for (Lint i = 0; i < 4; ++i)		//循环加四次
		{

			bing_base_card[index].m_color = 2;
			bing_base_card[index].m_number = k + 1;
			index++;
		}
	}

	//条
	index = 0;
	for (Lint k = 0; k < 9; k++)  //9张牌
	{
		for (Lint i = 0; i < 4; ++i)		//循环加四次
		{

			tiao_base_card[index].m_color = 3;
			tiao_base_card[index].m_number = k + 1;
			index++;
		}
	}

	//字
	index = 0;
	for (Lint k = 0; k < 7; k++)  //6张牌
	{
		for (Lint i = 0; i < 4; ++i)		//循环加四次
		{

			zi_base_card[index].m_color = 4;
			zi_base_card[index].m_number = k + 1;
			index++;
		}
	}

	for (Lint i = 0; i < BASE_CARD_COUNT; ++i)
	{
		m_3_laizi.push_back(&wan_base_card[i]);
		m_3_laizi.push_back(&bing_base_card[i]);
		m_3_laizi.push_back(&tiao_base_card[i]);
	}
	for (Lint i = 0; i < 28; ++i)
	{
		m_3_laizi.push_back(&zi_base_card[i]);
	}

	Lint temp = 0;
	for (Lint i = 1; i < 4; i++)
	{
		yao_13[temp].m_color = i;
		yao_13[temp].m_number = 1;
		temp++;
		yao_13[temp].m_color = i;
		yao_13[temp].m_number = 9;
		temp++;
	}
	for (Lint i = 1; i < 8; i++)
	{
		yao_13[temp].m_color = 4;
		yao_13[temp].m_number = i;
		temp++;
	}
	for (Lint i = 0; i < 13; i++)
	{
		m_13yao.push_back(&yao_13[i]);
	}


	SortCard(m_13yao);
	SortCard(m_3_laizi);
	return true;
}

bool CardManager::Final()
{
	return true;
}

void CardManager::SwapCardBySpecial(CardVector& cvIn, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84])
{
	Lint cardcount = cvIn.size();
	std::list<Card*> card(cvIn.begin(), cvIn.end());
	CardVector playCard[5];
	Lint playcount[4];
	memset(playcount, 0, sizeof(playcount));
	for (Lint i = 0; i < 13; ++i)
	{
		if (player1Card[i].m_color == 0 || player1Card[i].m_number == 0)
		{
			break;
		}
		for (auto x = card.begin(); x != card.end(); ++x)
		{
			if ((*x)->m_color == player1Card[i].m_color && (*x)->m_number == player1Card[i].m_number)
			{
				playCard[0].push_back(*x);
				card.erase(x);
				break;
			}
		}
	}
	for (Lint i = 0; i < 13; ++i)
	{
		if (player2Card[i].m_color == 0 || player2Card[i].m_number == 0)
		{
			break;
		}
		for (auto x = card.begin(); x != card.end(); ++x)
		{
			if ((*x)->m_color == player2Card[i].m_color && (*x)->m_number == player2Card[i].m_number)
			{
				playCard[1].push_back(*x);
				card.erase(x);
				break;
			}
		}
	}
	for (Lint i = 0; i < 13; ++i)
	{
		if (player3Card[i].m_color == 0 || player3Card[i].m_number == 0)
		{
			break;
		}
		for (auto x = card.begin(); x != card.end(); ++x)
		{
			if ((*x)->m_color == player3Card[i].m_color && (*x)->m_number == player3Card[i].m_number)
			{
				playCard[2].push_back(*x);
				card.erase(x);
				break;
			}
		}
	}
	for (Lint i = 0; i < 13; ++i)
	{
		if (player4Card[i].m_color == 0 || player4Card[i].m_number == 0)
		{
			break;
		}
		for (auto x = card.begin(); x != card.end(); ++x)
		{
			if ((*x)->m_color == player4Card[i].m_color && (*x)->m_number == player4Card[i].m_number)
			{
				playCard[3].push_back(*x);
				card.erase(x);
				break;
			}
		}
	}
	for (Lint i = 0; i < 84; ++i)
	{
		if (SendCard[i].m_color == 0 || SendCard[i].m_number == 0)
		{
			break;
		}
		for (auto x = card.begin(); x != card.end(); ++x)
		{
			if ((*x)->m_color == SendCard[i].m_color && (*x)->m_number == SendCard[i].m_number)
			{
				playCard[4].push_back(*x);
				card.erase(x);
				break;
			}
		}
	}
	cvIn.clear();
	cvIn.insert(cvIn.end(), playCard[0].begin(), playCard[0].end());
	while (cvIn.size() != 13)
	{
		cvIn.push_back(card.back());
		card.pop_back();
	}
	cvIn.insert(cvIn.end(), playCard[1].begin(), playCard[1].end());
	while (cvIn.size() != 26)
	{
		cvIn.push_back(card.back());
		card.pop_back();
	}
	cvIn.insert(cvIn.end(), playCard[2].begin(), playCard[2].end());
	while (cvIn.size() != 39)
	{
		cvIn.push_back(card.back());
		card.pop_back();
	}
	cvIn.insert(cvIn.end(), playCard[3].begin(), playCard[3].end());
	while (cvIn.size() != 52)
	{
		cvIn.push_back(card.back());
		card.pop_back();
	}
	cvIn.insert(cvIn.end(), playCard[4].begin(), playCard[4].end());
	cvIn.insert(cvIn.end(), card.begin(), card.end());
}

void CardManager::DealCard2(CardVector& m_3_laizi, CardVector hand_card[], Lint user_count, CardVector& rest_card, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84], Lint game_type, PlayTypeInfo& playtype)
{
	CardVector mCards(m_3_laizi);
	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}
	SwapCardBySpecial(cardtmp, player1Card, player2Card, player3Card, player4Card, SendCard);
	for (int x = 0; x<user_count; x++)
	{
		hand_card[x].insert(hand_card[x].end(), cardtmp.begin() + 13 * x, cardtmp.begin() + 13 * (x + 1));
		SortCard(hand_card[x]);
	}
	rest_card.insert(rest_card.end(), cardtmp.begin() + 13 * user_count, cardtmp.end());
	std::reverse(rest_card.begin(), rest_card.end());		//逆序桌上牌
}

void CardManager::DealCard(CardVector& m_3_laizi, CardVector hand_card[], Lint user_count, CardVector& rest_card, Lint game_type, PlayTypeInfo& playtype)
{
	CardVector mCards(m_3_laizi);
	CardVector cardtmp;
	Lint nSize = mCards.size();
	//设置随机因子
	LTime timeNow;
	srand(timeNow.Secs());
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards[seed1] = mCards[nSize - 1];
		nSize--;
	}
	//m_3_laizi = cardtmp;
	for (int x = 0; x<user_count; x++)
	{
		hand_card[x].insert(hand_card[x].end(), cardtmp.begin() + 13 * x, cardtmp.begin() + 13 * (x + 1));
		SortCard(hand_card[x]);
	}
	rest_card.insert(rest_card.end(), cardtmp.begin() + 13 * user_count, cardtmp.end());
}

void CardManager::SortCard(CardVector& vec)
{
	std::sort(vec.begin(), vec.end(), CardSortFun);
}

void CardManager::EraseCard(CardVector& src, CardVector& des)
{
	//这里直接比较地址是否相等
	CardVector::iterator it2 = des.begin();
	for (; it2 != des.end(); ++it2)
	{
		CardVector::iterator it1 = src.begin();
		for (; it1 != src.end();)
		{
			if ((*it1) == (*it2))
			{
				it1 = src.erase(it1);
			}
			else
			{
				++it1;
			}
		}
	}
}

void CardManager::EraseCard(CardVector& src, Card* pCard)
{
	//这里直接比较地址是否相等
	for (Lsize i = 0; i < src.size(); ++i)
	{
		if (src[i] == pCard)
		{
			src.erase(src.begin() + i);
			break;
		}
	}
}

//删除src 与pCard 数据一样的牌 N张
void CardManager::EraseCard(CardVector& src, Card* pCard, Lint n)
{
	Lint cnt = 0;
	CardVector::iterator it2 = src.begin();
	for (; it2 != src.end();)
	{
		if (cnt >= n)
			break;

		if (IsSame(pCard, *it2))
		{
			cnt++;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}
}

void CardManager::EarseSameCard(CardVector& src)
{
	if (src.empty())
	{
		return;
	}

	Lint color = src.front()->m_color;
	Lint number = src.front()->m_number;
	CardVector::iterator it = src.begin() + 1;
	for (; it != src.end();)
	{
		if (color == (*it)->m_color && number == (*it)->m_number)
		{
			it = src.erase(it);
		}
		else
		{
			color = (*it)->m_color;
			number = (*it)->m_number;
			++it;
		}
	}
}

ThinkVec CardManager::CheckOutCardOperator(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	bool bTing = false;
	if (gameInfo.b_CanHu&&CheckCanHu(tingstate, handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}
	if (gameInfo.b_CanDianGang&&CheckCanGang(handcard, outCard, true))
	{
		unit.Clear();
		unit.m_type = THINK_OPERATOR_MGANG;
		unit.m_card.push_back(outCard);
		result.push_back(unit);

		////判断明杠听
		//CardVector tmp(handcard);
		//EraseCard(tmp, outCard, 3);
		//CardVector tmp_minggangcard(mgangCard);
		//tmp_minggangcard.push_back(outCard);

		////明杠后可以听
		//TingUnit xx;
		//if (CheckCanTing(tmp, pengCard, agangCard, tmp_minggangcard, eatCard, gameInfo, xx.hu_units))
		//{
		//	unit.Clear();
		//	unit.m_type = THINK_OPERATOR_MINGGANGTING;
		//	unit.m_card.push_back(outCard);
		//	xx.out_card = NULL;
		//	unit.ting_unit.push_back(xx);
		//	result.push_back(unit);
		//	bTing = true;
		//}
	}

	if (gameInfo.b_CanPeng&&CheckCanPeng(handcard, outCard, true))
	{
		unit.Clear();
		unit.m_type = THINK_OPERATOR_PENG;
		unit.m_card.push_back(outCard);
		result.push_back(unit);

		////判断碰听
		//CardVector tmp(handcard);
		//EraseCard(tmp, outCard, 2);
		//CardVector tmp_pengcard(pengCard);
		//tmp_pengcard.push_back(outCard);

		////碰后可以听
		//unit.Clear();
		//if (all_CheckTing(tmp, tmp_pengcard, agangCard, mgangCard, eatCard, NULL, gameInfo, unit.ting_unit))
		//{
		//	unit.m_type = THINK_OPERATOR_PENGTING;
		//	unit.m_card.push_back(outCard);
		//	result.push_back(unit);
		//	bTing = true;
		//}
	}

	std::vector<Card*> vec;
	if (gameInfo.b_CanChi && CheckCanChi(handcard, outCard, vec))
	{
		for (Lsize i = 0; i < vec.size(); i += 3)
		{
			if (gameInfo.b_CanChi)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_CHI;
				unit.m_card.push_back(vec[i]);
				unit.m_card.push_back(vec[i + 1]);
				unit.m_card.push_back(vec[i + 2]);
				result.push_back(unit);
			}

			////判断吃听
			//CardVector tmp(handcard);
			//EraseCard(tmp, vec[i], 1);
			//EraseCard(tmp, vec[i + 1], 1);
			//CardVector tmp_chicard(eatCard);

			//tmp_chicard.push_back(vec[i]);
			//tmp_chicard.push_back(vec[i + 1]);
			//tmp_chicard.push_back(vec[i + 2]);

			////吃后可以听
			//unit.Clear();
			//if (all_CheckTing(tmp, pengCard, agangCard, mgangCard, tmp_chicard, NULL, gameInfo, unit.ting_unit))
			//{
			//	unit.m_type = THINK_OPERATOR_CHITING;
			//	unit.m_card.push_back(vec[i]);
			//	unit.m_card.push_back(vec[i + 1]);
			//	unit.m_card.push_back(vec[i + 2]);
			//	result.push_back(unit);
			//	bTing = true;
			//}
		}
	}
	return result;
}

ThinkVec CardManager::CheckGetCardOperator(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	bool bTing = false;
	//判断可以胡
	if (!gameInfo.b_CanTing && gameInfo.b_CanHu && CheckCanHu(tingstate, handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, unit.m_hu, true))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		result.push_back(unit);
	}
	CardVector tmp = handcard;
	if (getCard)
	{
		tmp.push_back(getCard);
	}
	SortCard(tmp);

	std::vector<Card*> vec;
	if (gameInfo.b_CanAnGang&&CheckAnGang(tmp, vec))
	{
		for (Lsize i = 0; i < vec.size(); i++)
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_AGANG;
			unit.m_card.push_back(vec[i]);
			result.push_back(unit);

			////判断暗杠听
			//CardVector tmp2(tmp);
			//EraseCard(tmp2, vec[i], 4);
			//CardVector tmp_angangcard(agangCard);
			//tmp_angangcard.push_back(vec[i]);

			////暗杠后可以听
			//TingUnit xx;
			//if (CheckCanTing(tmp2, pengCard, tmp_angangcard, mgangCard, eatCard, gameInfo, xx.hu_units))
			//{
			//	unit.Clear();
			//	xx.out_card = NULL;
			//	unit.m_type = THINK_OPERATOR_ANGANGTING;
			//	unit.m_card.push_back(vec[i]);
			//	unit.ting_unit.push_back(xx);
			//	result.push_back(unit);
			//	bTing = true;
			//}
		}
	}

	vec.clear();
	if (gameInfo.b_CanMingGang&&CheckMGang(tmp, pengCard, vec))
	{
		for (Lsize i = 0; i < vec.size(); ++i)
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(vec[i]);
			result.push_back(unit);

			////判断暗杠听
			//CardVector tmp2(tmp);
			//EraseCard(tmp2, vec[i], 1);
			////明杠后可以听
			//unit.Clear();
			//TingUnit xx;
			//if (CheckCanTing(tmp2, pengCard, agangCard, mgangCard, eatCard, gameInfo, xx.hu_units))
			//{
			//	xx.out_card = NULL;
			//	unit.m_type = THINK_OPERATOR_MINGGANGTING;
			//	unit.m_card.push_back(vec[i]);
			//	unit.ting_unit.push_back(xx);
			//	result.push_back(unit);
			//	bTing = true;
			//}
		}
	}

	//判断听
	unit.Clear();
	if (gameInfo.b_CanTing&&all_CheckTing(handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, unit.ting_unit))
	{
		unit.m_type = THINK_OPERATOR_TING;
		for (int x = 0; x < unit.ting_unit.size(); x++)
		{
			if (unit.ting_unit[x].out_card)
			{
				unit.m_card.push_back(unit.ting_unit[x].out_card);
			}
		}
		result.push_back(unit);
		bTing = true;
	}
	return result;
}

ThinkVec CardManager::CheckOutCardOpeartorAfterTing(TingState & tingstate, CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector & eatCard, Card * outCard, OperateState & gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	if (gameInfo.b_CanHu&&CheckCanHuAfterTing(tingstate, outCard, gameInfo, unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}

	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	SortCard(tmp);

	std::vector<Card*> vec;

	vec.clear();
	if (CheckCanGang(handcard, outCard, true, 0))
	{
		CardVector tmp2(tmp);
		EraseCard(tmp2, outCard, 4);
		//明杠后可以听
		unit.Clear();
		TingUnit xx;
		if (CheckCanTing(tmp2, pengCard, agangCard, mgangCard, eatCard, gameInfo, xx.hu_units))
		{
			if (!xx.hu_units.empty() && xx.hu_units.size() == tingstate.hu_units.size())
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MGANG;
				unit.m_card.push_back(outCard);
				result.push_back(unit);
			}
		}
	}
	return result;
}

//听后摸到牌
ThinkVec CardManager::CheckGetCardOpeartorAfterTing(TingState & tingstate, CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector & eatCard, Card* getCard, OperateState & gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	//判断可以胡
	if (CheckCanHuAfterTing(tingstate, getCard, gameInfo, unit.m_hu, true))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		result.push_back(unit);
	}

	CardVector tmp = handcard;
	if (getCard)
	{
		tmp.push_back(getCard);
	}
	SortCard(tmp);

	std::vector<Card*> vec;
	if (CheckAnGang(tmp, vec))
	{
		for (Lsize i = 0; i < vec.size(); i++)
		{
			CardVector tmp2(tmp);
			EraseCard(tmp2, vec[i], 4);
			CardVector tmp_angangcard(agangCard);
			tmp_angangcard.push_back(vec[i]);

			//暗杠后可以听
			TingUnit xx;
			if (CheckCanTing(tmp2, pengCard, tmp_angangcard, mgangCard, eatCard, gameInfo, xx.hu_units))
			{
				if (!xx.hu_units.empty()&&xx.hu_units.size()==tingstate.hu_units.size())
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				}
			}
		}
	}

	vec.clear();
	if (CheckMGang(getCard,pengCard, vec))
	{
		for (Lsize i = 0; i < vec.size(); ++i)
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(vec[i]);
			result.push_back(unit);
		}
	}
	return result;
}

ThinkVec CardManager::CheckDuiBao(TingState& tingstate, PlayTypeInfo& playTypeInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	for (int x = 0; x < tingstate.hu_units.size(); x++)
	{
		/*if (playTypeInfo.b_DuiBao && tingstate.bao_unit.m_baocard)
		{
			if (tingstate.bao_unit.m_baocard->m_color >= 4)
			{
				unit.m_type = THINK_OPERATOR_BOMB;
				unit.m_card.push_back(tingstate.bao_unit.m_baocard);
				Hu_type hu;
				hu.m_hu_basicTypes.assign(tingstate.hu_units[x].m_hu_basicTypes.begin(), tingstate.hu_units[x].m_hu_basicTypes.end());
				hu.m_hu_type = HU_DuiBao;
				unit.m_hu.push_back(hu);
				result.push_back(unit);
				break;
			}
			if (tingstate.hu_units[x].hu_card->m_number == tingstate.bao_unit.m_baocard->m_number)
			{
				unit.m_type = THINK_OPERATOR_BOMB;
				unit.m_card.push_back(tingstate.bao_unit.m_baocard);
				Hu_type hu;
				hu.m_hu_basicTypes.assign(tingstate.hu_units[x].m_hu_basicTypes.begin(), tingstate.hu_units[x].m_hu_basicTypes.end());
				hu.m_hu_type = HU_DuiBao;
				unit.m_hu.push_back(hu);
				result.push_back(unit);
				break;
			}
		}*/
	}
	return result;
}

bool CardManager::CheckCanHu(TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard)
{
	bool base_need = false;
	ThinkUnit unit;
	Lint isSpecial = 0;
	if (gameInfo.m_GameType == TuiDaoHu)
	{
		if (CheckXiaoqidui(handcard, isSpecial, outCard, isGetCard, gameInfo))
		{
			if (isSpecial == 1)
			{
				Hu_type hu;
				hu.m_hu_basicTypes.push_back(HU_DaQixiaodui);
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
			else {
				Hu_type hu;
				hu.m_hu_basicTypes.push_back(HU_Qixiaodui);
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
		}
		if (CheckShiSanYiao(handcard, outCard))
		{
			Hu_type hu;
			hu.m_hu_basicTypes.push_back(HU_DaQixiaodui);
			hu.m_hu_way.push_back(1);
			vec.push_back(hu);
		}
		if (xiaohu_CheckHu(handcard, outCard, true))
		{
			Hu_type hu;
			hu.m_hu_way.push_back(1);
			vec.push_back(hu);
		}
	}
	else if(gameInfo.m_GameType == KouDian)
	{
		if (CheckXiaoqidui(handcard, isSpecial, outCard, isGetCard, gameInfo))
		{
			if (isSpecial == 1)
			{
				Hu_type hu;
				hu.m_hu_basicTypes.push_back(HU_DaQixiaodui);
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
			else {
				Hu_type hu;
				hu.m_hu_basicTypes.push_back(HU_Qixiaodui);
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
		}
		if (CheckShiSanYiao(handcard, outCard))
		{
			Hu_type hu;
			hu.m_hu_basicTypes.push_back(HU_DaQixiaodui);
			hu.m_hu_way.push_back(1);
			vec.push_back(hu);
		}
		if (xiaohu_CheckHu(handcard, outCard, true))
		{
			Hu_type hu;
			hu.m_hu_way.push_back(1);
			vec.push_back(hu);
		}
	}
	return !vec.empty();
}

bool CardManager::CheckCanHuAfterTing(TingState& tingstate, Card* outCard, OperateState& gameInfo, std::vector<Hu_type>& vec, bool isGetCard)
{
	for (int x = 0; x < tingstate.hu_units.size(); x++)
	{
		if (tingstate.hu_units[x].hu_card->m_color == outCard->m_color
			&&tingstate.hu_units[x].hu_card->m_number == outCard->m_number)
		{
			Hu_type hu;
			hu.m_hu_basicTypes.assign(tingstate.hu_units[x].m_hu_basicTypes.begin(), tingstate.hu_units[x].m_hu_basicTypes.end());
			vec.push_back(hu);
			break;
		}
	}
	if (isGetCard && !vec.empty())
	{
		for (auto x = vec.begin(); x != vec.end(); ++x)
		{
			x->m_hu_way.push_back(HUWAY_ZiMo);
		}
	}
	return !vec.empty();
}

bool CardManager::CheckCanGang(CardVector& handcard, Card* outCard, bool huType, bool pt_laizi)
{
	CardVector tmpVec;
	for (Lsize i = 0; i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if (pcard->m_color == outCard->m_color &&
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	/*if (pt_laizi&&tmpVec.size()>0)
	{
		if (tmpVec[0]->m_color == 4 && tmpVec[0]->m_number == 5)
		{
			return false;
		}
	}*/

	return tmpVec.size() >= 3;
}

bool CardManager::CheckCanPeng(CardVector& handcard, Card* outCard, bool huType, bool pt_laizi)
{
	if (handcard.size() < 5 && !huType)
		return false;
	CardVector tmpVec;
	if (outCard)
	{
		if (pt_laizi&&outCard->m_color == 4 && outCard->m_number == 5)
			return false;
	}
	else
	{
		return false;
	}

	for (Lsize i = 0; i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if (pcard->m_color == outCard->m_color &&
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	return (tmpVec.size() >= 2);
}

bool CardManager::CheckCanChi(CardVector& handcard, Card* outCard, std::vector<Card*>& vec)
{
	if (handcard.size() < 5)
		return false;
	if (!outCard)
	{
		return false;
	}
	if (outCard->m_color == 4)
	{
		return false;
	}

	CardVector one = handcard;
	SortCard(one);
	EarseSameCard(one);
	EraseCard(one, outCard, 4);

	for (Lint i = 0; i + 1 < one.size(); ++i)
	{
		CardVector tmp;
		tmp.push_back(one[i]);
		tmp.push_back(one[i + 1]);
		tmp.push_back(outCard);
		SortCard(tmp);
		if (IsContinue(tmp))
		{
			vec.push_back(one[i]);
			vec.push_back(one[i + 1]);
			vec.push_back(outCard);
		}
	}

	return true;
}

bool CardManager::CheckAnGang(CardVector& handcard, std::vector<Card*>& vec, bool pt_laizi)
{
	if (handcard.size() < 5)
		return false;

	for (Lsize i = 0; i + 3 < handcard.size(); ++i)
	{
		Card* pcarda = handcard[i];
		Card* pcardb = handcard[i + 3];
		if (pcarda->m_color == pcardb->m_color && pcarda->m_number == pcardb->m_number)
		{
			vec.push_back(pcarda);
		}
	}
	return true;
}

bool CardManager::CheckMGang(CardVector& handcard, CardVector& pengCard, std::vector<Card*>& vec, bool pt_laizi)
{
	if (handcard.size() < 5)
		return false;
	for (Lsize i = 0; i < pengCard.size(); i += 3)
	{
		Card* pcard = pengCard[i];
		for (Lsize j = 0; j < handcard.size(); ++j)
		{
			if (pt_laizi)
			{
				if (pcard->m_number == handcard[j]->m_number &&
					pcard->m_color == handcard[j]->m_color && !(pcard->m_color == 4 && pcard->m_number == 5))
				{
					vec.push_back(pcard);
					break;
				}
			}
			else {
				if (pcard->m_number == handcard[j]->m_number &&
					pcard->m_color == handcard[j]->m_color)
				{
					vec.push_back(pcard);
					break;
				}
			}
		}
	}
	return !vec.empty();
}


bool CardManager::CheckMGang(Card * getCard, CardVector & pengCard, std::vector<Card*>& vec, bool pt_laizi)
{
	for (Lsize i = 0; i < pengCard.size(); i += 3)
	{
		Card* pcard = pengCard[i];
		if (pcard->m_number == getCard->m_number &&pcard->m_color == getCard->m_color)
		{
			vec.push_back(pcard);
			break;
		}
	}
	return !vec.empty();
}

bool CardManager::CheckYiTiaoLong(CardVector& handcard, Card* winCard)
{
	CardVector tmp(handcard);
	tmp.push_back(winCard);
	SortCard(tmp);
	std::vector<Lint> card_vector(40, 0);
	for (int x = 0; x<tmp.size(); x++)
	{
		card_vector[(tmp[x]->m_color - 1) * 10 + (tmp[x]->m_number) * 1]++;
	}
	Lint ContinueNum = 0;
	for (size_t color = 0; color < 3; color++)
	{
		ContinueNum = 0;
		for (size_t number = 1; number < 10; number++)
		{
			Lint index = color * 10 + number;
			if (card_vector[index] >= 1 && card_vector[index] != 2)
			{
				ContinueNum++;
				if (ContinueNum >= 9)
				{
					return true;
				}
			}
			else
			{
				color++;
				break;
			}
		}
	}
	return false;
}

bool CardManager::IsContinue(CardVector& result)
{
	if (result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for (Lsize i = 1; i < result.size(); ++i)
	{
		if (result[i]->m_number != number + i || color != result[i]->m_color)
			return false;
	}

	return true;
}

bool CardManager::IsSame(CardVector& result)
{
	if (result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for (Lsize i = 1; i < result.size(); ++i)
	{
		if (result[i]->m_number != number || color != result[i]->m_color)
			return false;
	}

	return true;
}
bool CardManager::IsSame(Card* c1, Card* c2)
{
	return c1->m_color == c2->m_color&&c1->m_number == c2->m_number;
}

bool CardManager::IsNineOne(Card* c)
{
	return c->m_number == 0 || c->m_number == 9;
}

bool CardManager::GetSpecialOneTwoThreeFour(CardVector& src, CardVector& one, CardVector& two, CardVector& three, CardVector& four)
{
	if (src.empty())
	{
		return false;
	}

	Lsize i = 0;
	Lint number = 0, color = 0, length = 0;
	for (; i < src.size(); ++i)
	{
		Card* pCard = src[i];
		if (number == pCard->m_number && color == pCard->m_color)
		{
			length += 1;
		}
		else
		{
			if (length == 1)
			{
				one.push_back(src[i - 1]);
			}
			else if (length == 2)
			{
				two.push_back(src[i - 2]);
				two.push_back(src[i - 1]);
			}
			else if (length == 3)
			{
				three.push_back(src[i - 3]);
				three.push_back(src[i - 2]);
				three.push_back(src[i - 1]);
			}
			else if (length == 4)
			{
				four.push_back(src[i - 4]);
				four.push_back(src[i - 3]);
				four.push_back(src[i - 2]);
				four.push_back(src[i - 1]);
			}
			length = 1;
			number = pCard->m_number;
			color = pCard->m_color;
		}
	}

	if (length == 1)
	{
		one.push_back(src[i - 1]);
	}
	else if (length == 2)
	{
		two.push_back(src[i - 2]);
		two.push_back(src[i - 1]);
	}
	else if (length == 3)
	{
		three.push_back(src[i - 3]);
		three.push_back(src[i - 2]);
		three.push_back(src[i - 1]);
	}
	else if (length == 4)
	{
		four.push_back(src[i - 4]);
		four.push_back(src[i - 3]);
		four.push_back(src[i - 2]);
		four.push_back(src[i - 1]);
	}

	return true;
}

bool CardManager::xiaohu_CheckHu2(CardVector& card_vector, Card* curCard)
{
	return true;
}
//检测基本夹胡
bool CardManager::xiaohu_CheckHu(CardVector handcard, Card* curCard, bool hu_37jia)
{
	if (curCard)
	{
		handcard.push_back(curCard);
	}
	std::vector<Lint> card_vector(40, 0);

	//第一步获取癞子,分数量统计
	for (int x = 0; x<handcard.size(); x++)
	{
		if (handcard[x])
		{
			card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
		}
	}

	for (int x = 0; x<card_vector.size(); x++)
	{
		int index = x % 10;
		if (x % 10 == 0)
		{
			continue;
		}
		else if (x>37)
		{
			break;
		}

		if (card_vector[x]>1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 2;
			if (explorer_zheng_function(tmp_cards, 0, 0))
			{
				return true;
			}
		}
	}
	return false;
}

bool CardManager::Hu_explorer_zheng_function(std::vector<Lint> card_vector, Lint index)
{
	if (index<39)
	{
		if (index % 10 == 0)
		{
			index++;
		}
		if (card_vector[index]>0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			if (judge_ke(tmp_cards, index))
			{
				if (Hu_explorer_zheng_function(tmp_cards, index))
					return true;
			}
			if (index % 10<8 && index<30)
			{
				std::vector<Lint> tmp_cards2(card_vector);
				if (judge_shun(tmp_cards2, index))
				{
					if (Hu_explorer_zheng_function(tmp_cards2, index))
						return true;
				}
			}
			return false;
		}
		else {
			index++;
			if (Hu_explorer_zheng_function(card_vector, index))
			{
				return true;
			}
			return false;
		}
	}
	else {
		return true;
	}
}

Lint CardManager::judge_ke(std::vector<Lint>& targe_vector, Lint index)
{
	if (index >= targe_vector.size())
	{
		return 100;
	}

	Lint Lai_num = 0;
	switch (targe_vector[index])
	{
	case 4:
	case 3:
	{
		targe_vector[index] -= 3;
		Lai_num = 0;
		break;
	}
	default:
		return 100;
		break;
	}
	return Lai_num;
}

Lint CardManager::judge_shun(std::vector<Lint>& targe_vector, Lint index)
{
	if (targe_vector[index]>0 && targe_vector[index + 2]>0 && targe_vector[index + 1]>0)
	{
		targe_vector[index]--;
		targe_vector[index + 1]--;
		targe_vector[index + 2]--;
		return 0;
	}
	else if (targe_vector[index]>0 && targe_vector[index + 1]>0)
	{
		targe_vector[index]--;
		targe_vector[index + 1]--;
		//ting_index = index + 2;
		return 1;
	}
	else if (targe_vector[index]>0 && targe_vector[index + 2]>0)
	{
		targe_vector[index]--;
		targe_vector[index + 2]--;
		//ting_index = index + 1;
		return 1;
	}
	else if (targe_vector[index + 1]>0 && targe_vector[index + 2]>0)
	{
		targe_vector[index + 1]--;
		targe_vector[index + 2]--;
		//ting_index = index;
		return 1;
	}

	return 100;
}

Lint CardManager::CheckLaiziNum(CardVector& handcard, OperateState& gameInfo)
{
	Lint laiziCount = 0;
	if (gameInfo.LaiziCard == NULL)
	{
		return NULL;
	}
	for (auto iter = handcard.cbegin(); iter != handcard.cend(); iter++)
	{
		if ((*iter)->m_color == gameInfo.LaiziCard->m_color && (*iter)->m_number == gameInfo.LaiziCard->m_number)
		{
			laiziCount++;
		}
	}
	return laiziCount;
}

Lint CardManager::get_card_index(Card * card)
{
	if (card)
		return  (card->m_color - 1) * 10 + card->m_number;
	else {
		return 0;
	}
}

Card * CardManager::get_card(Lint index)
{
	for (int x = 0; x < m_3_laizi.size(); x++)
	{
		if ((m_3_laizi[x]->m_color == index / 10 + 1) && (m_3_laizi[x]->m_number == index % 10))
		{
			return m_3_laizi[x];
		}
	}
	return nullptr;
}

Card * CardManager::get_card2(Lint index)
{
	for (int x = 0; x < m_3_laizi.size(); x++)
	{
		if ((m_3_laizi[x]->m_color == index / 10 + 1) && (m_3_laizi[x]->m_number == index % 10))
		{
			return m_3_laizi[x];
		}
	}
	return nullptr;
}
//bool CardManager::have_play_type(Lint playtype,std::vector<Lint> playtypes)
//{
//	for(auto x=playtypes.begin();x!=playtypes.end();x++)
//	{
//		if(*x == playtype)
//			return true;
//	}
//	return false;
//}
bool CardManager::CheckQingYiSe(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	//两门以上才能胡 含19
	CardVector tmp(handcard);
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), agangCard.begin(), agangCard.end());
	tmp.insert(tmp.end(), mgangCard.begin(), mgangCard.end());
	tmp.insert(tmp.end(), eatCard.begin(), eatCard.end());

	SortCard(tmp);

	bool two_color = false;
	Lint color = 0;
	if (!tmp.empty())
	{
		color = tmp[0]->m_color;
	}
	for (int x = 0; x < tmp.size(); x++)
	{
		if (!two_color&&color != tmp[x]->m_color&&tmp[x]->m_color<4)
		{
			LLOG_DEBUG("NO QINGYISE ");
			two_color = true;
		}
	}
	return two_color;
}

bool CardManager::CheckBaseNeed(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	//必须开门,	闭门听不用开门
	//if (!gameInfo.b_BimenTing)
	//{
	//	if (pengCard.empty() && mgangCard.empty() && eatCard.empty())
	//	{
	//		return false;
	//	}
	//}
	////两门以上才能胡 含19
	//CardVector tmp(handcard);
	//tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	//tmp.insert(tmp.end(), agangCard.begin(), agangCard.end());
	//tmp.insert(tmp.end(), mgangCard.begin(), mgangCard.end());
	//tmp.insert(tmp.end(), eatCard.begin(), eatCard.end());
	//if (getCard)
	//{
	//	tmp.push_back(getCard);
	//}
	//SortCard(tmp);

	//bool two_color = false;
	//bool have_19 = false;
	//Lint color = 0;
	//if (!tmp.empty())
	//{
	//	color = tmp[0]->m_color;
	//}
	//for (int x = 0; x < tmp.size(); x++)
	//{
	//	if (!two_color&&color != tmp[x]->m_color&&tmp[x]->m_color<4)
	//	{
	//		two_color = true;
	//	}

	//	if (!have_19&&tmp[x]->m_color == 4 || tmp[x]->m_number == 1 || tmp[x]->m_number == 9)
	//	{
	//		have_19 = true;
	//	}
	//	if (gameInfo.b_QingYiSe&&have_19)
	//	{
	//		return true;
	//	}
	//	if (two_color&&have_19)
	//	{
	//		return true;
	//	}
	//}
	return true;
}

bool CardManager::CheckHasTriplet(CardVector& handcard)
{
	return true;
}

bool CardManager::CheckCanTing(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, std::vector<HuUnit>& tingInfo)
{
	if (!gameInfo.b_CanTing)
	{
		return false;
	}
	//七小队
	HuUnit new_unit;

	std::vector<Lint> card_vector(40, 0);
	for (int x = 0; x<handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}

	if (true)
	{
		CardVector tmp(handcard);
		CardVector one, two, three, four;
		SortCard(tmp);
		GetSpecialOneTwoThreeFour(tmp, one, two, three, four);
		Lint twoNum = two.size() + four.size() + (three.size() / 3) * 2;
		Lint oneNum = one.size() + (three.size() - (three.size() / 3) * 2);
		if (oneNum == 1 && twoNum == 12)
		{
			if (!mgangCard.empty() || !agangCard.empty() || four.size() >= 4)
			{
				new_unit.hu_type = HU_DaQixiaodui;
				new_unit.m_hu_basicTypes.push_back(HU_DaQixiaodui);
			}
			else
			{
				new_unit.hu_type = HU_Qixiaodui;
				new_unit.m_hu_basicTypes.push_back(HU_Qixiaodui);
			}
			if (!one.empty())
			{
				new_unit.hu_card = one[0];
				tingInfo.push_back(new_unit);
			}
			if (three.size() == 3)
			{
				new_unit.hu_card = three[0];
				tingInfo.push_back(new_unit);
			}
			else if (three.size() == 6)
			{
				new_unit.hu_card = three[0];
				tingInfo.push_back(new_unit);
				new_unit.hu_card = three[3];
				tingInfo.push_back(new_unit);
			}
		}
	}
	//十三幺
	if (handcard.size() == 13)
	{
		Lint value[13] = {31,32,33,34,35,36,37,1,9,11,19,21,29};
		CardVector tmp(handcard);
		SortCard(tmp);
		std::vector<Lint> card_vector(40, 0);
		for (int x = 0; x<tmp.size(); x++)
		{
			card_vector[(tmp[x]->m_color - 1) * 10 + (tmp[x]->m_number) * 1]++;
		}
		Lint indexCount = sizeof(value) / sizeof(Lint);
		std::vector<Lint> indexExistVec;
		std::vector<Lint> indexVec;
		for (size_t i = 0; i < indexCount; i++)
		{
			Lint index = value[i];
			if (index >=0 && index <= 40 && card_vector[index] == 1)
			{
				indexExistVec.push_back(index);
			}
		}
		if (indexExistVec.size() == 13)
		{
			new_unit.hu_type = HU_ShiSanYao;
			for (size_t i = 0; i < indexCount; i++)
			{
				Lint index = value[i];
				new_unit.hu_card = get_card2(index);
				tingInfo.push_back(new_unit);
			}
			if (!tingInfo.empty())
			{
				return !tingInfo.empty();
			}
		}
		for (size_t i = 0; i < indexCount; i++)
		{
			Lint index = value[i];
			if (indexExistVec.size() == 11 && card_vector[index] == 2)
			{
				indexExistVec.push_back(index);
				indexExistVec.push_back(index);
			}
			if (card_vector[index] == 0)
			{
				indexVec.push_back(index);
				break;
			}
		}
		if (indexExistVec.size() == 13 && !indexVec.empty())
		{
			new_unit.hu_type = HU_ShiSanYao;
			Lint index = indexVec[0];
			new_unit.hu_card = get_card2(index);
			tingInfo.push_back(new_unit);
		}
	}
	if (CheckBaseNeed(handcard, pengCard, agangCard, mgangCard, eatCard, NULL, gameInfo))
	{
		CardVector tingcards;
		if (xiaohu_CheckTing(handcard, gameInfo, tingcards, eatCard.size(), pengCard.size() || agangCard.size() || mgangCard.size()))
		{
			EarseSameCard(tingcards);
			Lint conditionNum = std::count_if(tingcards.begin(), tingcards.end(), SetRange);
			for (int x = 0; x < tingcards.size(); x++)
			{
				if (KouDian == gameInfo.m_GameType && conditionNum > 0)
				{
					new_unit.hu_card = tingcards[x];
					tingInfo.push_back(new_unit);
				}
				if (TuiDaoHu == gameInfo.m_GameType)
				{
					new_unit.hu_card = tingcards[x];
					tingInfo.push_back(new_unit);
				}
			}
		}
	}
	return !tingInfo.empty();
}

bool CardManager::HasKezi(CardVector handcard, CardVector& pengCard, OperateState& gameInfo)
{
	bool hasKezi = false;
	if (pengCard.size()>0)
	{
		return true;
	}
	std::vector<Lint> card_vector(40, 0);
	for (int x = 0; x<handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}
	for (int x = 0; x < card_vector.size(); x++)
	{
		if (card_vector[x] > 2)
		{
			hasKezi = true;
			break;
		}
	}
	return hasKezi;
}

bool CardManager::xiaohu_CheckTing(CardVector handcard, OperateState& gameInfo, CardVector& tingCard, bool have_shun, bool have_kezi)
{
	std::vector<Lint> card_vector(40, 0);
	for (int x = 0; x<handcard.size(); x++)
	{
		if (handcard[x])
		{
			card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
		}
	}
	CardVector tingcards;
	for (int x = 0; x<card_vector.size(); x++)
	{
		int index = x % 10;
		if (x % 10 == 0)
		{
			continue;
		}
		else if (x>37)
		{
			break;
		}

		if (card_vector[x]>1)
		{
			card_vector[x] -= 2;
			Ting_explorer_zheng_function(card_vector, 0, 1, tingCard, NULL, gameInfo.b_37jia, gameInfo.m_GameType);
			card_vector[x] += 2;
		}
		if (card_vector[x]>0)
		{
			card_vector[x] -= 1;
			Ting_explorer_zheng_function(card_vector, 0, 0, tingCard, get_card(x), gameInfo.b_37jia, gameInfo.m_GameType);
			card_vector[x] += 1;
		}
	}
	return !tingCard.empty();
}


bool CardManager::xiaohu_CheckTing2(CardVector handcard, OperateState& gameInfo, CardVector& tingCard, bool have_shun, bool have_kezi)
{
	std::vector<Lint> card_vector(40, 0);
	Lint Lai_num = CheckLaiziNum(handcard, gameInfo);

	//第一步获取癞子,分数量统计
	for (int x = 0; x<handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}

	Lint num = std::count(card_vector.begin(), card_vector.end(), 2);//计算对子的数量

	for (int x = 0; x<card_vector.size(); x++)
	{
		int index = x % 10;
		if (x % 10 == 0)
		{
			continue;
		}
		else if (x>37)
		{
			break;
		}
		if (card_vector[x]>1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 2;
			xiao_hu_Ting_explorer_zheng_function(tmp_cards, 0, 1, tingCard, NULL, have_shun, have_kezi);

		}
		else if (card_vector[x] == 1 && Lai_num>0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 1;
			xiao_hu_Ting_explorer_zheng_function(tmp_cards, 0, 0, tingCard, get_card(x), have_shun, have_kezi);
		}
	}
	if (!tingCard.empty())
	{
		for (size_t i = 0; i < tingCard.size(); i++)
		{
			Card* card = tingCard[i];
			if (card != NULL)
			{
				LLOG_ERROR("xiaohu_CheckTing2, THING: m_color = %d, m_number = %d", card->m_color, card->m_number);
			}
		}

	}
	return !tingCard.empty();
}

void CardManager::xiao_hu_Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index, Lint Lai_num, CardVector& tingcards, Card* tingcard, bool have_shun, bool have_kezi)
{
	if (index<39)
	{
		if (index % 10 == 0)
		{
			index++;
		}

		if (card_vector[index]>0)
		{
			if (card_vector[index]>2)
			{
				card_vector[index] -= 3;
				xiao_hu_Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, have_shun, true);
				card_vector[index] += 3;
			}
			if (card_vector[index]>1 && Lai_num>0)
			{
				card_vector[index] -= 2;
				xiao_hu_Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), have_shun, true);
				card_vector[index] += 2;
			}
			if (index % 10 < 8)
			{
				if (card_vector[index] > 0 && card_vector[index + 1] > 0 && card_vector[index + 2] > 0)
				{
					card_vector[index] -= 1;
					card_vector[index + 1] -= 1;
					card_vector[index + 2] -= 1;
					xiao_hu_Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, true, have_kezi);
					card_vector[index] += 1;
					card_vector[index + 1] += 1;
					card_vector[index + 2] += 1;
				}

				if (Lai_num)
				{
					Lint ting_index = 0;
					if (card_vector[index + 1] > 0 && card_vector[index + 2] > 0)
					{
						card_vector[index + 1] -= 1;
						card_vector[index + 2] -= 1;
						xiao_hu_Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), true, have_kezi);
						card_vector[index + 1] += 1;
						card_vector[index + 2] += 1;
					}
					if (card_vector[index + 1] > 0 && card_vector[index] > 0)
					{
						card_vector[index + 1] -= 1;
						card_vector[index] -= 1;
						xiao_hu_Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index + 2), true, have_kezi);
						card_vector[index + 1] += 1;
						card_vector[index] += 1;
					}
					if (card_vector[index + 2] > 0 && card_vector[index] > 0)
					{
						card_vector[index + 2] -= 1;
						card_vector[index] -= 1;
						xiao_hu_Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index + 1), true, have_kezi);
						card_vector[index + 2] += 1;
						card_vector[index] += 1;
					}
				}
			}
		}
		else {
			if (Lai_num>0 && card_vector[index + 2]>0 && card_vector[index + 1]>0)
			{
				card_vector[index + 2]--;
				card_vector[index + 1]--;
				xiao_hu_Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), true, have_kezi);
				card_vector[index + 2]++;
				card_vector[index + 1]++;
			}
			index++;
			xiao_hu_Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, have_shun, have_kezi);
		}
	}
	else {
		if (tingcard)
		{
			tingcards.push_back(tingcard);
		}
	}
}


void CardManager::Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index, Lint Lai_num, CardVector& tingcards, Card* tingcard, bool hu_37jia, bool have_shun, bool have_kezi)
{
	if (index<39)
	{
		if (index % 10 == 0)
		{
			index++;
		}

		if (card_vector[index]>0)
		{
			if (card_vector[index]>2)
			{
				card_vector[index] -= 3;
				Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, hu_37jia);
				card_vector[index] += 3;
			}

			if (card_vector[index]>1 && Lai_num>0)
			{
				card_vector[index] -= 2;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), hu_37jia);
				card_vector[index] += 2;
			}

			if (index % 10<8 && index<30 && card_vector[index]>0 && card_vector[index + 2]>0 && card_vector[index + 1]>0)
			{
				card_vector[index]--;
				card_vector[index + 1]--;
				card_vector[index + 2]--;
				Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, hu_37jia);
				card_vector[index]++;
				card_vector[index + 1]++;
				card_vector[index + 2]++;
			}
			if (Lai_num > 0 && index<30 && card_vector[index]>0 && card_vector[index + 2]>0)
			{
				card_vector[index]--;
				card_vector[index + 2]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index + 1), hu_37jia);
				card_vector[index]++;
				card_vector[index + 2]++;

			}
			if (Lai_num > 0 && index<30 && card_vector[index]>0 && card_vector[index + 1]>0)
			{
				card_vector[index]--;
				card_vector[index + 1]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index + 2), hu_37jia);
				card_vector[index]++;
				card_vector[index + 1]++;

			}
			if (Lai_num > 0 && index<30 && card_vector[index + 1]>0 && card_vector[index + 2]>0)
			{
				card_vector[index + 1]--;
				card_vector[index + 2]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), hu_37jia);
				card_vector[index + 1]++;
				card_vector[index + 2]++;

			}
			if (hu_37jia&&index % 10 == 1 && index<30 && Lai_num>0 && card_vector[index + 1]>0)
			{
				card_vector[index]--;
				card_vector[index + 1]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index + 2), hu_37jia);
				card_vector[index]++;
				card_vector[index + 1]++;
			}
			if (hu_37jia&&index % 10 == 7 && index<30 && Lai_num>0 && card_vector[index + 2]>0 && card_vector[index + 1]>0)
			{
				card_vector[index + 2]--;
				card_vector[index + 1]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), hu_37jia);
				card_vector[index + 2]++;
				card_vector[index + 1]++;
			}
		}
		else
		{
			//某类型牌数量为0
			if (Lai_num > 0 && index<30 && card_vector[index + 1]>0 && card_vector[index + 2]>0)
			{
				card_vector[index + 1]--;
				card_vector[index + 2]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), hu_37jia);
				card_vector[index + 1]++;
				card_vector[index + 2]++;

			}
			if (hu_37jia&&index % 10 == 7 && Lai_num>0 && index<30 && card_vector[index + 2]>0 && card_vector[index + 1]>0)
			{
				card_vector[index + 2]--;
				card_vector[index + 1]--;
				Ting_explorer_zheng_function(card_vector, index, 0, tingcards, get_card(index), hu_37jia);
				card_vector[index + 2]++;
				card_vector[index + 1]++;
			}
			index++;
			Ting_explorer_zheng_function(card_vector, index, Lai_num, tingcards, tingcard, hu_37jia);
		}
	}
	else
	{
		if (tingcard)
		{
			tingcards.push_back(tingcard);
		}
	}
}

Lint CardManager::ting_judge_shun(std::vector<Lint>& targe_vector, Lint index)
{
	if (targe_vector[index]>0 && targe_vector[index + 2]>0)
	{
		targe_vector[index]--;
		targe_vector[index + 2]--;
		return true;
	}
	else {
		return false;
	}
}

bool CardManager::explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num)
{
	if (index<39)
	{
		if (index % 10 == 0)
		{
			index++;
		}

		if (cards[index]>0)
		{
			std::vector<Lint> tmp_cards(cards);
			Lint ke_dif = Lai_num - judge_ke(tmp_cards, index);
			if (ke_dif >= 0)
			{
				if (explorer_zheng_function(tmp_cards, index, ke_dif))
				{
					return true;
				}
			}

			if (index % 10<8 && index<30)
			{
				std::vector<Lint> tmp_cards2(cards);
				Lint ting_index = 0;
				Lint shun_dif = Lai_num - judge_shun(tmp_cards2, index);
				if (shun_dif >= 0)
				{
					if (explorer_zheng_function(tmp_cards2, index, shun_dif))
					{
						return true;
					}
				}
			}
			return false;
		}
		else
		{
			if (Lai_num>0 && index % 10<8 && index<30)
			{
				std::vector<Lint> tmp_cards(cards);
				Lint ting_index = 0;
				Lint shun_dif = Lai_num - judge_shun(tmp_cards, index);
				if (shun_dif >= 0)
				{
					if (explorer_zheng_function(tmp_cards, index, shun_dif))
					{
						return true;
					}
				}
			}
			index++;
			LLOG_ERROR("=============%d", index);
			if (explorer_zheng_function(cards, index, Lai_num))
			{
				return true;
			}
			return false;
		}
	}
	else
	{
		return true;
	}
}

//摸到牌后判断听 (多一张牌）
bool CardManager::all_CheckTing(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, std::vector<TingUnit>& tingInfo)
{
	CardVector tmp(handcard);
	if (getCard)
		tmp.push_back(getCard);
	SortCard(tmp);
	EarseSameCard(tmp);

	for (int x = 0; x < tmp.size(); ++x)
	{
		CardVector tmp2(handcard);
		if (getCard)
			tmp2.push_back(getCard);
		SortCard(tmp2);
		EraseCard(tmp2, tmp[x], 1);
		TingUnit xx;
		if (CheckCanTing(tmp2, pengCard, agangCard, mgangCard, eatCard, gameInfo, xx.hu_units))
		{
			xx.out_card = tmp[x];
			tingInfo.push_back(xx);
		}
	}
	return !tingInfo.empty();
}

bool CardManager::CheckPiaoHu(CardVector & handcard, CardVector & eatCard, Card * outCard)
{
	if (!eatCard.empty())
	{
		return false;
	}

	CardVector check, one, tow, three, four;
	check = handcard;
	if (outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}

	GetSpecialOneTwoThreeFour(check, one, tow, three, four);

	if (one.size() || four.size() || tow.size() != 2)
		return false;

	return true;
}

bool CardManager::CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo)
{
	special = 0;
	CardVector all = handcard;
	Lint nLaiZiCount = 0;
	if (isGetCard)
	{
		if (outCard)
		{
			all.push_back(outCard);
		}
		for (Lint i = 0; i < all.size(); )
		{
			if (false&&all[i]->m_color == 4 && all[i]->m_number == 5)
			{
				gCardMgr.EraseCard(all, all[i]);
				nLaiZiCount++;
			}
			else
			{
				++i;
			}
		}
	}
	else
	{
		for (Lint i = 0; i < all.size(); )
		{
			if (false&&all[i]->m_color == 4 && all[i]->m_number == 5)
			{
				gCardMgr.EraseCard(all, all[i]);
				nLaiZiCount++;
			}
			else
			{
				++i;
			}
		}
		if (outCard)
		{
			all.push_back(outCard);
		}
	}
	gCardMgr.SortCard(all);
	if (nLaiZiCount > 0)
	{
		if (nLaiZiCount + all.size() != 14)
		{
			return false;
		}
		CardVector one, tow, three, four;
		GetSpecialOneTwoThreeFour(all, one, tow, three, four);
		if (one.size() + three.size() / 3 <= nLaiZiCount)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (all.size() != 14)
		{
			return false;
		}
		CardVector one, tow, three, four;
		GetSpecialOneTwoThreeFour(all, one, tow, three, four);
		if (one.empty() && three.empty())
		{
			if (four.size() == 4)
			{
				special = 1;
			}
			else if (four.size()>4)
			{
				special = 2;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool CardManager::CheckShiSanYiao(CardVector & handcard, Card * outCard)
{
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	SortCard(tmp);
	EarseSameCard(tmp);
	if (tmp.size() < 13)
	{
		return false;		//手牌不够十三张，肯定不是十三
	}
	for (Lint i = 0; i < 13; i++)
	{
		if (tmp[i]->m_color != m_13yao[i]->m_color || tmp[i]->m_number != m_13yao[i]->m_number)
		{
			return false;
		}
	}
	return true;
}