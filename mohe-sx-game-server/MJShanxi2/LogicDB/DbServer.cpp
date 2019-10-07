#include "LLog.h"
#include "DbServer.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////

DbServer::DbServer()
	: m_dbsession(NULL)
{
	m_bUseMasterSource = true;
}

bool DbServer::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	if (m_bUseMasterSource)
	{
		LLOG_DEBUG("logwyz  master mysql,host[%s],port[%d],user[%s],pw[%s],dbnanme[%s]", gConfig.GetDbHost().c_str(), gConfig.GetDbPort(), gConfig.GetDbUser().c_str(), gConfig.GetDbPass().c_str(), gConfig.GetDbName().c_str());
		return m_dbsession->Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()); 
	}
	LLOG_DEBUG("logwyz  bak mysql,host[%s],port[%d],user[%s],pw[%s],dbnanme[%s]", gConfig.GetBKDbHost().c_str(), gConfig.GetBKDbPort(), gConfig.GetBKDbUser().c_str(), gConfig.GetBKDbPass().c_str(), gConfig.GetDbName().c_str());
	return m_dbsession->Init(gConfig.GetBKDbHost(), gConfig.GetBKDbUser(), gConfig.GetBKDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetBKDbPort());
}

bool DbServer::Final()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}
	return true;
}

void DbServer::Clear()
{

}

void DbServer::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}

	while(!GetStop())
	{
		Lstring* str = (Lstring*)Pop();
		if(str == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			Excute(*str);
			delete str;
		}
	}

	this->Final();
}

void DbServer::Excute( const std::string& str )
{
	if( m_dbsession )
	{
		MYSQL* mysql = m_dbsession->GetMysql();

		//测试输出
		LLOG_ERROR("DBTEST- access database source: %s",   mysql->host ? mysql->host : "");

		if(mysql == NULL)
		{
			LLOG_ERROR("DbServer::Excute MYSQL NULL");
			return ;
		}
		if(mysql_real_query(mysql,str.c_str(),str.size()))
		{
			LLOG_ERROR("DbServer::Excute error = %s,sql = %s ", mysql_error(mysql),str.c_str());
			//this->Push(sql);   // 执行不成功，重新放入队列
		}
	}
	else
	{
		LLOG_ERROR("DbServer::Excute dbsession is NULL, sql = %s ",str.c_str());
	}
}

void DbServer::SetDatabaseSource(bool  bUserMaster)
{
	m_bUseMasterSource = bUserMaster;
}