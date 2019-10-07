#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	Lint	GetId();

	Lint	GetLogLevel();

	Lstring	GetLogicManagerIp();
	Lshort	GetLogicManagerPort();
	Lstring GetLogicKey();

	Lstring GetInsideIp();
	Lshort GetInsidePort();

	Lstring	GetOutsideIp();
	Lshort	GetOutsidePort();
	Lint    GatGateGaoFangFlag(); //1£º·Ç¸ß·À 2:¸ß·À

private:
	LIniConfig	m_ini;
};


#define gConfig Config::Instance()

#endif