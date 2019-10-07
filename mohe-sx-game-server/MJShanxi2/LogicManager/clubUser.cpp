#include "clubUser.h"

bool ClubUserManager::Init()
{
	return true;
}

bool ClubUserManager::Final()
{
	return true;
}
//这个函数暂时没有用
/*
void ClubUserManager::addClubUser(Lint clubId, boost::shared_ptr<ClubUser> user)
{
	if (user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
	if (m_mapId2ClubUser.find(user->m_userId) == m_mapId2ClubUser.end())
	{
		m_mapId2ClubUser[user->m_userId] = user;
	}

	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList == m_mapClubId2ClubUserIdList.end())
	{
		std::set<Lint> tempUserIdList;
		tempUserIdList.insert(user->m_userId);
		m_mapClubId2ClubUserIdList[clubId] = tempUserIdList;
	}
	else
	{
		ItUserIdList->second.insert(user->m_userId);
	}


}
*/
void ClubUserManager::addClubUser(clubUser &userInfo, Lint online)
{
	LLOG_DEBUG("addClubUser user[%d] type[%d] add club[%d] clubSq[%d] point[%d]  online[%d]",userInfo.m_userId,userInfo.m_type,userInfo.m_clubId,userInfo.m_clubSq,userInfo.m_point,online);
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
	//用户数据
	auto ItClubUser = m_mapId2ClubUser.find(userInfo.m_userId);
	if (ItClubUser == m_mapId2ClubUser.end())
	{
		boost::shared_ptr<ClubUser> clubUser(new ClubUser(userInfo.m_userId, userInfo.m_clubId));
		clubUser->m_nike = userInfo.m_nike;
		clubUser->m_headImageUrl = userInfo.m_headUrl;
		
		//更新状态
		if (online != USER_IN_CLUB_OFFLINE)
			clubUser->setLine(online);
		//记录用户加入俱乐部
		clubUser->addNewClub(userInfo.m_clubId, userInfo.m_type);
		//初始化用户在该俱乐部能量值
		clubUser->setClubPoint(userInfo.m_clubId, userInfo.m_point);
		m_mapId2ClubUser[userInfo.m_userId] = clubUser;
	}
	else
	{
		//存在则更新用户状态
		if (online != USER_IN_CLUB_OFFLINE)
			ItClubUser->second->setLine(online);
		//记录用户加入俱乐部
		ItClubUser->second->addNewClub(userInfo.m_clubId, userInfo.m_type);

		//初始化用户在该俱乐部能量值
		ItClubUser->second->setClubPoint(userInfo.m_clubId, userInfo.m_point);

	}

	

	//用户加入俱乐部
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(userInfo.m_clubId);
	if (ItUserIdList == m_mapClubId2ClubUserIdList.end())
	{
		std::set<Lint> tempUserIdList;
		tempUserIdList.insert(userInfo.m_userId);
		m_mapClubId2ClubUserIdList[userInfo.m_clubId] = tempUserIdList;
	}
	else
	{
		ItUserIdList->second.insert(userInfo.m_userId);
	}

}
//暂时不用
//void ClubUserManager::addClubUser(Lint clubId, Lint iUserId,Lstring nike,Lstring headurl, Lint type, Lint online)
//{
//	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
//	auto ItClubUser = m_mapId2ClubUser.find(iUserId);
//	if (ItClubUser == m_mapId2ClubUser.end())
//	{
//		boost::shared_ptr<ClubUser> clubUser(new ClubUser(iUserId, clubId));
//		clubUser->m_nike = nike;
//		clubUser->m_headImageUrl = headurl;
//		if (type == 2)
//			clubUser->m_adminClub.insert(clubId);
//		else if (type == 3)
//			clubUser->m_ownerClub.insert(clubId);
//
//		if (online != USER_IN_CLUB_OFFLINE)
//			clubUser->setLine(online);
//
//			//clubUser->m_type = type;
//		m_mapId2ClubUser[iUserId] = clubUser;
//	}
//	else
//	{
//		if (type == 2)
//			ItClubUser->second->m_adminClub.insert(clubId);
//		else if (type == 3)
//			ItClubUser->second->m_ownerClub.insert(clubId);
//
//		if (online != USER_IN_CLUB_OFFLINE)
//			ItClubUser->second->setLine(online);
//	}
//
//	
//
//
//	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
//	if (ItUserIdList == m_mapClubId2ClubUserIdList.end())
//	{
//		std::set<Lint> tempUserIdList;
//		tempUserIdList.insert(iUserId);
//		m_mapClubId2ClubUserIdList[clubId] = tempUserIdList;
//	}
//	else
//	{
//		ItUserIdList->second.insert(iUserId);
//	}
//
//}

void ClubUserManager::delClubUser(clubUser &userInfo)
{
	LLOG_ERROR("ClubUserManager::delClubUser  clubId[%d]  userId[%d] type[%d]", userInfo.m_clubId, userInfo.m_userId, userInfo.m_type);
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(userInfo.m_clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		ItUserIdList->second.erase(userInfo.m_userId);
	}

	//用户数据
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
	auto ItClubUser = m_mapId2ClubUser.find(userInfo.m_userId);
	if (ItClubUser != m_mapId2ClubUser.end())
	{
		ItClubUser->second->delFromClub(userInfo.m_clubId, userInfo.m_type);
		if (ItClubUser->second->m_currClubInfo.m_clubId == userInfo.m_clubId)
			ItClubUser->second->m_currClubInfo.m_clubId = 0;
	}

	auto ItOnlineUserIdList = m_mapClubId2ClubOnLineUserIdList.find(userInfo.m_clubId);
	if (ItOnlineUserIdList != m_mapClubId2ClubOnLineUserIdList.end())
	{
		ItOnlineUserIdList->second.erase(userInfo.m_userId);
	}

}
//暂时不用
//void ClubUserManager::delClubUser(Lint iUserId)
//{
//	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
//
//	auto itUser = m_mapId2ClubUser.find(iUserId);
//	if (itUser != m_mapId2ClubUser.end())
//	{
//		m_mapId2ClubUser.erase(itUser);
//	}
//}

boost::shared_ptr<CSafeResourceLock<ClubUser> > ClubUserManager::getClubUserbyUserId(Lint iUserId)
{
	boost::shared_ptr<ClubUser> user;

	do
	{
		boost::mutex::scoped_lock l(m_mutexClubUserQueue);

		auto itUser = m_mapId2ClubUser.find(iUserId);
		if (itUser == m_mapId2ClubUser.end())
		{
			break;
		}

		user = itUser->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser;
	if (user)
	{
		safeUser.reset(new CSafeResourceLock<ClubUser>(user));
	}

	return safeUser;
}

//获取用户数据信息根据俱乐部ID和玩家ID
boost::shared_ptr<CSafeResourceLock<ClubUser>> ClubUserManager::getClubUserbyClubIdUserId(Lint clubId, Lint iUserId)
{
	boost::shared_ptr<ClubUser> user;

	do
	{
		boost::mutex::scoped_lock l(m_mutexClubUserQueue);

		auto itUser = m_mapId2ClubUser.find(iUserId);
		if (itUser == m_mapId2ClubUser.end())
		{
			break;
		}

		user = itUser->second;

	} while (false);

	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser;
	if (user)
	{
		for (auto iClubId = user->m_allClubId.begin(); iClubId != user->m_allClubId.end(); iClubId++)
		{
			if (*iClubId == clubId)
			{
				safeUser.reset(new CSafeResourceLock<ClubUser>(user));
			}
		}
	}

	return safeUser;
}


void ClubUserManager::userOffLine(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);
	LLOG_ERROR("userId[%d] offline ", iUserId);
	auto itUser = m_mapId2ClubUser.find(iUserId);
	if (itUser == m_mapId2ClubUser.end())
	{
		return;
	}
	itUser->second->setLine(USER_IN_CLUB_OFFLINE);
	itUser->second->m_offLineTime = time(NULL);

	auto ItclubUserList = m_mapClubId2ClubOnLineUserIdList.find(itUser->second->m_currClubInfo.m_clubId);
	if (ItclubUserList != m_mapClubId2ClubOnLineUserIdList.end())
	{
		//LLOG_ERROR("userId[%d] offline clubId ");
		ItclubUserList->second.erase(iUserId);
	}
}

