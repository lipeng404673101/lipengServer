#include "UserManager.h"

bool UserManager::Init()
{
	return true;
}

bool UserManager::Final()
{
	return true;
}

User* UserManager::GetUserByGateIdAndUUID(Lint gateId, const Lstring& strUUID)
{
	auto itUser = m_mapUUID2User.find(strUUID);
	if(itUser == m_mapUUID2User.end())
	{
		return NULL;
	}

	if(itUser->second->getUserGateID() != gateId)
	{
		return NULL;
	}

	return itUser->second;
}

User* UserManager::GetUserbyDataId(Lint dataId)
{
	auto itUser = m_mapId2User.find(dataId);
	if(itUser == m_mapId2User.end())
	{
		return NULL;
	}

	return itUser->second;
}

void UserManager::AddUser(User* user)
{
	if(user == NULL)
	{
		return;
	}

	if(m_mapId2User.find(user->m_userData.m_id) == m_mapId2User.end())
	{
		m_mapId2User[user->m_userData.m_id] = user;
		m_mapUUID2User[user->m_userData.m_unioid] = user;
	}
}

void UserManager::DelUser(User* user)
{
	if(user == NULL)
	{
		return;
	}

	m_mapId2User.erase(user->m_userData.m_id);
	m_mapUUID2User.erase(user->m_userData.m_unioid);
}

void UserManager::BoadCast(LMsg& msg)
{
	for(auto it = m_mapId2User.begin() ; it != m_mapId2User.end(); ++it)
	{
		if(it->second->GetOnline())
			it->second->Send(msg);
	}
}

//与manager重新连接后，向manager同步消息
void UserManager::SynchroUserData(std::vector<LogicUserInfo> & userInfo)
{
	LLOG_DEBUG("UserManager::SynchroUserData,userSize=[%d]", m_mapId2User.size());
	//用户信息
	for (auto ItUser=m_mapId2User.begin(); ItUser!=m_mapId2User.end(); ItUser++)
	{
		LogicUserInfo  tempUserInfo;
		tempUserInfo.m_userId=ItUser->second->m_userData.m_id;
		//tempUserInfo.m_unioid=ItUser->second->m_userData.m_unioid;
		tempUserInfo.m_gateId=ItUser->second->m_gateId;
		tempUserInfo.m_ip=ItUser->second->m_ip;

		userInfo.push_back(tempUserInfo);
	}
}