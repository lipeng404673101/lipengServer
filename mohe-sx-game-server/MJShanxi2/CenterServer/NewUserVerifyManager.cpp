#include "LLog.h"
#include "NewUserVerifyManager.h"
#include "BaseCommonFunc.h"
#include "Work.h"

//////////////////////////////////////////////////////////////////////////
bool NewUserVerifyManager::Init()
{
	for (int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpool[i].SetDatabaseSource(true);	
	}

	for (int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpoolBK[i].SetDatabaseSource(false);		
	}
	return true;
}

bool NewUserVerifyManager::Final()
{

	return true;
}

void NewUserVerifyManager::Clear()
{

}


void NewUserVerifyManager::Push(const Lstring& strUUID, LMsg* pMsg)
{
	unsigned int uHashKey = caculateHashKey(strUUID.c_str(), strUUID.length());
	int iIndex = uHashKey % NEWUSERVERIFY_NUM;
	if (gWork.GetDBMode())
	{
		LLOG_DEBUG("NewUserVerify  PUSH SQL TO MANSTER MYSQL");
		m_dbpool[iIndex].Push(pMsg);
	}
	else
	{
		LLOG_DEBUG("NewUserVerify  PUSH SQL TO BAK MYSQL");
		m_dbpoolBK[iIndex].Push(pMsg);
	}
}

void NewUserVerifyManager::Start()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++ i)
	{
		m_dbpool[i].Start();
		//_sleep(10);
	}

	for (int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpoolBK[i].Start();
		//_sleep(10);
	}
}

void NewUserVerifyManager::Join()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpool[i].Join();
	}

	for (int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpoolBK[i].Join();
	}
}

void NewUserVerifyManager::Stop()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpool[i].Stop();
	}

	for (int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpoolBK[i].Stop();
	}
}
