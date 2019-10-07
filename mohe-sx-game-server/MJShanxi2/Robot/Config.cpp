#include "StdAfx.h"
#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("robot_config.ini");
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

Lstring Config::GetServerIp()
{
	return m_ini.GetString("ServerIp", "127.0.0.1");
}

Lshort Config::GetServerPort()
{
	return m_ini.GetShort("ServerPort", 7001);
}

Lstring Config::GetServerKey()
{
	return m_ini.GetString("LogicDBKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}

Lshort Config::GetRobotBegin()
{
	return m_ini.GetShort("RobotBeginID", 0);
}

Lshort Config::GetRobotCount()
{
	return m_ini.GetShort("RobotCount", 0);
}

Lstring Config::GetLocolIp()
{
	return m_ini.GetString("LocolIp", "127.0.0.1");
}

Lshort Config::GetLocolPort()
{
	return m_ini.GetShort("LocolPort", 8888);
}

Lshort Config::GetThreadCount()
{
	return m_ini.GetShort("ThreadCount", 2);
}

Lshort Config::GetWaitSecond()
{
	return m_ini.GetShort("EveryTimeWaitSecond",0);
}
Lint Config::GetServerID()
{
	return m_ini.GetShort("ServerID",0);
}

Lint Config::GetPlayType()
{
	return m_ini.GetShort("m_flag",0);
}
