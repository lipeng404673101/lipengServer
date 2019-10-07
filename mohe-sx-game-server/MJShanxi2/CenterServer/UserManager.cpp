#include "UserManager.h"
#include "DbServer.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "LDBSession.h"
#include "InsideNet.h"
#include "Config.h"

#include <openssl/md5.h>


#define LOADUSER_COUNT			500000
#define LOADUSER_THREAD			16
#define LOADUSER_ALLOC_SIZE		50000

#define FreeStartTime   "FreeStartTime"
#define FreeEndTime     "FreeEndTime"

UserManager::UserManager()
{
	m_bLoadUserSuccess = false;
}

UserManager::~UserManager()
{

}

bool UserManager::Init()
{
	_loadUserInfoFromDB();

	//_loadTestData();
	return true;
}

void UserManager::addUser(boost::shared_ptr<DUser> user)
{
	if(user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexQueue);

	_addUser(user);
}

void UserManager::delUser(const Lstring& strUserId)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	auto itUser = m_mapUUID2User.find(strUserId);
	if(itUser != m_mapUUID2User.end())
	{
		m_mapId2User.erase(itUser->second->m_usert.m_id);
		m_mapUUID2User.erase(itUser);
	}
}

void UserManager::delUser(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	auto itUser = m_mapId2User.find(iUserId);
	if(itUser != m_mapId2User.end())
	{
		m_mapUUID2User.erase(itUser->second->m_usert.m_unioid);
		m_mapId2User.erase(itUser);
	}
}

