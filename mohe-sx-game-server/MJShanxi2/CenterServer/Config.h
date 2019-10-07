#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool Init();

	virtual	bool	Final();


	Lint	GetLogLevel();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lstring GetClubDbName();
	Lshort	GetDbPort();

	Lstring	 GetBKDbHost();
	Lstring	 GetBKDbUser();
	Lstring	 GetBKDbPass();
	Lshort GetBKDbPort();

	Lstring	 GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetRedisIp();
	Lshort	GetRedisPort();

	Lstring	GetLocalLogin();//±¾µØµÇÂ¼

	Lstring GetGMIp();
	Lshort  GetGMPot();

	Lstring	GetCenterKey();

	Lint    GetTime();

	Lshort    GetWechatVerify();
private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif