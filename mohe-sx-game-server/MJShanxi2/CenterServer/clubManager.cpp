#include "clubManager.h"
#include "UserManager.h"
#include "DbServer.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "LDBSession.h"
#include "InsideNet.h"
#include "Config.h"
#include "Work.h"


ClubManager::ClubManager()
{
	resetData();
}

ClubManager::~ClubManager()
{
	resetData();
}

bool ClubManager::Init()
{
	loadClubData();
	return true;
}

bool ClubManager::Final()
{
	return true;
}

void  ClubManager::resetData()
{
	m_mapId2Club.clear();
	m_playTypeList.clear();
	m_clubUserList.clear();
}

bool ClubManager::loadClubData()
{
	loadClubInfoFromDB();
	loadClubPlayTypesFromDB();
	loadClubUsersFromDB();
	loadClubAdminAuthFromDB();
	//添加俱乐部对应的玩法和玩家
	addClubDataWithPlayTypesAndUsers();

	return true;
}

bool ClubManager::loadClubInfoFromDB()
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	LLOG_ERROR("Load ClubInfo");


	std::stringstream sql;
	sql << "SELECT COUNT(*) FROM club";

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("No data from db. Error  = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	int iClubCount = atoi(*row);
	mysql_free_result(res);

	LLOG_ERROR("ClubManager::loadClubInfoFromDB() club Number = %d", iClubCount);
	if(iClubCount>0)
		return _loadClubInfoFromDB(iClubCount);

	return true;
}

bool ClubManager::loadClubPlayTypesFromDB()
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	LLOG_ERROR("Load PlayTypes");


	std::stringstream sql;
	sql << "SELECT COUNT(*) FROM club_playtypes";

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("No data from db. Error  = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	int iPlayTypesCount = atoi(*row);
	mysql_free_result(res);
	LLOG_ERROR("ClubManager::loadClubPlayTypesFromDB() PlayTypesCount = %d", iPlayTypesCount);
	if(iPlayTypesCount>0)
		return  _loadClubPlayTypesFromDB(iPlayTypesCount);

	return true;
}

bool ClubManager::loadClubUsersFromDB()
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	LLOG_ERROR("Load ClubUsers");


	std::stringstream sql;
	sql << "SELECT COUNT(*) FROM club_users";

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("No data from db. Error  = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	int iClubUserCount = atoi(*row);
	mysql_free_result(res);
	LLOG_ERROR("ClubManager::loadClubUsersFromDB() club user number = %d", iClubUserCount);
	if(iClubUserCount>0)
		return  _loadClubPlayUserFromDB(iClubUserCount);

	return true;

}

bool ClubManager::addClubDataWithPlayTypesAndUsers()
{
	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end()) 
	{
		Lint clubSq = itor->second->m_id;
		Lint clubNo = itor->second->m_clubId;
		//增加俱乐部玩法
		auto itorPlayType = m_playTypeList.begin();
		while (itorPlayType != m_playTypeList.end())
		{
		    boost::shared_ptr<clubPlayType> temp = *itorPlayType;

			if (temp->m_clubSq == clubSq)                      //找到该俱乐部对应的所有玩法
			{
				temp->m_clubId = clubNo;
				itor->second->playTypeList.push_back(temp);    //添加俱乐部的新玩法				
			}
			itorPlayType++;
		}

		//增加俱乐部用户
		auto itorUser = m_clubUserList.begin();
		while (itorUser != m_clubUserList.end())
		{
			boost::shared_ptr<clubUser> tempUser = *itorUser;
			if (tempUser->m_clubSq == clubSq)
			{
				tempUser->m_clubId = clubNo;
				itor->second->memberList.push_back(tempUser);
			}
			itorUser++;
		}
		LLOG_DEBUG("clubId=[%d],clubNO=[%d],usersize[%d],playTypeSize=[%d]", itor->second->m_id, itor->second->m_clubId, itor->second->memberList.size(), itor->second->playTypeList.size());

		itor++;
	}
	return true;
}

bool ClubManager::sendClubDataToLogicManager(Lint serverID)
{
	LLOG_DEBUG("ClubManager::sendClubDataToLogicManager ");
	auto Itor = m_mapId2Club.begin();
	while (Itor != m_mapId2Club.end())
	{
		ClubItem temp = *(Itor->second);

		bool sendToManager = true;
		std::set<Lint> sendedUserList;

		do {

			LMsgCe2LAddClub mSend;
			//俱乐部信息
			mSend.m_id = temp.m_id;
			mSend.m_clubId = temp.m_clubId;
			mSend.m_name = temp.m_name;
			mSend.m_coin = temp.m_coin;
			mSend.m_ownerId = temp.m_ownerId;
			mSend.m_userSum = temp.m_userSum;
			mSend.m_clubUrl = temp.m_clubUrl;
			mSend.m_type = temp.m_type;
			mSend.m_feeType = temp.m_feeType;
			mSend.m_createDeskCount = temp.m_createDeskCount;
			mSend.m_adminAuth = temp.m_adminAuth;
			//俱乐部桌子
			mSend.m_clubDeskList = temp.m_clubDeskList;

			//玩法
			for (auto itorPlayType = temp.playTypeList.rbegin(); itorPlayType != temp.playTypeList.rend(); itorPlayType++)
			{
				if (mSend.playTypeList.size() >= 1)break;
				if (itorPlayType->get() == NULL)continue;
				clubPlayType clubPlayType;
				if (clubPlayType.init(*itorPlayType))
				{
					mSend.playTypeList.push_back(clubPlayType);
				}

			}
			mSend.playTypeCout = mSend.playTypeList.size();


			//用户
			auto itorMember = temp.memberList.begin();
			for (; itorMember != temp.memberList.end(); itorMember++)
			{
				if (itorMember->get() == NULL)continue;
				//已发送不做处理
				if (sendedUserList.find(itorMember->get()->m_userId) != sendedUserList.end())continue;

				clubUser clubUser;
				if (clubUser.init(*itorMember))
				{
					mSend.memberList.push_back(clubUser);
					sendedUserList.insert(clubUser.m_userId);
				}
				//每次发生100个用户
				if (mSend.memberList.size() >= 100)break;
			}
			mSend.memberCount = mSend.memberList.size();

		
			gWork.SendMsgToLogic(mSend, serverID);
			LLOG_ERROR("ClubManager::sendClubDataToLogicManager send clubSq[%d] clubNO=[%d] ,userSize=[%d],playTypeSize=[%d]", mSend.m_id, mSend.m_clubId, mSend.memberList.size(), mSend.playTypeList.size());

			if (itorMember == temp.memberList.end())
			{
				sendToManager = false;
				LLOG_ERROR("ClubManager::sendClubDataToLogicManager clubSq[%d] clubNO=[%d] send user over", mSend.m_id, mSend.m_clubId);
			}
			else
			{
				LLOG_ERROR("ClubManager::sendClubDataToLogicManager clubSq[%d] clubNO=[%d] send user continue", mSend.m_id, mSend.m_clubId);
			}

		} while (sendToManager);

		Itor++;
	}
	return true;
}