boost::shared_ptr<CSafeUser> UserManager::getUser(const Lstring& strUserId)
{
	boost::shared_ptr<DUser> user;

	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		auto itUser = m_mapUUID2User.find(strUserId);
		if(itUser != m_mapUUID2User.end())
		{
			user = itUser->second;
		}
	}
	
	boost::shared_ptr<CSafeUser> safeUser;
	if(user)
	{
		safeUser.reset(new CSafeUser(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeUser> UserManager::getUserbyMd5(const Lstring& strMd5)
{
	boost::shared_ptr<DUser> user;

	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		auto itUser = m_mapMD5User.find(strMd5);
		if (itUser != m_mapMD5User.end())
		{
			user = itUser->second;
		}
	}

	boost::shared_ptr<CSafeUser> safeUser;
	if (user)
	{
		safeUser.reset(new CSafeUser(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeUser> UserManager::getUser(Lint iUserId)
{
	boost::shared_ptr<DUser> user;

	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		auto itUser = m_mapId2User.find(iUserId);
		if(itUser != m_mapId2User.end())
		{
			user = itUser->second;
		}
	}

	boost::shared_ptr<CSafeUser> safeUser;
	if(user)
	{
		safeUser.reset(new CSafeUser(user));
	}

	return safeUser;
}

void UserManager::LoadUserIdInfo(Lint serverID)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	LTime cur;

	int iUserCount = 0;
 	auto it = m_mapId2User.begin();
	if(it != m_mapId2User.end())
	{
		while(it != m_mapId2User.end())
		{
			int iCount = 0;
			LMsgCe2LUserIdInfo send;
			for(; it != m_mapId2User.end() && iCount < 50; it++)
			{
				UserIdInfo info;
				info.m_id = it->second->m_usert.m_id;
				info.m_nike = it->second->m_usert.m_nike;
				info.m_headImageUrl = it->second->m_usert.m_headImageUrl;
				info.m_openId = it->second->m_usert.m_openid;
				info.m_unionId = it->second->m_usert.m_unioid;
				info.m_sex = it->second->m_usert.m_sex;
				send.m_info.push_back(info);

				iCount++;
				iUserCount++;
			}

			send.m_count = iCount;
			send.m_hasSentAll = iUserCount == m_mapId2User.size() ? 1 : 0;

			gWork.SendMsgToLogic(send, serverID, 2000);
		}
	}
	else
	{
		LMsgCe2LUserIdInfo send;
		send.m_count = 0;
		send.m_hasSentAll = 1;

		gWork.SendMsgToLogic(send, serverID);
	}

	LTime now;
	LLOG_ERROR("Complete load user info. time = %ld, UserCount = %d", now.Secs() - cur.Secs(), iUserCount);
}

Lstring generateKey(int iLen)
{
	static Lstring strSeed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(){}[];,.?|";
	Lstring strKey;
	for(int i = 0; i < iLen; i++)
	{
		int iRand = L_Rand(0, strSeed.length() - 1);
		strKey += strSeed.at(iRand);
	}

	return strKey;
}

void UserManager::_loadTestData()
{
	std::map<std::string, DUser*> mapUser;
	for(int i = 0; i < 2000000; i++)
	{
		boost::shared_ptr<DUser> user(new DUser);
		user->m_usert.m_id = i;
		user->m_usert.m_openid = generateKey(32);
		user->m_usert.m_nike = generateKey(20);
		user->m_usert.m_sex = 0;
		user->m_usert.m_provice = "beijing";
		user->m_usert.m_city = "beijing";
		user->m_usert.m_country = "china";
		user->m_usert.m_headImageUrl = "";
		user->m_usert.m_unioid = user->m_usert.m_openid;
		user->m_usert.m_numOfCard1s = 0;
		user->m_usert.m_numOfCard2s = 50;
		user->m_usert.m_numOfCard3s = 0;
		user->m_usert.m_lastLoginTime = 5643131;
		user->m_usert.m_regTime = 5643131;
		user->m_usert.m_new = 0;
		user->m_usert.m_gm = 0;
		user->m_usert.m_totalbuynum = 0;
		user->m_usert.m_totalplaynum = 0;
		user->m_usert.m_customInt[0] = 0;
		user->m_usert.m_customInt[1] = 0;
		_addUser(user);

		if(mapUser.size() < 40000)
		{
			mapUser[user->m_usert.m_unioid] = user.get();
		}
	}

// 	LTime cur;
// 
// 	LTime now;
// 	LLOG_ERROR("miao %ld", now.MSecs() - cur.MSecs());

	//插入消息
	for(int i = 0; i < 4; i++)
	for(auto it = mapUser.begin(); it != mapUser.end(); it++)
	{

		LMsgC2SMsg* pMsg = new LMsgC2SMsg;

		pMsg->m_openId = it->second->m_usert.m_openid;
		pMsg->m_nike	   = it->second->m_usert.m_nike;
		pMsg->m_sign	   = "";
		pMsg->m_plate  = "wechat";
		pMsg->m_accessToken = "";
		pMsg->m_refreshToken = "";
		pMsg->m_md5		= "";
		pMsg->m_severID	= 15001;
		pMsg->m_uuid		= it->second->m_usert.m_unioid;
		pMsg->m_sex		= it->second->m_usert.m_sex;
		pMsg->m_imageUrl= it->second->m_usert.m_headImageUrl;
		pMsg->m_nikename= it->second->m_usert.m_nike;


		LMsgFromLoginServer* pTest = new LMsgFromLoginServer;
		pTest->m_userMsgId = MSG_C_2_S_MSG;
		pTest->m_userMsg   = pMsg;

		gWork.Push(pTest);
	}
}

bool UserManager::_loadUserInfoFromDB()
{
	LDBSession dbSession;

	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			return false;
		}
	}

	std::stringstream sql;
	sql << "SELECT COUNT(*) FROM user";

	if(mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if(res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
	{
		LLOG_ERROR("No data from db. Error  = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	int iUserCount = atoi(*row);
	mysql_free_result(res);

	LLOG_ERROR("%d users will be loaded", iUserCount);

	int iUserCountTmp = iUserCount;
	int iUserCountOnThread[LOADUSER_THREAD] = {0};
	int iNeedThread = iUserCountTmp / LOADUSER_COUNT;
	if(iNeedThread < LOADUSER_THREAD)
	{
		int iIndex = 0;
		while(iUserCountTmp > 0 && iIndex < LOADUSER_THREAD)
		{
			iUserCountOnThread[iIndex] = iUserCountTmp > LOADUSER_COUNT ? LOADUSER_COUNT : iUserCountTmp;
			iIndex++;
			iUserCountTmp = iUserCountTmp - LOADUSER_COUNT;
		}
	}
	else
	{
		int iAverageCount = iUserCountTmp / LOADUSER_THREAD;
		int iLastRemain = iUserCountTmp - (iAverageCount * LOADUSER_THREAD);
		for(int i = 0; i < LOADUSER_THREAD; i++)
		{
			iUserCountOnThread[i] = iAverageCount;
		}
		iUserCountOnThread[LOADUSER_THREAD - 1] += iLastRemain;
	}

	//start to load user
	m_bLoadUserSuccess = true;
	boost::shared_ptr<boost::thread> loadThread[LOADUSER_THREAD];
	int iBeginIndex = 0;
	for(int i = 0; i < LOADUSER_THREAD; i++)
	{
		if(iUserCountOnThread[i] == 0)
		{
			break;
		}

		loadThread[i] = boost::shared_ptr<boost::thread> (new boost::thread(boost::bind(&UserManager::_loadUserInfoFromDB,this, iBeginIndex, iUserCountOnThread[i])));
		iBeginIndex += iUserCountOnThread[i];
	}

	for(int i = 0; i < LOADUSER_THREAD; i++)
	{
		if(loadThread[i])
		{
			loadThread[i]->join();
		}
	}
	
	if(!m_bLoadUserSuccess)
	{
		LLOG_ERROR("Fail to load user");
		return false;
	}

	LLOG_ERROR("Load users successfuly. User count %d:%d", iUserCount, m_mapUUID2User.size());

	return true;
}

void UserManager::_loadUserInfoFromDB(int iBeginIndex, int iCount)
{
	if(iBeginIndex < 0 || iCount <= 0)
	{
		LLOG_ERROR("The value is wrong. BeginIndex = %d, Count = %d", iBeginIndex, iCount);
		return;
	}

	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session. BeginIndex = %d, Count = %d", iBeginIndex, iCount);
			m_bLoadUserSuccess = false;
			return;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session. BeginIndex = %d, Count = %d", iBeginIndex, iCount);
			m_bLoadUserSuccess = false;
			return;
		}
	}	 

	LLOG_ERROR("Load user from %d. Count = %d", iBeginIndex, iCount);

	int iRemainUserCount = iCount;
	while(iRemainUserCount > 0)
	{
		int iLoadCount = iRemainUserCount > LOADUSER_COUNT ? LOADUSER_COUNT : iRemainUserCount;
		std::stringstream sql;//, Mobile, Block
		sql << "select Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,TotalCircleNum, TotalWinNum,Coins,Credits,IsGiven,userIp,userGps,Block,ChangeStatus,phoneUUID,userIdTempIn,userIdTempOut,loginDevice FROM user ORDER BY Id DESC LIMIT ";
		sql << iBeginIndex << "," << iLoadCount;

		LTime loadBegin;

		if(mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(dbSession.GetMysql()));
			m_bLoadUserSuccess = false;
			break;
		}

		MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
		if(res == NULL)
		{
			LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
			m_bLoadUserSuccess = false;
			break;
		}

		MYSQL_ROW row = mysql_fetch_row(res);
		if(!row)
		{
			mysql_free_result(res);
			break;
		}

		LTime loadEnd;
		LLOG_ERROR("Spend %d seconds to load user from db(%d:%d)", loadEnd.Secs() - loadBegin.Secs(), iBeginIndex, iLoadCount);

		iRemainUserCount = iRemainUserCount - iLoadCount;

		//Start to fill user info
		int iRemainLoadCount = iLoadCount; 
		
		int iArraySize = iRemainLoadCount > LOADUSER_ALLOC_SIZE ? LOADUSER_ALLOC_SIZE : iRemainLoadCount;
		int iArrayIndex = 0;
		DUser* pArrayUser = new DUser[iArraySize];
		while(row && iRemainLoadCount > 0)
		{
			++iBeginIndex;

			DUser* user = &pArrayUser[iArrayIndex];

			user->m_usert.m_id = atoi(*row++);
			user->m_usert.m_openid = *row++;
			user->m_usert.m_nike = *row++;
			user->m_usert.m_sex = atoi(*row++);
			user->m_usert.m_provice = *row++;
			user->m_usert.m_city = *row++;
			user->m_usert.m_country = *row++;
			user->m_usert.m_headImageUrl = *row++;
			user->m_usert.m_unioid = *row++;
			user->m_usert.m_numOfCard1s = atoi(*row++);
			user->m_usert.m_numOfCard2s = atoi(*row++);
			user->m_usert.m_numOfCard3s = atoi(*row++);
			user->m_usert.m_lastLoginTime = atoi(*row++);
			user->m_usert.m_regTime = atoi(*row++);
			user->m_usert.m_new = atoi(*row++);
			user->m_usert.m_gm = atoi(*row++);
			user->m_usert.m_totalbuynum = atoi(*row++);
			user->m_usert.m_totalplaynum = atoi(*row++);
			user->m_usert.m_customInt[0] = atoi(*row++);
			user->m_usert.m_customInt[1] = atoi(*row++);
			user->m_usert.m_coins = atoi(*row++);
			user->m_usert.m_creditValue = atoi(*row++);
			user->m_usert.m_customInt[2] = atoi(*row++);
			user->m_usert.m_customString1 = *row++;
			user->m_usert.m_customString2 = *row++;
			//user->m_usert.m_customString3 = *row++;  
			//user->m_usert.m_customInt[3] = atoi(*row++);
			user->m_usert.m_customInt[3] = atoi(*row++);
			user->m_usert.m_customInt[7] = atoi(*row++);
			user->m_usert.m_customString4= *row++;
			user->m_usert.m_customString5= *row++;
			user->m_usert.m_customString6= *row++;
			user->m_usert.m_customString7= *row++;
			iArrayIndex++;

			if(iArrayIndex == iArraySize)
			{
				boost::mutex::scoped_lock l(m_mutexQueue);
				for(int i = 0; i < iArrayIndex; i++)
				{
					
					if (!(pArrayUser[i].m_usert.m_customString4.empty()))
					{
						std::vector<Lstring>  tempPhoneUUIDList;
						L_ParseString(pArrayUser[i].m_usert.m_customString4, tempPhoneUUIDList, ",");
						//LLOG_ERROR("userId[%d] phoneUUID is not empty [%s] ,size[%d]", pArrayUser[i].m_usert.m_id, pArrayUser[i].m_usert.m_customString4.c_str(), tempPhoneUUIDList.size());
						if (tempPhoneUUIDList.size() != 0)
							pArrayUser[i].m_usert.m_customString8 = tempPhoneUUIDList[tempPhoneUUIDList.size() - 1];
					}

					boost::shared_ptr<DUser> ptrUser(&pArrayUser[i]);

					m_mapUUID2User[pArrayUser[i].m_usert.m_unioid] = ptrUser;
					m_mapId2User[pArrayUser[i].m_usert.m_id] = ptrUser;
				}

				iRemainLoadCount = iRemainLoadCount - iArraySize;
				iArrayIndex = 0;

				if(iRemainLoadCount > 0)
				{
					iArraySize = iRemainLoadCount > LOADUSER_ALLOC_SIZE ? LOADUSER_ALLOC_SIZE : iRemainLoadCount;
					pArrayUser = new DUser[iArraySize];
				}
			}

			row = mysql_fetch_row(res);
		}

		mysql_free_result(res);

		for(int i = 0; i < iArrayIndex; i++)
		{
			if (!(pArrayUser[i].m_usert.m_customString4.empty()))
			{
				std::vector<Lstring>  tempPhoneUUIDList;
				L_ParseString(pArrayUser[i].m_usert.m_customString4, tempPhoneUUIDList, ",");
				//LLOG_ERROR("userId[%d] phoneUUID is not empty [%s] ,size[%d]", pArrayUser[i].m_usert.m_id, pArrayUser[i].m_usert.m_customString4.c_str(), tempPhoneUUIDList.size());
				if (tempPhoneUUIDList.size() != 0)
					pArrayUser[i].m_usert.m_customString8 = tempPhoneUUIDList[tempPhoneUUIDList.size() - 1];
			}

			boost::shared_ptr<DUser> ptrUser(&pArrayUser[i]);
			m_mapUUID2User[pArrayUser[i].m_usert.m_unioid] = ptrUser;
			m_mapId2User[pArrayUser[i].m_usert.m_id] = ptrUser;
		}
	}
}

bool UserManager::loadUserInfoFromDBByUserId(Lint userId, Lint& goldNum, Lstring& userIp, Lstring& userGps, Lint& blocked)
{
	bool m_bLoadUserSuccess = false;


	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			m_bLoadUserSuccess = false;
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			m_bLoadUserSuccess = false;
			return false;
		}
	}


	std::stringstream sql;
	sql << "select NumsCard2, userIp, userGps,Block FROM user ";
	sql << "where Id = " << userId;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(dbSession.GetMysql()));
		m_bLoadUserSuccess = false;
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		m_bLoadUserSuccess = false;
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

	//DUser* pUser = new DUser();
	Lint coinNum = 0;
	Lstring userIP;
	Lstring userGPS;
	while (row)
	{
		coinNum = atoi(*row++);
		userIP = *row++;
		userGPS = *row++;
		blocked = atoi(*row++);
		break;
	}
	mysql_free_result(res);
	goldNum = coinNum;
	userIp = userIP;
	userGps = userGPS;
	return true;
}

