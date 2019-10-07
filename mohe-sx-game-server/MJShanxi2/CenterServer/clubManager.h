#ifndef _CLUB_MANAGER_H_
#define _CLUB_MANAGER_H_

#include "LSingleton.h"
#include "LBase.h"
#include "LMsgS2S.h"


class ClubManager : public LSingleton<ClubManager>
{
public:
	ClubManager();
	~ClubManager();

	virtual	bool		Init();
	virtual	bool		Final();

public:
	void    resetData();

public:
	bool    loadClubData();
	bool    loadClubInfoFromDB();
	bool    loadClubPlayTypesFromDB();
	bool    loadClubUsersFromDB();
	bool    loadClubAdminAuthFromDB();   //需要加载管理员权限，在加载俱乐部信息以后

public:
	bool   sendClubDataToLogicManager(Lint serverID);
public:
	bool    addClubDataWithPlayTypesAndUsers();

public:
	void   updateClubDeskInfo(LMsgLMG2CEUpdateData *pdata);       //manager ->centen 同步俱乐部桌子信息
	void   updateClubUserPoint(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);  //每场游戏更新用户能量值
	void   updatePointRecord(LMsgLMG2CEUpdatePointRecord*msg);                   //会长或管理员设置用户能量值 
	void   updateClubInfo(LMsgCE2LMGClubInfo *msg);
	void   updatePlayInfo(LMsgCE2LMGPlayInfo *msg);
																
	bool   userRequestRecordLog(LMsgLMG2CERecordLog  *msg);      //查询记录
	bool  _loadRecordLog(LMsgLMG2CERecordLog  *msg, std::vector<UserPowerRecord>& recordList);


public:
	bool	addClub(Lint id);
	bool    addClubPlayType(Lint id);
	bool    addClubUser(Lint id);
	bool    delClubUser(Lint id, Lint clubId, Lint userId,Lint type);
	bool    hideClubPlayType(Lint id);
	bool    alterClubName(Lint id);
    bool    modifyClubFeeType(Lint id);
	bool    alterPlayTypeName(Lint id);
	bool    userApplyClub(Lint clubId);
	//计算申请该俱乐部，且还未被同意的玩家
	bool	calcApplayClubUser(Lint clubId);

	bool     Excute(const std::string& str);

	boost::shared_ptr<ClubItem> getClubItemByClubId(Lint clubId);

protected:
	bool	_loadClubInfoFromDB(Lint iClubCount);
	bool    _loadClubPlayTypesFromDB(Lint iPlayTypesCount);
	bool    _loadClubPlayUserFromDB(Lint iUserCount);
	bool    _updateClubUserPoint(Lint userId, Lint clubId, Lint point);
	bool   _updateClubUserPointSql(Lint userId, Lint clubId, Lint point);
	bool   _updateClubUserPointMemory(Lint userId, Lint clubId, Lint point);



private:
	boost::mutex m_mutexQueue;

	std::map<Lint, boost::shared_ptr<ClubItem>> m_mapId2Club;         //俱乐部  key=clubId
	std::map<Lint, boost::shared_ptr<ClubItem>> m_mapclubId2Club;         //俱乐部  key=id 
	std::list<boost::shared_ptr<clubPlayType>>  m_playTypeList;          //玩法
	std::list<boost::shared_ptr<clubUser>>      m_clubUserList;                //用户

};

#define gClubManager ClubManager::Instance()

#endif
