#include "CardSx.h"
#include "LTool.h"
#include "LLog.h"
#include "LTime.h"
#include "Utils.h"

#include "malgo.h"
#include "ShanXiFeatureToggles.h"

#include <iostream>
#include <iomanip>

#define SUIJUN_LOG LLOG_ERROR
// 清一色是指手牌、杠牌、碰牌都是一种颜色的
// 全是万，或者全是筒，或者全是条，全是字不行
// 洪洞王牌的清一色 不能有字牌，且妃子牌不能是其他颜色，和不带混子的判断一致
static FANXING hongtong_judge_qingyise(tile_mask_t mask, tile_t tile_haozi) {
	//mask &= TILE_MASK_SHUPAI; // 字牌除外
	if (0 == (mask & ~TILE_MASK_WAN)) return FAN_QingYiSe;
	if (0 == (mask & ~TILE_MASK_TONG)) return FAN_QingYiSe;
	if (0 == (mask & ~TILE_MASK_TIAO)) return FAN_QingYiSe;
	return FAN_NONE;
}

// 凑一色是指万、筒、条有且必须只能有一种，必须有字牌，没有字牌就是清一色了
// 洪洞王牌中如果只有妃子牌， 筒，万，条中的一门，包括风，字牌，也算是凑一色
// 洪洞王牌中凑一色: 一门牌 加字 或者 1门牌加妃子
static FANXING hongtong_judge_couyise(tile_mask_t mask, tile_t tile_haozi) {
	// 没有字牌不行，没有妃子不行
	if ( (0 == (mask & TILE_MASK_ZIPAI))
		&& (0 == (mask & TILE_TO_MASK(tile_haozi)))) return FAN_NONE;
	// 没有数牌不行
	if (0 == (mask & TILE_MASK_SHUPAI)) return FAN_NONE;
	// 然后排除字牌，剩下的是清一色就行了
	mask &= TILE_MASK_SHUPAI; // 字牌除外
	// 排除掉耗子牌
	mask &= ~TILE_TO_MASK(tile_haozi);
	return hongtong_judge_qingyise(mask, tile_haozi) ? FAN_CouYiSe : FAN_NONE;
}

static FANXING hongtong_judge_ziyise(tile_mask_t mask, tile_t tile_haozi) {
	// 有数牌不行 
	if ((mask & TILE_MASK_SHUPAI)) return FAN_NONE;	
	return FAN_ZiYiSe;
}



// 满足一条龙
static bool hongtong_can_be_yitiaolong(tile_mask_t mask, tile_t tile_haozi) {
	// 万牌够9张
	if (TILE_MASK_WAN == (mask & TILE_MASK_WAN)) return true;
	// 筒子牌够9张
	if (TILE_MASK_TONG == (mask & TILE_MASK_TONG)) return true;
	// 梭子牌够9张
	if (TILE_MASK_TIAO == (mask & TILE_MASK_TIAO)) return true;
	return false;
}

static FANXING hongtong_reduce_fans(FANXING fans, tile_mask_t all_mask, tile_mask_t hand_mask, tile_t tile_haozi)
{
	FANXING result = fans;

	if (hongtong_judge_qingyise(all_mask, tile_haozi))
	{
		result |= FAN_QingYiSe;
	}
	else
	{
		result &= (~FAN_QingYiSe);
	}

	if (result & FAN_QingYiSe)
	{
	}
	else //不是清一色才判断凑一色
	{
		if (hongtong_judge_couyise(all_mask, tile_haozi))
		{
			result |= FAN_CouYiSe;
		}
		else
		{
			result &= ~(FAN_CouYiSe);
		}
	}
	if (result & FAN_ZiYiSe)
	{
		if (hongtong_judge_ziyise(all_mask, tile_haozi))
		{
		}
		else
		{
			result &= ~FAN_ZiYiSe;
		}
	}

	if(hongtong_can_be_yitiaolong(hand_mask, tile_haozi))
	{
	}
	else
	{
		result &= ~(FAN_YiTiaoLong);
	}

	return result;
}

// 洪洞王牌判断缺门数目是否满足
static bool hongtong_judge_quemen(const tile_mask_t hand_mask, const tile_mask_t pg_mask, const tile_t last_tile, const FANXING conf)
{
	tile_mask_t mask = hand_mask | pg_mask | ( 0 == last_tile ? 0 : TILE_TO_MASK(last_tile));
	int que_men_count = 0;
	if (0 == ( mask  & TILE_MASK_WAN)) ++que_men_count;
	if (0 == ( mask  & TILE_MASK_TONG)) ++que_men_count;
	if (0 == ( mask & TILE_MASK_TIAO)) ++que_men_count;

	// 缺一门玩法，默认
	if (conf & FAN_MUST_QueMen)
	{
		if (que_men_count < 1) return false;
	}
	 
	// 洪洞王牌中缺2门的玩法
	if (conf & FAN_HongTong_MustQueLiangMen)
	{
		if (que_men_count < 2) return false;
	}	
	return true;
}

static bool SetRange(Card* outCard)
{
	if (outCard->m_number >= 6 || outCard->m_color > 3)
	{
		return true;
	}
	return false;
}

static bool SetRange2(HuUnit& huUnit)
{
	if (huUnit.hu_card->m_color >3 || huUnit.hu_card->m_number >= 6)
	{
		return true;
	}
	return false;
}