void ClubUserManager::userOnLine(Lint iUseId, bool IsPlay)
{
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);

	auto itUser = m_mapId2ClubUser.find(iUseId);
	if (itUser != m_mapId2ClubUser.end())
	{
		itUser->second->setLine(USER_IN_CLUB_ONLINE);
		if (IsPlay)
			itUser->second->setState(USER_STATE_PLAY);
		else
			itUser->second->setState(USER_STATE_INIT);
	}
	
}

void ClubUserManager::setUserPlayStatus(Lint iUseId, bool IsPlay)
{
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);

	auto itUser = m_mapId2ClubUser.find(iUseId);
	if (itUser != m_mapId2ClubUser.end())
	{
		if (IsPlay)
			itUser->second->setState(USER_STATE_PLAY);
		else
			itUser->second->setState(USER_STATE_INIT);
	}

}

void ClubUserManager::userLoginClub(Lint clubId, Lint userId)
{
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);

	auto itUserList = m_mapClubId2ClubOnLineUserIdList.find(clubId);
	if (itUserList != m_mapClubId2ClubOnLineUserIdList.end())
	{
		itUserList->second.insert(userId);
	}
	else
	{
		std::set<Lint > clubUserList;
		clubUserList.insert(userId);
		m_mapClubId2ClubOnLineUserIdList[clubId] = clubUserList;
	}
	
}


void ClubUserManager::userLogoutClub(Lint clubId, Lint userId)
{
	boost::mutex::scoped_lock l(m_mutexClubUserQueue);

	auto itUserList = m_mapClubId2ClubOnLineUserIdList.find(clubId);
	if (itUserList != m_mapClubId2ClubOnLineUserIdList.end())
	{
		itUserList->second.erase(userId);
	}

}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubAllUser(Lint clubId)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		LLOG_ERROR("getClubAllUser clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser->second.get() != NULL && itUser != m_mapId2ClubUser.end())
			{
				clubUserList.insert(itUser->second);
			}
		}

	}
	LLOG_ERROR("getClubAllUser clubId[%d] userSize[%d]",clubId, clubUserList.size());
	return clubUserList;
}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubAllOnLineUser(Lint clubId)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		//LLOG_ERROR("getClubAllOnLineUser clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end() && itUser->second.get() != NULL && itUser->second->getLine() == USER_IN_CLUB_ONLINE)
			{
				clubUserList.insert(itUser->second);
			}
		}

	}
	LLOG_ERROR("getClubAllOnLineUser clubId[%d] clubOnLineUserSize[%d]", clubId, clubUserList.size());
	return clubUserList;
}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubUserInGame(Lint clubId)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end() && itUser->second.get() != NULL && (itUser->second->isPlay()))
			{
				clubUserList.insert(itUser->second);
			}
		}

	}
	//LLOG_ERROR("getClubUserInGame clubId[%d] clubUserInGameSize[%d]", clubId, clubUserList.size());
	return clubUserList;
}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubAllOnLineUserNoGame(Lint clubId)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		//LLOG_ERROR("getClubAllOnLineUserNoGame clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end() && itUser->second.get()!=NULL && itUser->second->getLine() == USER_IN_CLUB_ONLINE &&  !(itUser->second->isPlay()))
			{
				clubUserList.insert(itUser->second);
			}
		}

	}
	//LLOG_ERROR("getClubAllOnLineUserNoGame clubId[%d] clubOnLineUserNoGameSize[%d]", clubId, clubUserList.size());
	return clubUserList;
}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubAllOffLineUserNoGame(Lint clubId)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		//LLOG_ERROR("getClubAllOnLineUserNoGame clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end() && itUser->second.get() != NULL && itUser->second->getLine() == USER_IN_CLUB_OFFLINE && !(itUser->second->isPlay()))
			{
				clubUserList.insert(itUser->second);
			}
		}

	}
	//LLOG_ERROR("getClubAllOnLineUserNoGame clubId[%d] clubOnLineUserNoGameSize[%d]", clubId, clubUserList.size());
	return clubUserList;
}

