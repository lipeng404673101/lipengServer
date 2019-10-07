#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

#define INVALID_GATEID			0xFFFFFFFF

struct LOGIC_SERVER_INFO
{
	LOGIC_SERVER_INFO()
	{
		m_tickTime = 0;
	}

	LogicInfo m_logicInfo;
	Llong	  m_tickTime;								//当这个logic被tick的时候记录时间，3分钟不恢复才进行剔除.默认值为0
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	Work();
	virtual ~Work();
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void    Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);
	void			SaveCurrentOnline();
	LTime&			GetCurTime();

protected:
	void	HanderUserKick(LMsgKick* msg);
	void	HanderClientIn(LMsgIn* msg);
	void	HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void	HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);

//===================== Manager ======================================
protected:
	void	HanderCoinsLogin(LMsgLMG2CNLogin* msg);
	void	HanderFreeDeskReply(LMsgLMG2CNFreeDeskReply* msg);

public:
	void	SendToLogicManager(LMsg& msg);

//===================== Gate ======================================
protected:
	void	HanderGateLogin(LMsgG2CNLogin* msg);
	void	HanderGateLogout(LMsgKick* msg);

public:
	Lint	getGateIdBySp(LSocketPtr sp);
	Lint	GetGateCount();
	void	SendToAllGate(LMsg& msg);
	void	SendToGate(Lint gateID, LMsg& msg);

//===================== Logic ======================================
protected:
	void	HanderLogicLogin(LMsgL2CNLogin* msg);
	void	HanderLogicLogout(LMsgKick* msg);
	void	HanderCreateCoinDeskRet(LMsgL2CNCreateCoinDeskRet* msg);
	void	HanderEndCoinDesk(LMsgL2CNEndCoinDesk* msg);
	void	_checkLogicServerState();

public:
	bool	isLogicServerExist(int iLogicServerId);
	void	SendToAllLogic(LMsg& msg);
	void	SendToLogic(Lint iLogicServerId, LMsg& msg);


protected:
	void	_initTickTime();
protected:
	void	_connectLogicManager();
	void	_reconnectLogicManager();
	void	_checkLogicManagerHeartBeat();
	
private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;//30秒循环一次
	Llong		m_5SceTick;//15秒循环一次
	Llong		m_30SceTick;//30秒循环一次
	Llong		m_600SceTick;//5分钟循环一次

private:
	LSocketPtr m_logicManager;

	// Gate
	boost::recursive_mutex m_mutexGateInfo;
	std::map<Lint, GateInfo> m_gateInfo;

	// Logic
	boost::recursive_mutex m_mutexLogicServerInfo;
	std::map<Lint, LOGIC_SERVER_INFO> m_logicServerInfo;

private:
	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
private:
	CMemeoryRecycle m_memeoryRecycle;
};


#define gWork Work::Instance()

#endif