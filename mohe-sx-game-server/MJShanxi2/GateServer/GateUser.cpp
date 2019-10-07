#include "GateUser.h"
#include "LLog.h"
#include "LTool.h"

bool GateUserManager::Init()
{
	return true;
}

bool GateUserManager::Final()
{
	return true;
}

boost::shared_ptr<GateUser> GateUserManager::createUser(const Lstring& strUUID, LSocketPtr sp)
{
	if(findUser(strUUID))
	{
		return NULL;
	}

	LTime now;

	boost::shared_ptr<GateUser> pUser(new GateUser());

	pUser->m_strUUID		= strUUID;
	pUser->m_strKey		= _generateKey();
	pUser->m_uMsgOrder	= L_Rand(0, 9999);
	pUser->m_timeActive = now.MSecs();
	pUser->m_sp			= sp;
	pUser->m_login		= 0;

	AddUser(pUser);

	return pUser;
}

boost::shared_ptr<GateUser> GateUserManager::findUser(const Lstring& strUUID)
{
	auto it = m_mapUUID2GU.find(strUUID);
	if(it == m_mapUUID2GU.end())
	{
		return NULL;
	}

	return it->second;
}

boost::shared_ptr<GateUser> GateUserManager::findUser(LSocketPtr sp)
{
	auto it = m_mapSp2GU.find(sp);
	if(it == m_mapSp2GU.end())
	{
		return NULL;
	}

	return it->second;
}

void GateUserManager::AddUser(boost::shared_ptr<GateUser> user)
{
	if(user == NULL)
	{
		return;
	}
	if(findUser(user->m_strUUID))
	{
		LLOG_ERROR("The user had exist. UUID=%s", user->m_strUUID.c_str());
		return;
	}

	m_mapUUID2GU[user->m_strUUID] = user;
	m_mapSp2GU[user->m_sp] = user;
}

void GateUserManager::DelUser(const Lstring& strUUID)
{
	boost::shared_ptr<GateUser> user = findUser(strUUID);
	if(!user)
	{
		return;
	}

	m_mapUUID2GU.erase(user->m_strUUID);
	m_mapSp2GU.erase(user->m_sp);
}

const std::map<Lstring, boost::shared_ptr<GateUser> >& GateUserManager::getAllUsers()
{
	return m_mapUUID2GU;
}

Lstring GateUserManager::_generateKey()
{
	static Lstring strSeed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(){}[];,.?|";
	Lstring strKey;
	for(int i = 0; i < 32; i++)
	{
		int iRand = L_Rand(0, strSeed.length() - 1);
		strKey += strSeed.at(iRand);
	}

	return strKey;
}