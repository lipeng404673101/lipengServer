#ifndef _L_MEMEORY_RECYCLE_
#define _L_MEMEORY_RECYCLE_

#include "LRunnable.h"
#include "LMsg.h"

class API_LBASESX CMemeoryRecycle : public LRunnable
{
public:
	CMemeoryRecycle();
	virtual ~CMemeoryRecycle();
public:
	virtual void Run();
	virtual void Clear();
public:
	void recycleMsg(LMsg* pMsg);
};

#endif