static bool CompRange(HuUnit& huUnit, HuUnit& huUnit2)
{
	return huUnit.hu_card->m_number > huUnit2.hu_card->m_number;
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

bool CardManagerSx::Init()
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

bool CardManagerSx::Final()
{
	return true;
}

void CardManagerSx::SwapCardBySpecial(CardVector& cvIn, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84])
{
	Lint cardcount = cvIn.size();
	std::list<Card*> card(cvIn.begin(), cvIn.end());
	CardVector playCard[5];
	Lint playcount[4] = {0};
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

void CardManagerSx::DealCard2(CardVector& m_3_laizi, CardVector hand_card[], Lint user_count, CardVector& rest_card, const Card player1Card[13], const Card player2Card[13], const Card player3Card[13], const Card player4Card[13], const Card SendCard[84], Lint game_type, GameOptionSet& playtype)
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

void CardManagerSx::DealCard(CardVector& m_3_laizi, CardVector hand_card[], Lint user_count, CardVector& rest_card, Lint game_type, GameOptionSet& playtype)
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

void CardManagerSx::SortCard(CardVector& vec)
{
	std::sort(vec.begin(), vec.end(), CardSortFun);
}

void CardManagerSx::EraseCard(CardVector& src, CardVector& des)
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

void CardManagerSx::EraseCard(CardVector& src, Card* pCard)
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
void CardManagerSx::EraseCard(CardVector& src, Card* pCard, Lint n)
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

void CardManagerSx::EarseSameCard(CardVector& src)
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

ThinkVec CardManagerSx::CheckOutCardOperator(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{
	ThinkVec result;
	ThinkUnit unit;
	bool bTing = false;
	if (gameInfo.b_CanHu&&CheckCanHu(ps, tingstate, handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, toggles, unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}
	if (gameInfo.b_CanDianGang&&CheckCanGang(handcard, suoCard, outCard, true, false, gameInfo, toggles))
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
		//if (CheckCanTing(ps, tmp, pengCard, agangCard, tmp_minggangcard, eatCard, gameInfo, toggles, xx.hu_units))
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

	if (gameInfo.b_CanPeng&&CheckCanPeng(handcard, suoCard, outCard, true))
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
		//if (all_CheckTing(tmp, tmp_pengcard, agangCard, mgangCard, eatCard, NULL, gameInfo, toggles, unit.ting_unit))
		//{
		//	unit.m_type = THINK_OPERATOR_PENGTING;
		//	unit.m_card.push_back(outCard);
		//	result.push_back(unit);
		//	bTing = true;
		//}
	}
	return result;
}

//用于不报听查听（**Ren 2017-12-27）
ThinkVec CardManagerSx::CheckCanTing_Check(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{
	ThinkVec result;
	ThinkUnit unit;
	if (all_CheckTing(ps, handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, toggles, unit.ting_unit))
	{
		unit.m_type = THINK_OPERATOR_TING;
		for (size_t x = 0; x < unit.ting_unit.size(); x++)
		{
			if (unit.ting_unit[x].out_card)
			{
				unit.m_card.push_back(unit.ting_unit[x].out_card);
			}
		}
		result.push_back(unit);
	}
	return result;
}

ThinkVec CardManagerSx::CheckGetCardOperator(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, CardVector &suoCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{
	ThinkVec result;
	ThinkUnit unit;
	bool bTing = false;
	//判断可以胡
	if (!gameInfo.b_CanTing && gameInfo.b_CanHu && CheckCanHu(ps, tingstate, handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, toggles, unit.m_hu, true))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		//else if (handcard.size() == 14) // 天胡
		//{
		//	unit.m_card.push_back(handcard.back());
		//}
		result.push_back(unit);
	}
	CardVector tmp = handcard;
	if (getCard)
	{
		tmp.push_back(getCard);
	}
	SortCard(tmp);

	std::vector<Card*> vec;
	if (gameInfo.b_CanAnGang && CheckAnGang(tmp, suoCard, vec, gameInfo, toggles))
	{
		for (Lsize i = 0; i < vec.size(); i++)
		{
			bool bCanAngGang = true;
			if ((HongTongWangPai == toggles.FEAT_GAME_TYPE) && (gameInfo.allowed_fans & FAN_HongTong_MustQueLiangMen)) // 洪洞王牌缺2门玩法
			{
				tile_mask_t pg_mask = ps.make_pg_mask();
				tile_mask_t get_tile_mask = TILE_TO_MASK(ToTile(vec[i]));
				//pg_mask &= ~TILE_TO_MASK(ToTile(gameInfo.m_hCard[0])); // 排除万能牌(**Ren 2017-12-27)
				pg_mask &= ~TILE_MASK_ZIPAI;                       // 排除字牌				
				if (VecHas(gameInfo.m_hCard, vec[i])) //万能牌
				{
				}
				else
				{
					int quemen_count = ps.get_quemen_count(pg_mask | get_tile_mask); // 计算缺门数量
					if (quemen_count < 2)      //破了缺2门，不允许再杠
					{
						bCanAngGang = false;
					}
					else
					{
					}
				}
			}
			//一门牌中如果暗杠破坏了一门牌则不允许暗杠（**Ren 2017-12-23）
			if (LinFenYiMenPai == toggles.FEAT_GAME_TYPE)
			{
				tile_mask_t pg_mask = ps.make_pg_mask();
				tile_mask_t get_tile_mask = TILE_TO_MASK(ToTile(vec[i]));
				int quemen_count = ps.get_quemen_count(pg_mask | get_tile_mask);
				if (quemen_count < 2)
				{
					bCanAngGang = false;
				}
			}

			if (bCanAngGang)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_AGANG;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}

			////判断暗杠听
			//CardVector tmp2(tmp);
			//EraseCard(tmp2, vec[i], 4);
			//CardVector tmp_angangcard(agangCard);
			//tmp_angangcard.push_back(vec[i]);

			////暗杠后可以听
			//TingUnit xx;
			//if (CheckCanTing(ps, tmp2, pengCard, tmp_angangcard, mgangCard, eatCard, gameInfo, toggles, xx.hu_units))
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
	if (gameInfo.b_CanMingGang && CheckMGang(tmp, pengCard, suoCard, vec, gameInfo, toggles))
	{
		for (Lsize i = 0; i < vec.size(); ++i)
		{
			bool bCanMingGang = true;
			if ((HongTongWangPai == toggles.FEAT_GAME_TYPE) && (gameInfo.allowed_fans & FAN_HongTong_MustQueLiangMen)) // 洪洞王牌缺2门玩法
			{
				tile_mask_t pg_mask = ps.make_pg_mask();
				tile_mask_t get_tile_mask = TILE_TO_MASK(ToTile(vec[i]));
				//pg_mask &= ~TILE_TO_MASK(ToTile(gameInfo.m_hCard[0])); // 排除万能牌(**Ren 2017-12-27)
				pg_mask &= ~TILE_MASK_ZIPAI;                       // 排除字牌				
				if (VecHas(gameInfo.m_hCard, vec[i])) //万能牌
				{
				}
				else
				{
					int quemen_count = ps.get_quemen_count(pg_mask | get_tile_mask); // 计算缺门数量
					if (quemen_count < 2)      //破了缺2门，不允许再杠
					{
						bCanMingGang = false;
					}
					else
					{
					}
				}
			}
			if (bCanMingGang)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MGANG;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}
			////判断暗杠听
			//CardVector tmp2(tmp);
			//EraseCard(tmp2, vec[i], 1);
			////明杠后可以听
			//unit.Clear();
			//TingUnit xx;
			//if (CheckCanTing(ps, tmp2, pengCard, agangCard, mgangCard, eatCard, gameInfo, toggles, xx.hu_units))
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
	if (gameInfo.b_CanTing && all_CheckTing(ps, handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, toggles, unit.ting_unit))
	{
		unit.m_type = THINK_OPERATOR_TING;
		for (size_t x = 0; x < unit.ting_unit.size(); x++)
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

static tile_mask_t hu_units_to_tile_mask(std::vector<HuUnit> const &units) {
	tile_mask_t mask = 0;
	for (size_t i = 0; i < units.size(); ++i) {
		mask |= TILE_TO_MASK(ToTile(units[i].hu_card));
	}
	MHLOG("***********************Mask = %x*****", mask);
	return mask;
}

static bool is_TingKou_equal(std::vector<HuUnit> const &lhs, std::vector<HuUnit> const &rhs) {
	return hu_units_to_tile_mask(lhs) == hu_units_to_tile_mask(rhs);
}

// churunmin：2017-3-29：听后杠规则最新变更：杠后的听口不需要和杠前的听口完全相同，只需要是子集即可（把听口看成牌值的集合），但是还得满足杠后的听口必须不能为空
static bool TingKou_is_subset_of_old(std::vector<HuUnit> const &lhs, std::vector<HuUnit> const &rhs) {
	return tile_mask_is_subset_of(hu_units_to_tile_mask(lhs), hu_units_to_tile_mask(rhs));
}
static bool can_TingHouGang(std::vector<HuUnit> const &after_gang, std::vector<HuUnit> const &before_gang) {
	if (after_gang.empty()) return false;
	return TingKou_is_subset_of_old(after_gang, before_gang);
}

ThinkVec CardManagerSx::CheckOutCardOpeartorAfterTing(PlayerState &ps, TingState & tingstate, CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector & eatCard, CardVector &suoCard, Card * outCard, OperateState & gameInfo, ShanXiFeatureToggles const &toggles)
{
	ThinkVec result;
	ThinkUnit unit;	
	if (gameInfo.b_CanHu && CheckCanHuAfterTing(ps, tingstate, outCard, gameInfo, toggles, unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}
	if(gameInfo.m_GameType == LiSi)
	{
		return result;
	}
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	SortCard(tmp);
	
	if (gameInfo.b_CanDianGang && CheckCanGang(handcard, suoCard, outCard, true, false, gameInfo, toggles))
	{		
		// 推倒胡：豪华七小对听牌的情况下不允许杠牌
		if (gameInfo.m_GameType==TuiDaoHu && !tingstate.hu_units.empty() && tingstate.hu_units.front().hu_type == HU_DaQixiaodui) {
			return result;
		}
		//撵中子：只有门清的听口是不可以杠牌的（**Ren2017-11-30）
		if (NianZhongZi == gameInfo.m_GameType)
		{
			std::vector<HuUnit> hu_unit = tingstate.hu_units;
			size_t i = hu_unit.size();
			bool flag_menqing = false;
			while (i--)
			{
				if (!((hu_unit[i].fans & ~FAN_ZiMo) & ~(FAN_MenQing | FAN_PingHu)))
				{
					flag_menqing = true;
				}
			}
			if (flag_menqing)
			{
				return result;
			}
		}

		CardVector tmp2(tmp);
		EraseCard(tmp2, outCard, 4);

		// 使用临时的明杠数组存放用户的杠牌
		CardVector tmp_mgangcard(mgangCard);
		for (int i = 0; i < 4; i++)
		{
			tmp_mgangcard.push_back(outCard);
		}

		//临时的状态
		PlayerState tmpPs(ps);
		GangItem gangItem;
		gangItem.tile = ToTile(outCard);
		gangItem.fire_pos = gameInfo.m_playerPos;
		tmpPs.gangs.add(gangItem);
		
		//明杠后可以听
		unit.Clear();
		TingUnit xx;
		gameInfo.b_CanTing = true; // 杠后听检测要临时打开这个 b_CanTing
		if (CheckCanTing(tmpPs, tmp2, pengCard, agangCard, tmp_mgangcard, eatCard, gameInfo, toggles, xx.hu_units))
		{			
			if (can_TingHouGang(xx.hu_units, tingstate.hu_units))
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MGANG;
				unit.m_card.push_back(outCard);
				//result.push_back(unit);     -- 放到洪洞王牌修正杠后胡口的后面了
				//洪洞王牌暂存杠后听口，用于修正杠后胡口
				MHLOG("************洪洞王牌暂存杠后听口，用于修正杠后胡口********");				
				//if ((HongTongWangPai == gameInfo.m_GameType) && (xx.hu_units.size() != tingstate.hu_units.size()))
				//if ((xx.hu_units.size() != tingstate.hu_units.size()))
				{
					MHLOG("if (xx.hu_units.size() != tingstate.hu_units.size()))");
					xx.out_card = outCard;
					unit.ting_unit.push_back(xx);
				}
				result.push_back(unit);
			}else if(outCard && VecHas(gameInfo.m_hCard, *outCard))//抬耗子
			{
				tingstate.hu_units = xx.hu_units;
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
ThinkVec CardManagerSx::CheckGetCardOpeartorAfterTing(PlayerState &ps, TingState & tingstate, CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector & eatCard, CardVector &suoCard, Card* getCard, OperateState & gameInfo, ShanXiFeatureToggles const &toggles)
{
	ThinkVec result;
	ThinkUnit unit;
	//判断可以胡
	if (CheckCanHuAfterTing(ps, tingstate, getCard, gameInfo, toggles, unit.m_hu, true)) {
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard) unit.m_card.push_back(getCard);
		result.push_back(unit);
	}

	CardVector tmp = handcard;
	if (getCard) tmp.push_back(getCard);
	SortCard(tmp);

	// 就算玩法或者小选项允许杠，杠后如果听口改变也不允许杠
	std::vector<Card*> vec;
	if (gameInfo.b_CanAnGang && CheckAnGang(tmp, suoCard, vec, gameInfo, toggles)) {
		// 推倒胡：豪华七小对听牌的情况下不允许杠牌
		if (gameInfo.m_GameType == TuiDaoHu && !tingstate.hu_units.empty() && tingstate.hu_units.front().hu_type == HU_DaQixiaodui) {
			return result;
		}

		for (Lsize i = 0; i < vec.size(); i++) {
			CardVector tmp2(tmp);
			EraseCard(tmp2, vec[i], 4);
			CardVector tmp_angangcard(agangCard);
			tmp_angangcard.push_back(vec[i]);

			//临时的状态
			PlayerState tmpPs(ps);
			GangItem gangItem; 
			gangItem.tile = ToTile(vec[i]);
			tmpPs.gangs.add(gangItem);
			

			//暗杠后可以听
			TingUnit xx;
			gameInfo.b_CanTing = true; // 杠后听检测要临时打开这个 b_CanTing
			if (CheckCanTing(tmpPs, tmp2, pengCard, tmp_angangcard, mgangCard, eatCard, gameInfo, toggles, xx.hu_units)) {
				if (can_TingHouGang(xx.hu_units, tingstate.hu_units)) {
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;
					unit.m_card.push_back(vec[i]);
					//result.push_back(unit);
					//洪洞王牌暂存杠后听口，用于修正杠后胡口					 
					//if ((HongTongWangPai == gameInfo.m_GameType) && (xx.hu_units.size() != tingstate.hu_units.size()))
					//if ( (xx.hu_units.size() != tingstate.hu_units.size()))
					{
						xx.out_card = vec[i];
						unit.ting_unit.push_back(xx);
					}
					result.push_back(unit);
				}
			}
		}
	}

	vec.clear();
	if (gameInfo.b_CanMingGang && CheckMGang(getCard,pengCard, vec, gameInfo, toggles)) {
		for (Lsize i = 0; i < vec.size(); ++i) {
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(vec[i]);
			result.push_back(unit);
		}
	}
	return result;
}

ThinkVec CardManagerSx::CheckAfterPeng(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{// pengCard是碰了之后形成的3张牌
	//判断碰听
	CardVector tmp(handcard);
	//EraseCard(tmp, outCard, 2); // 这句不需要，因为手中碰的牌已经被删掉了，多删可能会出问题
	CardVector tmp_pengcard(pengCard);
	//tmp_pengcard.push_back(outCard); // 这里也不能再把那张碰牌push一次，因为pengCard里面已经是3张牌了

	ThinkVec result;
	ThinkUnit unit;
	TingUnit xx;

	//bool can_ting = CheckCanTing(ps, handcard, pengCard, agangCard, mgangCard, eatCard, gameInfo, toggles, xx.hu_units);
	if (all_CheckTing(ps, tmp, tmp_pengcard, agangCard, mgangCard, eatCard, NULL, gameInfo, toggles, unit.ting_unit))
	{
		unit.m_type = THINK_OPERATOR_TING;
		//unit.m_card.push_back(outCard);
		for (size_t x = 0; x < unit.ting_unit.size(); x++)
		{
			if (unit.ting_unit[x].out_card)
			{
				unit.m_card.push_back(unit.ting_unit[x].out_card);
			}
		}
		result.push_back(unit);
	}
	/*if (can_ting)
	{
		unit.Clear();
		unit.m_type = THINK_OPERATOR_TING;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}*/
	return result;
}

tile_mask_t calc_pg_mask(CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard) {
	tile_mask_t mask = 0;
	for (size_t i = 0; i < pengCard.size(); ++i) {
		if (NULL == pengCard[i]) continue; // sanity check
		mask |= TILE_TO_MASK(ToTile(pengCard[i]));
	}
	for (size_t i = 0; i < agangCard.size(); ++i) {
		if (NULL == agangCard[i]) continue; // sanity check
		mask |= TILE_TO_MASK(ToTile(agangCard[i]));
	}
	for (size_t i = 0; i < mgangCard.size(); ++i) {
		if (NULL == mgangCard[i]) continue; // sanity check
		mask |= TILE_TO_MASK(ToTile(mgangCard[i]));
	}
	return mask;
}



bool CardManagerSx::CheckCanHu(PlayerState &ps, TingState& tingstate, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<Hu_type>& vec, bool isGetCard)
{
	// 强制报听的玩法，不会走到这里
#if 1
	TileVec hands;
	unsigned num_huns = 0;
	for (size_t i = 0; i < handcard.size(); ++i) {
		if (NULL == handcard[i]) continue; // sanity check
		if (VecHas(gameInfo.m_hCard, *handcard[i])) {
			++num_huns;
			continue;
		}
		tile_t tile = CardToTile(handcard[i]);
		hands.add(tile); 
	}
	tile_t last_tile = 0;
	if (outCard) {
		last_tile = CardToTile(outCard);
		if (VecHas(gameInfo.m_hCard, outCard))
		{
			++num_huns;
		}
		else
		{
			hands.add(last_tile);
		}
	} else {
		// churunmin: 应该肯定有outCard吧？应该不会走到这里
		if (hands.size()) last_tile = hands.back();
	}
	FANXING conf = gameInfo.allowed_fans;
	if (isGetCard) conf |= FAN_ZiMo; // 抠点自摸的胡牌范围大
	FANXING fans = toggles.call_judge(ps, num_huns, hands, last_tile, conf, calc_pg_mask(pengCard, agangCard, mgangCard), ps.sum_pg_tile_count_by_color());
	if (fans) {
		Hu_type hu;
		if (fans & FAN_ShiSanYao) hu.m_hu_type = HU_ShiSanYao;
		else if (fans & FAN_HaoHuaQiXiaoDui) hu.m_hu_type = HU_DaQixiaodui;
		else if (fans & FAN_QiXiaoDui) hu.m_hu_type = HU_Qixiaodui;
		hu.m_hu_way.push_back(1);
		vec.push_back(hu);
	}

	return !vec.empty();

#else

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
				hu.m_hu_type = HU_DaQixiaodui;
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
			else {
				Hu_type hu;
				hu.m_hu_type = HU_Qixiaodui;
				hu.m_hu_way.push_back(1);
				vec.push_back(hu);
			}
		}
		if (CheckShiSanYiao(handcard, outCard))
		{
			Hu_type hu;
			hu.m_hu_type = HU_ShiSanYao;
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
		
	}
	else if(gameInfo.m_GameType == LiSi)
	{

	}
	return !vec.empty();
#endif
}

bool CardManagerSx::CheckCanHuAfterTing(PlayerState &ps, TingState& tingstate, Card* outCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<Hu_type>& vec, bool isGetCard)
{
	if (NULL == outCard) return false; // 安全监测
	
	MHLOG("***********CheckCanHuAfterTing王牌的万能牌已经在胡口中");
	if (toggles.FEAT_GAME_TYPE == HongTongWangPai)
	{
	}
	else
	{
		// 抠点：别人出的万能牌只能当牌值本身来用，不能当万能牌用
		// 自摸到万能牌，应该胡一个最大分的，不过这里不需要找最大分的牌值啊，只需要知道胡不胡就行了
		// 万能牌命中听口中任何一个元素，肯定胡啊
		if (isGetCard && !gameInfo.m_hCard.empty())
		{
			if (outCard && VecHas(gameInfo.m_hCard, *outCard))
			{
				std::sort(tingstate.hu_units.begin(), tingstate.hu_units.end(), CompRange);
				Hu_type hu;
				hu.m_hu_type = tingstate.hu_units[0].hu_type;
				hu.m_hufan = tingstate.hu_units.front().hu_card->m_number;
				vec.push_back(hu);
			}
		}
	}
	/*
	//洪洞王牌的中发白顺只能自摸，三风顺牌，万能牌也不能命中胡口
	if (toggles.FEAT_GAME_TYPE == HongTongWangPai)
	{
		std::vector<HuUnit>::iterator it = tingstate.hu_units.begin();
		for (; it < tingstate.hu_units.end(); it++)
		{
			FANXING fans = (*it).fans;
			tile_t tile = ToTile((*it).hu_card);
			if (fans & (FAN_HongTong_YingKou | FAN_HongTong_LouShangLou))
			{
				vec.clear(); // 清除掉前面万能牌命中的胡口
				MHLOG("********洪洞王牌清除万能牌胡口命中 fans=%s", malgo_format_fans(fans).c_str());
			}

			if ( tile > 40 && tile < 45 && (fans &(FAN_JUDGE_SanFeng)) )
			{
				vec.clear(); // 清除掉前面万能牌命中的胡口
				MHLOG("********洪洞王牌清除万能牌胡口命中 fans=%s", malgo_format_fans(fans).c_str());
			}

			if (fans &(FAN_QiXiaoDui))
			{
				if ((*it).hu_card != (*outCard))
				{
					vec.clear();
					MHLOG("********洪洞王牌清除万能牌胡口命中 fans=%s", malgo_format_fans(fans).c_str());
				} 
			}
		}	
	}
	*/
	// 这里是根据先前计算的听口来判断胡不胡
	for (size_t x = 0; x < tingstate.hu_units.size(); x++)
	{
		if (tingstate.hu_units[x].hu_card->m_color == outCard->m_color
			&&tingstate.hu_units[x].hu_card->m_number == outCard->m_number)
		{
			//// 硬三嘴，再听牌的时候已经判断了嘴数，胡牌的时候不在判断  by wyz
			// 硬三嘴：在听牌时不检测嘴数限制，在胡牌时才检测
			//if (YingSanZui == gameInfo.m_GameType) {
			//	int zui_score = toggles.calc_zui_score(tingstate.hu_units[x].fans);
			//	// 虽然胡牌命中，但是嘴数不够，这个胡就是无效的了
			//	if (zui_score < 3) break;
			//}

			Hu_type hu;
			hu.m_hu_type = tingstate.hu_units[x].hu_type;
			hu.m_hufan = outCard->m_number;
			vec.push_back(hu);
			break;
		}
	}

	//撵中子：如果明杠刚上开花不算自摸 （**Ren 2017-11-30）
	if (isGetCard && !vec.empty())
	{
		for (auto x = vec.begin(); x != vec.end(); ++x)
		{
			if (NianZhongZi == gameInfo.m_GameType && false == gameInfo.b_getCardFromMAGang)
			{
				x->m_hu_way.push_back(HUWAY_MoBao);
			}
			x->m_hu_way.push_back(HUWAY_ZiMo);
		}
	}
	return !vec.empty();
}

bool CardManagerSx::CheckCanGang(CardVector& handcard, CardVector &suoCard, Card* outCard, bool huType, bool pt_laizi, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{
	// 二人抠点不能杠耗子
	if (toggles.m_registered_game_type == ErRenKouDian) {
		if (VecHas(gameInfo.m_hCard, ToCard(outCard))) return false;
	}
	// 杠牌的话如果把立四的锁牌给杠空了，是不行的
	if (suoCard.size() && outCard) {
		size_t suo_size_after_gang = suoCard.size() - std::count_if(suoCard.begin(), suoCard.end(), CardPtrPredicate(*outCard));
		if (suo_size_after_gang == 0) return false;
	}
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

bool CardManagerSx::CheckCanPeng(CardVector& handcard, CardVector &suoCard, Card* outCard, bool huType, bool pt_laizi)
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

	size_t count_in_hand = tmpVec.size();
	if (count_in_hand < 2) return false;

	// 碰牌后，立四的锁牌为空了，也不行
	if (suoCard.size() && outCard) { // 安全监测
		size_t count_in_suo = std::count_if(suoCard.begin(), suoCard.end(), CardPtrPredicate(*outCard));
		if (count_in_hand >= count_in_suo) { // 安全监测
			size_t count_in_free = count_in_hand - count_in_suo;
			if (count_in_free < 2) {
				size_t used_in_suo = 2 - count_in_free;
				if (suoCard.size() == used_in_suo) return false;
			}
		}
	}

	return true;
}

bool CardManagerSx::CheckAnGang(CardVector& handcard, CardVector &suoCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles)
{
	if (handcard.size() < 5)
		return false;

	for (Lsize i = 0; i + 3 < handcard.size(); ++i)
	{
		Card* pcarda = handcard[i];
		Card* pcardb = handcard[i + 3];
		if (pcarda->m_color == pcardb->m_color && pcarda->m_number == pcardb->m_number)
		{
			// 二人抠点不能杠耗子
			if (toggles.m_registered_game_type == ErRenKouDian) {
				if (VecHas(gameInfo.m_hCard, ToCard(pcarda))) continue;
			}

			// 过杠的牌不让再杠
			if (VecHas(gameInfo.m_cards_guo_gang_MH, ToCard(pcarda)))
			{
				continue;
			}

			// 贴金：金牌不可杠，点杠、碰在别人出牌时已经处理了，这里只需要处理暗杠
			if (toggles.FEAT_HunPaiBuKeGang && VecHas(gameInfo.m_hCard, *pcarda)) continue;
			// 这4张牌把立四的锁牌杠空了，不行的
			if (suoCard.size()) {
				size_t suo_size_after_gang = suoCard.size() - std::count_if(suoCard.begin(), suoCard.end(), CardPtrPredicate(*pcarda));
				if (suo_size_after_gang == 0) continue;
			}
			vec.push_back(pcarda);
		}
	}
	// 为什么返回true？应该返回 !vec.empty() 吧？
	return true;
}

bool CardManagerSx::CheckMGang(CardVector& handcard, CardVector& pengCard, CardVector &suoCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, bool pt_laizi)
{
	//这里怎么会有5张牌这个判断，估计是从暗杠那里复制过来的
	//if (handcard.size() < 5) 
	if (handcard.size() < 2)
		return false;
	for (Lsize i = 0; i < pengCard.size(); i += 3)
	{
		Card* pcard = pengCard[i];
		// 二人抠点不能杠耗子
		if (toggles.m_registered_game_type == ErRenKouDian) {
			if (VecHas(gameInfo.m_hCard, ToCard(pcard))) continue;
		}

		// 扣点推到胡--新版扣点，推倒胡如果玩家过杠，碰后，再也不能杠这张牌
		if (toggles.FEAT_GAME_TYPE == KouDian || toggles.FEAT_GAME_TYPE == TuiDaoHu || GuaiSanJiao == toggles.FEAT_GAME_TYPE)
		{
			if (VecHas(gameInfo.m_cards_guo_gang_MH, ToCard(pcard)))
			{
				continue;
			}
		}
		// 从手牌中找，顶多能找到一张
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
					// 从锁牌中删除这张牌后，锁牌为空不行
					if (suoCard.size() == 1 && suoCard[0] && *suoCard[0] == *pcard) break;
					vec.push_back(pcard);
					break;
				}
			}
		}
	}
	return !vec.empty();
}


bool CardManagerSx::CheckMGang(Card * getCard, CardVector & pengCard, std::vector<Card*>& vec, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, bool pt_laizi)
{
	// 二人抠点不能杠耗子
	if (toggles.m_registered_game_type == ErRenKouDian) {
		if (VecHas(gameInfo.m_hCard, ToCard(getCard))) return false;
	}

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

bool CardManagerSx::IsContinue(CardVector& result)
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

bool CardManagerSx::IsSame(CardVector& result)
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
bool CardManagerSx::IsSame(Card* c1, Card* c2)
{
	return c1->m_color == c2->m_color&&c1->m_number == c2->m_number;
}

bool CardManagerSx::IsNineOne(Card* c)
{
	return c->m_number == 0 || c->m_number == 9;
}

bool CardManagerSx::GetSpecialOneTwoThreeFour(CardVector& src, CardVector& one, CardVector& two, CardVector& three, CardVector& four)
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

bool CardManagerSx::xiaohu_CheckHu(CardVector handcard, Card* curCard, bool hu_37jia)
{
	if (curCard)
	{
		handcard.push_back(curCard);
	}
	std::vector<Lint> card_vector(40, 0);

	//第一步获取癞子,分数量统计
	for (size_t x = 0; x<handcard.size(); x++)
	{
		if (handcard[x])
		{
			card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
		}
	}

	for (size_t x = 0; x<card_vector.size(); x++)
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

bool CardManagerSx::Hu_explorer_zheng_function(std::vector<Lint> card_vector, Lint index)
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

Lint CardManagerSx::judge_ke(std::vector<Lint>& targe_vector, Lint index)
{
	if (index >= (Lint)targe_vector.size())
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

Lint CardManagerSx::judge_shun(std::vector<Lint>& targe_vector, Lint index)
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

Lint CardManagerSx::CheckLaiziNum(CardVector& handcard, OperateState& gameInfo)
{
	Lint laiziCount = 0;
	/*if (gameInfo.LaiziCard == NULL)
	{
		return NULL;
	}
	for (auto iter = handcard.cbegin(); iter != handcard.cend(); iter++)
	{
		if ((*iter)->m_color == gameInfo.LaiziCard->m_color && (*iter)->m_number == gameInfo.LaiziCard->m_number)
		{
			laiziCount++;
		}
	}*/
	return laiziCount;
}

Lint CardManagerSx::get_card_index(Card * card)
{
	if (card)
		return  (card->m_color - 1) * 10 + card->m_number;
	else {
		return 0;
	}
}

static Card NULL_CARD;
Card * CardManagerSx::get_card(Lint index)
{
	for (size_t x = 0; x < m_3_laizi.size(); x++)
	{
		if ((m_3_laizi[x]->m_color == index / 10 + 1) && (m_3_laizi[x]->m_number == index % 10))
		{
			return m_3_laizi[x];
		}
	}
	return &NULL_CARD; // 为避免出现崩溃，这里不返回空指针，输入参数都应该是合理值，如果出现不合理值，返回个空Card也没问题
	return nullptr;
}

Card * CardManagerSx::get_card2(Lint index)
{
	for (size_t x = 0; x < m_3_laizi.size(); x++)
	{
		if ((m_3_laizi[x]->m_color == index / 10 + 1) && (m_3_laizi[x]->m_number == index % 10))
		{
			return m_3_laizi[x];
		}
	}
	return nullptr;
}
//bool CardManagerSx::have_play_type(Lint playtype,std::vector<Lint> playtypes)
//{
//	for(auto x=playtypes.begin();x!=playtypes.end();x++)
//	{
//		if(*x == playtype)
//			return true;
//	}
//	return false;
//}
bool CardManagerSx::CheckQingYiSe(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	//两门以上才能胡 含19
	CardVector tmp(handcard);
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), agangCard.begin(), agangCard.end());
	tmp.insert(tmp.end(), mgangCard.begin(), mgangCard.end());
	tmp.insert(tmp.end(), eatCard.begin(), eatCard.end());
	if(getCard != NULL)
	{
		tmp.push_back(getCard);
	}
	SortCard(tmp);

	bool two_color = false;
	Lint color = 0;
	if (!tmp.empty())
	{
		color = tmp[0]->m_color;
	}
	for (size_t x = 0; x < tmp.size(); x++)
	{
		if (!two_color&&color != tmp[x]->m_color&&tmp[x]->m_color<4)
		{
			LLOG_DEBUG("NO QINGYISE ");
			two_color = true;
		}
	}
	return two_color;
}


bool CardManagerSx::CheckQueMen(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	CardVector tmp(handcard);
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), agangCard.begin(), agangCard.end());
	tmp.insert(tmp.end(), mgangCard.begin(), mgangCard.end());
	tmp.insert(tmp.end(), eatCard.begin(), eatCard.end());
	if(getCard != NULL)
	{
		tmp.push_back(getCard);
	}
	SortCard(tmp);

	Lint color = 0;
	Lint colorCount = 1;
	for (size_t x = 0; x < tmp.size(); x++)
	{
		if (color != tmp[x]->m_color&&tmp[x]->m_color<4)
		{
			color = tmp[x]->m_color;
			colorCount++;
		}
	}
	return colorCount < 3;
}

bool CardManagerSx::CheckBaseNeed(CardVector & handcard, CardVector & pengCard, CardVector & agangCard, CardVector & mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo)
{
	return true;
}

bool CardManagerSx::CheckHasTriplet(CardVector& handcard)
{
	return true;
}

enum { DEBUG_CALC_TINGKOU = 0 };

void CardManagerSx::calc_ting_kou(PlayerState &ps, CardVector &handCard, tile_mask_t pg_mask, OperateState &gameInfo, ShanXiFeatureToggles const &toggles, std::vector<HuUnit>& tingInfo) {
	MHLOG("******CardManagerSx::calc_ting_kou 计算听口函数 pos ************");
	pg_tile_count_by_color_t sum_pg_tile_count_by_color = ps.sum_pg_tile_count_by_color();
	TileVec hands;
	unsigned num_huns = 0;
	tile_mask_t mask = 0;
	tile_mask_t all_can_hu_mask = 0;
	tile_mask_t all_hand_mask = 0;
	tile_mask_t all_mask_with_pg = 0;
	for (size_t i = 0; i < handCard.size(); ++i) {
		if (NULL == handCard[i]) continue; // sanity check
		if (VecHas(gameInfo.m_hCard, *handCard[i])) {
			++num_huns;
			all_hand_mask |= TILE_TO_MASK(ToTile(handCard[i]));
			continue;
		}
		tile_t tile = CardToTile(handCard[i]);
		hands.add(tile);
		//if ((TILE_TO_MASK(tile) & TILE_MASK_VALID) == 0) {
		//	std::cout << i << ": " << tile << std::endl;
		//}
		mask |= TILE_TO_MASK(tile);
		all_hand_mask |= mask;
	}
	all_mask_with_pg = all_hand_mask | pg_mask;
	tile_mask_t valid_mask = TILE_MASK_VALID;
	//if (mask & ~TILE_MASK_VALID) {
	//	std::cout << mask << std::endl;
	//}
	// 缩小范围，优化听口计算
	valid_mask = toggles.calc_hu_candidate_set(mask);
	if (valid_mask & ~TILE_MASK_VALID) {
		std::cout << "ERROR: calc_ting_kou_candidate_set:" << valid_mask << std::endl;
	}
	// 但是对于两张万能牌做将，得给它点数10的覆盖范围
	if (num_huns) valid_mask |= TILE_MASK_ZIPAI;
	// 得计算完整听口，这样别人点炮才能全部覆盖啊，这样如果自己有两张万能牌，那就得全部检测
	// 手里没有万能牌时，顶多再摸到一张万能牌，那张万能牌只能靠在手牌范围附近
	if (num_huns) valid_mask |= TILE_MASK_VALID;

	// 抠点自摸的胡牌范围大
	FANXING const conf = gameInfo.allowed_fans | FAN_ZiMo;

	if (DEBUG_CALC_TINGKOU >= 2) {
		std::cout << gameInfo.m_pos << ": calc_ting_kou: num_huns=" << num_huns << " conf=" << malgo_format_fans(conf) << std::endl;
	}
	
	for (tile_t last_tile = TILE_MIN; last_tile <= TILE_MAX; ++last_tile) {
		if (!(TILE_TO_MASK(last_tile) & valid_mask)) continue;
		if ((TILE_TO_MASK(last_tile) & TILE_MASK_VALID) == 0) {
			//for (tile_t t = TILE_MIN; t <= TILE_MAX; ++t) {
			//	if ((TILE_TO_MASK(t) & valid_mask) != 0) std::cout << (int)t;
			//}
			//std::cout << std::endl;
			continue;
		}

		// 洪洞王牌的缺门在这里单独判断,妃子牌不计入缺门计算, 破了缺门肯定不能胡
		//if ((HongTongWangPai == toggles.FEAT_GAME_TYPE))
		//{
		//	tile_t tile_tmp = VecHas(gameInfo.m_hCard, TileToCard(last_tile)) ? 0 : last_tile;
		//	if( !hongtong_judge_quemen(mask, pg_mask, tile_tmp, conf))
		//	    continue;
		//}
		

		//洪洞王牌的万能牌在循环外面单独进行胡牌判定，这里不再计算
		//和当地玩家沟通后，王牌也能当自己牌本身使用，因此注释掉该代码，继续加入胡口
		//if (toggles.FEAT_GAME_TYPE == HongTongWangPai)
		//{
		//	if (VecHas(gameInfo.m_hCard, TileToCard(last_tile)))
		//	{
		//		continue;
		//	}
		//}

		unsigned tmp_num_huns = num_huns;
		tile_t tmp_last_tile = last_tile;

	 	// 听口计算不应该考虑最后这张牌是万能牌
		if (0 && VecHas(gameInfo.m_hCard, TileToCard(last_tile))) {
			++tmp_num_huns;
			tmp_last_tile = 0;
		}
		else hands.add(last_tile);
		FANXING fans = toggles.call_judge(ps, tmp_num_huns, hands, tmp_last_tile, conf, pg_mask, sum_pg_tile_count_by_color);

		if (fans & FAN_DiaoZhang) {
			fans |= FAN_DiaoZhang;
		}

		// 调试输出用
		if (DEBUG_CALC_TINGKOU >= 2) {
			std::cout << "hands.size(): " << hands.size() << ":";
			for (size_t i = 0; i < hands.size(); ++i) {
				std::cout << ' ' << (int)hands[i];
			}
			std::cout << " " << malgo_format_fans(fans) << std::endl;
		}

		hands.pop_back();		

		// 如果用这张牌能胡，就把这张牌添加到听口
		if (fans) {
			if (DEBUG_CALC_TINGKOU >= 1) {
				std::ostringstream out;
				out << gameInfo.m_pos << ": ";
				for (size_t i = 0; i < hands.size(); ++i) std::cout << (int)hands[i] << ",";
				out << " +" << (int)tmp_last_tile << malgo_format_fans(fans) << std::hex << fans << std::dec << std::endl;
				std::cout << out.str();
			}

			//洪洞王牌减少妃子牌配出来的特殊番型
			if (toggles.FEAT_GAME_TYPE == HongTongWangPai)
			{				
				fans = hongtong_reduce_fans(fans, all_mask_with_pg | TILE_TO_MASK(tmp_last_tile), all_hand_mask | TILE_TO_MASK(tmp_last_tile), ToTile(gameInfo.m_hCard[0]));				
			}
			
			HuUnit new_unit;
			new_unit.hu_card = get_card(last_tile - 10); // 可能得到NULL
			if (fans & FAN_ShiSanYao) new_unit.hu_type = HU_ShiSanYao;
			else if (fans & FAN_HaoHuaQiXiaoDui) new_unit.hu_type = HU_DaQixiaodui;
			else if (fans & FAN_QiXiaoDui) new_unit.hu_type = HU_Qixiaodui;
			else if (fans & FAN_SanFeng_X1) new_unit.hu_type = HU_SanFeng_X1;
			else if (fans & FAN_SanFeng_X2) new_unit.hu_type = HU_SanFeng_X2;
			else if (fans & FAN_SanFeng_X3) new_unit.hu_type = HU_SanFeng_X3;
			else if (fans & FAN_SanFeng_X4) new_unit.hu_type = HU_SanFeng_X4;
			else if (fans & FAN_HongTong_YingKou) new_unit.hu_type = HU_HongTong_YingKou;
			else if (fans & FAN_HongTong_LouShangLou) new_unit.hu_type = HU_HongTong_LouShangLou;

			// 抠点最后摸到万能牌时需要排序求最优解，所以这里记录每张胡牌的分值
			new_unit.fans = fans;
			new_unit.score = toggles.calc_common_loss(fans, tmp_last_tile);
			//new_unit.zui_score = judgement.zui_score;
			tingInfo.push_back(new_unit);
			all_can_hu_mask |= TILE_TO_MASK(tmp_last_tile);
		}

		// 洪洞王牌，需要增加检测 风牌为王牌（方言妃子）的情况下，不当混的情况下能不能胡牌	
		if (HongTongWangPai == gameInfo.m_GameType)
		{
			tile_t hun_value = ToTile(gameInfo.m_hCard[0]);
			if (hun_value > 40 && hun_value < 45)
			{
				tmp_num_huns = num_huns;
				tmp_last_tile = last_tile;
				TileVec temp_hands = hands;
			
				while (tmp_num_huns > 0)
				{
					tmp_last_tile = last_tile;					 
					--tmp_num_huns;
					temp_hands.add(hun_value);
					temp_hands.add(tmp_last_tile);
					FANXING fans = toggles.call_judge(ps, tmp_num_huns, temp_hands, tmp_last_tile, conf, pg_mask, sum_pg_tile_count_by_color);
					temp_hands.pop_back();					 
					if (fans)
					{
						fans = hongtong_reduce_fans(fans, all_mask_with_pg | TILE_TO_MASK(tmp_last_tile), all_hand_mask | TILE_TO_MASK(tmp_last_tile), ToTile(gameInfo.m_hCard[0]));
						HuUnit new_unit;
						new_unit.hu_card = get_card(last_tile - 10); // 可能得到NULL
						if (fans & FAN_ShiSanYao) new_unit.hu_type = HU_ShiSanYao;
						else if (fans & FAN_HaoHuaQiXiaoDui) new_unit.hu_type = HU_DaQixiaodui;
						else if (fans & FAN_QiXiaoDui) new_unit.hu_type = HU_Qixiaodui;
						else if (fans & FAN_SanFeng_X1) new_unit.hu_type = HU_SanFeng_X1;
						else if (fans & FAN_SanFeng_X2) new_unit.hu_type = HU_SanFeng_X2;
						else if (fans & FAN_SanFeng_X3) new_unit.hu_type = HU_SanFeng_X3;
						else if (fans & FAN_SanFeng_X4) new_unit.hu_type = HU_SanFeng_X4;
						else if (fans & FAN_HongTong_YingKou) new_unit.hu_type = HU_HongTong_YingKou;
						else if (fans & FAN_HongTong_LouShangLou) new_unit.hu_type = HU_HongTong_LouShangLou;

						// 抠点最后摸到万能牌时需要排序求最优解，所以这里记录每张胡牌的分值
						new_unit.fans = fans;
						new_unit.score = toggles.calc_common_loss(fans, tmp_last_tile);
						//new_unit.zui_score = judgement.zui_score;
						// 如果这张牌在风为万能牌时能胡，某些情况下风为万能牌时胡分大于风为万能牌时的胡分，替换掉
						bool bInHuList = false;
						HuUnit * pHuUnit = NULL;
						for (int i = 0; i < tingInfo.size(); i++)
						{
							if (ToTile(tingInfo[i].hu_card) == tmp_last_tile)
							{
								bInHuList = true;
								pHuUnit = &tingInfo[i];								 
								break;
							}
						}
						if(!bInHuList)
						    tingInfo.push_back(new_unit);
						else if( new_unit.score > pHuUnit->score )
						{
							pHuUnit->fans = new_unit.fans;
							pHuUnit->m_hu_basicType = new_unit.m_hu_basicType;
							pHuUnit->hu_type = new_unit.hu_type;
							pHuUnit->score = new_unit.score;
						}
						all_can_hu_mask |= TILE_TO_MASK(tmp_last_tile);
					}
				}//while (tmp_num_huns > 0)
			}
		}//if (HongTongWangPai == gameInfo.m_GameType)
	}

	// 忻州扣点：6点以上听牌可以自摸可以接炮，3、4、5听牌只可自摸（无耗子） （**Ren 2017-11-27）
	if (XinZhouKouDian == gameInfo.m_GameType)
	{
		bool has_points_ge6 = false;
		//如果听口有 > 6 的，则可以听
		for (size_t i = 0; i < tingInfo.size(); ++i) {
			if (tile_to_points(ToTile(tingInfo[i].hu_card)) >= 6) {
				has_points_ge6 = true;
				break;
			}
		}
		// 听口中如果没有6点以上的，3、4、5可听，但是只能自摸
		if (!has_points_ge6) {
			size_t i = tingInfo.size();
			while (i--) {
				if (tile_to_points(ToTile(tingInfo[i].hu_card)) >= 3) {
					has_points_ge6 = true; // 这样就不用清空听口了
				}
				else {
					// 不胡3、4、5的胡牌是不行的，得删了
					VecErase(tingInfo, i);
				}
			}
		}
		if (!has_points_ge6) tingInfo.clear();
	}
	// 对于抠点，听口中必须有6点以上的才能听，所以发现没有这样的，就清空听口
	if (KouDian == gameInfo.m_GameType) {
		bool has_points_ge6 = false;
		for (size_t i = 0; i < tingInfo.size(); ++i) {
			if (tile_to_points(ToTile(tingInfo[i].hu_card)) >= 6) {
				has_points_ge6 = true;
				break;
			}
		}
		// 听口中如果没有6点以上的，就看看有没有3点以上的一条龙，这种胡法要保留，让听
		if (!has_points_ge6 && toggles.m_playtype[PT_YiTiaoLongKeHuSanSiWu]) {
			size_t i = tingInfo.size();
			while (i--) {
				if ((tingInfo[i].fans & FAN_YiTiaoLong) && tile_to_points(ToTile(tingInfo[i].hu_card)) >= 3) {
					has_points_ge6 = true; // 这样就不用清空听口了
				} else {
					// 不胡一条龙345的胡牌是不行的，得删了
					VecErase(tingInfo, i);
				}
			}
		}

		if (!has_points_ge6) tingInfo.clear();
	}

	//撵中子：门清、缺门、断幺、边张、坎张、吊张、凑一色、清一色、一条龙、青龙、十三幺番型才能听牌（**Ren 2017-11-28）
	//只要不是允许的番型就删除听口
	if (NianZhongZi == gameInfo.m_GameType)
	{
		FANXING tmp_duyiting = FAN_BianZhang | FAN_KanZhang | FAN_DiaoZhang | FAN_PingHu | FAN_ZiMo;
		size_t i = tingInfo.size();
		size_t tmp_tingInfo_size = i;
		while (i--)
		{
			if ((tingInfo[i].fans & FAN_BianKanDiao) && tmp_tingInfo_size > 1)
			{
				tingInfo[i].fans &= ~FAN_BianKanDiao;
				tingInfo[i].score -= 2;
			}
			////判断只含有边卡吊的如果听口数>1 则删除此听口
			//if (!((tingInfo[i].fans & gameInfo.allowed_fans) & ~tmp_duyiting) && tmp_tingInfo_size > 1)
			//{
			//	VecErase(tingInfo, i);
			//}
			//如果只有平胡和自摸的番，没有其他中子，则删除听口
			if (!(tingInfo[i].fans & (gameInfo.allowed_fans & ~FAN_PingHu & ~FAN_ZiMo)))
			{
				VecErase(tingInfo, i);
			}
			////如果有边卡吊的番与其他的番，且边卡掉听口大于>1就删除边卡吊的番
			//if ((tingInfo[i].fans & tmp_duyiting) != 0 && (tingInfo[i].fans & ~tmp_duyiting) != 0 && tmp_tingInfo_size > 1)
			//{
			//	tingInfo[i].fans &= ~FAN_BianKanDiao;
			//}
			//创建房间时候选择是否两个中子以上才能听牌（**Ren 2018-01-22）
			if (toggles.m_playtype[MH_PT_NianZhongZiErZhongZi] && tingInfo[i].score < 4)
			{
				VecErase(tingInfo, i);
			}
		}
	}

	// 边卡吊就是听口大小必须为1
	if (BianKaDiao == gameInfo.m_GameType) {
		if (tingInfo.size() != 1) {
			tingInfo.clear();
		}
	}

	if (JinZhong == gameInfo.m_GameType)
	{
		//晋中的边，卡，吊只能是胡一张牌的情况下才有效
		if (tingInfo.size() > 1)
		{
			for (int i = 0; i < tingInfo.size(); i++)
			{
				tingInfo[i].fans &= ~FAN_BianKanDiao;
			}
		}	
	}

	if (YingSanZui == gameInfo.m_GameType  || LinFenYiMenPai == gameInfo.m_GameType) {
		// 一张赢需要在听口计算时判定
		if (tingInfo.size() == 1) {
			if (tingInfo[0].fans & FAN_DiaoZhang) {

			}
			tingInfo[0].fans |= FAN_YiZhangYing;
			tingInfo[0].fans = toggles.reduce_fans_in_TingKou_stage(tingInfo[0].fans, conf);
		}
		// 硬三嘴，必须3嘴以上才能胡牌
		size_t n = tingInfo.size();
		// 必须3嘴以上才能胡牌
		while (n--) { 
			int zui_score = 0;
			if ((tingInfo[n].fans & FAN_QiXiaoDui) || (tingInfo[n].fans & FAN_HaoHuaQiXiaoDui) || (tingInfo[n].fans & FAN_QingQiDui) || (tingInfo[n].fans & FAN_QingHaoHuaQiXiaoDui))
				zui_score = toggles.calc_zui_score_spec_qixiaodui(hands, pg_mask);
			else
				zui_score = toggles.calc_zui_score(tingInfo[n].fans);
			if (DEBUG_CALC_TINGKOU) {
				std::cout << gameInfo.m_pos << ": zui_score=" << zui_score << ": " << malgo_format_fans(tingInfo[n].fans) << std::endl;
			}
			// 现在改成听牌时不检测嘴数限制，只在胡牌时检测
			// 又改回来了
			//continue;
			if (zui_score < 3) VecErase(tingInfo, n);
		}
	}

	// 洪洞王牌加入万能牌到平胡的胡口中
	if (HongTongWangPai == gameInfo.m_GameType)
	{
		//if (tingInfo.size() > 0)
		{
			// 测试万能牌能不能胡牌?
			for (int j = 0; j < gameInfo.m_hCard.size(); j++)
			{
				MHLOG("********测试万能牌能不能胡????????????");
				unsigned tmp_num_huns = num_huns;
				tile_t tmp_last_tile = 0;
				++tmp_num_huns;
				tmp_last_tile = 0;
				FANXING fans = toggles.call_judge(ps, tmp_num_huns, hands, tmp_last_tile, conf, pg_mask, sum_pg_tile_count_by_color);
				if (fans)
				{
					//洪洞王牌减少妃子牌配出来的特殊番型
					if (toggles.FEAT_GAME_TYPE == HongTongWangPai)
					{
						fans = hongtong_reduce_fans(fans, all_mask_with_pg | TILE_TO_MASK(ToTile(gameInfo.m_hCard[j])), all_hand_mask | TILE_TO_MASK(ToTile(gameInfo.m_hCard[j])), ToTile(gameInfo.m_hCard[0]));
					}

					HuUnit new_unit;
					new_unit.hu_card = get_card(get_card_index(&gameInfo.m_hCard[j])); // 可能得到NULL
					if (fans & FAN_ShiSanYao) new_unit.hu_type = HU_ShiSanYao;
					else if (fans & FAN_HaoHuaQiXiaoDui) new_unit.hu_type = HU_DaQixiaodui;
					else if (fans & FAN_QiXiaoDui) new_unit.hu_type = HU_Qixiaodui;
					else if (fans & FAN_SanFeng_X1) new_unit.hu_type = HU_SanFeng_X1;
					else if (fans & FAN_SanFeng_X2) new_unit.hu_type = HU_SanFeng_X2;
					else if (fans & FAN_SanFeng_X3) new_unit.hu_type = HU_SanFeng_X3;
					else if (fans & FAN_SanFeng_X4) new_unit.hu_type = HU_SanFeng_X4;
					else if (fans & FAN_HongTong_YingKou) new_unit.hu_type = HU_HongTong_YingKou;
					else if (fans & FAN_HongTong_LouShangLou) new_unit.hu_type = HU_HongTong_LouShangLou;

					// 抠点最后摸到万能牌时需要排序求最优解，所以这里记录每张胡牌的分值
					new_unit.fans = fans;
					new_unit.score = toggles.calc_common_loss(fans, tmp_last_tile);
					//new_unit.zui_score = judgement.zui_score;

					//万能牌是否已经在胡口中了
					bool bExitHaozi = false;
					HuUnit * pHuUnit = NULL;
					for (int i = 0; i < tingInfo.size(); i++)
					{
						if ((*tingInfo[i].hu_card) == gameInfo.m_hCard[j])
						{
							bExitHaozi = true;
							pHuUnit = &tingInfo[i];
							break;
						}
					}
					if(! bExitHaozi)
					    tingInfo.push_back(new_unit);
					else if (new_unit.score > pHuUnit->score)
					{
						pHuUnit->fans = new_unit.fans;
						pHuUnit->m_hu_basicType = new_unit.m_hu_basicType;
						pHuUnit->hu_type = new_unit.hu_type;
						pHuUnit->score = new_unit.score;
					}
					MHLOG("*****加上万能牌的胡番fans = %s", malgo_format_fans(fans).c_str());
				}
			}
		}

		
	    //吊王就是胡所有的牌-- 判断方法不对，不能胡缺门的牌, 删除掉，改成上面用万能牌获取番型
		//吊王时摸上任意一张牌都能能胡，只要不破缺门
		/*
		if (HongTongWangPai == toggles.FEAT_GAME_TYPE)
		{
			bool b_diaowang_flag = false;
			if (conf & FAN_MUST_QueMen)
			{
				if (tingInfo.size() >= (18 + 7)) 
				{
					b_diaowang_flag = true;					
				}
			}
			if (conf &FAN_HongTong_MustQueLiangMen)
			{
				if (tingInfo.size() >= (9 + 7))
				{
					b_diaowang_flag = true;
				}
			}
			if (b_diaowang_flag)
			{
				for (int k = 0; k < tingInfo.size(); k++)
				{
					tingInfo[k].fans |= FAN_HongTong_DiaoWang;
					//tingInfo[k].hu_type = HU_HongTong_DiaoWang;
				}
			}
		}
		*/
	
	 
	}

}

enum { TING_USE_MALGO = 1 };
bool CardManagerSx::CheckCanTing(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<HuUnit>& tingInfo)
{
	//MHLOG("**********************CardManagerSx::CheckCanTing 听口判断函数**********************************");
	/*if (!gameInfo.b_CanTing) {
		return false;
	}*/

	////临汾一门牌 听牌需要混一色   by wyz
	if (LinFenYiMenPai == gameInfo.m_GameType && !CheckCanTing_yimenpai_spec(handcard, pengCard, agangCard, mgangCard, eatCard)) {
		MHLOG("logwyz....CheckCanTing_yimenpai_spec 不是混一色，不能听");
		return false;
	}

	// 现在有13张牌，差一张胡，这个函数要计算出听口
	// 抠点的话，自摸和点炮的胡牌范围是不一样的，自摸的胡牌范围大
	// 计算听口的算法，应该是遍历所有牌种，凑够14张，看看能不能胡
	// 13张牌加1张万能牌只能计算出能不能听，计算不出听口
	if (TING_USE_MALGO) {
		calc_ting_kou(ps, handcard, calc_pg_mask(pengCard, agangCard, mgangCard), gameInfo, toggles, tingInfo);
		return !tingInfo.empty();
	}

	MHLOG("**********************CardManagerSx::CheckCanTing 听口判断函数--逻辑不该走到这里**********************************");
	//七小队
	HuUnit new_unit;

	std::vector<Lint> card_vector(40, 0);
	for (size_t x = 0; x<handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}

	if (!gameInfo.b_isHun)
	{
		CardVector tmp(handcard);
		CardVector one, two, three, four;
		SortCard(tmp);
		GetSpecialOneTwoThreeFour(tmp, one, two, three, four);

		// 现在是CheckCanTing，手中的牌有13张，还差1张
		// 如果要成七小对的话，3张一组的牌最多只能有一组
		Lint twoNum = two.size() + four.size() + (three.size() == 3 ? 2 : 0);
		bool sanity_valid = one.size() || three.size(); // 安全检测，避免未定义情况下它们俩全是空的
		// 只有手中的牌有12张，并且全是对子就足够判断是七小对了
		// 有暗杠、明杠，就不能成立七小对
		// 有暗杠、明杠的话，handCard中就没有那些牌了，所以这里不用管agangCard和mgangCard
		if (twoNum == 12 && sanity_valid) {
			if (three.size()) {
				new_unit.hu_card = three[0];
				new_unit.hu_type = HU_DaQixiaodui;
			} else {
				new_unit.hu_card = one[0];
				new_unit.hu_type = four.size() ? HU_DaQixiaodui : HU_Qixiaodui;
			}
			if(KouDian == gameInfo.m_GameType)
			{
				if(new_unit.hu_card->m_number >= 6 || new_unit.hu_card->m_color > 3)
				{
					tingInfo.push_back(new_unit);
				}
			}
			if(TuiDaoHu == gameInfo.m_GameType)
			{
				tingInfo.push_back(new_unit);
			}
		}

	}
	//十三幺
	if (!gameInfo.b_isHun && handcard.size() == 13)
	{
		// churunmin: 十三幺判断逻辑看明白了，一块是判断十三张全是单张的情况，一块是判断有对子缺单张的情况
		Lint value[13] = {31,32,33,34,35,36,37,1,9,11,19,21,29};
		CardVector tmp(handcard);
		SortCard(tmp);
		std::vector<Lint> card_vector(40, 0);
		for (size_t x = 0; x<tmp.size(); x++)
		{
			card_vector[(tmp[x]->m_color - 1) * 10 + (tmp[x]->m_number) * 1]++;
		}
		size_t indexCount = sizeof(value) / sizeof(Lint);
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
	if (true)
	{
		CardVector tingcards;
		if (xiaohu_CheckTing(handcard, gameInfo, tingcards, !eatCard.empty(), pengCard.size() || agangCard.size() || mgangCard.size()))
		{
			EarseSameCard(tingcards);
			Lint conditionNum = std::count_if(tingcards.begin(), tingcards.end(), SetRange);
			for (size_t x = 0; x < tingcards.size(); x++)
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
				if(LiSi == gameInfo.m_GameType)
				{
					new_unit.hu_card = tingcards[x];
					tingInfo.push_back(new_unit);
				}
				if(GuaiSanJiao == gameInfo.m_GameType)
				{
					new_unit.hu_card = tingcards[x];
					tingInfo.push_back(new_unit);
				}
			}
		}
	}
	return !tingInfo.empty();
}

bool CardManagerSx::HasKezi(CardVector handcard, CardVector& pengCard, OperateState& gameInfo)
{
	bool hasKezi = false;
	if (pengCard.size()>0)
	{
		return true;
	}
	std::vector<Lint> card_vector(40, 0);
	for (size_t x = 0; x<handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}
	for (size_t x = 0; x < card_vector.size(); x++)
	{
		if (card_vector[x] > 2)
		{
			hasKezi = true;
			break;
		}
	}
	return hasKezi;
}

bool CardManagerSx::xiaohu_CheckTing(CardVector handcard, OperateState& gameInfo, CardVector& tingCard, bool have_shun, bool have_kezi)
{
	std::vector<Lint> card_vector(40, 0);
	for (size_t x = 0; x<handcard.size(); x++)
	{
		if (handcard[x])
		{
			card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
		}
	}
	for (size_t x = 0; x<card_vector.size(); x++)
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
			Ting_explorer_zheng_function(card_vector, 0, 1, tingCard, NULL, gameInfo.b_37jia);
			card_vector[x] += 2;
		}
		if (card_vector[x]>0)
		{
			card_vector[x] -= 1;
			Ting_explorer_zheng_function(card_vector, 0, 0, tingCard, get_card(x), gameInfo.b_37jia);
			card_vector[x] += 1;
		}
	}
	return !tingCard.empty();
}
void CardManagerSx::Ting_explorer_zheng_function(std::vector<Lint> card_vector, Lint index, Lint Lai_num, CardVector& tingcards, Card* tingcard, bool hu_37jia, bool have_shun, bool have_kezi)
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

Lint CardManagerSx::ting_judge_shun(std::vector<Lint>& targe_vector, Lint index)
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

bool CardManagerSx::explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num)
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

static FANXING HongTong_Calc_QiXiaoDui_AddtionalFans(FANXING conf, tile_mask_t all_hand_mask, Card * pOutCard, tile_t tile_haozi)
{
	FANXING fans = FAN_NONE;
	tile_mask_t haozi_mask = TILE_TO_MASK(tile_haozi);
	tile_mask_t mask = all_hand_mask;
	if (NULL == pOutCard)
	{
		mask = all_hand_mask;
	}
	else
	{
		mask = all_hand_mask & ~TILE_TO_MASK(ToTile(pOutCard));
	}

	// 计算缺门时候去掉耗子颜色
	tile_mask_t all_mask_without_haozi = mask & ~haozi_mask;
 
	int que_men_count = 0;	
	if (0 == (all_mask_without_haozi & TILE_MASK_WAN)) ++que_men_count;
	if (0 == (all_mask_without_haozi & TILE_MASK_TONG)) ++que_men_count;
	if (0 == (all_mask_without_haozi & TILE_MASK_TIAO)) ++que_men_count;
	//MHLOG("**********quemen_count = %d, conf = %s", que_men_count, malgo_format_fans(conf).c_str());
	if ( (conf & FAN_MUST_QueMen) && (que_men_count == 0))
	{
		return FAN_NONE;
	}
	if ( (conf & FAN_HongTong_MustQueLiangMen) && (que_men_count < 2))
	{
		return FAN_NONE;
	}
	fans = FAN_QiXiaoDui;
	if (hongtong_judge_qingyise(mask, tile_haozi)) 
	{
		fans |= FAN_QingYiSe;
	}
	else if (hongtong_judge_couyise(mask, tile_haozi))
	{
		fans |= FAN_CouYiSe;
	}
	if (hongtong_judge_ziyise(mask, tile_haozi))
	{
		fans |= FAN_ZiYiSe;
	}
	return fans & conf;
}

// 合并2个听口信息， tingInfoAdd的信息添加到tingInfoTarget
// 合并规则，按照不同听口胡牌的
// 胡牌按照最后分数大小进行比较
void CardManagerSx::Merge_TingInfo(std::vector<TingUnit> & tingInfoTarget, std::vector<TingUnit> & tingInfoAdd, ShanXiFeatureToggles const &toggles)
{
	std::vector<TingUnit>::iterator it_add = tingInfoAdd.begin();
	for (; it_add < tingInfoAdd.end(); it_add++)
	{
		bool is_hu_unit_merged = false;
		std::vector<TingUnit>::iterator it_target = tingInfoTarget.begin();		 
		for (; it_target < tingInfoTarget.end(); it_target++)
		{			 
			if ( * it_add->out_card == * it_target->out_card) // 听牌一致，胡牌可能不同，胡牌相同，胡牌的分值可能不同
			{				 
				std::vector<HuUnit>::iterator it2_hu = it_add->hu_units.begin();				 
				for (; it2_hu < it_add->hu_units.end(); it2_hu++)  // 遍历每张胡牌
				{
					bool is_hu_card_same = false;
					std::vector<HuUnit>::iterator it1_hu = it_target->hu_units.begin();
					for (; it1_hu < it_target->hu_units.end(); it1_hu++)
					{
						if (* it2_hu->hu_card == * it1_hu->hu_card) // 胡的牌相同，保留胡分高的
						{
							is_hu_card_same = true;
							break;
						}
					}
					if (is_hu_card_same)
					{
						int s1 = toggles.calc_common_loss(it1_hu->fans, ToTile(it1_hu->hu_card));
						int s2 = toggles.calc_common_loss(it2_hu->fans, ToTile(it2_hu->hu_card));						 
						if (s2 > s1)
						{
							*(it1_hu) = *(it2_hu);
						}
						
					}
					else // 添加新的胡牌
					{
						it_target->hu_units.push_back(*it2_hu);						 
					}				
					
				}
				is_hu_unit_merged = true;
				break;				
			}
		}
		if (is_hu_unit_merged)
		{

		}
		else                   // 添加新的听单元
		{
			tingInfoTarget.push_back(*it_add);		 
		}
	}
}

bool CardManagerSx::HongTong_CheckQiXiaoDui_Ting(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<TingUnit>& tingInfo)
{
	if ( (gameInfo.m_GameType != HongTongWangPai)|| (!(gameInfo.allowed_fans & FAN_QiXiaoDui)))
	{
		//MHLOG(" ************洪洞王牌七小队退出检测 gameInfo.allowed_fans & FAN_QiXiaoDui = %d", (bool)(gameInfo.allowed_fans & FAN_QiXiaoDui));
		return false;
	}
	//MHLOG(" ************洪洞王牌七小队检测函数 handcard.size() = %d", handcard.size());
	FANXING conf = gameInfo.allowed_fans;
	CardVector tmp(handcard);
	if (getCard)
		tmp.push_back(getCard);
	SortCard(tmp);
	TileVec hands;

 
	tile_mask_t mask = 0;
	tile_mask_t all_can_hu_mask = 0;
	for (size_t i = 0; i < tmp.size(); ++i) {
		if (NULL == tmp[i]) continue; // sanity check		 
		tile_t tile = CardToTile(tmp[i]);
		hands.add(tile);		
		mask |= TILE_TO_MASK(tile);
	}
	if (tmp.size() != 14)
	{
		return false;
	}
	
	TileZoneDivider tmp_stats(hands.begin(), hands.size());
	/*
	if (conf & FAN_MUST_QueMen) {
		if (!PlayerState::is_quemen(tmp_stats.mask)) return false;
		// 然后2张万能牌当将时不破缺门就行了，它肯定可以不破缺门啊，这个函数只是判断能不能胡
	}
	if (tmp_stats.total != 14)
	{
		return false;
	}

	if (conf & FAN_HongTong_MustQueLiangMen)
	{
		int que_men_count = 0;
		if (0 == (tmp_stats.mask & TILE_MASK_WAN)) ++que_men_count;
		if (0 == (tmp_stats.mask & TILE_MASK_TONG)) ++que_men_count;
		if (0 == (tmp_stats.mask & TILE_MASK_TIAO)) ++que_men_count;
		if (que_men_count < 2)
		{
			return false;
		}
	}	
	*/
	unsigned nSingleTileCount = 0;
	CardVector vec_single_cards;
	std::vector<Lint> vec_single_cards_count;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {	 
		unsigned const *a = tmp_stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] % 2) {
				nSingleTileCount++;
				vec_single_cards.push_back(get_card( (Lint)(tc - TILE_COLOR_MIN) * 10 + (i + 1) ));
				vec_single_cards_count.push_back(a[i]);
				//MHLOG(" 七小队 找到单牌 = %d", ToTile(get_card((Lint)(tc - TILE_COLOR_MIN) * 10 + (i +1) )));
				if (nSingleTileCount > 2)
					return false;
			}
		}
	}
 

	 
	int size_single_card = vec_single_cards.size();
	EarseSameCard(tmp);
	// tmp是手牌unique之后的状态
	if (size_single_card  < 1)
	{
		for (int i = 0; i < tmp.size(); i++)
		{
			TingUnit xx;
			HuUnit huUnit;
			huUnit.clear();
			huUnit.hu_type = HU_Qixiaodui;
			huUnit.fans = HongTong_Calc_QiXiaoDui_AddtionalFans(conf, mask, NULL, ToTile(gameInfo.m_hCard[0]));
			if (huUnit.fans)
			{
				huUnit.hu_card = tmp[i];
				xx.out_card = tmp[i];
				xx.hu_units.push_back(huUnit);
				tingInfo.push_back(xx);
			}
		}
	}
	else if (size_single_card == 2)
	{
		{
			TingUnit xx;
			HuUnit huUnit;
			huUnit.clear();
			huUnit.hu_type = HU_Qixiaodui;
			if (vec_single_cards_count[1] == 1)
			{
				huUnit.fans = HongTong_Calc_QiXiaoDui_AddtionalFans(conf, mask, vec_single_cards[1], ToTile(gameInfo.m_hCard[0]));
			}
			else
			{
				huUnit.fans = HongTong_Calc_QiXiaoDui_AddtionalFans(conf, mask, NULL, ToTile(gameInfo.m_hCard[0]));
			}
			if (huUnit.fans)
			{
				huUnit.hu_card = vec_single_cards[0];
				xx.out_card = vec_single_cards[1];
				xx.hu_units.push_back(huUnit);
				tingInfo.push_back(xx);
			}
		}
		{
			TingUnit xx;
			HuUnit huUnit;
			huUnit.clear();
			huUnit.hu_type = HU_Qixiaodui;
			if (vec_single_cards_count[0] == 1)
			{
				huUnit.fans = HongTong_Calc_QiXiaoDui_AddtionalFans(conf, mask, vec_single_cards[0], ToTile(gameInfo.m_hCard[0]));
			}
			else
			{
				huUnit.fans = HongTong_Calc_QiXiaoDui_AddtionalFans(conf, mask, NULL, ToTile(gameInfo.m_hCard[0]));
			}
			if (huUnit.fans)
			{
				huUnit.hu_card = vec_single_cards[1];
				xx.out_card = vec_single_cards[0];
				xx.hu_units.push_back(huUnit);
				tingInfo.push_back(xx);
			}
		}

	}
	MHLOG(" 七小队 tingInfo.size() = %d", tingInfo.size());
	
	return true;
}
//摸到牌后判断听 (多一张牌）
bool CardManagerSx::all_CheckTing(PlayerState &ps, CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo, ShanXiFeatureToggles const &toggles, std::vector<TingUnit>& tingInfo)
{
	CardVector tmp(handcard);
	if (getCard)
		tmp.push_back(getCard);
	SortCard(tmp);

	std::vector<TingUnit> hong_tong_qixiaodui_tingInfo;
	//洪洞王牌会修改番型，计算听口的时候不计算7小对
	FANXING allow_fans_backup = gameInfo.allowed_fans;
	if (!gameInfo.b_isHun || TING_USE_MALGO)
	{		
		if (HongTongWangPai == toggles.FEAT_GAME_TYPE)
		{
			HongTong_CheckQiXiaoDui_Ting(ps, handcard, pengCard, agangCard, mgangCard, eatCard, getCard, gameInfo, toggles, hong_tong_qixiaodui_tingInfo);			
			gameInfo.allowed_fans &= ~(FAN_QiXiaoDui);
			 
		}
		//去出重复牌（**Ren 2017-12-11）
		EarseSameCard(tmp);
		// tmp是手牌unique之后的状态
		for (size_t x = 0; x < tmp.size(); ++x)
		{
			if (LiSi == gameInfo.m_GameType && gameInfo.m_suoCard && !VecHas(gameInfo.m_suoCard[gameInfo.m_pos], tmp[x]))
			{
				continue;
			}

			//洪洞王牌修改，任何一张牌都能听口出掉包括王牌，其他情况按原来逻辑处理
			if (HongTongWangPai == toggles.FEAT_GAME_TYPE)
			{
			}
			else
			{
				// 万能牌不能放到听出口中：点“听”按钮后出的那张牌不能是万能牌
				if (VecHas(gameInfo.m_hCard, ToCard(tmp[x]))) continue;
			}

			//洪洞王牌，添加大王报听玩法：听牌出的牌只能是中发白（**Ren 2017-12-11）
			/*if (HongTongWangPai == toggles.FEAT_GAME_TYPE && toggles.m_playtype[MH_PT_HongTongWangPai_ShuangWang])
			{
				if (!(TILE_TO_MASK(ToTile(tmp[x])) & TILE_MASK_Jian))
				{
					continue;
				}
			}*/

			CardVector tmp2(handcard);
			if (getCard)
				tmp2.push_back(getCard);
			SortCard(tmp2);
			EraseCard(tmp2, tmp[x], 1);  //打出去一张手牌
			TingUnit xx;
			MHLOG("*******尝试打出手牌card = %d tmp[x]=%X", ToTile(tmp[x]), tmp[x]);
			if (CheckCanTing(ps, tmp2, pengCard, agangCard, mgangCard, eatCard, gameInfo, toggles, xx.hu_units))
			{
				xx.out_card = tmp[x];
				tingInfo.push_back(xx);
			}
		}

		if (HongTongWangPai == toggles.FEAT_GAME_TYPE)
		{	
			//回复原来的番型
			gameInfo.allowed_fans = allow_fans_backup;

			// 合并7小对的听口
			Merge_TingInfo(tingInfo, hong_tong_qixiaodui_tingInfo, toggles);
			
			MHLOG("*********打印听口信息tingInfo.size() = %d", tingInfo.size());
			 
			for (int i = 0; i < tingInfo.size(); i++)
			{
				MHLOG("********* index = %d, out_card = %d ", i, ToTile(tingInfo[i].out_card));
				MHLOG("********* 胡口信息 ", i, ToTile(tingInfo[i].out_card));
				for (int j = 0; j < tingInfo[i].hu_units.size(); j++)
				{

					MHLOG("********* index = %d, hu_card = %d , fans=%s", j, ToTile(tingInfo[i].hu_units[j].hu_card), malgo_format_fans(tingInfo[i].hu_units[j].fans).c_str());
				}
			}
			
		}
	} else {
		Calc(tmp, pengCard, agangCard, mgangCard, eatCard, gameInfo, tingInfo);
	}
	return !tingInfo.empty();
}

bool CardManagerSx::CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo)
{
	special = 0;
	CardVector all = handcard;
	size_t nLaiZiCount = 0;
	if (isGetCard)
	{
		if (outCard)
		{
			all.push_back(outCard);
		}
		for (size_t i = 0; i < all.size(); )
		{
			if (false&&all[i]->m_color == 4 && all[i]->m_number == 5)
			{
				gCardMgrSx.EraseCard(all, all[i]);
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
		for (size_t i = 0; i < all.size(); )
		{
			if (false&&all[i]->m_color == 4 && all[i]->m_number == 5)
			{
				gCardMgrSx.EraseCard(all, all[i]);
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
	gCardMgrSx.SortCard(all);
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

bool CardManagerSx::CheckShiSanYiao(CardVector & handcard, Card * outCard)
{
	CardVector tmp = handcard;
	if (outCard) tmp.push_back(outCard);
	SortCard(tmp);
	EarseSameCard(tmp);

	// 擦掉相同的牌后，应该正好十三张才是十三幺
	if (tmp.size() != 13) return false;
	static char sorted_shisanyao[] = {11, 19, 21, 29, 31, 39, 41, 42, 43, 44, 45, 46, 47};
	return std::equal(&sorted_shisanyao[0], &sorted_shisanyao[0] + 13, tmp.begin(), CardPtrEqualChar());
}


//================================================================================================================

void CardManagerSx::Calc(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, std::vector<TingUnit>& tingInfo)
{
	CardVector tmp(handcard);
	SortCard(tmp);

//	result.clear();
	bool checked[4][9] = {false};
	Lsize size = tmp.size();
	for ( Lsize i = 0; i < size; ++i )
	{
		// 判断这张牌是否检查过
		if ( !tmp[i] )
		{
			continue;
		}
		// 判断要移除的牌是否是混
		bool ishun = false;
		if (VecHas(gameInfo.m_hCard, *tmp[i]))
		{
			continue;
		}
		Lint color = tmp[i]->m_color;
		Lint number = tmp[i]->m_number;
		if ( (color <= 0 || color > 4) || (number <= 0 || number > 9) )
		{
			continue;
		}
		if ( checked[color-1][number-1] )
		{
			continue;
		}
		checked[color-1][number-1] = true;

		TingUnit unit;
		unit.out_card = tmp[i];	// 表示要打出去的牌

		// 组织手牌
		CardVector cards;
		cards.reserve( size-1 );
		for ( Lsize k = 0; k < size; ++k )
		{
			if ( k != i )
			{
				cards.push_back( tmp[k] );
			}
		}

		CheckTing( cards, pengCard, agangCard, mgangCard, eatCard, gameInfo, unit.hu_units);
		Lint conditionNum = std::count_if(unit.hu_units.begin(), unit.hu_units.end(), SetRange2);
		if ( unit.hu_units.size() > 0 && conditionNum > 0)
		{
			//result.push_back( unit );
			tingInfo.push_back(unit);
		}
	}
}

// 查听
Lint CardManagerSx::CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<HuUnit>& winCards, const Card* ignore)
{
	Lint ret = -1;
	int retfan = -1;

	for (Lint i = 0; i < 4; ++i)
	{
		if ( (i+1) == gameInfo.m_dingQue )
		{
			continue;
		}
		for (Lint j = 0; j < 9; ++j)
		{
			Card card, hunCard;
			card.m_color = i+1;
			card.m_number = j+1;
			if ( ignore && card.m_color == ignore->m_color && card.m_number == ignore->m_number )
			{
				continue;
			}
			// 带混玩法 如果是混 跳过去
			if (VecHas(gameInfo.m_hCard, card))
			{
				continue;
			}
			Lint index = get_card_index(&card);
			TingCard tingcard;
			tingcard.m_card = card;

			HuUnit huUint;
			gameInfo.bChaTing = true;
			bool canHu = CheckCanHu333(handcard, pengCard, agangCard, mgangCard, eatCard, &card, gameInfo, tingcard.m_hu);
			if (canHu && get_card(index))
			{
				huUint.hu_card = get_card(index);
				winCards.push_back( huUint );
			}
		}
	}
	return ret;
}

bool CardManagerSx::CheckCanHu333(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{ 
	vec.clear();
	// 是否带混的玩法
	CheckCanHuHun( handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec );
	return vec.size()!=0;
}

void CardManagerSx::CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{
	// 临时手牌 不包括outCard
	CardVector tmpcards;
	int cardnum[4][9];
	memset(cardnum,0,sizeof(cardnum));

	int hunnum = 0;
	for ( auto it = handcard.begin(); it != handcard.end(); ++it )
	{
		if (VecHas(gameInfo.m_hCard, **it))
		{
			hunnum++;
		}
		else
		{
			tmpcards.push_back( *it );
			const int color = (*it)->m_color;
			const int number = (*it)->m_number;
			if(color > 0 && color < 5 && number > 0 && number < 10)
			{
				cardnum[color-1][number-1]++;
			}
		}
	}

	if (outCard)
	{
		// 需要判断outCard是自己抓取的还是别人的打的牌 别人打的牌就按照幺鸡处理 否则按照混处理
		if ( gameInfo.m_MePos == gameInfo.m_playerPos && outCard->m_color == 3 && outCard->m_number == 1 )
		{
			hunnum++;
			outCard = NULL;
		}
		else
		{
			const int color = outCard->m_color;
			const int number = outCard->m_number;
			if(color > 0 && color < 5 && number > 0 && number < 10)
			{
				cardnum[color-1][number-1]++;
			}
		}
	}

	if ( CheckHooWithHun(cardnum, hunnum) )	// 33332
	{
		vec.push_back(HU_XIAOHU);
	}
}

bool CardManagerSx::CheckHooWithHun( int cards[4][9], int hunnum )
{
	// 统计牌的数量
	int cardsnum[4] = {0};
	for( int i = 0; i < 4 ; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3] + hunnum)%3 != 2 )
	{
		return false;
	}

	int needhun[4] = {0};
	for ( int i = 0; i < 4; ++i )
	{
		needhun[i] = ToBeShunKeNeedHun( cards[i], i >= 3 );
	}
	// 混的数量大于需要混的数量 肯定能胡
	if ( needhun[0] + needhun[1] + needhun[2] + needhun[3] < hunnum )
	{
		return true;
	}
	// 逐个测试蒋的位置
	for ( int i = 0; i < 4; ++i )
	{
		int hunnum2 = hunnum;	// 除去非蒋位置所需要的混 剩余混的数量
		for ( int k = 0; k < 4; ++k )
		{
			if ( k != i )
			{
				hunnum2 -= needhun[k];
			}
		}
		if ( hunnum2 >= 0 )
		{
			if ( needhun[i] == 0 && hunnum2 == 2 )	// 测试两个混当蒋
			{
				return true;
			}

			for( int k = 0; k < 9 ; ++k )	// 测试没有混当蒋
			{
				if ( cards[i][k] >= 2 )
				{
					cards[i][k] -= 2;
					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= hunnum2 )
					{
						cards[i][k] += 2;
						return true;
					}
					cards[i][k] += 2;
				}
				else if ( cards[i][k] >= 1 ) // 测试一个混当蒋
				{
					cards[i][k] -= 1;
					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= (hunnum2-1) )
					{
						cards[i][k] += 1;
						return true;
					}
					cards[i][k] += 1;
				}
			}
		}
	}
	return false;
}

int CardManagerSx::ToBeShunKeNeedHun( const int card[9], bool bzipai )
{
	/*
	churunmin: 以前的最少万能牌算法正确性存在问题，对于19万输入可能性，约有9000多个返回错误结果
	数组有9项，每项取值范围[0, 4]，9项之和<=12，能组合出约19万种情况
	在抠点捉耗子中可以通过特定的牌型试出问题：玩家1牌型 11,11,11,12,12,13,15,15,15,16,16,17,17 服务器牌型 47 耗子牌型47
	现在改为新实现，正确性经过测试
	*/ 
	return malgo_min_huns((unsigned const *)card, bzipai);
	int needhun[6] = {0};
	{
		int cardindex[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		needhun[0] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 1, 4, 7, 2, 5, 8, 3, 6, 9 };
		needhun[1] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 2, 5, 8, 1, 4, 7, 3, 6, 9 };
		needhun[2] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 2, 5, 8, 3, 6, 9, 1, 4, 7 };
		needhun[3] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 3, 6, 9, 2, 5, 8, 1, 4, 7 };
		needhun[4] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 3, 6, 9, 1, 4, 7, 2, 5, 8 };
		needhun[5] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	int minneedhun = needhun[0];
	for ( int i = 1; i < 6; ++i )
	{
		if ( minneedhun > needhun[i] )
		{
			minneedhun = needhun[i];
		}
	}
	return minneedhun;
}