std::set<Lint> ClubUserManager::getClubAllOnLineUserId(Lint clubId)
{
	std::set<Lint>  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		//LLOG_ERROR("getClubAllOnLineUser clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end() && itUser->second != NULL && itUser->second->getLine() == USER_IN_CLUB_ONLINE)
			{
				clubUserList.insert(itUser->second->getID());
			}
		}

	}

	//LLOG_ERROR("getClubAllOnLineUserId clubId[%d] clubOnLineUserSize[%d]", clubId, clubUserList.size());
	return clubUserList;
}

Lint  ClubUserManager::getUserInAllClubCount(const std::set<Lint> &clubIdList, std::vector<clubOnlineUserSize> &clubOnlineSize)
{
	Lint onLineCountSum = 0;

	for (auto ItClubId = clubIdList.begin(); ItClubId != clubIdList.end(); ItClubId++)
	{
		clubOnlineUserSize  tempClubOnlineSize;
		tempClubOnlineSize.m_clubId = *ItClubId;
		tempClubOnlineSize.m_onlineUserSize = getClubAllOnLineUserId(*ItClubId).size();
		clubOnlineSize.push_back(tempClubOnlineSize);

		onLineCountSum += (tempClubOnlineSize.m_onlineUserSize);
	}

	return onLineCountSum;

}

Lint ClubUserManager::getClubUserCountOnline(Lint clubId)
{
	return getClubAllOnLineUserId(clubId).size();
}

Lint ClubUserManager::getClubUserCount(Lint clubId)
{
	auto ItUserSize = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserSize != m_mapClubId2ClubUserIdList.end())
	{
		return ItUserSize->second.size();
	}
	return 0;
}

void ClubUserManager::setUserAdmin(Lint type,Lint clubId, Lint userId)
{

	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(userId);
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		if (type == 1)
			safeClubUserInfo->getResource()->m_adminClub.insert(clubId);
		else if(type==2)
			safeClubUserInfo->getResource()->m_adminClub.erase(clubId);
	}
}

std::set<boost::shared_ptr<ClubUser>  > ClubUserManager::getClubUserbyUserNike(Lint clubId,Lstring nike)
{
	std::set<boost::shared_ptr<ClubUser>  >  clubUserList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		//LLOG_ERROR("getClubAllUser clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if (itUser != m_mapId2ClubUser.end())
			{
				if (itUser->second.get()!=NULL &&itUser->second->m_nike.find(nike) != Lstring::npos)
				{
					clubUserList.insert(itUser->second);
				}
			}
		}

	}
	//LLOG_ERROR("getClubAllUser clubId[%d] userSize[%d]", clubId, clubUserList.size());
	return clubUserList;


}

//判断用户是否在俱乐部中
bool ClubUserManager::isUserInClub(Lint clubId, Lint userId)
{
	if (clubId == 0 || userId == 0)return false;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		
		if (ItUserIdList->second.find(userId) != ItUserIdList->second.end())
		{
			return true;
		}
		
	}
	return false;
}


//进入俱乐部时获取用户默认的clubId
Lint ClubUserManager::getDefaultClubId(Lint userId)
{
	if ( userId == 0)return 0;
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = gClubUserManager.getClubUserbyUserId(userId);
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		//会长
		if (!safeClubUserInfo->getResource()->m_ownerClub.empty())
		{
			return *(safeClubUserInfo->getResource()->m_ownerClub.begin());
		}
		//管理员
		if (!safeClubUserInfo->getResource()->m_adminClub.empty())
		{
			return *(safeClubUserInfo->getResource()->m_adminClub.begin());
		}
		if (!safeClubUserInfo->getResource()->m_allClubId.empty())
		{
			return *(safeClubUserInfo->getResource()->m_allClubId.begin());
		}
	}
	return 0;
}