bool UserManager::loadUserInfoFromDBByUserId(Lint userId, Lint& goldNum, Lstring& userIp, Lstring& userGps, Lint& blocked, Lint &numsDiamond)
{
	bool m_bLoadUserSuccess=false;
	LDBSession dbSession;
	if (gWork.GetDBMode())
	{
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			m_bLoadUserSuccess = false;
			return false;
		}
	}
	else
	{
		if (!dbSession.Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort()))
		{
			LLOG_ERROR("Fail to init db session");
			m_bLoadUserSuccess = false;
			return false;
		}
	}

	std::stringstream sql;
	sql<<"select NumsCard2, userIp, userGps,Block,NumsDiamond FROM user ";
	sql<<"where Id = "<<userId;

	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(dbSession.GetMysql()));
		m_bLoadUserSuccess=false;
		return false;
	}

	MYSQL_RES* res=mysql_store_result(dbSession.GetMysql());
	if (res==NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		m_bLoadUserSuccess=false;
		return false;
	}

	MYSQL_ROW row=mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("Fail to fetch row");
		mysql_free_result(res);
		return false;
	}

	LTime loadEnd;
	LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs()-loadBegin.Secs());

	Lint coinNum=0;
	Lstring userIP;
	Lstring userGPS;
	Lint   NumDiamond;
	while (row)
	{
		coinNum=atoi(*row++);
		userIP=*row++;
		userGPS=*row++;
		blocked=atoi(*row++);
		NumDiamond=atoi(*row++);
		break;
	}
	mysql_free_result(res);
	goldNum=coinNum;
	userIp=userIP;
	userGps=userGPS;
	numsDiamond=NumDiamond;
	return true;
}