//添加俱乐部
bool ClubManager::addClub(Lint id)
{
	LLOG_DEBUG("ClubManager::addClub  id=[%d]", id);
	auto itorClub=m_mapId2Club.begin();
	while (itorClub!=m_mapId2Club.end())
	{
		boost::shared_ptr<ClubItem> temp=itorClub->second;
		if (temp.get()==NULL)
		{
			itorClub++;
			continue;
		}
		if (temp->m_id==id)
		{
			LLOG_ERROR("this club is already exist:  id =[%d],clubId=[%d] ", temp->m_id,temp->m_clubId);
			return false;
		}
		itorClub++;
	}

	//不存在，新加
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}	 

	std::stringstream sql;
	sql << "select id,clubNO,name,coin,ownerId,userSum,clubUrl,status,type,feeType,tableNum FROM club ";
	sql << "where id = "<< id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::addClub sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("ClubManager::addClub: Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("ClubManager::addClub: this club not in table:  id = %d ", id);
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("ClubManager::addClub :Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());
	
	bool  isAddClub = false;
	boost::shared_ptr<ClubItem> pClub(new ClubItem());
	while (row)
	{
		pClub->m_id = atoi(*row++);
		pClub->m_clubId = atoi(*row++);
		pClub->m_name = *row++;
		pClub->m_coin = atoi(*row++);
		pClub->m_ownerId = atoi(*row++);
		pClub->m_userSum = atoi(*row++);
		pClub->m_clubUrl = *row++;
		pClub->m_status = atoi(*row++);
		pClub->m_type = atoi(*row++);
		pClub->m_feeType = atoi(*row++);
		pClub->m_createDeskCount = atoi(*row++);

		isAddClub = true;

		break;
	}

	mysql_free_result(res);

	if (isAddClub)
	{
		std::stringstream ss;
		ss << "INSERT INTO club_adminauth (clubId, ctime) VALUES (  " << pClub->m_id;
		ss << " , now() ) ";
		if (mysql_real_query(dbSession.GetMysql(), ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("ClubManager::addClub sql error: %s  Sql: %s", mysql_error(dbSession.GetMysql()), ss.str().c_str());
			return false;
		}
		LLOG_DEBUG(" club_adminauth [%s]", ss.str().c_str());
	}

	//boost::shared_ptr<ClubItem> ptrClubItem(pClub);

	boost::mutex::scoped_lock l(m_mutexQueue);
	m_mapId2Club[pClub->m_clubId] =pClub;
	m_mapclubId2Club[pClub->m_id] = pClub;

	LMsgCe2LAddClub msgSend;
	msgSend.m_id = pClub->m_id;
	msgSend.m_clubId = pClub->m_clubId;
	msgSend.m_name = pClub->m_name;
	msgSend.m_coin = pClub->m_coin;
	msgSend.m_ownerId = pClub->m_ownerId;
	msgSend.m_userSum = pClub->m_userSum;
	msgSend.m_clubUrl = pClub->m_clubUrl;
	msgSend.m_status = pClub->m_status;
	msgSend.m_type = pClub->m_type;
	msgSend.m_feeType = pClub->m_feeType;
	msgSend.m_createDeskCount = pClub->m_createDeskCount;
	msgSend.memberCount = 0;
	msgSend.playTypeCout = 0;
	Lint serverID = 16210;
	LLOG_DEBUG("ClubManager::addClub: m_id=[%d],m_clubId=[%d],m_name=[%s],m_coin=[%d],m_ownerId=[%d],m_userSum=[%d],m_clubUrl=[%s],m_status=[%d],m_type=[%d],m_feeType=[%d],table[%d]",
		msgSend.m_id, msgSend.m_clubId, msgSend.m_name.c_str(), msgSend.m_coin, msgSend.m_ownerId, msgSend.m_userSum, msgSend.m_clubUrl.c_str(), msgSend.m_status, msgSend.m_type, msgSend.m_feeType, msgSend.m_createDeskCount);
	gWork.SendMsgToLogic(msgSend, serverID);
	return true;
}

bool ClubManager::addClubPlayType(Lint id)
{
	LLOG_DEBUG("ClubManager::addClubPlayType id=[%d]",id);

	//判断是否有记录
	auto itorPlayType=m_playTypeList.begin();
	while (itorPlayType!=m_playTypeList.end())
	{
		boost::shared_ptr<clubPlayType> temp=*itorPlayType;
		if (temp.get()==NULL)
		{
			itorPlayType++;
			continue;
		}
		if (temp->m_id==id)
		{
			LLOG_ERROR("ClubManager::addClubPlayType:this clubPlayType is already in Club: playType id = %d ", id);
			return false;
		}
		itorPlayType++;
	}


	//操作数据库
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select id, clubId,playType,tableNum,status FROM club_playtypes ";
	sql << "where id = " << id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::addClubPlayType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("ClubManager::addClubPlayType Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("this clubType not in table: club id = %d ", id);
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("ClubManager::addClubPlayType Spend %d seconds to load playType from db", loadEnd.Secs() - loadBegin.Secs());

	//clubPlayType *tempPT = new clubPlayType();
	boost::shared_ptr<clubPlayType> ptrTempPT(new clubPlayType());
	while (row)
	{
		ptrTempPT->m_id = atoi(*row++);
		ptrTempPT->m_clubSq = atoi(*row++);
		ptrTempPT->m_playTypes = *row++;
		ptrTempPT->m_tableNum = atoi(*row++);
		ptrTempPT->m_status = atoi(*row++);

		break;
	}

	mysql_free_result(res);

	//保存
	m_playTypeList.push_back(ptrTempPT);

	//找到俱乐部
	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end()) 
	{
		if (itor->second->m_id ==ptrTempPT->m_clubSq)
		{
			auto itorPlayType = itor->second->playTypeList.begin();
			while (itorPlayType != itor->second->playTypeList.end())
			{
				boost::shared_ptr<clubPlayType> temp = *itorPlayType;
				if (temp->m_id == ptrTempPT->m_id)
				{
					LLOG_ERROR("this PlayType is already in this Club: clubPlayType id = %d  clubId = %d", ptrTempPT->m_id, ptrTempPT->m_clubId);
					return false;
				}
				itorPlayType++;
			}
			ptrTempPT->m_clubId = itor->second->m_clubId;
			itor->second->playTypeList.push_back(ptrTempPT);

			//通知到logicManager
			LMsgCe2LMGClubAddPlayType mSend;
			mSend.m_playInfo.init(ptrTempPT);
			Lint serverID = 16210;
			gWork.SendMsgToLogic(mSend, serverID);

			LLOG_DEBUG("ClubManager::addClubPlayType send to logic msgID = %d,m_id=[%d],m_clubId=[%d],m_playTypes=[%s],m_tableNum=[%d],m_status[%d]", mSend.m_msgId, mSend.m_playInfo.m_id, mSend.m_playInfo.m_clubId, mSend.m_playInfo.m_playTypes.c_str(), mSend.m_playInfo.m_tableNum, mSend.m_playInfo.m_status);
		
			return true;
		}
		itor++;
	}

	LLOG_ERROR("ClubManager::addClubPlayType playTypeId[%d] in clubId [%d], but not find club");
	return false;
}

//会长或管理员同意申请玩家加入俱乐部
bool ClubManager::addClubUser(Lint id)
{
	LLOG_DEBUG("ClubManager::addClubUser  id=[%d]",id);

	//数据库操作
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select id, clubId,userId,status,type,point FROM club_users ";
	sql << "where id = " << id;
	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::addClubUser sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("ClubManager::addClubUser Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("ClubManager::addClubUser Fail to fetch row");
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("ClubManager::addClubUser  Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());
	//clubUser *temp = new clubUser();
	boost::shared_ptr<clubUser> ptrTemp(new clubUser());
	while (row)
	{
		ptrTemp->m_id = atoi(*row++);
		ptrTemp->m_clubSq = atoi(*row++);
		ptrTemp->m_userId = atoi(*row++);
		ptrTemp->m_status = atoi(*row++);
		ptrTemp->m_type = atoi(*row++);
		ptrTemp->m_point = atoi(*row++);
		row = mysql_fetch_row(res);
		break;
	}
	LLOG_ERROR("ClubManager::addClubUser m_id[%d],m_clubId[%d],m_usrId[%d],m_status[%d],point[%d]", ptrTemp->m_id, ptrTemp->m_clubId, ptrTemp->m_userId, ptrTemp->m_status,ptrTemp->m_point);
	

	//添加m_clubUserList
	//auto itorUser = m_clubUserList.begin();
	//LLOG_DEBUG("ClubManager::addClubUser  m_clubUserList   size=[%d]", m_clubUserList.size());
	//while (itorUser != m_clubUserList.end())
	//{
	//	boost::shared_ptr<clubUser> temp = *itorUser;
	//	if ((*itorUser)->m_userId == ptrTemp->m_userId)
	//	{
	//		LLOG_ERROR("this clubUser is already in  Club m_clubUserList: clubUser id = %d ", ptrTemp->m_id);
	//		return false;
	//	}
	//	itorUser++;
	//}
	//m_clubUserList.push_back(ptrTemp);


	
	for (auto itorClub = m_mapId2Club.begin(); itorClub != m_mapId2Club.end(); itorClub++)
	{
		if (itorClub->second.get() == NULL)continue;
		if (itorClub->second->m_id ==ptrTemp->m_clubSq)   //找到俱乐部
		{
			//std::list<boost::shared_ptr<clubUser>> tempMemberList = itorClub->second->memberList;
			LLOG_DEBUG("ClubManager::addClubUser  tempMemberList SIZE=[%d]", itorClub->second->memberList.size());
			bool bfind = false;
			auto itorMember =itorClub->second->memberList.begin();
			while (itorMember !=itorClub->second->memberList.end())
			{
				if (itorMember->get() == NULL)continue;
				//boost::shared_ptr<clubUser> tempItem = *itorMember;
				if ((*itorMember)->m_userId==ptrTemp->m_userId )
				{
					LLOG_ERROR("this clubUser is already in this Club tempMemberList: clubUser id = %d ", ptrTemp->m_userId);
					return false;
				}
				itorMember++;
			}
			ptrTemp->m_clubId = itorClub->second->m_clubId;
			itorClub->second->memberList.push_back(ptrTemp);

			boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(ptrTemp->m_userId);
			if (safeUser.get() != NULL && safeUser->isValid())
			{
				boost::shared_ptr<DUser> user = safeUser->getUser();
				ptrTemp->m_headUrl = user->m_usert.m_headImageUrl;
				ptrTemp->m_nike = user->m_usert.m_nike;
			}
			LMsgCe2LMGClubAddUser mSend;
			mSend.m_msgType = CLUB_USER_MSG_TYPE_ADD_USER;
			mSend.m_userInfo.init(ptrTemp);

			Lint serverID = 16210;
			
			//通知到logicManager
			gWork.SendMsgToLogic(mSend, serverID);
			LLOG_DEBUG("ClubManager::addClubUser  m_id=[%d],m_clubId=[%d],m_userId=[%d],m_status=[%d],type[%d],point[%d] nike[%s]headUrl[%s]", mSend.m_userInfo.m_id, mSend.m_userInfo.m_clubId, mSend.m_userInfo.m_userId, mSend.m_userInfo.m_status, mSend.m_userInfo.m_type, mSend.m_userInfo.m_point, mSend.m_userInfo.m_nike.c_str(), mSend.m_userInfo.m_headUrl.c_str());
			return true;

		}
		
	}
	return false;
}

bool ClubManager::delClubUser(Lint id, Lint clubId, Lint userId, Lint type)
{
	LLOG_ERROR("ClubManager::delClubUser id=[%d],clubId=[%d],userId=[%d] type[%d]",id,clubId,userId,type);
	//数据库操作
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select clubId,userId,status,type FROM club_users ";
	sql << "where id = " << id;
	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::_loadClubPlayType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("this user is not in  table id = %d userId = %d ", id, userId);
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	Lint  myClubId=0;
	Lint  myUserId=0;
	Lint  myStatus=0;
	Lint  myType = 0;
	while (row)
	{
		myClubId=atoi(*row++);
		myUserId=atoi(*row++);
		myStatus=atoi(*row++);
		myType= atoi(*row++);
		break;
	}
	LLOG_DEBUG("ClubManager::delClubUser  CLUBID=[%d],userid=[%d],status=[%d],type[%d]", myClubId, myUserId, myStatus, myType);
	mysql_free_result(res);

	std::stringstream ss;
	ss<<"update  club_users  set status = 2  where id = "<<id;
	if (mysql_real_query(dbSession.GetMysql(), ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB4 sql error: %s  Sql: %s", mysql_error(dbSession.GetMysql()), ss.str().c_str());
		return false;
	}

	//删除
	//添加m_clubUserList
	//auto itorUser=m_clubUserList.begin();
	//while (itorUser!=m_clubUserList.end())
	//{
	//	boost::shared_ptr<clubUser> temp=*itorUser;
	//	
	//	if (temp->m_userId==myUserId)
	//	{
	//		LLOG_DEBUG("ClubManager::delClubUser from m_clubUserList ");
	//		m_clubUserList.erase(itorUser);
	//		break;       
	//	}
	//	itorUser++;
	//}
	//LLOG_DEBUG("ClubManager::delClubUser  m_clubUserList   size=[%d]", m_clubUserList.size());

	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end())
	{
		if (itor->second->m_id ==myClubId)   //找到俱乐部
		{
			//std::list<boost::shared_ptr<clubUser>> tempMemberList = itor->second->memberList;
			auto itorMember =itor->second->memberList.begin();
			while (itorMember !=itor->second->memberList.end())
			{
				//boost::shared_ptr<clubUser> tempItem = *itorMember;
				if ((*itorMember)->m_userId ==myUserId)
				{
					LLOG_DEBUG("ClubManager::delClubUser  clubId=[%d]  userid=[%d]   tempMemberList  size=[%d]   ", itor->second->m_id ,myUserId, itor->second->memberList.size());
					itorMember =itor->second->memberList.erase(itorMember);   //删除用户
					LLOG_DEBUG("ClubManager::delClubUser    tempMemberList  size=[%d]", itor->second->memberList.size());
					//LMsgCe2LMGClubDelUser mSend;
					//mSend.m_id = id;
					//mSend.m_clubId =itor->second->m_clubId;
					//mSend.m_userId=myUserId;
					//mSend.m_status=myStatus;
					//mSend.m_type = type;
					//mSend.m_clubName = itor->second->m_name;
					//LLOG_ERROR("ClubManager::delClubUser m_id=[%d],m_clubId=[%d],m_userId=[%d],type[%d] clubName[%s]", mSend.m_id, mSend.m_clubId, mSend.m_userId,mSend.m_clubName.c_str());
					LMsgCe2LMGClubAddUser  mSend;
					mSend.m_msgType = CLUB_USER_MSG_TYPE_DEL_USER;
					mSend.m_userInfo.m_userId = myUserId;
					mSend.m_userInfo.m_clubSq = myClubId;
					mSend.m_userInfo.m_clubId = itor->second->m_clubId;
					mSend.m_userInfo.m_status = myStatus;
					mSend.m_userInfo.m_type = myType;

					Lint serverID = 16210;
					gWork.SendMsgToLogic(mSend, serverID);
					//通知到logicManager
					return true;
				}
				itorMember++;
			}
			LLOG_ERROR("ClubManager::delClubUser the user is not in this club: clubId = %d  userId = %d", clubId, userId);
			return false;
		}
		itor++;
	}




	return false;
}

bool ClubManager::hideClubPlayType(Lint id)
{
	LLOG_DEBUG("ClubManager::hideClubPlayType  id=[%d]",id);

	//数据库操作
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select id, clubId,playType,tableNum,status FROM club_playtypes ";
	sql << "where id = " << id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::hideClubPlayType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	clubPlayType temp;
	while (row)
	{
		temp.m_id = atoi(*row++);
		temp.m_clubId = atoi(*row++);
		temp.m_playTypes = *row++;
		temp.m_tableNum = atoi(*row++);
		temp.m_status = atoi(*row++);
		break;
	}
	mysql_free_result(res);

	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end())
	{
		if (itor->second->m_id == temp.m_clubId)   
		{
			//std::list<boost::shared_ptr< clubPlayType>> tempList = itor->second->playTypeList;
			auto itorPlayType =itor->second->playTypeList.begin();
			while (itorPlayType!=itor->second->playTypeList.end())
			{   
				boost::shared_ptr<clubPlayType> tempItem = *itorPlayType;
				if (tempItem->m_id == id) 
				{
					//更新状态
					tempItem->m_status = temp.m_status;
					//通知到logicManager
					LMsgCe2LMGClubHidePlayType mSend;
					mSend.m_id = temp.m_id;
					mSend.m_clubId =itor->second->m_clubId;
					mSend.m_playTypes = temp.m_playTypes;
					mSend.m_tableNum = temp.m_tableNum;
					mSend.m_status = temp.m_status;
					Lint serverID = 16210;
					gWork.SendMsgToLogic(mSend, serverID);
					return true;
				}	
				itorPlayType++;
			}
			LLOG_ERROR("ClubManager::hideClubPlayType the playType is not in this club: clubId = %d  playType id = %d", temp.m_clubId, temp.m_id);
			break;
		}
		itor++;
	}

	LLOG_ERROR("ClubManager::hideClubPlayType the clubId is not in  club: clubId = %d ", temp.m_clubId);
	return false;
}

bool ClubManager::alterClubName(Lint id)
{
	LLOG_DEBUG("ClubManager::alterClubName  id=[%d]",id);
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select name FROM club ";
	sql << "where id = " << id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::addClub sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	//ClubItem* pClub = new ClubItem();        //??? 为什么这里申请
	Lstring strName;
	while (row)
	{
		strName = *row++;
		break;
	}

	mysql_free_result(res);

	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end()) 
	{
		//boost::shared_ptr<ClubItem> ptrClubItem = itor->second;
		if (itor->second->m_id == id) {
			itor->second->m_name = strName;
			LMsgCe2LMGClubModifyClubName msgSend;
			msgSend.m_clubId =itor->second->m_clubId;
			msgSend.m_clubName = strName;
			Lint serverID = 16210;
			gWork.SendMsgToLogic(msgSend, serverID);
			return true;
		}
		itor++;
	}
	LLOG_ERROR("ClubManager::alterClubName error  id=[%d] not find m_mapId2Club ", id);
	return false;
}

bool ClubManager::modifyClubFeeType(Lint id)
{
	LLOG_ERROR("ClubManager::ModifyClubFeeType  id=[%d]", id);
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select feeType FROM club ";
	sql << "where id = " << id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::ModifyClubFeeType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	Lint feeType = -1 ;
	while (row)
	{
		feeType =atoi( *row++);
		break;
	}

	mysql_free_result(res);

	if (feeType < 0)
	{
		LLOG_ERROR("ModifyClubFeeType id[%d]  modify feeType error!!!",id);
		return false;
	}

	auto itor = m_mapId2Club.begin();
	while (itor != m_mapId2Club.end())
	{
		if (itor->second->m_id == id) {
			itor->second->m_feeType = feeType;
			LMsgCe2LMGClubModifyClubFee msgSend;
			msgSend.m_clubId = itor->second->m_clubId;
			msgSend.m_clubFeeType = feeType;
			Lint serverID = 16210;
			LLOG_ERROR("ModifyClubFeeType,clubNo[%d] modify feeType to [%d]", msgSend.m_clubId, msgSend.m_clubFeeType);
			gWork.SendMsgToLogic(msgSend, serverID);
			return true;
		}
		itor++;
	}
	LLOG_ERROR("ClubManager::ModifyClubFeeType error  id=[%d] not find m_mapId2Club ", id);
	return false;
}

bool ClubManager::_loadClubInfoFromDB(Lint iClubCount)
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "SELECT id,clubNO,name,coin,ownerId,userSum,clubUrl,status,type,feeType,tableNum FROM club";

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::_loadClubInfoFromDB sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	int iArrayIndex = 0;
	ClubItem* pArrayClub = new ClubItem[iClubCount];
	while (row&& iArrayIndex<iClubCount)
	{
		ClubItem* club = &pArrayClub[iArrayIndex];

		club->m_id = atoi(*row++);
		club->m_clubId = atoi(*row++);
		club->m_name = *row++;
		club->m_coin = atoi(*row++);
		club->m_ownerId = atoi(*row++);
		club->m_userSum = atoi(*row++);
		club->m_clubUrl = *row++;
	    club->m_status = atoi(*row++);
		club->m_type = atoi(*row++);
		club->m_feeType = atoi(*row++);
		club->m_createDeskCount= atoi(*row++);
		iArrayIndex++;
		//LLOG_DEBUG("club data id = %d clubID = %d", club->m_id, club->m_clubId);

		row = mysql_fetch_row(res);
	}

	mysql_free_result(res);
	if (iArrayIndex == iClubCount)
	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		for (int i = 0; i < iArrayIndex; i++)
		{
			boost::shared_ptr<ClubItem> ptrClubItem(&pArrayClub[i]);
			m_mapId2Club[pArrayClub[i].m_clubId] = ptrClubItem;
			m_mapclubId2Club[pArrayClub[i].m_id] = ptrClubItem;
		}
	}
	return true;
}

