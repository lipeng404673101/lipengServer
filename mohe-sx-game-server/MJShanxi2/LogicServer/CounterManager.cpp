#include "LLog.h"
#include "CounterManager.h"
#include "BaseCommonFunc.h"

//////////////////////////////////////////////////////////////////////////
bool CounterManager::Init()
{
	return true;
}

bool CounterManager::Final()
{

	return true;
}

void CounterManager::Clear()
{

}

void CounterManager::CalcTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, CardBase* cardmgr, TingVec& result )
{
	static CounterTing counter[COUNTER_NUM];
	CounterUnit* units[COUNTER_NUM];
	for ( Lsize i = 0; i < COUNTER_NUM; ++i )
	{
		units[i] = &counter[i];
		counter[i].cardmgr = cardmgr;
		counter[i].handcard = &handcard;
		counter[i].pengCard = &pengCard;
		counter[i].agangCard = &agangCard;
		counter[i].mgangCard = &mgangCard;
		counter[i].eatCard = &eatCard;
		counter[i].gameInfo = &gameInfo;
		counter[i].index = i;
	}

	Calc( units );

	for ( Lsize i = 0; i < COUNTER_NUM; ++i )
	{
		TingVec::iterator it = counter[i].result.begin();
		for ( ; it != counter[i].result.end(); ++it )
		{
			// 判断这张牌是否检查过
			TingVec::iterator rstit = result.begin();
			for ( ; rstit != result.end(); ++rstit )
			{
				/*if ( rstit->m_card.m_color == it->m_card.m_color && rstit->m_card.m_number == it->m_card.m_number )
				{
					break;
				}*/
			}
			if ( rstit == result.end() )
			{
				result.push_back( *it );
			}
		}
	}
}

void CounterManager::CalcTingWithHun( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, CardBase* cardmgr, const std::vector<CardValue>& huntype, TingVec& result )
{
	static CounterTingWithHun counter[COUNTER_NUM];
	CounterUnit* units[COUNTER_NUM];
	for ( Lsize i = 0; i < COUNTER_NUM; ++i )
	{
		units[i] = &counter[i];
		counter[i].cardmgr = cardmgr;
		counter[i].handcard = &handcard;
		counter[i].pengCard = &pengCard;
		counter[i].agangCard = &agangCard;
		counter[i].mgangCard = &mgangCard;
		counter[i].eatCard = &eatCard;
		counter[i].gameInfo = &gameInfo;
		counter[i].index = i;
		counter[i].huntype = huntype;
	}

	Calc( units );

	for ( Lsize i = 0; i < COUNTER_NUM; ++i )
	{
		TingVec::iterator it = counter[i].result.begin();
		for ( ; it != counter[i].result.end(); ++it )
		{
			// 判断这张牌是否检查过
			TingVec::iterator rstit = result.begin();
			for ( ; rstit != result.end(); ++rstit )
			{
				/*if ( rstit->m_card.m_color == it->m_card.m_color && rstit->m_card.m_number == it->m_card.m_number )
				{
					break;
				}*/
			}
			if ( rstit == result.end() )
			{
				result.push_back( *it );
			}
		}
	}
}


void CounterManager::Calc( CounterUnit* counter[COUNTER_NUM] )
{
	// test 单线程
// 	for ( int i = 0; i < COUNTER_NUM; ++i )
// 	{
// 		counter[i]->Calc();
// 	}
// 	return;

	for ( int i = 0; i < COUNTER_NUM; ++i )
	{
		m_counter[i].Notify( counter[i] );
	}

	for ( int i = 0; i < COUNTER_NUM; ++i )
	{
		counter[i]->WaitFinish();
	}
}

void CounterManager::Start()
{
	for(int i = 0; i < COUNTER_NUM; ++ i)
	{
		m_counter[i].Start();
	}
}

void CounterManager::Join()
{
	for(int i = 0; i < COUNTER_NUM; ++i)
	{
		m_counter[i].Join();
	}
}

void CounterManager::Stop()
{
	for(int i = 0; i < COUNTER_NUM; ++i)
	{
		m_counter[i].Stop();
		m_counter[i].Notify( NULL );
	}
}