//Lint  UserManager::CheckInFreeTime()
//{
//	LTime cur;
//	Lint curTime = cur.Secs();
//	CRedisClient	tempRedisClient;
//
//	if (!tempRedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
//	{
//		return 0;
//	}
//
//	Lint nTimeStart = 0;
//	Lint nTimeEnd = 0;
//	if (!tempRedisClient.readDataByString(FreeStartTime, nTimeStart))
//	{
//		LLOG_ERROR("UserManager::CheckInFreeTime FreeStartTime get from redis is failed, now set is empty");
//		return 0;
//	}
//
//	if (!tempRedisClient.readDataByString(FreeEndTime, nTimeEnd))
//	{
//		LLOG_ERROR("UserManager::CheckInFreeTime FreeEndTime get from redis is failed, now set is empty");
//		return 0;
//	}
//
//	if (curTime < nTimeStart || curTime > nTimeEnd) {
//		return 0;
//	}
//
//	return 1;
//}

//bool  UserManager::GetFreeTime(Lint& nStart, Lint& nEnd)
//{
//	CRedisClient	tempRedisClient;
//
//	if (!tempRedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
//	{
//		return 0;
//	}
//
//	if (!tempRedisClient.readDataByString(FreeStartTime, nStart))
//	{
//		LLOG_ERROR("UserManager::GetFreeTime FreeStartTime get from redis is failed, now set is empty");
//		return 0;
//	}
//
//	if (!tempRedisClient.readDataByString(FreeEndTime, nEnd))
//	{
//		LLOG_ERROR("UserManager::GetFreeTime FreeEndTime get from redis is failed, now set is empty");
//		return 0;
//	}
//}

