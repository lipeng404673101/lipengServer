#include "Robot.h"
#include "LTime.h"
#include "Config.h"
#include "UserManager.h"

Robot::Robot(const LUser& data)
	: User(data, 0, "")
{
	m_userData.m_coins = gConfig.GetGiveCoinsCount();
}

Robot::~Robot(void)
{
}

void Robot::Login()
{
	LTime now;
	m_userData.m_lastLoginTime = now.Secs();
	m_online = true;
}

void Robot::Logout()
{
	m_online = false;
}

void Robot::Send(LMsg& msg)
{

}

void Robot::Send(const LBuffPtr& buff)
{
	
}

void Robot::AddCoinsCount(Lint count, Lint operType)
{
	m_userData.m_coins += count;
}

void Robot::DelCoinsCount(Lint count, Lint operType)
{
	if ( m_userData.m_coins < count )
	{
		m_userData.m_coins = gConfig.GetGiveCoinsCount();
	}
	else
	{
		m_userData.m_coins -= count;
	}
}
