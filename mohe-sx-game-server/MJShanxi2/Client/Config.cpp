#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("client_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	//Ä¬ÈÏdebug log
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetCenterIp()
{
	return m_ini.GetString("CenterIp", "123.57.41.73");
}

Lshort Config::GetCenterPort()
{
	return m_ini.GetShort("CenterPort", 5001);
}

Lstring Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "127.0.0.1");
}


Lstring Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp", "127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 6001);
}

Lint   Config::GetRoomId()
{
	return m_ini.GetInt("RoomId", 0);
}
Lstring Config::GetOpenId()
{
	return m_ini.GetString("OpenId", "test");
}
