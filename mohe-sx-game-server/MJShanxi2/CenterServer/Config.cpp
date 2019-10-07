#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	m_ini.LoadFile("center_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring	Config::GetDbHost()
{
	return m_ini.GetString("DbHost", "192.168.1.139");
}

Lstring	Config::GetDbUser()
{
	return m_ini.GetString("DbUser", "root");
}

Lstring	Config::GetDbPass()
{
	return m_ini.GetString("DbPass", "");
}

Lstring	Config::GetBKDbHost()
{
	return m_ini.GetString("BKDbHost", "192.168.1.139");
}

Lstring	Config::GetBKDbUser()
{
	return m_ini.GetString("BKDbUser", "root");
}

Lstring	Config::GetBKDbPass()
{
	return m_ini.GetString("BKDbPass", "");
}

Lshort	Config::GetBKDbPort()
{
	return m_ini.GetShort("BKDbPort", 3306);
}

Lstring	Config::GetDbName()
{
	return m_ini.GetString("DbName", "test");
}

Lstring Config::GetClubDbName()
{
	return m_ini.GetString("ClubDbName", "test");
}

Lshort	Config::GetDbPort()
{
	return m_ini.GetShort("DbPort", 3306);
}


Lstring	Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp","127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 2001);
}

Lstring Config::GetRedisIp()
{
	return m_ini.GetString("RedisIp","127.0.0.1");
}

Lshort Config::GetRedisPort()
{
	return m_ini.GetShort("RedisPort", 6379);
}

Lstring Config::GetGMIp()
{
	return m_ini.GetString("GMIp", "127.0.0.1");
}

Lshort  Config::GetGMPot()
{
	return m_ini.GetShort("GMPort", 9001);
}

Lstring Config::GetLocalLogin()
{
	return m_ini.GetString("LocalLogin", "false");
}

Lstring	Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}

Lint Config::GetTime()
{
	return m_ini.GetInt("StartTime", 0);
}

Lshort Config::GetWechatVerify()
{
	return m_ini.GetShort("NeedWechat", 1);
}

