#include "ShanXiCardKouDian.h"
#include "CounterManager.h"

ShanXiCardKouDian* ShanXiCardKouDian::mInstance = NULL;

ShanXiCardKouDian::ShanXiCardKouDian()
{
}

ThinkVec ShanXiCardKouDian::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	gameInfo.bChaTing = false;
	if(gameInfo.b_canHu && !IsHuaZhu(handcard, gameInfo.m_dingQue) && CheckCanHu(handcard,pengCard,agangCard,mgangCard,eatCard, outCard,gameInfo,unit.m_hu))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}
	
	if (!gameInfo.b_onlyHu)
	{
		if(!gameInfo.bNoCard && outCard->m_color != gameInfo.m_dingQue && CheckCanGang(handcard,outCard))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
		if(outCard->m_color != gameInfo.m_dingQue && CheckCanPeng(handcard,outCard))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_PENG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
	}
	else
	{
		// 考虑听牌还可以杠  但目前外层 b_onlyHu 还有其他的值得含义 待完善
	}
	
	return result;
}

ThinkVec ShanXiCardKouDian::CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	gameInfo.bChaTing = false;
	if(gameInfo.b_canHu && !IsHuaZhu(handcard, gameInfo.m_dingQue) && CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCard,gameInfo,unit.m_hu))
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

	std::vector<Card*> vec;
	if( !gameInfo.b_onlyHu && !gameInfo.bNoCard && CheckAnGang(tmp,vec))
	{
		for(Lsize i = 0 ; i < vec.size(); ++i)
		{
			if (vec[i]->m_color == gameInfo.m_dingQue)
				continue;

			unit.Clear();
			unit.m_type = THINK_OPERATOR_ABU;
			unit.m_card.push_back(vec[i]);
			result.push_back(unit);
		}
	}

	// 补杠随时可以补 补杠不会破坏牌型
	vec.clear();
	if(!gameInfo.bNoCard && CheckMGang(tmp,pengCard,vec))
	{
		for(Lsize i = 0 ; i < vec.size(); ++i)
		{
			if (vec[i]->m_color == gameInfo.m_dingQue)
				continue;

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

	return result;
}

bool ShanXiCardKouDian::CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Hu_type>& vec)
{ 
	vec.clear();
	// 是否带混的玩法
	if ( gameInfo.m_playtype.wzGametypeYaoJiDai() )
	{
//		CheckCanHuHun( handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec );
	}
	else
	{
		//CheckCanHuNormal( handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec );
	}

	if ( vec.size() > 0 )
	{
		// 前四和后四可以查听
		/*SiChuanHandler_WanZhou* handler = (SiChuanHandler_WanZhou*)gameInfo.m_gameHandler;
		if ( handler )
		{
			Lint deskcardsize = handler->GetDeskCardSize();
			if ( deskcardsize <= 4 )
			{
				vec.push_back(HU_HOUSI);
			}
			else if ( (108-DESK_USER_COUNT*13 - deskcardsize) <= 4 )
			{
				vec.push_back(HU_QIANSI);
			}

			if ( handler->IsTing( gameInfo.m_MePos ) )
			{
				vec.push_back(HU_TING);
			}
		}*/
	}
	if(vec.size() >= 2)
	{
		/*std::vector<Lint>::iterator it = vec.begin();
		for(; it !=  vec.end(); )
		{
			if((*it) == HU_XIAOHU)
			{
				vec.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}*/
	}

	return vec.size()!=0;
}

