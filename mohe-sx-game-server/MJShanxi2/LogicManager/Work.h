#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

#define INVALID_GATEID			0xFFFFFFFF
#define INVALID_LOGICSERVERID	0xFFFFFFFF

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
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

protected:
	void			Tick(LTime& cur);
	LTime&			GetCurTime();
protected:
	void			HanderMsg(LMsg* msg);

	void			HanderClientIn(LMsgIn* msg);
	void			HanderUserKick(LMsgKick* msg);

	//center发送玩家id信息
	void			HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg);
	void			HanderCenterGMHorse(LMsgCe2LGMHorse* msg);
	void			HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg);
	void			HanderCenterGMHide(LMsgCe2LGMHide* msg);
	void			HanderLogicSetGameFree(LMsgCE2LSetGameFree* msg);
	void			HanderLogicSetPXActive(LMsgCE2LSetPXActive* msg);
	void			HanderLogicSetOUGCActive(LMsgCE2LSetOUGCActive* msg);

	void            HanderLogicSetExchActive(LMsgCE2LSetExchActive* msg);
	void            HanderLogicSetActivity(LMsgCE2LSetActivity* msg);
	void            HanderCenterConfig(LMsgCE2LMGConfig* msg);
	void            HanderCenterSpecActive(LMsgLMG2LSpecActive* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void			HanderGateLogin(LMsgG2LMGLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	void            HanderUpdateGateIpFromCenter(MHLMsgCe2LUpdateGateIp * msg);
	

	//发送网关信息到center
	void			SendGateInfoToCenter();

	//发送logic消息到其他gate
	void			SendLogicInfoToGates(Lint nGateID = 0);

	//与center 传递俱乐部相关信息的接口
private:
	void			HanderCenterClubInfo(LMsgCe2LClubInfo*msg);
	void           HanderAddClub(LMsgCe2LAddClub* msg);
	void           HanderClubAddPlayType(LMsgCe2LMGClubAddPlayType *msg);
	void           HanderClubAddUser(LMsgCe2LMGClubAddUser *msg);
	void           HanderClubDelUser(LMsgCe2LMGClubDelUser *msg);
	void           HanderClubHidePlayType(LMsgCe2LMGClubHidePlayType *msg);
	void           HanderClubModifyName(LMsgCe2LMGClubModifyClubName *msg);
   void           HanderClubModifyFeeType(LMsgCe2LMGClubModifyClubFee *msg);
   void           HanderUserApplyClub(LMsgCe2LMGClubAddUser *msg);

   void           HanderDismissRoom(LMsgCe2LMGDismissDesk *msg);

   void           HanderDelUserFromMap(LMsgCe2LMGDelUserFromManager *msg);

	//接收logic,gate发送的同步数据
private:
	void           HanderLogicSynchroData(LMsgL2LMGSynchroData*msg);
	void           HanderGateSynchroData(LMsgG2LMGSynchroData*msg);
	void           SendRequestInfoToGateOrLogic(Lint GOrL, Lint serverId, Lint needData=1);    //  1-gate  2-logic,
	void           SendRequestInfoToAllGateOrLogic(Lint GOrL);//  1-gate  2-logic,

	//发送控制信息到logic
	void           HanderControlMsgToLogic(LMsgCe2LMGControlMsg *msg);

protected:
	// 金币服务器相关
	CoinsInfo		m_coinsServer;
	void			HanderCoinsLogin(LMsgCN2LMGLogin* msg);
	void			HanderCoinsLogout(LMsgKick* msg);
	void			HanderCoinFreeDeskReq(LMsgCN2LMGFreeDeskReq* msg);
	void			HanderCoinRecycleDesk(LMsgCN2LMGRecycleDesk* msg);

	void			_checkCoinsServerState();

	//发送Coins信息到Gate
	void			SendCoinsInfoToGates(Lint nGateID = 0);
	//发送Coins信息到Logic
	void			SendCoinsInfoToLogic(Lint nLogicID = 0);

public:
	bool			IsCoinsServerConnected();
	void			SendMessageToCoinsServer(LMsg& msg);

public:
	Lint			getGateIdBySp(LSocketPtr sp);
	void			SendMessageToAllGate(LMsg& msg);
	void			SendMessageToGate(Lint gateID, LMsg& msg);
private:
	boost::recursive_mutex m_mutexGateInfo;
	std::map<Lint, GateInfo> m_gateInfo;
	std::set<Lint>  m_mapGateId;
protected:
	void			HanderLogicLogin(LMsgL2LMGLogin* msg);
	void			HanderLogicLogout(LMsgKick* msg);
	void			_checkLogicServerState();
	void			_checkAndTouchLogicServer();
	void			_checkLogicServerOnlySupportGameType();
	void			_checkRLogStart();
protected:
	void            _checkFreeModel();
	void            _checkExchActive();
	void            _checkMaxDeskCountLimitForLogicServer();

public:
	bool			isLogicServerExist(int iLogicServerId);
	void			SendMessageToAllLogic(LMsg& msg);
	void			SendMessageToLogic(Lint iLogicServerId, LMsg& msg);
	Lint			allocateLogicServer(int iGameType);
	
	void			delDeskCountOnLogicServer(Lint iLogicServerId, int iDelCount = 1);
private:
	boost::recursive_mutex m_mutexLogicServerInfo;
	std::map<Lint, LOGIC_SERVER_INFO> m_logicServerInfo;
	std::map<Lint, std::map<Lint, Lint> > m_mapLogicServerOnlySupportType; //这些服务器只支持这些游戏类型。格式： 主键是logicserver。后面是游戏类型
public:
	void			SendToCenter(LMsg& msg);
	void			SendMsgToDb(LMsg& msg);
protected:
	//以下在程序启动的时候连接不需要互斥
	void			ConnectToCenter();
	void			ConnectToDb();

	void			_reconnectDB();
	void			_reconncetCenter();
	void			_checkCenterHeartBeat();
	void			_checkLogicDBHeartBeat();
private:
	boost::mutex m_mutexCenterClient;
	boost::mutex m_mutexDBClient;

	LSocketPtr	m_centerClient;//链接center
	LSocketPtr	m_dbClient;//链接db

	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
protected:
	void		_initTickTime();
private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;		//1秒循环一次
	Llong		m_5SecTick;		//5秒循环一次
	Llong		m_15SceTick;	//15秒循环一次
	Llong		m_30SceTick;	//30秒循环一次
	Llong		m_180SecTick;	//3分钟循环一次
	Llong		m_600SceTick;	//5分钟秒循环一次
	Llong		m_1DaySceTick;  //一天
	Lint        m_last_alert_time; // 上次报警时间

	Lint        m_maxDeskCountLimitForEachLogic; //Logic Server 上最大支持的桌子数量
	bool		m_bHadLoadedData;
private:
	CMemeoryRecycle m_memeoryRecycle;
};

#define gWork Work::Instance()

#endif