int	CardManagerSx::ToBeShunKeNeedHun( const int card[9], bool bzipai, int cardindex[9] )
{
	int tcard[11] = {0};
	memcpy( &tcard[0], card, sizeof(int)*9 );

	for( int i = 0; i < 9; )
	{
		int index = cardindex[i] - 1;
		// 先组刻
		if( tcard[index] >= 3 )
		{
			tcard[index] -= 3;
		}

		// 组顺子
		if( !bzipai && tcard[index] > 0 && tcard[index+1] > 0 && tcard[index+2] > 0 )
		{
			--tcard[index];
			--tcard[index+1];
			--tcard[index+2];
			continue;	// 防止还有一个一样的顺
		}
		++i;
	}

	int needhun = 0;
	// 剩余的牌组顺刻
	for( int i = 0; i < 9; ++i )
	{
		// 先组顺
		if ( !bzipai )
		{
			int num = tcard[i];
			for ( int k = 0; k < num; ++k )
			{
				if ( tcard[i+1] > 0 )
				{
					tcard[i]--;
					tcard[i+1]--;
					needhun++;
				}
				else if ( tcard[i+2] > 0 )
				{
					tcard[i]--;
					tcard[i+2]--;
					needhun++;
				}
			}
		}
		// 组刻
		if ( tcard[i] == 1 )
		{
			tcard[i]--;
			needhun += 2;
		}
		else if ( tcard[i] == 2 )
		{
			tcard[i] -= 2;
			needhun++;
		}
	}
	return needhun;
}

