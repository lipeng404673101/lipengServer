#ifndef _COUNTER_H_
#define _COUNTER_H_

#include "LRunnable.h"
#include "LTime.h"
#include "CardBase.h"

class CounterUnit
{
public:
	CounterUnit()
		: m_finish(false)
	{

	}

	virtual ~CounterUnit() {}

	void Finish();
	void WaitFinish();
	virtual void Calc() = 0;

	bool m_finish;
protected:
	boost::mutex m_mutex;
	boost::condition_variable m_condition;
};

class CounterTing : public CounterUnit
{
public:
	virtual void Calc();

	CardBase* cardmgr;
	CardVector* handcard;
	CardVector* pengCard;
	CardVector* agangCard;
	CardVector* mgangCard;
	CardVector* eatCard;
	OperateState* gameInfo;

	int index;	// 表示第几个 用来分段计算
	TingVec result;
};

class CounterTingWithHun : public CounterTing
{
public:
	virtual void Calc();

	std::vector<CardValue> huntype;	// 表示混牌的类型 如：如果只有一个幺鸡做混 直接填入一个幺鸡即可
};


//db服务器
class Counter : public LRunnable
{
public:
	Counter();

protected:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

public:
	void				Notify( CounterUnit* unit );

protected:
	CounterUnit* m_counter;
	boost::mutex m_mutex;
	boost::condition_variable m_condition;
};

#endif