#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	m_ini.LoadFile("loginserver_config.ini");
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

Lstring Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp", "0.0.0.0");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 5101);
}

Lstring Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "0.0.0.0");
}

Lshort	Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 5001);
}

Lstring Config::GetCenterIp()
{
	return m_ini.GetString("CenterIp", "127.0.0.1");
}

Lshort	Config::GetCenterPort()
{
	return m_ini.GetShort("CenterPort", 2001);
}

Lint Config::GetLoginFlag() //0: 非高防 1:ali高防，2:yunman高防
{
	return m_ini.GetInt("GaoFangFlag", 0);
}