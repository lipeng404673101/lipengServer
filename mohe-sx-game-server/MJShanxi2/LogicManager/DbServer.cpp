#include "LLog.h"
#include "DbServer.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////

DbServer::DbServer()
	: m_dbsession(NULL)
{

}

bool DbServer::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	return m_dbsession->Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort());
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