bool ClubManager::_loadClubPlayTypesFromDB(Lint iPlayTypesCount)
{	
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select id, clubId,playType,tableNum,status FROM club_playtypes";
	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::_loadClubPlayType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("ClubManager::_loadClubPlayTypesFromDB: Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("Fail to fetch row");
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load PlayTypes from db", loadEnd.Secs() - loadBegin.Secs());

	while (row)
	{
		clubPlayType Temp;
		Temp.m_id = atoi(*row++);
		Temp.m_clubSq = atoi(*row++);
		Temp.m_playTypes = *row++;
		Temp.m_tableNum = atoi(*row++);
		Temp.m_status = atoi(*row++);
		if (Temp.m_status == 1)        // 0 --为隐藏玩法，1--为显示玩法
		{
			boost::shared_ptr<clubPlayType> ptrTemp(new clubPlayType(Temp));
			m_playTypeList.push_back(ptrTemp);
		}
		row = mysql_fetch_row(res);
	}
	return true;
}

bool ClubManager::_loadClubPlayUserFromDB(Lint iUserCount)
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "select id, clubId,userId,status,type,point FROM club_users";
	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::_loadClubPlayType sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("Fail to fetch row");
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	while (row)
	{
		//clubUser *temp = new clubUser();
		boost::shared_ptr<clubUser> ptrTemp(new clubUser());
		ptrTemp->m_id = atoi(*row++);
		ptrTemp->m_clubSq = atoi(*row++);
		ptrTemp->m_userId = atoi(*row++);
		ptrTemp->m_status = atoi(*row++);
		ptrTemp->m_type = atoi(*row++);
		ptrTemp->m_point = atoi(*row++);

		if (ptrTemp->m_status == 1)        // 0 为通过的用户，1已经通过用户
		{
			boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(ptrTemp->m_userId);
			if (safeUser.get() != NULL && safeUser->isValid())
			{
				boost::shared_ptr<DUser> user = safeUser->getUser();
				ptrTemp->m_nike = user->m_usert.m_nike;
				ptrTemp->m_headUrl = user->m_usert.m_headImageUrl;
			}
			else
			{
				LLOG_ERROR("_loadClubPlayUserFromDB ERROR not find userId[%d] in userInfo ", ptrTemp->m_userId);
			}
			m_clubUserList.push_back(ptrTemp);

		}
		if (ptrTemp->m_nike.empty())
		{
			LLOG_ERROR("_loadClubPlayUserFromDB userId[%d]  nike[%s]", ptrTemp->m_userId, ptrTemp->m_nike.c_str());
		}
		row = mysql_fetch_row(res);
	}
	return true;
}

