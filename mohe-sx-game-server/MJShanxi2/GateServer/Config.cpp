#include "Config.h"
#include "LLog.h"

#if _LINUX
#include "../LBase/Func.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

bool Config::Init()
{
	std::string strFilePath = "gate_config.ini";
#if _LINUX
	char strCurPath[512] = {0};
	getcwd(strCurPath, 512);
	strFilePath = strCurPath;
	strFilePath += "/gate_config.ini";
#endif

	m_ini.LoadFile(strFilePath);
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetId()
{
	return m_ini.GetInt("Id", 1);
}

Lint Config::GetLogLevel()
{
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp", "127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 8101);
}

Lstring	Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "127.0.0.1");
}

Lshort Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 8001);
}

Lstring	Config::GetLogicManagerIp()
{
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	return m_ini.GetShort("LogicManagerPort", 10001);
}

Lstring Config::GetLogicKey()
{
	return m_ini.GetString("LogicKey", "123231");
}

Lint Config::GatGateGaoFangFlag()
{
	return m_ini.GetInt("GaoFangFlag", 0);
}