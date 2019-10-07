#pragma once

#include "LNet.h"
#include "LSingleton.h"
#include "LMsg.h"

class Net_Server_Inside :public LNet, public LSingleton<Net_Server_Inside>
{
public:
	void Handle_Msg_FuncCall(LMsg* msg);

public:
	virtual bool Init()	{ return true;}
	virtual bool Final(){ return true;}

public:
	bool Start(Lstring Ip,Lshort Port,Lint ThreadCount);
	void Tick();


private:
	virtual void	RecvMsgPack(LBuffPtr recv,LSocketPtr s);
	void			Push(void* msg);
	void*			Pop();
	void			Swap();

private:
	std::list<void*>	m_msgQueue[2];
	std::list<void*>*	m_msgRecv;
	std::list<void*>*	m_msgDoing;
	boost::mutex		m_msgMutex;
};

#define gServer_InsideNet Net_Server_Inside::Instance()