bool ClubManager::userRequestRecordLog(LMsgLMG2CERecordLog  *msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("userRequestRecordLog  msg=NULL");
		return false;
	}
	Lint serverID = 16210;
	LMsgCE2LMGRecordLog logList;
	logList.m_clubId = msg->m_clubId;
	logList.m_mode = msg->m_mode;
	logList.m_type = msg->m_type;
	logList.m_userId = msg->m_userId;
	logList.m_clubSq = msg->m_clubSq;
	logList.m_searchCount = msg->m_search.size();
	logList.m_search = msg->m_search;

	if (msg->m_clubSq == 0 || msg->m_userId == 0)
	{
		logList.m_errorCode = 1;
	}
	else
	{
		if (_loadRecordLog(msg, logList.m_List))
		{
			logList.m_errorCode = 0;
			logList.m_count = logList.m_List.size();
			//标记结束
			if (logList.m_count < 10)
			{
				logList.m_HasSendAll = 1;
			}
		}
		else
		{
			logList.m_errorCode = 1;
		}
	}
	gWork.SendMsgToLogic(logList, serverID);

	LLOG_ERROR("userRequestRecordLog   user[%d] mode[%d] request  search.size[%d] clubid[%d] clubsq[%d] type[%d] record[%d] ", logList.m_userId, logList.m_mode, logList.m_search.size(), logList.m_clubId, logList.m_clubSq, logList.m_type, logList.m_List.size());
}