std::set<Lint> ClubUserManager::getClubPlayTypeUserId(Lint clubId, Lint playTypeId)
{
	std::set<Lint>  clubUserIdList;
	if (playTypeId != 0)
	{
		auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
		if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
		{
			LLOG_ERROR("getClubAllOnLineUser clubId[%d] clubUserSize[%d]", clubId, ItUserIdList->second.size());
			for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
			{
				auto itUser = m_mapId2ClubUser.find(*ItUser);
				if (itUser->second.get() != NULL && itUser != m_mapId2ClubUser.end() && itUser->second != NULL && itUser->second->m_currClubInfo.m_playTypeId == playTypeId)
				{
					clubUserIdList.insert(*ItUser);
				}
			}

		}
	}
	LLOG_ERROR("getClubPlayTypeUserId clubId[%d] playType[%d] usersize[%d]", clubId, playTypeId, clubUserIdList.size());
	return clubUserIdList;
}

std::set<Lint> ClubUserManager::getNeedFreshSceneUserId(Lint clubId)
{
	std::set<Lint>  clubUserIdList;
	auto ItUserIdList = m_mapClubId2ClubUserIdList.find(clubId);
	if (ItUserIdList != m_mapClubId2ClubUserIdList.end())
	{
		for (auto ItUser = ItUserIdList->second.begin(); ItUser != ItUserIdList->second.end(); ItUser++)
		{
			auto itUser = m_mapId2ClubUser.find(*ItUser);
			if ( itUser != m_mapId2ClubUser.end() && itUser->second.get() != NULL && itUser->second->m_currClubInfo.m_clubId == clubId && !itUser->second->isPlay())
			{
				clubUserIdList.insert(*ItUser);
			}
		}

	}

	LLOG_ERROR("getNeedFreshSceneUserId clubId[%d]  usersize[%d]", clubId, clubUserIdList.size());
	return clubUserIdList;
}

//  0 ==成功  1=未找到用户信息  2=未找到用户在俱乐部信息 
Lint  ClubUserManager::_changePoint(Lint userId, Lint clubId, Lint point, Lint type)
{
	if (userId == 0 || clubId == 0 || point == 0)
	{
		LLOG_ERROR("_changePoint fail userId[%d] club[%d] point[%d] ",userId,clubId,point);
		return 0;
	}
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo =getClubUserbyUserId(userId);
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		auto ItPoint = safeClubUserInfo->getResource()->m_powerPoint.find(clubId);
		if (ItPoint != safeClubUserInfo->getResource()->m_powerPoint.end())
		{
			if (CHANGE_POINT_TYPE_ADD== type)
				ItPoint->second->addPoint(point);
			else if (CHANGE_POINT_TYPE_REDUCE==type)
				ItPoint->second->reducePoint(point);
		}
		else
		{
			if (type == CHANGE_POINT_TYPE_ADD)
			{
				boost::shared_ptr<UserPowerPoint> temp(new UserPowerPoint(userId,clubId,point));
				safeClubUserInfo->getResource()->m_powerPoint[clubId] = temp;
			}
			else if (type == CHANGE_POINT_TYPE_REDUCE)return 2;
		}
	}
	else
	{
		return 1;  //
	}
	return 0;
}

Lint  ClubUserManager::getPoint(Lint userId, Lint clubId)
{
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = getClubUserbyUserId(userId);
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		auto ItPoint = safeClubUserInfo->getResource()->m_powerPoint.find(clubId);
		if (ItPoint != safeClubUserInfo->getResource()->m_powerPoint.end())
		{
			return ItPoint->second->m_point;
		}
	}
	return 0;
}

