#include "LLog.h"
#include "DbServerManager.h"
#include "Config.h"


//////////////////////////////////////////////////////////////////////////
bool DbServerManager::Init()
{
	return true;
}

bool DbServerManager::Final()
{

	return true;
}

void DbServerManager::Clear()
{

}


void DbServerManager::Push( std::string* sql)
{
	static unsigned int index = 0;
	Lint i = index++ % (THREAD_NUM);
	if(i < THREAD_NUM)
	{
		m_dbpool[i].Push(sql);
	}
}

void DbServerManager::Start()
{
	for(int i = 0; i < THREAD_NUM; ++ i)
	{
		m_dbpool[i].Start();
		//_sleep(10);
	}
}

void DbServerManager::Join()
{
	for(int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpool[i].Join();
	}
}

void DbServerManager::Stop()
{
	for(int i = 0; i < THREAD_NUM; ++i)
	{
		m_dbpool[i].Stop();
	}
}