bool ClubManager::_loadRecordLog(LMsgLMG2CERecordLog  *msg, std::vector<UserPowerRecord>& recordList)
{
	if (msg == NULL)
	{
		LLOG_ERROR("_loadRecordLog ERROR  msg=NULL");
		return false;
	}
	if (msg->m_mode <= 0 || msg->m_mode > 3)
	{
		LLOG_ERROR("_loadRecordLog  ERROR mode[%d]",msg->m_mode);
		return false;
	}

	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("_loadRecordLog Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("_loadRecordLog  Fail to init db session");
			return false;
		}
	}

	bool isSearch = false;
	if (!msg->m_search.empty())
	{
		isSearch = true;
	}

	std::stringstream sql;
	sql << "select id,clubId,userId,type,point,operId,Time, operR FROM club_chargepoint_log ";
	sql << " where ";
	sql << " clubId ='" << msg->m_clubSq << "'";
	//普通成员查询自己的能量值变化记录
	if (msg->m_mode == 1)
	{ 
		sql << "  and userId = '" << msg->m_userId<< "'";
	}
	//管理员查看自己 或 自己操作的过的玩家的能量变化记录
	else if (msg->m_mode == 2)
	{
		sql << "  and (userId = '" << msg->m_userId << "'";
		sql << "  or operId = '" << msg->m_userId << "' )";
	}
	//俱乐部会长查看所有符合搜索条件的用户
	else if (msg->m_mode == 3 && isSearch)
	{
		sql << "  and (userId in ( ";
		for (auto t_itUser = msg->m_search.begin(); t_itUser != msg->m_search.end(); t_itUser++)
		{
			sql << "'" << *t_itUser << "'";
			if (t_itUser + 1 != msg->m_search.end())
			{
				sql << " ,";
			}
		}
		sql << "  ) or operId in (";
		for (auto t_itUser = msg->m_search.begin(); t_itUser != msg->m_search.end(); t_itUser++)
		{
			sql << " '" << *t_itUser << "'";
			if (t_itUser + 1 != msg->m_search.end())
			{
				sql << " ,";
			}
		}
		sql << " )  )";
	}

	if (msg->m_type != 0)
	{
		sql << "  and type = '" << msg->m_type << "'";
	}

	sql << "  and time < '" << msg->m_time << "'";
	sql << " order by Time desc  Limit 10";

	LLOG_ERROR("_loadRecordLog sql[%s]", sql.str().c_str());

	LTime loadBegin;
	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("_loadRecordLog sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("_loadRecordLog Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("_loadRecordLog Fail to fetch row");
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("_loadRecordLog   Spend %d seconds to load  from db", loadEnd.Secs() - loadBegin.Secs());
	
	while (row)
	{
		UserPowerRecord  log;
		log.m_id = atoi(*row++);
		log.m_clubSq = atoi(*row++);
		log.m_userId = atoi(*row++);
		log.m_type = atoi(*row++);
		log.m_point = atoi(*row++);
		log.m_operId = atoi(*row++);
		log.m_time = atoi(*row++);
		log.m_operR = *row++;
		
		recordList.push_back(log);

		row = mysql_fetch_row(res);

	}

	mysql_free_result(res);

	return true;

}