void ShanXiCardKouDian::CheckCanHuNormal(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{
	Lint special;
	if ( CheckXiaoqidui(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, special) )
	{
		if ( special >= 3 )
		{
			vec.push_back(HU_TRIPLESPECIAL_XIAOQIDUI);
		}
		else if ( special == 2 )
		{
			vec.push_back(HU_DOUBLESPECIAL_XIAOQIDUI);
		}
		else if ( special == 1 )
		{
			vec.push_back(HU_SPECIAL_XIAOQIDUI);
		}
		else
		{
			vec.push_back(HU_XIAOQIDUI);
		}
	}
	else if ( CheckHoo(handcard,outCard) )	// 33332
	{
		vec.push_back(HU_XIAOHU);
		if (CheckPengpenghu(handcard, eatCard, outCard))
		{ 
			vec.push_back(HU_PENGPENGHU);
		}
	}

	if(vec.size() > 0)
	{
		if(CheckQingyise(handcard,eatCard,pengCard,agangCard,mgangCard,outCard))
		{
			if ( pengCard.size()/3 + mgangCard.size()/4 >= 3 )
			{
				vec.push_back(HU_QINGSANDA);
			}
			else
			{
				vec.push_back(HU_QINGYISE);
			}
		}
	}
}

void ShanXiCardKouDian::CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{
	// 临时手牌 不包括outCard
	CardVector tmpcards;
	int cardnum[4][9];
	memset(cardnum,0,sizeof(cardnum));

	int hunnum = 0;
	for ( auto it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( (*it)->m_color == CARD_COLOR_SUO && (*it)->m_number == 1 )
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
		if ( gameInfo.m_MePos == gameInfo.m_playerPos && outCard->m_color == CARD_COLOR_SUO && outCard->m_number == 1 )
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

	Lint special;
	std::vector<Card> huncards;	// 混做的牌
	int needhun = CheckXiaoqiduiNeedHun( cardnum, eatCard, pengCard, agangCard, mgangCard, special, huncards );
	if ( needhun != -1 && needhun <= hunnum )
	{
		if ( (hunnum - needhun)/2 + special >= 3 )
		{
			vec.push_back(HU_TRIPLESPECIAL_XIAOQIDUI);
		}
		else if ( (hunnum - needhun)/2 + special == 2 )
		{
			vec.push_back(HU_DOUBLESPECIAL_XIAOQIDUI);
		}
		else if ( (hunnum - needhun)/2 + special == 1 )
		{
			vec.push_back(HU_SPECIAL_XIAOQIDUI);
		}
		else
		{
			vec.push_back(HU_XIAOQIDUI);
		}
	}
	else if ( CheckHooWithHun(cardnum, hunnum) )	// 33332
	{
		vec.push_back(HU_XIAOHU);

		int pengpenghuneedhun = CheckPengpenghuNeedHun(cardnum);
		if( pengpenghuneedhun != -1 && pengpenghuneedhun <= hunnum)
		{
			vec.push_back(HU_PENGPENGHU);
		}
	}

	if(vec.size() > 0)
	{
		if(CheckQingyise(tmpcards,eatCard,pengCard,agangCard,mgangCard,outCard))
		{
			if ( pengCard.size()/3 + mgangCard.size()/4 >= 3 )
			{
				vec.push_back(HU_QINGSANDA);
			}
			else
			{
				vec.push_back(HU_QINGYISE);
			}
		}
	}
}

// 查听
Lint ShanXiCardKouDian::CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore)
{
	Lint ret = -1;
	int retfan = -1;

	// 花猪未听
	if (IsHuaZhu(handcard, gameInfo.m_dingQue))
		return -1;

	for (Lint i = 0; i < 3; ++i)
	{
		if ( (i+1) == gameInfo.m_dingQue )
		{
			continue;
		}
		for (Lint j = 0; j < 9; ++j)
		{
			Card card;
			card.m_color = i+1;
			card.m_number = j+1;
			if ( ignore && card.m_color == ignore->m_color && card.m_number == ignore->m_number )
			{
				continue;
			}
			if ( gameInfo.m_playtype.wzGametypeYaoJiDai() )
			{
				// 带混玩法 如果是混 跳过去
				if ( card.m_color == CARD_COLOR_SUO && card.m_number == 1 )
				{
					continue;
				}
			}

			TingCard tingcard;
			tingcard.m_card = card;

			gameInfo.bChaTing = true;
			bool canHu = false;
			//canHu = CheckCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, &card, gameInfo, tingcard.m_hu);
			if (canHu)
			{
				// 计算番数
				for (Lsize k = 0; k < tingcard.m_hu.size(); ++k)
				{
					/*if ( gameInfo.m_hufan && gameInfo.m_hufan[tingcard.m_hu[k]] > 0 )
					{
						tingcard.m_hufan += gameInfo.m_hufan[tingcard.m_hu[k]];
					}*/
				}

				// 不查根
				// Lint gen = CheckGen(handcard, eatCard, pengCard, agangCard, mgangCard, &card);
				Lint gen = 0;
				tingcard.m_hufan += gen;
				tingcard.m_gen = gen;

				Lint maxFan = gameInfo.m_playtype.gametypeGetFan();
				if(tingcard.m_hufan > maxFan)
				{
					tingcard.m_hufan = maxFan;
				}

				winCards.push_back( tingcard );

				if(tingcard.m_hufan > retfan)
				{
					ret = winCards.size() - 1;
					retfan = tingcard.m_hufan;
				}
			}
		}
	}
	return ret;
}

TingVec ShanXiCardKouDian::CheckGetCardTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo )
{
	TingVec result;

	// 
	CardVector tmp = handcard;
	if (getCard)
	{
		tmp.push_back(getCard);
	}

	static int calccount = 0;
	static Llong calctime = 0;
	calccount++;
	LTime precur;

	// 多线程计算
	if ( gameInfo.m_playtype.wzGametypeYaoJiDai() )
	{
		gCounterManager.CalcTing( tmp, pengCard, agangCard, mgangCard, eatCard, gameInfo, this, result );
	}
	else
	{
		std::vector<CardValue> huntype;
		CardValue huncard;
		huncard.kColor = CARD_COLOR_SUO;
		huncard.kNumber = 1;
		huntype.push_back( huncard );
		gCounterManager.CalcTingWithHun( tmp, pengCard, agangCard, mgangCard, eatCard, gameInfo, this, huntype, result );
	}

	LTime now;
	calctime += now.MSecs() - precur.MSecs();
	if ( calccount == 10000 )
	{
//		LLOG_ERROR("[CardBase::CheckGetCardTing] %d calc spend %lld MSecs", calccount, calctime);
		calccount = 0;
		calctime = 0;
	}
	return result;
}