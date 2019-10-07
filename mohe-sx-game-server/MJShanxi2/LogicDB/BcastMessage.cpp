
#include "BcastMessage.h"
#include "config.h"
#include "DbServerManager.h"
#include "Work.h"


bool bcastMessage::Init()
{
	if (m_dbsession)
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	if (gWork.GetDBMode())
	{
		m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort());
	}
	else
	{
		m_dbsession->Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort());
	}
	return true;
}

bool bcastMessage::Final()
{
	if (m_dbsession)
	{
		delete m_dbsession;
	}
	return true;
}

Lint bcastMessage::GetMessageListNum()
{
	return m_messageList.size();
}

bool  bcastMessage::LoadMessageDataFromDB()
{
	MYSQL* pMySQL = NULL;
	if (m_dbsession)
	{
		pMySQL = m_dbsession->GetMysql();
	}

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		return false;
	}

	Lint sumAround = 0;
	std::stringstream ss;

	ss << "SELECT Id, message FROM bcastmessage";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_DEBUG("bcastMessage::LoadMessageDataFromDB sql error %s", mysql_error(pMySQL));
		return false;
	}
	m_messageList.clear();
	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		Lint id = atoi(*row++);
		Lstring tempStr = *row++;
		m_messageList.push_back(tempStr);
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	return true;
}


void   bcastMessage::produceBcastMessage(LMsgLDB2LMBCast& bCast)
{
	bCast.m_count = m_messageList.size();
	bCast.m_message = m_messageList;

}

