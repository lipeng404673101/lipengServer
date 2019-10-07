#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

class User;

struct UserLoginInfo
{
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

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
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	//处理玩家登陆的消息 
	void			HanderUserLogin(LMsgLMG2LLogin* msg);
	
	//处理玩家登陆的消息
	void			HanderDeskUserLogin(User* pUser, Lint gateId, Lstring& ip, bool firstLogin = false, Lstring buyInfo = "", Lint hide = 0);

	//处理玩家登出的消息 
	void			HanderUserLogout(Lint gateId, const Lstring& strUUID);

	//处理玩家创建桌子
	void			handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg);

	void            handerLMGCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg);

	void            handerLMGCreatedDeskList(LMsgLMG2LCreatedDeskList* msg);

	void            handerLMGDisMissCreatedDesk(LMsgLMG2LDissmissCreatedDesk* msg);

	//处理玩家加入桌子
	void			handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg);

	// 查询桌子信息
	void            handerLMGQueryGPSInfo(MHLMsgLMG2LQueryRoomGPSInfo * msg);

	//关于俱乐部的部分
	void              handerLMGCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg);
	void              handerLMGCreateClubDeskAndEnter(LMsgLMG2LCreateClubDeskAndEnter*msg);
	void              handerLMGEnterClubDesk(LMsgLMG2LEnterClubDesk*msg);
	void              handerLMGDisMissClubDesk(LMsgLMG2LDissmissClubDesk*msg);

	//处理LM发送的牌型活动配置信息
	void			handerPXActiveInfo(LMsgLMG2LPXActive* msg);
	
	void            HanderLMGCreateCoinDesk(LMsgCN2LCreateCoinDesk* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);

	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);

	//回收桌子ID（创建失败，房间结束）
	void			RecycleDeskId(Lint deskID);
	//俱乐部
	void            RecycleDeskId(Lint deskID, Lint clubId, Lint playTypeId, Lint clubDeskId);
public:
	void			ConnectToLogicManager();

	void			SendToLogicManager(LMsg& msg);
public:
	void			HanderGateLogin(LMsgG2LLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);

	/* 处理用户打牌等消息 */
	void			HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate);

protected:
	//处理LogicManager同步的Coins信息
	void			HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg);
public:
	void			SendToCoinsServer(LMsg& msg);

	//处理manager断开连接后，再次连接manager后同步logic信息到manager上
private:
	//和manager断线重连后，向manager同步自己信息
	void           _SynchroData2Manager();
	//bool   m_needSynchroData2Manager=false;      //重连manager需要同步数据标志

public:
	//DB链接相关
	void			ConnectToDb();
	void			SendMsgToDb(LMsg& msg);
protected:
	void			_initTickTime();
protected:
	void			_reconnectLogicManager();
	void			_reconnectLogicDB();
	void			_reconnectCoinsServer();

	void			_checkLogicManagerHeartBeat();
	void			_checkLogicDBHeartBeat();
	void			_checkCoinsServerHeartBeat();

	void			_checkRLogStart();
private:
	LTime		m_tickTimer;

	Llong		m_1SceTick;//1秒循环一次
	Llong		m_15SecTick;//15秒循环一次
	Llong		m_30SceTick;//30秒循环一次
	Llong		m_600SceTick;//5分钟循环一次
private:
	LSocketPtr	m_logicManager;//链接center
	LSocketPtr	m_dbClient;//链接db
	CoinsInfo	m_coinsServer;

	std::map<Lint, GateInfo> m_gateInfo;
private:
	std::map<LSocketPtr, Llong> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
private:
	CMemeoryRecycle m_memeoryRecycle;
	Lstring m_logFileName;
	Lint m_logFileIndex;
};

#define gWork Work::Instance()

#endif