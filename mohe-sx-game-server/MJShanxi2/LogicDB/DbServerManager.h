#ifndef _DB_SERVER_GM_H_
#define _DB_SERVER_GM_H_

#include "LBase.h"
#include "LSingleton.h"
#include "DbServer.h"

#define  THREAD_NUM   2

//db·þÎñÆ÷
class DbServerManager : public LSingleton<DbServerManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Clear();

	void                Start();

	void                Join();

	void                Stop();

	void                Push(std::string* sql);


private:

	DbServer      m_dbpool[THREAD_NUM];
	DbServer      m_dbpoolBK[THREAD_NUM];
};


#define gDbServerManager DbServerManager::Instance()

#endif