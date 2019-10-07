#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "DbServer.h"
#include "LMemoryRecycle.h"

class ShipInfo
{
public:
	ShipInfo(Lint inviter,Lint count)
	{
		m_inviter = inviter;
		m_count = count;
	}
	Lint m_inviter;
	Lint m_count;
private:
	ShipInfo();
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void    Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();
public:
	void	Tick(LTime& cur);
	LTime&	GetCurTime();
public:
	void	HanderMsg(LMsg* msg);

	//处理客户端连接上来消息
	void	HanderClientIn(LMsgIn* msg);

	//处理客户端掉线的消息 
	void	HanderUserKick(LMsgKick* msg);
protected:
	void	HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg);
	void	HanderLogicServerLogin(LMsgL2LdbLogin* msg);
	void	HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
public:
	inline bool GetDBMode() { return m_bDBMode; }
	//以下三个目前并没有上线，以后考虑去掉
	void	HanderGetInvitingInfo(LMsgC2SGetInvitingInfo* msg);
	void	HanderBindInviterMsg(LMsgLM_2_LDBBindInviter* msg);
	void	HanderReqInviterInfo(LMsgLM_2_LDB_ReqInfo* msg);

	// 活动相关
	void    HanderLogicSetActivity(LMsgCE2LSetActivity* msg);

	void    SetDrawActive(const Lstring& strSet,int id);
	void    SetShareActive(const Lstring& strSet,int id);

	// 
	void	SendToLogicManager(LMsg& msg);
	// 发送给玩家
	void	SendToUser( const Lstring& uuid, LMsg& msg );
protected:
	void	_initTickTime();

	//清除24小时以上日志， 每天早上6点执行 
	void	_clearOldLogVideo();

	void	_loadFriendship();
private:
	LTime		m_tickTimer;
	Llong		m_200SceTick;//2分钟秒循环一次	
	Llong		m_600SceTick;//5分钟秒循环一次	
private:
	bool m_bDBMode;     // 数据库主，备切换使用
	LDBSession	m_dbsession;
	LDBSession	m_dbsessionBK;

	CMemeoryRecycle	m_memeoryRecycle;

	boost::mutex m_mutexLogicManager;
	LSocketPtr	m_logicManager;//链接manager

	std::map<int,ShipInfo> m_user2recommIdMap;
};


#define gWork Work::Instance()

#endif