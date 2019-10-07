#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LMsgS2S.h"


#define CLUB_DESK_ID_BEGIN_NUMBER  1000000

//clubdeskId 对于的Club信息
struct ClubInfos
{
	Lint  m_clubId;
	Lint  m_playTypeId;
	Lint  m_clubDeskId;

	ClubInfos()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
	}
	ClubInfos(Lint clubId, Lint playTypeId, Lint clubDeskId)
	{
		m_clubId=clubId;
		m_playTypeId=playTypeId;
		m_clubDeskId=clubDeskId;
	}
};


struct DeskInfos
{
	Lint m_logicServerId;   //logicserver id
	Lint m_creditsLimit;    //房间 信用限制值，>0 有限制，否则没有信用限制
	Lint m_MHfeeType;      // 1: 公摊 2:房主
	Lint m_MHfeeCost;      // 用户费用
	Lint m_MHMaxCircle;    // 房间局数
   ClubInfos m_clubInfo;
		//...
	//其他扩展
	DeskInfos(Lint serverid = 0,Lint climit = 0):m_logicServerId(serverid),m_creditsLimit(climit)
	{
		m_MHfeeType = 1;
		m_MHfeeCost = 1;
		m_MHMaxCircle = 0;

	}
};

class DeskManager:public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();
public:
	//返回LogicServer ID
	Lint	RecycleDeskId(Lint iDeskId);

	Lint	GetFreeDeskId(Lint nLogicServerID,Lint nInCredits);

	Lint    GetDeskCreditLimit(Lint iDeskId);

	Lint	GetLogicServerIDbyDeskID(Lint nDeskID);

	void	RecycleAllDeskIdOnLogicServer(Lint nLogicServerID);

	void	RecycleAllDeskIdOnLogicServer(Lint nLogicServerID,std::vector<ClubInfos> &clubInfos);

	Lint	GetCoinsDeskSize();
	// 分配金币桌子count表示分配的数量 deskid表示返回值
	void	GetFreeCoinDeskID( int count, std::vector<Lint>& deskid );
	void	RecycleCoinsDeskId( const std::vector<int>& deskid );
	void	RecycleCoinsDeskId();
	Lint	GetShareVideoId();

private:
	boost::mutex  m_mutexDeskId;

	std::queue<Lint>		m_FreeDeskList;
	std::queue<Lint>		m_sharevideoId;
	std::map<Lint, DeskInfos>	m_mapDeskId2LogicServerId;
	std::list<Lint>			m_coinsDeskID;		// 金币桌子列表

	std::set<Lint>             m_KeepDeskIdList;      //保留的，不对外分配的桌号
	///////////////////////////////////////////////////////////////////////////

	//关于俱乐部桌子
	boost::mutex              m_mutexClubDeskId;
	std::queue<Lint>		m_FreeClubDeskList;             //俱乐部空桌子号列表，7位号
	std::set<Lint>             m_KeepClubDeskIdList;      //保留的，不对外分配的俱乐部桌号

	std::map<Lint, ClubInfos>	m_mapClubDeskId2ClubInfo;     //俱乐部桌子信息  key=clubDeskId

	//分配给俱乐部的6位桌子号
	std::set<Lint>           m_DeskIdBelongToClub;             //俱乐部真实桌子列表

	
public:
	void   initClubDeskList();
	Lint   GetFreeClubDeskIds(Lint clubId, Lint playTypeId,Lint deskIdCount, std::set<Lint>& clubDeskIds);
	bool   GetClubInfoByClubDeskId(Lint clubDeskId, ClubInfos& clubInfo);
	void   SetDeskId2LogicServerId(Lint deskId, Lint serverId, Lint creditsLimit, Lint  MHfeeType, Lint MHfeeCost, Lint MHMaxCircle);

	void  SetClubDeskIds(Lint clubId, Lint playTypeId, Lint deskIdCount, const std::vector<Lint>& clubDeskIds);

	//俱乐部打算用到的函数,一次在一个server上申请多个deskid， 这个函数暂时不用   by wyz 20171121
	Lint  GetFreeDeskIds(Lint nServerID, Lint nInCredits, Lint requestCount, std::vector<Lint> & deskIds);

	//处理分派给俱乐部的6位桌子号
	Lint	 ClubGetFreeDeskId(Lint nServerID, Lint nInCredits,Lint clubId,Lint playTypeId,Lint clubDeskId);

	bool IsBelongToClub(Lint deskId);
	//Lint	RecycleDeskIdBelongToClub(Lint iDeskId);

	//logic 发给manager同步信息
	void   SynchroLogicServerDeskInfo(std::vector<LogicDeskInfo> deskInfo, Lint serverId);

	///////////////////////////////////////////////////////////////////////////
};
#define gDeskManager DeskManager::Instance()

#endif