void UserManager::_addUser(boost::shared_ptr<DUser> user)
{
	auto itUser = m_mapId2User.find(user->m_usert.m_id);
	if(itUser == m_mapId2User.end())
	{
		m_mapId2User[user->m_usert.m_id] = user;
		m_mapUUID2User[user->m_usert.m_unioid] = user;
	}
}

bool UserManager::Final()
{
	return true;
}

void UserManager::SaveUser(DUser* user)
{
	//查询数据库
	std::stringstream ss;

	ss << "UPDATE user SET ";
	ss << "OpenId='" << user->m_usert.m_openid << "',";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	//ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
	ss << "TotalCircleNum='" << user->m_usert.m_customInt[0] << "',";
	ss << "TotalWinNum='" << user->m_usert.m_customInt[1] << "',";
	//ss << "Coins='"<<  user->m_usert.m_coins<<"',";
	ss << "Credits='" << user->m_usert.m_creditValue<<"'";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	//LLOG_DEBUG("UserManager::SaveUser sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}


void UserManager::UpdateUserNumsCard2(Lint userId, Lint count)
{
	//更新数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "NumsCard2=";
	ss << "(SELECT CASE WHEN NumsCard2 >= " << count;
	ss << " THEN NumsCard2 -"<< count << " ELSE 0 END as Card)" ;
	//ss<< " NumsCard2 + "<< count << "',";
	//ss << "TotalCardNum='" << "TotalCardNum + "<< count << "'";
	ss << " WHERE Id='" << userId << "'";
	LLOG_ERROR("UpdateUserNumsCard2:: sql =%s", ss.str().c_str());
	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql, userId);

}

