#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

struct CLIENT_INFO
{
	Lstring strUUID;
	LSocketPtr sp;
	Lint  time;
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	Work();
	virtual ~Work();
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);
	void			SaveCurrentOnline();
	LTime&			GetCurTime();
public:
	void	HanderUserKick(LMsgKick* msg);
	void	HanderClientIn(LMsgIn* msg);
	void	HanderConvertClientMsg(LMsgConvertClientMsg* msg);
	void	HanderUserLogin(LMsgC2SMsg* msg, boost::shared_ptr<CLIENT_INFO> clientInfo);
	bool	HanderUserPhoneLogin(LMsgC2SPhoneLogin* msg);
	void	HanderToLoginServer(LMsgToLoginServer* msg);
	void	HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void	_initTickTime();
protected:
	void	_connectCenter();
	void	_delSp(LSocketPtr sp);
	void	_reconnectCenter();
	void	_checkCenterHeartBeat();
private:
	LTime		m_tickTimer;
	Llong		m_5SceTick;//15秒循环一次
	Llong		m_30SceTick;//30秒循环一次
private:
	LSocketPtr m_socketCenter;

	std::map<Lstring, boost::shared_ptr<CLIENT_INFO> > m_mapUUID2ClientInfo;
	std::map<LSocketPtr, boost::shared_ptr<CLIENT_INFO> > m_mapSp2ClientInfo; 
private:
	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
private:
	CMemeoryRecycle m_memeoryRecycle;
};


#define gWork Work::Instance()

#endif