void ClubManager::updateClubDeskInfo(LMsgLMG2CEUpdateData *pdata)
{
	if (pdata==NULL)return;
	auto ItClub=m_mapId2Club.find(pdata->m_clubId);
	if (ItClub!=m_mapId2Club.end())
	{
		ItClub->second->m_clubDeskList = pdata->m_info;
	}
	else
	{
		LLOG_ERROR("ClubManager::updateClubDeskInfo  error not find clubid[%d]",pdata->m_clubId);
		return;
	}
	

}

bool ClubManager::alterPlayTypeName(Lint id)
{
	LLOG_DEBUG("ClubManager::alterPlayTypeName  id=[%d]", id);
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session");
		return false;
	}

	std::stringstream sql;
	sql << "select name FROM club_playtypes ";
	sql << "where id = " << id;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::alterPlayTypeName sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	//ClubItem* pClub = new ClubItem();        //??? 为什么这里申请
	Lstring strName;
	while (row)
	{
		strName = *row++;
		break;
	}

	mysql_free_result(res);

	auto itor = m_playTypeList.begin();
	while (itor != m_playTypeList.end())
	{
		//boost::shared_ptr<ClubItem> ptrClubItem = itor->second;
		if ((*itor)->m_id == id) {
			//(*itor)->m_name = strName;
			LMsgCe2LMGClubModifyClubName msgSend;
			msgSend.m_type = 1;

			auto itorClub = m_mapclubId2Club.find((*itor)->m_clubId);
			if (itorClub != m_mapclubId2Club.end())
			{
				msgSend.m_clubId = itorClub->second->m_clubId;
			}
			else
			{
				LLOG_ERROR("alterPlayTypeName ERROR NOT FIND clubId");
				//SendRet("{\"errorCode\":1,\"errorMsg\":\"fail\"}", sp);//成功
				return false;
			}
			msgSend.m_playTypeId = id;
			msgSend.m_clubName = strName;
			Lint serverID = 16210;
			gWork.SendMsgToLogic(msgSend, serverID);
			return true;
		}
		itor++;
	}
	LLOG_ERROR("ClubManager::alterPlayTypeName error  id=[%d] not find m_mapId2Club ", id);
	return false;
}

