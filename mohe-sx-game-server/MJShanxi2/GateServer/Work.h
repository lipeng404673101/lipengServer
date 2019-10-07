#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "GateUser.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

class Work:public LRunnable,public LSingleton<Work>
{
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

	LTime&			GetCurTime();
	
public:
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);
	//新启动客户端ID冲突，停止掉该客户端
	void			HanderKillClient(LMsgKillClient* msg);
	//处理LogicManager同步的Logic消息
	void			HanderLogicInfo(LMsgLMG2GateLogicInfo* msg);
	//处理跑马灯消息
	void			HanderHorseInfo(LMsgLMG2GHorseInfo* msg);
	//修改User状态
	void			HanderModifyUserState(LMsgL2GModifyUserState* msg);

	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void			_doWithClientMessage(LMsgConvertClientMsg* pMsg);
	void			_doWithClientLoginGate(LMsgC2SLoginGate* msg);
	void			_doWithClientHeartBeat(LMsgC2SHeart* pMsg);
	void			_doWithClientOtherMsg(boost::shared_ptr<LMsg> msgEntity, boost::shared_ptr<LBuff> msgOriginData);

	bool			_isClientMessageSafe(boost::shared_ptr<LMsgC2SVerifyHead> msgHead);
public:
	void			ConnectToLogicManager();
	
	//发送消息到所有User
	void			SendMessageToAllUse(LMsg* msg);

	void			SendToLogicUserOut(boost::shared_ptr<GateUser> user);
	void			HanderLogicUserOut(Lint id);

	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	LogicInfo*		GetLogicInfoById(Lint id);

protected:
	//处理LogicManager同步的Coins信息
	void			HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg);
public:
	void			SendToCoinsServer(LMsg& msg);

protected:
	void			_initTickTime();
protected:
	//true: 查找到并且删除， false：没有查询到
	bool			_findLogicSPAndDel(LSocketPtr sp); 
	bool			_isLogicSP(LSocketPtr sp);
	LogicInfo*		_getLogicSP(LSocketPtr sp);

	void			_reconnectLogicManager();
	void			_reconnectLogicServer(int iLogicId);
	void			_reconnectCoinsServer();

	void			_checkLogicManagerHeartBeat();
	void			_checkLogicServerHeartBeat();
	void			_checkCoinsServerHeartBeat();
protected:
	void			_checkGateUserState();
	void			_checkClientSp();

private:
	//测试用
	//void        testPrintLog(Lstring uuid, Lint flag);

	//重启以后，同步manager信息
private:
	void	SendToLogicManager(LMsg& msg);
	//和manager断线重连后，向manager同步用户信息
	void    _SynchroData2Manager();
	//bool   m_needSynchroData2Manager=false;      //重连manager需要同步数据标志

private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;//1秒循环一次
	Llong		m_5SceTick;//5秒循环一次
	Llong		m_30SceTick;//30秒循环一次
private:
	std::map<LSocketPtr, Lint> m_mapClientSp;	//记录所有没有被认证过的客户端连接
private:
	LSocketPtr  m_logicManager;
	std::map<Lint, LogicInfo>	m_logicInfo;
	CoinsInfo	m_coinsServer;

	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;

	CMemeoryRecycle	m_memeoryRecycle;
};


#define gWork Work::Instance()

#endif