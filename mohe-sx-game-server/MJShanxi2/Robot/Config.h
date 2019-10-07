#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	Lint	GetLogLevel();

	Lstring GetServerIp();
	Lshort	GetServerPort();
	Lstring GetServerKey();

	Lshort	GetRobotBegin();
	Lshort	GetRobotCount();

	Lstring	GetLocolIp();
	Lshort	GetLocolPort();
	Lshort	GetThreadCount();
	Lshort	GetWaitSecond();
	Lint	GetServerID();
	Lint	GetPlayType();
	
private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif