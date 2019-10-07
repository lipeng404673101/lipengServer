#include "UserPlayData.h"
#include "Config.h"
#include "LLog.h"
#include "Work.h"

#define TOTAL_AROUND "totalAround"
#define WIN_AROUND "winAround"
#define USER_PLAY_DATA "UserPlayData"

bool PlayData::Init()
{
	if (m_dbsession)
	{
		delete m_dbsession;
	}
	if (m_dbsessionBK)
	{
		delete m_dbsessionBK;
		m_dbsessionBK->Init(gConfig.GetDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());
	}
	m_dbsession = new LDBSession;
	m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());

	if (!m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		return false;
	}

	return true;
}

bool PlayData::Final()
{
	if (m_dbsession)
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}

	if (m_dbsessionBK)
	{
		delete m_dbsessionBK;
		m_dbsessionBK = NULL;
	}
	return true;
}

bool PlayData::GetUserPlayTotalAround(LMsgL2LDBGetUserPlayData* msg,Lint& totalAro, Lint& winAro )
{
	// 保存库存数量
	char key[64];
	sprintf(key, "%s%d", USER_PLAY_DATA, msg->m_userId);
	if (!m_RedisClient.readDataByHash(std::string(key), std::string(TOTAL_AROUND), totalAro))
	{
		LLOG_DEBUG("PlayData::GetUserPlay totalAround from redis Failed");

		bool bGet = GetUserPlayTotalAroundFromDB(msg->m_sp, msg->m_userId, totalAro);
		if (bGet) {
			m_RedisClient.writeDataByHash(std::string(key), std::string(TOTAL_AROUND), totalAro);
		}
		else {
			return false;
		}

	}

	if (!m_RedisClient.readDataByHash(std::string(key), std::string(WIN_AROUND), winAro))
	{
		LLOG_DEBUG("PlayData::GetUserPlay winAround from redis Failed");

		bool bGet = GetUserPlayTotalAroundFromDB(msg->m_sp, msg->m_userId, winAro);
		{
			if (bGet) {
				m_RedisClient.writeDataByHash(std::string(key), std::string(WIN_AROUND), winAro);
			}
			else {
				return false;
			}
		}
	}
	return true;
}


bool PlayData::UpdateUserPlayScore(LMsgL2LDBUpdatePlayerScore* msg, Lint& totalAro, Lint& winAro)
{
	// 保存库存数量
	char key[64];
	sprintf(key, "%s%d", USER_PLAY_DATA, msg->m_userId);

	Lint totalAround = 0;
	Lint winAround = 0;

	if (!m_RedisClient.readDataByHash(std::string(key), std::string(TOTAL_AROUND), totalAround))
	{
		LLOG_DEBUG("PlayData::GetUserPlay totalAround from redis Failed");

		bool bGet = GetUserPlayTotalAroundFromDB(msg->m_sp,msg->m_userId, totalAround);
		if (bGet) {
			m_RedisClient.writeDataByHash(std::string(key), std::string(TOTAL_AROUND), totalAround + 1 );
		}
		else {
			return false;
		}
	}

	totalAro = totalAround + 1;


	if (!m_RedisClient.readDataByHash(std::string(key), std::string(WIN_AROUND), winAround))
	{
		LLOG_DEBUG("PlayData::GetUserPlay winAround from redis Failed");

		bool bGet = GetUserPlayTotalAroundFromDB(msg->m_sp, msg->m_userId, winAround);
		{
			if (bGet) {
				if (msg->m_win) {
					winAround++;
				}

				m_RedisClient.writeDataByHash(std::string(key), std::string(WIN_AROUND), winAround);
			}
			else {
				return false;
			}
		}
	}
	winAro = winAround;
	return true;
}


bool  PlayData::GetUserPlayTotalAroundFromDB(const LSocketPtr	m_sp, const Lint userId, Lint& totalAro)
{
	LMsgLDB2LGetUserPlayData log;
	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		m_sp->Send(log.GetSendBuff());
		return false;
	}

	LLOG_DEBUG("get the sums player take");
	Lint sumAround = 0;
	std::stringstream ss;

	ss << "SELECT COUNT(count) AS nums FROM access_log WHERE Pos1 = '";
	ss << userId << "'";
	ss << "or Pos2 = '";
	ss << userId << "'";
	ss << "or Pos3 = '";
	ss << userId << "'";
	ss << "or Pos4 = '";
	ss << userId << "'";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_DEBUG("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		m_sp->Send(log.GetSendBuff());
		return false;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		totalAro = atoi(*row);
	}
	mysql_free_result(res);
	return true;
}


bool   PlayData::GetUserPlayWinPercentFromDB(const LSocketPtr	m_sp, const Lint userId, Lint& winAro)
{
	LMsgLDB2LGetUserPlayData log;
	MYSQL* pMySQL = NULL;
	//if(m_dbsession)
	//{
	//	pMySQL = m_dbsession->GetMysql();
	//}
	pMySQL = this->GetMySQL();

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		m_sp->Send(log.GetSendBuff());
		return false;
	}
	Lint sumWins = 0;
	std::stringstream ss;

	ss << "SELECT COUNT(count) AS nums FROM access_log WHERE (Pos1 = '";
	ss << userId << "'";
	ss << " and " << " Score1 > 0) ";
	ss << "or ( Pos2 = '";
	ss << userId << "'";
	ss << " and " << " Score2 > 0) ";
	ss << "or  ( Pos3 = '";
	ss << userId << "'";
	ss << " and " << " Score3 > 0) ";
	ss << "or ( Pos4 = '";
	ss << userId << "'";
	ss << " and " << " Score4 > 0) ";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_DEBUG("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		m_sp->Send(log.GetSendBuff());
		return false;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		winAro = atoi(*row);
	}
	mysql_free_result(res);

	return true;
}

MYSQL * PlayData::GetMySQL()
{
	if (gWork.GetDBMode())
	{
		if (m_dbsession)  m_dbsession->GetMysql();
	}
	else
	{
		if (m_dbsessionBK) m_dbsessionBK->GetMysql();
	}
	return NULL;
}