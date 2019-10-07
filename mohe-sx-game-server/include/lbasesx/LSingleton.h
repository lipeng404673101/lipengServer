#ifndef _L_SINGLETON_H_
#define _L_SINGLETON_H_

#include "LBase.h"


/*
*	brief 单例模板类
*/
template<class T>
class LSingleton
{
private:
	LSingleton(const LSingleton&);
	LSingleton& operator = (const LSingleton&);

public:
	LSingleton()
	{

	}
	~LSingleton()
	{

	}

	virtual bool Init()=0;
	virtual bool Final()=0;

	static T& Instance()
	{
		static T t;
		return t;
	}
};

#endif