void UserManager::SaveUserLastLogin(DUser* user)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "OpenId='" << user->m_usert.m_openid << "',";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	//ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
	ss << "TotalCircleNum='" << user->m_usert.m_customInt[0] << "',";
	ss << "TotalWinNum='" << user->m_usert.m_customInt[1] << "',";
	//ss << "Coins='"<<  user->m_usert.m_coins<<"',";
	ss << "Credits='" << user->m_usert.m_creditValue << "'";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	//LLOG_DEBUG("UserManager::SaveUserLastLogin sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}

void UserManager::SaveUserLastGPS(DUser* user)
{
	//存储数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "userGps='" << user->m_usert.m_customString2 << "'";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	//LLOG_DEBUG("UserManager::SaveUserLastLogin sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql, user->m_usert.m_id);
}


void UserManager::SaveCardInfo(DUser* user,Lint cardType, Lint count, Lint operType, const Lstring& admin)
{
	if ( user == NULL )
	{
		return;
	}
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin,TotalCardNum) VALUES (";
	ss << "'" << gWork.GetCurTime().Secs() << "',";
	ss << "'" << cardType << "',";
	ss << "'" << count << "',";
	ss << "'" << operType << "',";
	ss << "'" << user->m_usert.m_id << "',";
	ss << "'" << admin << "',";
	ss << "'" << user->m_usert.m_numOfCard2s << "')";

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}



