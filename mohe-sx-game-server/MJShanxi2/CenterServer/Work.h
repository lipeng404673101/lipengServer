#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "DbServer.h"
#include "DbServerManager.h"
#include "LMemoryRecycle.h"
#include "LRedisClient.h"

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

	void		Tick(LTime& cur);
	void		SaveCurrentOnline();
	LTime&		GetCurTime();
protected:
	void		HanderMsg(LMsg* msg);
protected:	
	void		HanderClientIn(LMsgIn* msg);
	void		HanderUserKick(LMsgKick* msg);
	void		HanderLogicLogin(LMsgL2CeLogin* msg);
	void		HanderLogicGateInfo(LMsgL2CeGateInfo* msg);
	void		HanderLogicLogout(LMsgKick*msg);
	void		HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void        HanderModifyCredit(LMsgLM2CEN_ADD_CRE* msg);
protected:
	void		HanderHttp(LMsgHttp* msg);	
	void		HanderGMCharge(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGMCoins(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	bool		HanderCheckMd5(std::map<Lstring, Lstring>& param);
	void		HanderSetFreeSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderSetPXActive(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderSetOUGCActive(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void		HanderSetExchange(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void        HanderActivityDrawSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void        HanderActivityShareSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void        HanderClubAddClub(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void        HanderClubAddPlayType(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void        HanderClubAddUser(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void        HanderClubDelUser(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void        HanderClubHidePlayType(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void        HanderClubAlterClubName(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void        HanderClubAlterClubFee(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void        HanderDismissRoom(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void       HanderDismissClubRoom(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void       HanderDelUserFromManager(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void       HanderFrushSpecActive(std::map<Lstring, Lstring>& param, LSocketPtr sp);
   void        HanderClubUserApplyClub(std::map<Lstring, Lstring>& param, LSocketPtr sp);


	//接收manager更新俱乐部桌子信息
private:
	void       HanderManagerUpdateData(LMsgLMG2CEUpdateData*msg);
	
public:
	bool		isLogicManagerExist(Lint logicManagerID);
	bool		isLogicManagerExist(LSocketPtr sp);

	GateInfo	GetGateUsefulInfo(Lint logicID);
	GateInfo    GetAliGaoFangGateUsefulInfo(Lint logicID); //获取高防的GateInfo
	GateInfo    GetYunManGaoFangGateUsefulInfo(Lint logicID); //获取高防的GateInfo
	Lstring		getServerNameByID(Lint serverID);	 

	void		SendMsgToLogic(LMsg& msg, Lint logicID);
	void		SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool);
	void		SendMsgToAllLogic(LMsg& msg);
	Lstring     GetGaofangIp(Lint gaofang_flag);
	bool        LoadGaoFangIpConfigByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp);
	bool       LoadDatabaseConfigByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp);
	bool   AdjustUserByHttpCmd(std::map<Lstring, Lstring> & httpParams, LSocketPtr sp);
protected:
	void		_initTickTime();
	bool		_initRedisAndActive();
	void		SendActiveSetToLM(Lint LMid);

public:
	//md5加密算法
	Lstring			Md5Hex(const void* data, Lsize size);

private:
	LTime		m_tickTimer;
	Llong		m_30SceTick;//30秒循环一次
	Llong		m_600SceTick;//5分钟循环一次


public:
	Lint        m_gateIpSelect;          //0--初始值，不对IP进行筛选  1--只返回高防45ip  2--只返回28ip  
	Lint     getGateInfoSize(Lint severID);
	inline bool GetDBMode() { return m_bDBMode; }

private:
	boost::mutex m_mutexLogicManager;
	std::map<Lint, LSocketPtr> m_mapLogicManagerSp;
	std::map<Lint, Lstring> m_mapLogicManagerName;
private:
	boost::mutex m_mutexGateInfo;
	std::map<Lint, std::map<Lint, GateInfo> > m_mapGateInfo;
	std::map<Lint, std::map<Lint, GateInfo> > m_mapAliGaoFangGateInfo;
	std::map<Lint, std::map<Lint, GateInfo> > m_mapYunmanGaoFangGateInfo;
	std::map<Lstring, Lstring> m_mapGaoFangIp;

	bool m_bDBMode;                   // 数据库主，备切换使用
private:
	DbServer        m_dbServer;   //用于charge 、onlinelog表
	DbServer        m_dbServerBK;   //用于charge 、onlinelog表
private:
	CMemeoryRecycle m_memeoryRecycle;

private:
	CRedisClient	m_RedisClient;

// 活动配置相关
private:
	Lstring			m_strActiveFreeSet;
	Lstring			m_strActivePXSet;
	Lstring			m_strActiveOUGCSet;
	Lstring         m_strActiveExchangeSet;
};


#define gWork Work::Instance()

#endif