//获取俱乐部中玩家赢的最多的能量值玩家ID
std::list<Lint> ClubUserManager::getMaxPointUserId(Lint clubId)
{
	Lint t_maxMedalScore = 0;
	std::list<Lint> t_maxMedalUserIdList;
	std::set<boost::shared_ptr<ClubUser>> t_clubUserSet = getClubAllUser(clubId);

	if (t_clubUserSet.empty())
	{
		return t_maxMedalUserIdList;
	}
	
	for (auto t_clubUserSetIter = t_clubUserSet.begin(); t_clubUserSetIter != t_clubUserSet.end(); t_clubUserSetIter++)
	{
		if (*t_clubUserSetIter)
		{
			auto t_powerIter = (*t_clubUserSetIter)->m_powerPoint.find(clubId);
			if (t_powerIter != (*t_clubUserSetIter)->m_powerPoint.end() && t_powerIter->second)
			{
				if (t_powerIter->second->m_medalPoint > t_maxMedalScore && t_powerIter->second->m_userId != 0)
				{
					t_maxMedalScore = t_powerIter->second->m_medalPoint;
				}
			}
		}
	}

	if (t_maxMedalScore <= 0)
	{
		return t_maxMedalUserIdList;
	}

	for (auto t_clubUserSetIter = t_clubUserSet.begin(); t_clubUserSetIter != t_clubUserSet.end(); t_clubUserSetIter++)
	{
		if (*t_clubUserSetIter)
		{
			auto t_powerIter = (*t_clubUserSetIter)->m_powerPoint.find(clubId);
			if (t_powerIter != (*t_clubUserSetIter)->m_powerPoint.end() && t_powerIter->second)
			{
				if (t_powerIter->second->m_medalPoint == t_maxMedalScore && t_powerIter->second->m_userId != 0)
				{
					t_maxMedalUserIdList.push_back(t_powerIter->second->m_userId);
				}
			}
		}
	}
	return t_maxMedalUserIdList;
}

//置零俱乐部中所有玩家赢的能量值
void  ClubUserManager::clearClubUserMedal()
{
	for (auto t_clubUserIter = m_mapId2ClubUser.begin(); t_clubUserIter != m_mapId2ClubUser.end(); t_clubUserIter++)
	{
		if (t_clubUserIter->second)
		{
			auto t_clubUserMedalIter = t_clubUserIter->second->m_powerPoint.find(t_clubUserIter->first);
			if (t_clubUserMedalIter != t_clubUserIter->second->m_powerPoint.end() && t_clubUserMedalIter->second)
			{
				t_clubUserMedalIter->second->m_medalPoint = 0;
			}
		}
	}
}


void  ClubUserManager::userAddDesk(LMsgL2LMGUserAddClubDesk*msg)
{
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(msg->m_userInfo.m_userId);
	if (safeUser && safeUser->isValid())
	{
		safeUser->getResource()->m_currClubInfo.m_playTypeId = msg->m_playTypeId;
		safeUser->getResource()->m_currClubInfo.m_clubDeskId = msg->m_clubDeskId;
		//safeUser->getResource()->setState(USER_STATE_PLAY);
	}
}


void  ClubUserManager::userLeaveDesk(LMsgL2LMGUserLeaveClubDesk* msg)
{
	if (msg == NULL)return;
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = gClubUserManager.getClubUserbyUserId(msg->m_userId);
	if (safeUser && safeUser->isValid())
	{
		safeUser->getResource()->m_currClubInfo.m_playTypeId = msg->m_playTypeId;
		safeUser->getResource()->m_currClubInfo.m_clubDeskId = 0;
		//safeUser->getResource()->setState(USER_STATE_INIT);
	}
}

void  ClubUserManager::userLeaveDesk(LMsgL2LMGUserListLeaveClubDesk* msg)
{
	if (msg == NULL)return;
	for (auto ItUser = msg->m_userIdList.begin(); ItUser != msg->m_userIdList.end(); ItUser++)
	{
		boost::shared_ptr<CSafeResourceLock<ClubUser> > safeUser = getClubUserbyUserId(*ItUser);
		if (safeUser && safeUser->isValid())
		{
			safeUser->getResource()->m_currClubInfo.m_playTypeId = msg->m_playTypeId;
			safeUser->getResource()->m_currClubInfo.m_clubDeskId = 0;
			//safeUser->getResource()->setState(USER_STATE_INIT);
		}
	}
}

Lint  ClubUserManager::updatePoint(Lint userId, Lint clubId,Lint point)
{
	boost::shared_ptr<CSafeResourceLock<ClubUser> > safeClubUserInfo = getClubUserbyUserId(userId);
	if (safeClubUserInfo && safeClubUserInfo->isValid())
	{
		safeClubUserInfo->getResource()->updateClubPoint(clubId, point);
	}
	return 0;
}