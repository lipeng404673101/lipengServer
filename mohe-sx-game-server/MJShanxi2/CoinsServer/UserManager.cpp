#include "UserManager.h"
#include "LTime.h"

bool UserManager::Init()
{
	m_timeFreeCoins = 0;
	return true;
}

bool UserManager::Final()
{
	return true;
}

void UserManager::AddUser(const UserPtr& user)
{
	if(user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexUserQueue);
	if(m_mapId2User.find(user->GetUserDataId()) == m_mapId2User.end())
	{
		m_mapId2User[user->GetUserDataId()] = user;
		m_mapUUID2User[user->GetUserData().m_unioid] = user;
	}
}

void UserManager::DelUser(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);

	auto itUser = m_mapId2User.find(iUserId);
	if(itUser != m_mapId2User.end())
	{
		m_mapUUID2User.erase(itUser->second->GetUserData().m_unioid);
		m_mapId2User.erase(itUser);
	}
}

int UserManager::GetUsersCount()
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);
	return (int)m_mapUUID2User.size();
}

void UserManager::ClearUsers()
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);
	m_mapUUID2User.clear();
	m_mapId2User.clear();
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserId(Lint iUserId)
{
	UserPtr user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapId2User.find(iUserId);
		if(itUser == m_mapId2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserUUID(const Lstring& uuid)
{
	UserPtr user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapUUID2User.find(uuid);
		if(itUser == m_mapUUID2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

int UserManager::GetGiveCount( Lint iUserId )
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);

	LTime cur;
	if ( m_timeFreeCoins != cur.GetYday() )	// 不是一天的数据了 清楚所有的数据
	{
		m_timeFreeCoins = cur.GetYday();
		m_userFreeCoins.clear();
	}

	auto itUser = m_userFreeCoins.find(iUserId);
	if(itUser == m_userFreeCoins.end())
	{
		return 0;
	}
	return itUser->second;
}

void UserManager::IncreaseGiveCount( Lint iUserId )
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);
	auto itUser = m_userFreeCoins.find(iUserId);
	if(itUser == m_userFreeCoins.end())
	{
		m_userFreeCoins[iUserId] = 1;
		return;
	}
	itUser->second++;
}