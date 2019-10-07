#include "LLog.h"
#include "DbServerManager.h"
#include "Config.h"
#include "Work.h"


//////////////////////////////////////////////////////////////////////////
bool DbServerManager::Init()
{
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpool[i].SetDatabaseSource(true);	
	}

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpoolBK[i].SetDatabaseSource(false);	
	}
	return true;
}

bool DbServerManager::Final()
{

	return true;
}

void DbServerManager::Clear()
{

}


void DbServerManager::Push( std::string* sql,Lint userId)
{
	Lint index = userId % (THREAD_NUM);
	if(index < THREAD_NUM)
	{
		if (gWork.GetDBMode())
		{
			LLOG_DEBUG("PUSH SQL TO MANSTER MYSQL");
			m_dbpool[index].Push(sql);
		}
		else
		{
			LLOG_ERROR("PUSH SQL TO BAK MYSQL");
			m_dbpoolBK[index].Push(sql);
		}
	}
}

void DbServerManager::Start()
{
	for(int i = 0; i < THREAD_NUM; ++ i)
	{
		m_dbpool[i].Start();
		//_sleep(10);
	}

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpoolBK[i].Start();
		//_sleep(10);
	}
}

void DbServerManager::Join()
{
	for(int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpool[i].Join();
	}

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpoolBK[i].Join();
	}
}

void DbServerManager::Stop()
{
	for(int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpool[i].Stop();
	}

	for (int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpoolBK[i].Stop();
	}
}