//// 临汾一门牌听牌时是否时混一色或者清一色 by wyz
bool CardManagerSx::CheckCanTing_yimenpai_spec(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard)
{
	Lint color = -1; bool hasZi = false;
	CardVector* cards[5] = { &handcard, &eatCard, &pengCard, &gangcard, &mgangcard };
	for (int i = 0; i < 5; ++i)
	{
		int space = 1;
		// 吃碰杠只检查第一张牌 这样可以避免带混的牌 带混的牌放在了牌组的后面
		if (i == 1 || i == 2)
		{
			space = 3;
		}
		else if (i == 3 || i == 4)
		{
			space = 4;
		}
		int size = (int)cards[i]->size();
		for (int k = 0; k < size; k += space)
		{
			if (!cards[i]->at(k))
			{
				return false;
			}
			int color_ = cards[i]->at(k)->m_color;
			if (color_ == CARD_COLOR_ZI)
			{
				hasZi = true;
				continue;
			}
			if (color_ > 3)	// 万饼条才算清一色 其他不算
			{
				return false;
			}
			if (color == -1)
			{
				color = color_;
				continue;
			}
			if (color != color_)
			{
				return false;
			}
		}
	}

	//if (color != -1)
	//{
	//	if (hasZi)
	//		return true;
	//	else
	//		return false;
	//}
	return true;
}
