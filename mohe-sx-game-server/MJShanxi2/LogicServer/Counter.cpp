#include "LLog.h"
#include "Counter.h"
#include "CounterManager.h"

//////////////////////////////////////////////////////////////////////////

void CounterUnit::Finish()
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_finish = true;
	}
	m_condition.notify_one();
}

void CounterUnit::WaitFinish()
{
	boost::mutex::scoped_lock lock( m_mutex );
	if ( m_finish )
	{
		return;
	}
	m_condition.wait( lock );
}

void CounterTing::Calc()
{
	result.clear();
	bool checked[4][9] = {false};
	Lsize size = handcard->size();
	for ( Lsize i = 0; i < size; ++i )
	{
		if ( i%COUNTER_NUM != index )
		{
			continue;
		}

// 		char n[16];
// 		sprintf( n, "%d:%d\n", index, i );
// 		OutputDebugStringA(n);

		// 判断这张牌是否检查过
		if ( !(*handcard)[i] )
		{
			continue;
		}
		Lint color = (*handcard)[i]->m_color;
		Lint number = (*handcard)[i]->m_number;
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
//		unit.m_card = *(*handcard)[i];	// 表示要打出去的牌

		// 组织手牌
		CardVector cards;
		cards.reserve( size-1 );
		for ( Lsize k = 0; k < size; ++k )
		{
			if ( k != i )
			{
				cards.push_back( (*handcard)[k] );
			}
		}

//		cardmgr->CheckTing( cards, *pengCard, *agangCard, *mgangCard, *eatCard, *gameInfo, unit.m_tingcard );
	/*	if ( unit.m_tingcard.size() > 0 )
		{
			result.push_back( unit );
		}*/
	}
}

void CounterTingWithHun::Calc()
{
	result.clear();
	bool checked[4][9] = {false};
	Lsize size = handcard->size();
	for ( Lsize i = 0; i < size; ++i )
	{
		if ( i%COUNTER_NUM != index )
		{
			continue;
		}

		// 判断这张牌是否检查过
		if ( !(*handcard)[i] )
		{
			continue;
		}
		// 判断要移除的牌是否是混
		bool ishun = false;
		for ( auto it = huntype.begin(); it != huntype.end(); ++it )
		{
			if ( (*handcard)[i]->m_color == it->kColor && (*handcard)[i]->m_number == it->kNumber )
			{
				ishun = true;
				break;
			}
		}
		if ( ishun )
		{
			continue;
		}
		
		Lint color = (*handcard)[i]->m_color;
		Lint number = (*handcard)[i]->m_number;
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
//		unit.m_card = *(*handcard)[i];	// 表示要打出去的牌

		// 组织手牌
		CardVector cards;
		cards.reserve( size-1 );
		for ( Lsize k = 0; k < size; ++k )
		{
			if ( k != i )
			{
				cards.push_back( (*handcard)[k] );
			}
		}

//		cardmgr->CheckTing( cards, *pengCard, *agangCard, *mgangCard, *eatCard, *gameInfo, unit.m_tingcard );
		//if ( unit.m_tingcard.size() > 0 )
		//{
		//	result.push_back( unit );
		//}
	}
}

Counter::Counter()
	: m_counter(NULL)
{
}

bool Counter::Init()
{
	return true;
}

bool Counter::Final()
{
	return true;
}


void Counter::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}

	while(!GetStop())
	{
		boost::mutex::scoped_lock lock( m_mutex );
		if ( m_counter )
		{
			m_counter->Calc();
			m_counter->Finish();
			m_counter = NULL;
		}
		m_condition.wait( lock );

		if ( m_counter )
		{
			m_counter->Calc();
			m_counter->Finish();
			m_counter = NULL;
		}
	}
	
	this->Final();
}

void Counter::Clear()
{

}

void Counter::Notify( CounterUnit* unit )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_counter = unit;
		if ( m_counter )
		{
			m_counter->m_finish = false;
		}
	}
	m_condition.notify_one();
};