//玩家申请加入俱乐部
bool ClubManager::userApplyClub(Lint clubId)
{
	LLOG_DEBUG("ClubManager::userApplyClub  clubId=[%d]", clubId);

	LMsgCe2LMGClubAddUser mSend;
	mSend.m_msgType = CLUB_USER_MSG_TYPE_APPLY_ADD;
	mSend.m_userInfo.m_clubId = clubId;
	Lint serverID = 16210;

	gWork.SendMsgToLogic(mSend, serverID);
	
	LLOG_ERROR("ClubManager::userApplyClub clubId[%d] ", clubId);
	return false;
}

//计算申请该俱乐部，且还未被同意的玩家
bool ClubManager::calcApplayClubUser(Lint clubId)
{
	bool bfind = false;

	if (clubId < 0)
	{
		return false;
	}

	for (auto itorClub = m_mapId2Club.begin(); itorClub != m_mapId2Club.end(); itorClub++)
	{
		if (itorClub->second.get() == NULL)continue;

		//找到俱乐部
		if (itorClub->second->m_id == clubId)
		{
			LLOG_DEBUG("ClubManager::calcApplayClubUser()  tempMemberList SIZE=[%d]", itorClub->second->memberList.size());

			auto itorMember = itorClub->second->memberList.begin();
			while (itorMember != itorClub->second->memberList.end())
			{
				if (itorMember->get() == NULL)continue;

				if ((*itorMember)->m_status == 0)
				{
					//通知LogicManager有未处理的申请人
					return true;
				}
				itorMember++;
			}
		}
	}
	return bfind;
}

