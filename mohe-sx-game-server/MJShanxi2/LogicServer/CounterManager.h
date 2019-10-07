#ifndef _COUNTER_MG_GM_H_
#define _COUNTER_MG_GM_H_

#include "LBase.h"
#include "LSingleton.h"
#include "Counter.h"
#include "CardBase.h"

#define  COUNTER_NUM   4

//db·þÎñÆ÷
class CounterManager : public LSingleton<CounterManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Clear();

	void                Start();

	void                Join();

	void                Stop();

	// ¼ÆËãÌý
	void                CalcTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, CardBase* cardmgr, TingVec& result );

	void                CalcTingWithHun( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, CardBase* cardmgr, const std::vector<CardValue>& huntype, TingVec& result );

private:

	void                Calc( CounterUnit* counter[COUNTER_NUM] );

	Counter      m_counter[COUNTER_NUM];

	friend class Counter;
	boost::mutex m_mutex;
	boost::condition_variable m_condition;

};


#define gCounterManager CounterManager::Instance()

#endif