void UserManager::SaveCardInfo(Lint userId, Lint cardType, Lint count, Lint operType, const Lstring& admin)
{
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin,TotalCardNum) VALUES (";
	ss << "'" << gWork.GetCurTime().Secs() << "',";
	ss << "'" << cardType << "',";
	ss << "'" << count << "',";
	ss << "'" << operType << "',";
	ss << "'" << userId << "',";
	ss << "'" << admin << "',";
	//ss << "'" << user->m_usert.m_numOfCard2s << "')";
	ss << "'" << 0 << "')";

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql, userId);
}

//俱乐部钻石券
void UserManager::UpdateClubCreatorNumDiamond(Lint userId, Lint nFeeValue)
{
	//更新数据库
	if (nFeeValue<0)
	{
		LLOG_ERROR("UserManager::UpdateClubCreatorNumDiamond  nFeeValue[%d] can't < 0", nFeeValue);
		return;
	}
	//更新数据库
	std::stringstream ss;
	ss<<"UPDATE user SET ";
	ss<<"NumsDiamond=NumsDiamond-"<<nFeeValue;
	ss<<" WHERE Id='"<<userId<<"'";
	Lstring* sql=new Lstring(ss.str());
	LLOG_ERROR("UserManager::UpdateClubCreatorNumDiamond  sql=[%s]", sql->c_str());
	gDbServerManager.Push(sql, userId);

}

Lstring UserManager::Md5Hex(const void* src, Lsize len)
{
	unsigned char sign[17];
	memset(sign, 0, sizeof(sign));

	MD5((const unsigned char*)src, len, sign);

	//转换成16进制
	char signHex[33];
	memset(signHex, 0, sizeof(signHex));
	for (int i = 0; i < 16; ++i)
	{
		sprintf(signHex + i * 2, "%02x", sign[i]);
	}

	return std::string(signHex);
}

Lstring UserManager::getMd5User(Lstring nick, Lstring headUrl, Lint sex)
{
	std::stringstream ss;
	ss << nick << headUrl << sex;

	return Md5Hex(ss.str().c_str(),ss.str().size());

}

bool   UserManager::addMd5User(Lstring nick, Lstring headUrl, Lint sex, boost::shared_ptr<DUser>)
{
	

}


Lint UserManager::getUserIdByPhone(Lstring phone)
{
	if (phone.empty())
	{
		LLOG_ERROR("getUserIdByPhone phone is empty.");
		return 0;
	}
	Lint userId = 0;
	bool continueAgency = true;
	//代理寻找 agency
	{
		LDBSession dbSession;
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("getUserIdByPhone init error phone[%s]", phone.c_str());
			return 0;
		}

		std::stringstream sql;
		sql << "select user_id  FROM  agency ";
		sql << "where phone =" << phone;

		LTime loadBegin;

		if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
		{
			LLOG_ERROR("getUserIdByPhone agency sql error %s", mysql_error(dbSession.GetMysql()));
			//return 0;
			continueAgency = false;
		}
		if (continueAgency)
		{
			MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
			if (res == NULL)
			{
				LLOG_ERROR("getUserIdByPhone agency Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
				continueAgency = false;
				//return 0;
			}
			if (continueAgency)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				if (!row)
				{
					LLOG_ERROR("getUserIdByPhone  agency no this phone");
					mysql_free_result(res);
					continueAgency = false;
					//return 0;
				}
				if (continueAgency)
				{
					//LTime loadEnd;
					//LLOG_ERROR("Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());
					while (row)
					{
						userId = atoi(*row++);
						break;
					}
					mysql_free_result(res);
				}
			}
		}
	}

	//用户处寻找 user
	if(userId ==0)
	{
		LDBSession dbSession;
		if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
		{
			LLOG_ERROR("getUserIdByPhone user init error phone[%s]", phone.c_str());
			return 0;
		}

		std::stringstream sql;
		sql << "select Id  FROM  user ";
		sql << "where Mobile =" << phone;

		LTime loadBegin;

		if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
		{
			LLOG_ERROR("getUserIdByPhone user sql error %s", mysql_error(dbSession.GetMysql()));
			return 0;
		}

		MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
		if (res == NULL)
		{
			LLOG_ERROR("getUserIdByPhone user  Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
			return 0;
		}

		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			LLOG_ERROR("getUserIdByPhone user Fail to fetch row");
			mysql_free_result(res);
			return 0;
		}

		LTime loadEnd;
		LLOG_ERROR("getUserIdByPhone user Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());


		while (row)
		{
			userId = atoi(*row++);
			break;
		}
		mysql_free_result(res);
	}

	return userId;
}

