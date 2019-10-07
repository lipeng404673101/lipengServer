#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	boost::mutex::scoped_lock l(m_mutex);
	m_ini.LoadFile("coins_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetCoinsKey()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetString("CoinsKey", "123231");
}

Lstring Config::GetInsideIp()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetString("InsideIp", "0.0.0.0");
}

Lshort Config::GetInsidePort()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetShort("InsidePort", 11001);
}

Lstring Config::GetLogicManagerIp()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetShort("LogicManagerPort", 10001);
}

Lint Config::GetChangeOutTime()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("ChangeOutTime", 15);
}

Lint Config::GetOpOutTime()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("OpOutTime", 10);
}

Lint Config::GetMaxUserCount()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("MaxUserCount", 100000);
}

Lint Config::GetMaxGiveCount()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("MaxGiveCount", 3);
}

Lint Config::GetGiveCoinsCount()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("GiveCoinsCount", 1000);
}

bool Config::GetIsGiveCoins()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("IsGiveCoins", 0) != 0;
}

Lint Config::GetMinCoins()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("MinCoins", 1000);
}

Lint Config::GetBaseScore()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("BaseScore", 10);
}

Lint Config::GetDebugModel()
{
	boost::mutex::scoped_lock l(m_mutex);
	return m_ini.GetInt("DebugModel", 0);
}