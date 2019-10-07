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

	Lstring GetCenterIp();
	Lshort GetCenterPort();
	Lstring GetCenterKey();


	Lstring GetInsideIp();
	Lshort GetInsidePort();

	Lint   GetRoomId();
	Lstring GetOpenId();

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;
};

#define gConfig Config::Instance()

#endif