Lint UserManager::checkUserPhoneVcode(Lstring phone, Lstring vcode)
{
	if (phone.empty() || vcode.empty())
	{
		LLOG_ERROR("checkUserPhoneVcode phone or vcode  is empty.");
		return PHONE_LOGIN_ERROR_VCODE_FIAL;
	}

	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("checkUserPhoneVcode init error phone[%s]", phone.c_str());
		return PHONE_LOGIN_ERROR_VCODE_FIAL;
	}

	std::stringstream sql;
	sql << "select code, expire  FROM  captcha ";
	sql << "where mobile =" << phone;
	sql << "  and  type = 'userlogin' ";


	LTime loadBegin;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("checkUserPhoneVcode sql error %s  sql[%s]", mysql_error(dbSession.GetMysql()), sql.str().c_str());
		return PHONE_LOGIN_ERROR_VCODE_FIAL;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("checkUserPhoneVcode  Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return PHONE_LOGIN_ERROR_VCODE_FIAL;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("checkUserPhoneVcode  Fail to fetch row");
		mysql_free_result(res);
		return PHONE_LOGIN_ERROR_VCODE_FIAL;
	}

	LTime loadEnd;
	LLOG_ERROR("checkUserPhoneVcode  Spend %d seconds to load user from db", loadEnd.Secs() - loadBegin.Secs());

	Lstring code;
	Lint expire = 0;
	while (row)
	{
		code = *row++;
		expire = atoi(*row++);
		break;
	}
	mysql_free_result(res);

	LLOG_ERROR("checkUserPhoneVcode phone[%s] expire[%d]  code[%s] msg->code[%s] now[%d] ",phone.c_str(), expire,code.c_str(),vcode.c_str(),time(NULL));
	
	if (time(NULL) > expire)
	{
		LLOG_ERROR("checkUserPhoneVcode phone[%s]  expire[%d] < now[%d]", phone.c_str(),expire, time(NULL));
		return PHONE_LOGIN_ERROR_VCODE_EXPIRE;
	}
	
	//验证通过
	if (code.compare(vcode) == 0)
	{
		return 0;
	}

	return PHONE_LOGIN_ERROR_VCODE_FIAL;
}

Lint  UserManager::getPhoneTime(Lstring phone, bool isAdd)
{
	LLOG_DEBUG("getPhoneTime phone [%s] ",phone.c_str());
	if (phone.empty())
	{
		return 0;
	}
	auto ItBeforeTime = m_mapPhone2Time.find(phone);
	if (ItBeforeTime != m_mapPhone2Time.end())
	{
		LLOG_DEBUG("getPhoneTime phone [%s] TIME=[%d]", phone.c_str(),ItBeforeTime->second);
		return ItBeforeTime->second;
	}
	//else
	//{
	//	if (isAdd)
	//	{
	//		m_mapPhone2Time[phone] = time(NULL);
	//	}
	//	return 0;
	//}
	return 0;
}

Lint  UserManager::setPhoneTime(Lstring phone)
{
	if (phone.empty())
	{
		return 0;
	}
	m_mapPhone2Time[phone] = time(NULL);
	return 1;
}