bool ClubManager::Excute(const std::string& str)
{
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR(" ClubManager::Excute :Fail to init db session sql[%s]",str.c_str());
		return false;
	}

	if (mysql_real_query(dbSession.GetMysql(), str.c_str(), str.size()))
	{
		LLOG_ERROR("ClubManager::Excute sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}
	LLOG_ERROR("ClubManager::Excute sql[%s]",str.c_str());
	return true;
}

//boost::shared_ptr<ClubItem> ClubManager::getClubItemByClubId(Lint clubId)
//{
//	auto t_iClub = m_mapId2Club.find(clubId);
//	if (t_iClub == m_mapId2Club.end())
//	{
//		return NULL;
//	}
//	return t_iClub->second;
//}

void   ClubManager::updateClubUserPoint(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg)
{
	if (msg == NULL)return;
	if (msg->m_flag != 4)return;

	for (Lint i = 0; i < msg->m_playerCount; i++)
	{
		if (msg->m_playerId[i] == 0)continue;
		_updateClubUserPoint(msg->m_playerId[i], msg->m_deskId, msg->m_score[i]);
	}

}

bool   ClubManager::_updateClubUserPoint(Lint userId, Lint clubId, Lint point)
{
	if (_updateClubUserPointSql(userId, clubId, point))
		_updateClubUserPointMemory(userId, clubId, point);
	return true;
}
bool   ClubManager::_updateClubUserPointSql(Lint userId, Lint clubId, Lint point)
{
	std::stringstream ss;
	ss << "UPDATE club_users";
	ss << " SET point = point +  '" << point << "'";
	ss << " WHERE clubId = '" << clubId << "'";
	ss << " AND userId = '" << userId << "'";

	return Excute(ss.str());
}

bool   ClubManager::_updateClubUserPointMemory(Lint userId, Lint clubId, Lint point)
{
	for (auto itorClub = m_mapId2Club.begin(); itorClub != m_mapId2Club.end(); itorClub++)
	{
		if (itorClub->second.get() == NULL)continue;
		if (itorClub->second->m_id == clubId)   //找到俱乐部
		{
			bool bfind = false;
			auto itorMember = itorClub->second->memberList.begin();
			while (itorMember != itorClub->second->memberList.end())
			{
				if (itorMember->get() == NULL)continue;
				if ((*itorMember)->m_userId == userId)
				{
					(*itorMember)->m_point += point;
					LLOG_ERROR("_updateClubUserPointMemory userId[%d] clubid[%d] point[%d]", userId, clubId, point);
				}
				itorMember++;
			}


			return true;

		}
	}
}


void  ClubManager::updateClubInfo(LMsgCE2LMGClubInfo *msg)
{
	LMsgCE2LMGClubInfo toManager;
	toManager.m_errorCode = 1;
	toManager.m_operId = msg->m_operId;

	Lint serverID = 16210;

	if (msg == NULL)
	{
		return;
	}

	// 更新club
	std::stringstream updateClub;
	updateClub << "UPDATE club";
	updateClub << " SET type =   '" << msg->m_clubType << "',";
	updateClub << "  tableNum =   '" << msg->m_tableCount << "'";
	updateClub << " WHERE id = '" << msg->m_clubSq << "'";

	LLOG_ERROR("updateClubInfo  updateclubSql[%s]", updateClub.str().c_str());

	if (!Excute(updateClub.str()))
	{
		gWork.SendMsgToLogic(toManager, serverID);
		LLOG_ERROR("updateClubInfo ERROR SQL[%s]", updateClub.str().c_str());
		return;
	}

	//更新club_adminauth  管理员权限
	std::stringstream updateAdmin;
	updateAdmin << "UPDATE club_adminauth";
	updateAdmin << " SET changePoint =   '" << msg->m_adminCanChange << "',";
	updateAdmin << "  lookupPoint =   '" << msg->m_adminCanSeeAll << "'";
	updateAdmin << " WHERE clubId = '" << msg->m_clubSq << "'";

	LLOG_ERROR("updateClubInfo  updateAdmin[%s]", updateAdmin.str().c_str());

	if (!Excute(updateAdmin.str()))
	{
		LLOG_ERROR("updateClubInfo ERROR SQL[%s]", updateAdmin.str().c_str());
		gWork.SendMsgToLogic(toManager, serverID);
		return;
	}

	toManager.m_adminCanChange = msg->m_adminCanChange;
	toManager.m_adminCanSeeAll = msg->m_adminCanSeeAll;
	toManager.m_clubType = msg->m_clubType;
	toManager.m_tableCount = msg->m_tableCount;
	toManager.m_clubId = msg->m_clubId;
	toManager.m_clubSq = msg->m_clubSq;
	toManager.m_errorCode = 0;
	gWork.SendMsgToLogic(toManager, serverID);

	return;
}

void  ClubManager::updatePlayInfo(LMsgCE2LMGPlayInfo *msg)
{

}


bool ClubManager::loadClubAdminAuthFromDB()
{
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("loadClubAdminAuthFromDB Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetClubDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("loadClubAdminAuthFromDB Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "SELECT clubId,changePoint,lookupPoint FROM club_adminauth";

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("ClubManager::loadClubAdminAuthFromDB sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("loadClubAdminAuthFromDB Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("loadClubAdminAuthFromDB  Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	std::vector<ClubAdminAuth>  authList;
	while (row)
	{
		ClubAdminAuth  auth;
		auth.m_clubSq = atoi(*row++);
		auth.m_changePoint = atoi(*row++);
		auth.m_lookupPoint = atoi(*row++);

		authList.push_back(auth);

		row = mysql_fetch_row(res);
		LLOG_DEBUG("loadClubAdminAuthFromDB clubsq[%d] m_changePoint[%d] m_lookupPoint[%d] ", auth.m_clubSq, auth.m_changePoint, auth.m_lookupPoint);
	}
	mysql_free_result(res);

	for (auto ItAuth = authList.begin(); ItAuth != authList.end(); ItAuth++)
	{
		auto itorClub = m_mapclubId2Club.find(ItAuth->m_clubSq);
		if (itorClub != m_mapclubId2Club.end())
		{
			itorClub->second->m_adminAuth = *ItAuth;
		}
	}

	return true;
}

void   ClubManager::updatePointRecord(LMsgLMG2CEUpdatePointRecord*msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("Work::updatePointRecord  msg == NULL");
		return;
	}
	LLOG_ERROR("updatePointRecord  operId[%d]   type[%d] charge clubId[%d]  userId[%d]  point[%d] currPoint[%d] r[%s]", msg->m_record.m_operId, msg->m_record.m_type, msg->m_record.m_clubId, msg->m_record.m_userId, msg->m_record.m_point, msg->m_record.m_crruPoint, msg->m_record.m_operR.c_str());

	//返回给manager 通知 充值是否成功
	LMsgLMG2CEUpdatePointRecord mSend;
	mSend.m_msgType = 1;

	//if (msg->m_record.m_clubId == 0 || msg->m_record.m_userId == 0 || msg->m_record.m_point == 0||)

	//写入数据库 update club_users point
	Lint currPoint = msg->m_record.m_crruPoint;
	Lint changePoint = 0;
	std::stringstream sql;
	if (msg->m_record.m_type == CHANGE_POINT_TYPE_ADD)
	{
		sql << "UPDATE club_users";
		sql << " SET point =point + '" << msg->m_record.m_point << "'";
		sql << " WHERE clubId = '" << msg->m_record.m_clubSq << "'";
		sql << " AND userId = '" << msg->m_record.m_userId << "'";

		currPoint += msg->m_record.m_point;
		changePoint = msg->m_record.m_point;
	}
	else if (msg->m_record.m_type == CHANGE_POINT_TYPE_REDUCE)
	{
		sql << "UPDATE club_users";
		sql << " SET point =point - '" << msg->m_record.m_point << "'";
		sql << " WHERE clubId = '" << msg->m_record.m_clubSq << "'";
		sql << " AND userId = '" << msg->m_record.m_userId << "'";

		currPoint -= msg->m_record.m_point;
		changePoint = (-1)*msg->m_record.m_point;
	}
	LLOG_ERROR("updatePointRecord update club_users sql[%s]", sql.str().c_str());
	bool  ret = gClubManager.Excute(sql.str());                 //充值  

	if (ret)
	{
		//记录point_log
		std::stringstream logSql;
		logSql << "INSERT INTO club_chargepoint_log (clubId,userId,type,point,currPoint,operId,operR,Time,ctime,utime) VALUES (";
		logSql << "'" << msg->m_record.m_clubSq << "',";
		logSql << "'" << msg->m_record.m_userId << "',";
		logSql << "'" << msg->m_record.m_type << "',";
		logSql << "'" << msg->m_record.m_point << "',";
		logSql << "'" << currPoint << "',";
		logSql << "'" << msg->m_record.m_operId << "',";
		logSql << "'" << msg->m_record.m_operR << "',";
		logSql << "'" << msg->m_record.m_time << "',";
		logSql << " now(),now())";

		LLOG_ERROR("updatePointRecord club_chargepoint_log sql[%s]", logSql.str().c_str());
		ret = gClubManager.Excute(logSql.str());
	}
	else
	{
		//充值失败
		mSend.m_errorCode = 1;
	}

	mSend.m_record = msg->m_record;
	Lint serverID = 16210;

	gWork.SendMsgToLogic(mSend, serverID);

	if (mSend.m_errorCode == 0)
		_updateClubUserPointMemory(msg->m_record.m_userId, msg->m_record.m_clubSq, changePoint);

}
